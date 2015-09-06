#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>

#ifdef WIN32
#include "Misc/include_windows_headers.h"
#else
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>
#include <memory.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close
#include <netdb.h> // gethostbyname
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
#define SD_BOTH SHUT_RDWR
typedef int SOCKET;
typedef sockaddr_in SOCKADDR_IN; // typedef struct sockaddr_in SOCKADDR_IN;
typedef sockaddr SOCKADDR;
typedef in_addr IN_ADDR;
#define SOCKET_ERROR_CODE errno

#endif

#include "Preprocessors.h"
#include "Misc/Helpers.h"
#include "Misc/Metrics.h"

#undef h_addr

#ifndef HTTP_H
#define HTTP_H

#define HTTP_BUFFSIZE 512

typedef struct QueryValue
{
	const char * name;
	std::string value;
} QueryValueT;

typedef std::list<QueryValueT> PostQuery;

class Http
{
public:
	Http();

	~Http();

	void End();

	bool Connect(const char * hostname, u_short port);

	void Get(const char * uri, char* content, size_t max_content_length, size_t* content_length);

	void Post(const char * uri, PostQuery* query, char* content, size_t max_content_length, size_t* content_length);

	void WriteBinaryContentTo(const char * pathToFile, char* content, size_t content_length);
	void WriteContentTo(const char * pathToFile, char* content, size_t content_length);

private:
	void GetContent(char* content, size_t max_content_length, size_t* content_length);

	void Resolve(char * ip);

	void Send(char* packet, size_t length);

	void nonblock(int sock);

	size_t FindHeaderEnd(char* buf, size_t max_length);

	size_t GetContentLength(char* buf, size_t header_end);

	bool isReserved(char c);

	char hex_encode(char c);

	std::string FormEncode(const std::string &src);

	in_addr addr;
	hostent* host;
	SOCKET sock;
	SOCKADDR_IN sin;
	char ip[24];
	char m_packet[2048];

	const char * m_hostname;
	u_short m_port;

	Metrics load_time;
};

#endif
 