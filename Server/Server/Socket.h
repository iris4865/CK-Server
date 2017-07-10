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

	//������ ������ ���� ��Ų��.
	void CloseSocket(SOCKET socketClose, bool bIsForce = false);

	//------������ �Լ�-------//
	//������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� �� ������ ����ϴ� �Լ�
	bool InitServer(int port);
	//���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool ServerStart();

	//------Ŭ���̾�Ʈ�� �Լ�-------//
	//IP,Port�� �ش��ϴ� ������ ������ �Ѵ�.
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
	//Ŭ���̾�Ʈ������ ������ �����ϱ� ���� ����
	//���������� Ŭ���̾�Ʈ�� ���ӿ�û�� ��ٸ��� ����
	SOCKET mainSocket;
	SOCKET clientSocket;

	char message[1024];
	std::string className;
};
