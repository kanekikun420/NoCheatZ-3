#include "NczHttpSockHandler.h"
#include <string>
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>
#include <memory.h> 

#ifndef WIN32
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#undef h_addr
#include "NczHttpSockHandler.h"
#include <fcntl.h>

std::string shost_name( "" );
std::string g_inetaddr( "" );

std::string NczHttpSockHandler::Resolve( char *hostname )
{
	if(!g_inetaddr.empty())
		return g_inetaddr;

	 struct addrinfo hints, *res;
	 struct in_addr addr;

	 memset( &hints, 0, sizeof( hints ) );
	 hints.ai_socktype = SOCK_STREAM;
	 hints.ai_family = AF_INET;

	 while( getaddrinfo(hostname, NULL, &hints, &res) != 0 );
	 addr.s_addr = (( struct sockaddr_in * )( res->ai_addr ))->sin_addr.s_addr;
	 shost_name = hostname;

	 g_inetaddr = inet_ntoa( addr );

	 freeaddrinfo( res );

	 return g_inetaddr;
}

bool NczHttpSockHandler::Connect(const char *host_name, u_short port)
{
	sip = this->Resolve((char *)host_name);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_addr.s_addr = inet_addr(sip.c_str());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
	{
		return true;
	}
	return false;
}

std::string NczHttpSockHandler::MakeGetPacket(std::string file)
{
	std::ostringstream buf;
	buf << "GET http://" << shost_name << file << " HTTP/1.0" << crlf
	<< "Host: " << shost_name << crlf
		<< "Connection: Keep-Alive" << crlf
		<< "User-Agent: nocheatz3" << crlf
		<< crlf;
	return buf.str();
}

std::string NczHttpSockHandler::MakePostPacket(std::string file, std::string content)
{
	std::ostringstream buf;
	buf << "POST http://" << shost_name << file << " HTTP/1.0" << crlf
	<< "Host: " << shost_name << crlf
		<< "Connection: Close" << crlf
		<< "User-Agent: nocheatz3" << crlf
		<< "Content-type: application/x-www-form-urlencoded" << crlf
		<< "Content-Length: " << content.length() << crlf
		<< crlf
		<< content << crlf
		<< crlf;
	return buf.str();
}

void NczHttpSockHandler::Send(std::string packet)
{
	send(sock, packet.c_str(), packet.length()+1, 0);
	shutdown(sock, 0x01);
	nonblock(sock);
}

void NczHttpSockHandler::SendGet(std::string file)
{
	this->Send(this->MakeGetPacket(file));
}

void NczHttpSockHandler::SendPost(std::string file, std::string content)
{
	this->Send(this->MakePostPacket(file, content));
}

void NczHttpSockHandler::nonblock(int sock)
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


#define buffsize 1024

std::string NczHttpSockHandler::GetContent()
{
	char response[buffsize];
	int bytes;
	int package = 0;
	int start = 0;
	int bytesWritten = 0;
	int cl = 0;

	std::ostringstream resp;
	int ready = 0;

	while(1)
	{
		fd_set readfs;
		FD_ZERO(&readfs);
		FD_SET(sock, &readfs);
		timeval timeout = {10L,0L};
		ready = select(sock+1, &readfs, NULL, NULL, &timeout);
		switch(ready)
		{
		case SOCKET_ERROR:
			return "";
			break;

		case 0:
			break;

		default:
			if(FD_ISSET(sock,&readfs))
			{
				if((bytes = recv(sock, response, sizeof(response), 0)) > 0)
				{
					if (package == 0)
					{
						std::string sort = response;
						start = sort.find("\r\n\r\n");

						int posA = sort.find("Content-Length: ")+16;
						int posB = sort.find("\r\n", posA)-posA;
						std::string cl_s(sort,posA,posB);
						cl = atoi(cl_s.c_str());

						size_t dataStart = start + strlen("\r\n\r\n");
						// Get the length of the data in this packet                      
						size_t dataLen = bytes - dataStart;
						// Write the data to the string  
						resp.write(&response[dataStart], dataLen);
						bytesWritten = dataLen;
						package++;
					}
					else
					{
						resp.write(response, bytes);
						bytesWritten += bytes;
					}
				}
				else
				{
					return resp.str();
				}
			}
		}
	}
	return resp.str();
}

void NczHttpSockHandler::WriteBinaryContentTo(std::string pathEfilew)
{
	char response[buffsize];
	int bytes;
	int package = 0;
	int start = 0;
	int bytesWritten = 0;
	int cl = 0;

	std::ostringstream resp;
	int ready = 0;
	std::ofstream outfile(pathEfilew.c_str(), std::ios_base::binary | std::ios_base::out);
	while(1)
	{
		fd_set readfs;
		FD_ZERO(&readfs);
		FD_SET(sock, &readfs);
		timeval timeout = {10L,0L};
		ready = select(sock+1, &readfs, NULL, NULL, &timeout);
		switch(ready)
		{
		case SOCKET_ERROR:
			return;
			break;

		case 0:
			break;

		default:
			if(FD_ISSET(sock,&readfs))
			{
				if((bytes = recv(sock, response, sizeof(response), 0)) > 0)
				{
					if (package == 0)
					{
						std::string sort = response;
						start = sort.find("\r\n\r\n");

						int posA = sort.find("Content-Length: ")+16;
						int posB = sort.find("\r\n", posA)-posA;
						std::string cl_s(sort,posA,posB);
						cl = atoi(cl_s.c_str());

						size_t dataStart = start + strlen("\r\n\r\n");
						// Get the length of the data in this packet                      
						size_t dataLen = bytes - dataStart;
						// Write the data to the file    
						outfile.write(&response[dataStart], dataLen);
						bytesWritten = dataLen;
						package++;
					}
					else
					{
						outfile.write(response, bytes);
						bytesWritten += bytes;
					}
				}
				else
				{
					outfile.close();
					return;
				}
			}
		}
	}
	outfile.close();
	return;
}
