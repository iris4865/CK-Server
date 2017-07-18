#include "Socket.h"

//inet_ntop()
#include <WS2tcpip.h>
#include <iostream>
using namespace std;

Socket::Socket()
{
	this->messageSize = 1024;
	this->Initialize();
}

Socket::Socket(int messageSize)
{
	this->messageSize = messageSize;
	this->Initialize();
}

void Socket::Initialize()
{
	serverLogic = nullptr;
	className = "Socket";
	mainSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;

	message = new char[messageSize];
	ZeroMemory(message, messageSize);
}

Socket::~Socket(void)
{
	delete[] message;
	WSACleanup();
}

bool Socket::InitSocket()
{
	if (callSocket() && CreateTCPSocket())
		return true;

	return false;
}

bool Socket::callSocket()
{
	//set socket version and check available socket
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

void Socket::CloseSocket(SOCKET socketClose, bool forceClose)
{
	socketCloseOption option;
	if (forceClose)
		option.isBlocking = 1;
	option.timeout = 0;

	shutdown(socketClose, SD_BOTH);
	setsockopt(socketClose, SOL_SOCKET, SO_LINGER, (char *)&option, sizeof(option));
	closesocket(socketClose);

	socketClose = INVALID_SOCKET;
}

bool Socket::ServerOpen(char* ip, int port, ServerLogic logic)
{
	this->serverLogic = logic;
	return this->ServerOpen(ip, port);
}

bool Socket::ServerOpen(char* ip, int port)
{
	if (serverLogic == nullptr)
	{
		PrintError("ServerOpen", "serverLogic Function is Null", WSAGetLastError());
		return false;
	}
	cout << "Server Start" << endl;
	if (!this->InitSocket())
		return false;
	this->InitServer(ip, port);

	bool isClient = false;
	while (true)
	{
		if (!isClient)
			isClient = Accept();

		isClient = this->Receive(clientSocket);
		this->serverLogic(message);
		isClient = this->Send(clientSocket);
	}

	CloseSocket(clientSocket);
	CloseSocket(mainSocket);

	cout << "Server End" << endl;
	return true;
}

bool Socket::Connect(char * ip, int port)
{
	if (!this->InitSocket())
		return false;

	//return true;
	/**/
	SOCKADDR_IN		serverProperty;

	serverProperty.sin_family = IPv4;
	serverProperty.sin_port = htons(port);
	inet_pton(IPv4, ip, &serverProperty.sin_addr.s_addr);

	if (connect(mainSocket, (sockaddr*)&serverProperty, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		PrintError("Connect", "Fail to connect()", WSAGetLastError());
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
		cout << "send bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;

		this->Receive(mainSocket);
		cout << "receive bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;
	}

	CloseSocket(mainSocket);
	cout << "Client End" << endl;

	return true;
}

//htons(2byte) = LittleEdian -> BigEdian
//htonl(4byte) = LittleEdian -> BigEdian
bool Socket::InitServer(char* ip, int port)
{
	//sockaddr_storage 알아보기(IPv4 <=> IPv6)
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
		PrintError("StartServer", "accept()함수 실패", WSAGetLastError());
		return false;
	}

	char buffer[20];
	cout << "클라이언트 접속 : IP(" << inet_ntop(IPv4, &clientProperty.sin_addr, buffer, sizeof(buffer)) << ") SOCKET(" << clientSocket << ")" << endl;
	return true;
}

bool Socket::Receive(SOCKET socket)
{
	int receiveSize = recv(socket, message, messageSize, 0);
	if (receiveSize == 0)
	{
		cout << "연결이 종료되었습니다." << endl;
		CloseSocket(socket);

		return false;
	}
	else if (receiveSize == -1)
	{
		PrintError("Receive", "recv()함수 실패", WSAGetLastError());
		CloseSocket(socket);

		return false;
	}

	message[receiveSize] = NULL;
	return true;
}

bool Socket::Send(SOCKET socket)
{
	int sendSize = send(socket, message, (int)strlen(message), 0);
	if (sendSize == -1)
	{
		PrintError("Send", "send()함수 실패", WSAGetLastError());
		CloseSocket(socket);

		return false;
	}
	return true;
}

void Socket::PrintError(const char* methodName, const char * reason, const int errorCode)
{
	cout << "[ERROR: " << errorCode << "] ";
	this->PrintError(methodName, reason);
}

void Socket::PrintError(const char* methodName, const char * reason)
{
	cout << className << "::" << methodName << "Reason: " << reason << endl;
}