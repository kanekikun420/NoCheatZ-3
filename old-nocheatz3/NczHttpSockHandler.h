#ifdef _WIN32

#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#define SOCKET_ERROR_CODE WSAGetLastError()

#else

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
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fcntl.h>

#ifndef __NCZ_HTTP_SOCK_HANDLER_CLASS
#define __NCZ_HTTP_SOCK_HANDLER_CLASS

class NczHttpSockHandler
{
public:
		NczHttpSockHandler() : crlf("\r\n")
		{
#ifdef _WIN32
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2,2),&wsadata);
#endif // _WIN32

		};

		~NczHttpSockHandler()
		{
		};

		void End()
		{
				closesocket(sock);
#ifdef _WIN32
		WSACleanup();
#endif // _WIN32
		};

		std::string Resolve(char *host_name);

		bool Connect(const char *host_name, u_short port);

		std::string MakeGetPacket(std::string file);

		std::string MakePostPacket(std::string file, std::string content);

		void Send(std::string packet);

		void SendGet(std::string file);

		void SendPost(std::string file, std::string content);

		std::string GetContent();

		void nonblock(int sock);

		void WriteBinaryContentTo(std::string pathEfilew);

private:
		in_addr addr;
		hostent* host;
		SOCKET sock;
		SOCKADDR_IN sin;
		std::string sip;
		//std::string shost_name;
		std::string crlf;
};

#endif // __NCZ_HTTP_SOCK_HANDLER_CLASS
 