#include "Socket.h"
#include "Kstring.h"

#include <windows.h>
#include <process.h>
#include <iostream>

using namespace std;

#define SERVER_IP		"127.0.0.1"
#define SERVER_PORT		8000


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "usage : client.exe [ option : [ /server ][ /client ] ]" << endl;
		return 0;
	}

	Socket socket;

	if (Kstring::equal(argv[1], "/server"))
	{
		socket.InitSocket();
		socket.InitServer(SERVER_PORT);
		socket.ServerStart();
	}
	else if (Kstring::equal(argv[1], "/client"))
	{
		socket.InitSocket();
		socket.Connect(SERVER_IP, SERVER_PORT);
	}
	else
		cout << "usage : client.exe option : [ /server ][ /client ]" << endl;

	return 0;
}
