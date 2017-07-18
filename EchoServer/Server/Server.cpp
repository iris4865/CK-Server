#include "../Network/Socket.h"
#include "../Network/KString.h"

#include <windows.h>
#include <process.h>
#include <iostream>

using namespace std;
void EchoLogic(char* message);

#define IP "0.0.0.0"
#define PORT 8879

int main(int argc, char* argv[])
{
	cout << "count: " << argc << endl;
	if (argc < 2)
	{
		cout << "allowIP: All, port: 8879" << endl;
		Socket socket;
		socket.ServerOpen(IP, PORT, EchoLogic);
		return 0;
	}

	Socket socket;

	if (Kstring::equal(argv[1], "/server"))
	{
//		socket.InitSocket();
		socket.ServerOpen("0.0.0.0", 8080, EchoLogic);
	}
	else
		cout << "usage : client.exe option : [IP][PORT]" << endl;

	return 0;
}

void EchoLogic(char* message)
{
	cout << "receive bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;
	cout << "send bytes[" << strlen(message) << "] , message : [" << message << "]" << endl;
}