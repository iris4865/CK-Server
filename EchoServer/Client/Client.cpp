#include "../Network/Socket.h"
#include "../Network/KString.h"

#include <windows.h>
#include <process.h>
#include <iostream>

using namespace std;

#define IP "127.0.0.1"
#define PORT 8879


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "allowIP: All, port: 8879" << endl;
		Socket socket;
		socket.Connect(IP, PORT);
		return 0;
	}

	Socket socket;

	if (Kstring::equal(argv[1], "/client"))
	{
//		socket.InitSocket();
		socket.Connect(IP, PORT);
	}
	else
		cout << "usage : client.exe option : [ /server ][ /client ]" << endl;

	return 0;
}

void EchoLogic(char* message)
{
	cout << "send bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;
	cout << "receive bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;
}
