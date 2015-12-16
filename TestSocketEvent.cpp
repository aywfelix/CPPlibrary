#include "CSocketEvent.h"

int main(int argc, char *argv[])
{
	CServSocket servSocket;
	CSocketEvent socketEvent;
	char buf[256];
	memset(buf,0, sizeof(buf));
	if(!servSocket.Listen(9999))
	{
		std::cout << "listen err\n";		
	}
	for(;;)
	{
	    int ret = socketEvent.getEvents(1000);
		for(int i=0; i< ret; ++i)
		{
			if(socketEvent.events[i].data.fd == servSocket.getSockfd())
			{
				CSocket *socket = NULL;
				if((socket = servSocket.Accept()) == NULL)
				{
					std::cout << "accpt error\n";
					break;
				}
				socketEvent.eventAdd(socket, true, true);
				//delete socket;
			}
			else if(socketEvent.events[i].events & EPOLLIN)
			{
				std::cout << "event trigger ...\n";
				int sockfd = socketEvent.events[i].data.fd;
				CSocket *socket = new CSocket();
				socket->setUp(sockfd, NULL);
				socket->Recv(buf, 10, 0);
				std::cout << buf << "\n";
				socketEvent.eventSet(socket, true, true);
				//delete socket;
			}
		}
	}
    return 0;
}
