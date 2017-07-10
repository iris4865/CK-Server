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
	//윈속의 사용을 끝낸다.
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
	// SO_DONTLINGER로 설정
	struct linger stLinger = { 0, 0 };

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
	// 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (bIsForce)
		stLinger.l_onoff = 1;

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(socketClose, SD_BOTH);
	//소켓 옵션을 설정한다.
	setsockopt(socketClose, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger));
	//소켓 연결을 종료 시킨다. 
	closesocket(socketClose);

	socketClose = INVALID_SOCKET;
}

//htons(2byte) = LittleEdian -> BigEdian
//htonl(4byte) = LittleEdian -> BigEdian
bool Socket::InitServer(int port)
{
	//sockaddr_storage 알아보기
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

bool Socket::ServerStart()
{
	cout << "Server Start" << endl;
	bool isClient = false;
	while (true)
	{
		if (!isClient)
			isClient = Accept();

		isClient = this->Receive(clientSocket);
		cout << "메세지 수신 : 수신 bytes[" << strlen(message) << "] , 내용 : [" << message << "]" << endl;

		isClient = this->Send(clientSocket);
		cout << "메세지 송신 : 송신 bytes[" << strlen(message) << "] , 내용 : [" << message << "]" << endl;
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
	int sendSize = send(socket, message, strlen(message), 0);
	if (sendSize == -1)
	{
		PrintError("Send", "send()함수 실패", WSAGetLastError());
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
		PrintError("Connect", "connect()함수 실패", WSAGetLastError());
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
		cout << "메세지 송신 : 송신 bytes[" << strlen(message) << "] , 내용 : [" << message << "]" << endl;

		this->Receive(mainSocket);
		cout << "메세지 수신 : 수신 bytes[" << strlen(message) << "] , 내용 : [" << message << "]" << endl;
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