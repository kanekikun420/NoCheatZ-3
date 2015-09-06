#include "http.h"

Http::Http()
{
#ifdef WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2,2), &wsadata);
#endif
}

Http::~Http()
{
}

void Http::End()
{
	closesocket(sock);
#ifdef WIN32
	WSACleanup();
#endif
	load_time.EndExec();
	Msg("[NoCheatZ] %f : Transaction done with master server.\n", Plat_FloatTime(), m_hostname, m_port);
	Msg("[NoCheatZ] %f : HTTP Metrics Results\n", Plat_FloatTime(), m_hostname, m_port);
	load_time.PrintResults();
}

bool Http::Connect(const char * hostname, u_short port)
{
	load_time.StartExec();
	Msg("[NoCheatZ] %f : Connecting to %s:%ud ...\n", Plat_FloatTime(), hostname, port);
	m_hostname = hostname;
	m_port = port;
	Resolve(ip);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_port);
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
	{
		Msg("[NoCheatZ] %f : Connected\n", Plat_FloatTime(), hostname, port);
		return true;
	}
#ifdef WIN32
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)(&s), 0, NULL);
	CIFaceManager::GetInstance()->GetIengine()->LogPrint(Helpers::format("[NoCheatZ] /!\\ Unable to connect to master server : %S /!\\\n", s).c_str());
	LocalFree(s);
#endif
	return false;
}

void Http::Get(const char * uri, char* content, size_t max_content_length, size_t* content_length)
{
	Msg("[NoCheatZ] %f : GET http://%s:%u/%s ...\n", Plat_FloatTime(), m_hostname, m_port, uri);
	strcpy_s(m_packet, 2048, Helpers::format("GET http://%s:%d/%s HTTP/1.0\r\nHost: %s\r\nConnection: Close\r\nUser-Agent: " NCZ_PLUGIN_NAME "\r\n\r\n\0", m_hostname, m_port, FormEncode(uri).c_str(), m_hostname).c_str());
	size_t packet_length = Helpers::StrLen(m_packet);
	Send(m_packet, packet_length);
	GetContent(content, max_content_length, content_length);
}

void Http::Post(const char * uri, PostQuery* query, char* content, size_t max_content_length, size_t* content_length)
{
	Msg("[NoCheatZ] %f : Sending POST query to http://%s:%ud/%s ...\n", Plat_FloatTime(), m_hostname, m_port, uri);
	std::string query_string;
	bool another = false;
	for(PostQuery::iterator it = query->begin(); it != query->end(); ++it)
	{
		if(another) query_string.append("&");
		query_string.append((*it).name);
		query_string.append("=");
		query_string.append(FormEncode((*it).value));
		another = true;
	}
	strcpy_s(m_packet, 2048, Helpers::format("POST http://%s:%d/%s HTTP/1.0\r\nHost: %s\r\nConnection: Close\r\nUser-Agent: " NCZ_PLUGIN_NAME "\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\0", m_hostname, m_port, FormEncode(uri).c_str(), m_hostname, content_length, query_string.c_str()).c_str());
	size_t packet_length = Helpers::StrLen(m_packet);
	Send(m_packet, packet_length);
	GetContent(content, max_content_length, content_length);
}

void Http::WriteBinaryContentTo(const char * pathToFile, char* content, size_t content_length)
{
	std::ofstream outfile(pathToFile, std::ios_base::binary | std::ios_base::out);
	if(outfile.is_open())
	{
		outfile.write(content, content_length);
	}
}

