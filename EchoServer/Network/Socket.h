#pragma once

#include <string>
#include <winsock2.h>
#include "Kstring.h"

#pragma comment(lib, "ws2_32.lib")

//AF_INET(internetwork: UDP, TCP, etc.)
#define IPv4 AF_INET

struct socketCloseOpt {
	//1 = blocking shutdown in timeout, 0 = non-blocking shutdown(don't use timeout)
	u_short isBlocking;
	u_short timeout;
};

using ServerLogic = void(*)(char* message);
using socketCloseOption = socketCloseOpt;

class Socket
{
public:
	//callback function
	ServerLogic serverLogic;

private:
	SOCKET mainSocket;
	SOCKET clientSocket;

	char* message;
	int messageSize;
	std::string className;

public:
	Socket();
	Socket(int messageSize);
	~Socket();

	void CloseSocket(SOCKET socketClose, bool bIsForce = false);

	bool ServerOpen(char* ip, int port, ServerLogic logic);
	bool ServerOpen(char* ip, int port);
	bool Connect(char* ip, int port);

private:
	void Initialize();
	bool InitSocket();
	bool callSocket();
	bool CreateTCPSocket();
	bool Bind(SOCKADDR_IN &socketProperty);
	bool Listen(int allowQueue);

	bool InitServer(char* ip, int port);
	bool Accept();
	bool Receive(SOCKET socket);
	bool Send(SOCKET socket);
	void PrintError(const char* methodName, const char* reason, const int errorCode);
	void PrintError(const char* methodName, const char* reason);
};
