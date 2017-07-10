#pragma once

#include <string>
#include <winsock2.h>
#include "Kstring.h"

#pragma comment(lib, "ws2_32.lib")

#define IPv4 AF_INET

class Socket
{
public:
	Socket(void);
	~Socket(void);

	bool InitSocket();

	//소켓의 연결을 종료 시킨다.
	void CloseSocket(SOCKET socketClose, bool bIsForce = false);

	//------서버용 함수-------//
	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
	bool InitServer(int port);
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool ServerStart();

	//------클라이언트용 함수-------//
	//IP,Port에 해당하는 서버로 접속을 한다.
	bool Connect(char* ip, int port);

private:
	bool callSocket();
	bool CreateTCPSocket();
	bool Bind(SOCKADDR_IN &socketProperty);
	bool Listen(int allowQueue);
	bool Accept();
	bool Receive(SOCKET socket);
	bool Send(SOCKET socket);
	void PrintError(const char* methodName, const char* reason, const int errorCode);

private:
	//클라이언트에서는 서버에 연결하기 위한 소켓
	//서버에서는 클라이언트의 접속요청을 기다리는 소켓
	SOCKET mainSocket;
	SOCKET clientSocket;

	char message[1024];
	std::string className;
};