void Http::GetContent(char* content, size_t max_content_length, size_t* content_length)
{
	char response[HTTP_BUFFSIZE];
	char header[2048];

	size_t bytes = 0;
	size_t header_end = 0;
	size_t header_w_pos = 0;
	size_t bytesWritten = 0;

	while(1)
	{
		fd_set readfs;

#ifdef GNUC
		/* Got an error with stosd instruction by usinf FD_ZERO
		 Probably because I use -masm=intel for the hooks ... */
		memset(&readfs, 0, sizeof(fd_set));
#else
		FD_ZERO(&readfs);
#endif
		FD_SET(sock, &readfs);
		timeval timeout = {10L,0L};
		int ready = select(sock+1, &readfs, NULL, NULL, &timeout);
		switch(ready)
		{
		case SOCKET_ERROR:
			break;

		case 0:
			break;

		default:
			if(FD_ISSET(sock, &readfs))
			{
				if((bytes = recv(sock, response, sizeof(response), 0)) > 0)
				{
					if(!header_end)
					{
						Msg("[NoCheatZ] %f : Getting content from %s:%u ...\n", Plat_FloatTime(), ip, m_port);
						for(size_t b = 0; b < bytes; ++b, ++header_w_pos) header[header_w_pos] = response[b];
						header_end = FindHeaderEnd(header, 2048);
						if(header_end) // Copy left bytes after the header once we reached the body
						{
							*content_length = GetContentLength(header, header_end);
							Assert(*content_length < max_content_length);
							if(header_end < header_w_pos)
							{
								for(size_t x = header_end; x < header_w_pos && bytesWritten < *content_length; ++x, ++bytesWritten)
								{
									content[bytesWritten] = header[x];
									content[bytesWritten+1] = '\0';
									header[x] = '\0';
								}
							}
						}
					}
					else
					{
						for(size_t x = 0; x < bytes && bytesWritten < *content_length; ++x, ++bytesWritten)
						{
							content[bytesWritten] = response[x];
							content[bytesWritten+1] = '\0';
						}
						Msg("[NoCheatZ] %f : Getting content from %s:%u (%d / %d) ...\n", Plat_FloatTime(), ip, m_port, bytesWritten, *content_length);
					}
				}
				else
				{
					return;
				}
			}
		}
	}
	return;
}

void Http::Resolve(char * ip)
{
	struct addrinfo hints, *res;
	struct in_addr addr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	while(getaddrinfo(m_hostname, NULL, &hints, &res) != 0);
	addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;

	strcpy_s(ip, 24, inet_ntoa(addr));
	Msg("[NoCheatZ] %f : %s resolved to %s.\n", Plat_FloatTime(), m_hostname, ip);
	freeaddrinfo(res);
}

void Http::Send(char* packet, size_t length)
{
	send(sock, packet, length, 0);
	shutdown(sock, 0x01);
	nonblock(sock);
}

void Http::nonblock(int sock)
{
#ifdef WIN32

#else
	int opts;

	if ((opts = fcntl(sock, F_GETFL)) < 0) {
		perror("fcntl(F_GETFL)");
	}

	opts = (opts | O_NONBLOCK);

	if (fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(F_SETFL)");
	}
#endif
}

size_t Http::FindHeaderEnd(char* buf, size_t max_length)
{
	for(size_t header_end = 0; header_end < max_length; ++header_end)
	{
		if(Helpers::bBytesEq(buf, "\r\n\r\n", header_end, 4))
		{
			header_end += 3;
			return header_end;
		}
	}
	return 0;
}

size_t Http::GetContentLength(char* buf, size_t header_end)
{
	size_t start = 0, length = 0, content_length = 0, c = 0;
	char content_length_str[24];

	for(size_t x = 0; x < header_end; ++x)
	{
		if(Helpers::bBytesEq(buf, "Content-Length: ", x, 16))
		{
			start = x + 16;
			break;
		}
	}
	if(!start) return 0;
	for(size_t x = start; x < header_end; ++x)
	{
		if(Helpers::bBytesEq(buf, "\r\n", x, 2))
		{
			length = x - start;
			break;
		}
	}
	if(!length) return 0;
	for(; c < length; ++c)
	{
		content_length_str[c] = buf[c + start];
	}
	content_length_str[c] = '\0';
	content_length = atoi(content_length_str);
	return content_length;
}

bool Http::isReserved(char c)
{
	if( ( (c >= '0' && c <= '9') || ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) )) return false;
	return true;
}

char Http::hex_encode(char c)
{
	char hex[] = "0123456789abcdef";
	return hex[c & 15];
}

std::string Http::FormEncode(const std::string &src)
{
	std::string r_src;
	for(std::string::const_iterator it = src.begin(); it != src.end(); ++it)
	{
		if(isReserved(*it))
		{
			if(*it == ' ') r_src.push_back('+');
			else
			{
				r_src.push_back('%');
				r_src.push_back(hex_encode(*it >> 4));
				r_src.push_back(hex_encode(*it));
			}
		}
		else
		{
			r_src.push_back(*it);
		}
	}
	return r_src;
}
