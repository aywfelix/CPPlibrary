#include "CSocket.h"
#include <string.h>

int main(int argc, char *argv[])
{
    CServSocket servSocket;
	CSocket *socket;
	if(!servSocket.Listen(9999))
	{
		std::cout << "listen err\n";		
	}
	for(;;)
	{
		if(!(socket = servSocket.Accept()))
		{
			std::cout << "accept err\n";
			return -1;
		}
		if(socket->getSockfd()> 0)
		{
			std::cout <<"client ("<< socket->getCliIp() << ")\n";		   
			char recvbuf[11];
			memset(recvbuf, 0, sizeof recvbuf);
			socket->Recv(recvbuf, 10, 0);
			std::cout << recvbuf<<std::endl;
			char sendbuf[256];
			memset(sendbuf,0, sizeof sendbuf);
			strcpy(sendbuf, "come from server");
			if(!socket->Send(sendbuf, 16))
			{
				std::cout << "send err\n";
			}
		}
	}
    return 0;
}
