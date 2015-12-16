#include "CSocket.h"
#include <string.h>

int main(int argc, char *argv[])
{
	// const char *ip = "192.168.149.79";
	// CCliSocket cliSocket;
	// cliSocket.Connect(ip, 9999);
	// std::cout << "client connect server...\n";
	// if(!cliSocket.Send("ABCDEFGHIJ", 10))
	// {
	// 	std::cout <<"send err\n";
	// }
	// char recvbuf[256];
	// memset(recvbuf,0, sizeof recvbuf);
	// if(!cliSocket.Recv(recvbuf, 16))
	// {
	// 	std::cout <<"recv err\n";
	// }

	// std::cout << recvbuf << std::endl;

	const char *ip = "192.168.149.79";
	CSocket *socket = new CSocket();
	socket->setAddr(ip, 9999);
	socket->Connect();
	std::cout << "client connect server...\n";
	if(!socket->Send("ABCDEFGHIJ", 10))
	{
		std::cout <<"send err\n";
	}
	char recvbuf[256];
	memset(recvbuf,0, sizeof recvbuf);
	if(!socket->Recv(recvbuf, 16, 0))
	{
		std::cout <<"recv err\n";
	}

	std::cout << recvbuf << std::endl;
	getchar();
    return 0;
}
