#include <WS2tcpip.h>
#include "Socket.h"

#include <iostream>
using namespace std;

Socket::Socket(void)
{
	className = "Socket";
	mainSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;

	ZeroMemory(message, 1024);
}

Socket::~Socket(void)
{
	//������ ����� ������.
	WSACleanup();
}

bool Socket::InitSocket()
{
	if (!callSocket())
		return false;
	if (!CreateTCPSocket())
		return false;
	return true;
}

bool Socket::callSocket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		PrintError("callSocket", "Fail to WSAStartup()", WSAGetLastError());
		return false;
	}
	return true;
}

bool Socket::CreateTCPSocket()
{
	mainSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET)
	{
		PrintError("CreateTCPSocket", "Fail to socket()", WSAGetLastError());
		return false;
	}
	return true;
}

void Socket::CloseSocket(SOCKET socketClose, bool bIsForce)
{
	// SO_DONTLINGER�� ����
	struct linger stLinger = { 0, 0 };

	// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ�
	// ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if (bIsForce)
		stLinger.l_onoff = 1;

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown(socketClose, SD_BOTH);
	//���� �ɼ��� �����Ѵ�.
	setsockopt(socketClose, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger));
	//���� ������ ���� ��Ų��. 
	closesocket(socketClose);

	socketClose = INVALID_SOCKET;
}

//htons(2byte) = LittleEdian -> BigEdian
//htonl(4byte) = LittleEdian -> BigEdian
bool Socket::InitServer(int port)
{
	//sockaddr_storage �˾ƺ���
	SOCKADDR_IN socketProperty;

	socketProperty.sin_family = IPv4;
	socketProperty.sin_port = htons(port);
	socketProperty.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (this->Bind(socketProperty) && this->Listen(5))
		return true;
	return false;
}

bool Socket::Bind(SOCKADDR_IN &socketProperty)
{
	if (bind(mainSocket, (SOCKADDR*)&socketProperty, sizeof(SOCKADDR_IN)))
	{
		PrintError("Bind", "Fail to bind()", WSAGetLastError());
		return false;
	}
	return true;
}

bool Socket::Listen(int allowQueue)
{
	if (listen(mainSocket, allowQueue))
	{
		PrintError("Listen", "Fail to listen()", WSAGetLastError());
		return false;
	}
	return true;
}

bool Socket::Accept()
{
	SOCKADDR_IN clientProperty;
	int propertySize = sizeof(SOCKADDR_IN);

	clientSocket = accept(mainSocket, (SOCKADDR*)&clientProperty, &propertySize);
	if (clientSocket == INVALID_SOCKET)
	{
		PrintError("StartServer", "accept()�Լ� ����", WSAGetLastError());
		return false;
	}

	char buffer[20];
	cout << "Ŭ���̾�Ʈ ���� : IP(" << inet_ntop(IPv4, &clientProperty.sin_addr, buffer, sizeof(buffer)) << ") SOCKET(" << clientSocket << ")" << endl;
	return true;
}

bool Socket::ServerStart()
{
	cout << "Server Start" << endl;
	bool isClient = false;
	while (true)
	{
		if (!isClient)
			isClient = Accept();

		isClient = this->Receive(clientSocket);
		cout << "�޼��� ���� : ���� bytes[" << strlen(message) << "] , ���� : [" << message << "]" << endl;

		isClient = this->Send(clientSocket);
		cout << "�޼��� �۽� : �۽� bytes[" << strlen(message) << "] , ���� : [" << message << "]" << endl;
	}

	CloseSocket(clientSocket);
	CloseSocket(mainSocket);

	cout << "Server End"<< endl;
	return true;
}

bool Socket::Receive(SOCKET socket)
{
	int receiveSize = recv(socket, message, 1024, 0);
	if (receiveSize == 0)
	{
		cout << "������ ����Ǿ����ϴ�." << endl;
		CloseSocket(socket);

		return false;
	}
	else if (receiveSize == -1)
	{
		PrintError("Receive", "recv()�Լ� ����", WSAGetLastError());
		CloseSocket(socket);

		return false;
	}

	message[receiveSize] = NULL;
	return true;
}

bool Socket::Send(SOCKET socket)
{
	int sendSize = send(socket, message, strlen(message), 0);
	if (sendSize == -1)
	{
		PrintError("Send", "send()�Լ� ����", WSAGetLastError());
		CloseSocket(socket);

		return false;
	}
	return true;
}

bool Socket::Connect(char* ip, int port)
{
	SOCKADDR_IN		serverProperty;

	serverProperty.sin_family = AF_INET;
	serverProperty.sin_port = htons(port);
	inet_pton(IPv4, ip, &serverProperty.sin_addr.s_addr);

	if (connect(mainSocket, (sockaddr*)&serverProperty, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		PrintError("Connect", "connect()�Լ� ����", WSAGetLastError());
		return false;
	}

	cout << "connection success" << endl;
	while (true)
	{
		cout << ">>";
		cin >> message;
		if (Kstring::equal(message, "quit"))
			break;

		this->Send(mainSocket);
		cout << "�޼��� �۽� : �۽� bytes[" << strlen(message) << "] , ���� : [" << message << "]" << endl;

		this->Receive(mainSocket);
		cout << "�޼��� ���� : ���� bytes[" << strlen(message) << "] , ���� : [" << message << "]" << endl;
	}

	CloseSocket(mainSocket);
	cout << "Client End" << endl;

	return true;
}
void Socket::PrintError(const char* methodName, const char * reason, const int errorCode)
{
	cout << "[ERROR: " << errorCode << "] " << className << "::" << methodName << "Reason: " << reason << endl;
}
/**/