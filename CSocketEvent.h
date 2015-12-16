#include <sys/epoll.h>  
#include <iostream>
#include <cstdio>
#include "CSocket.h"

const int MaxEvents = 256;
const int EpollSize = 500;

class CSocketEvent
{
public:
    CSocketEvent();
    virtual ~CSocketEvent();
    bool eventSet(CSocket *socket, bool readable, bool writeable);
	bool eventAdd(CSocket *socket, bool readable, bool writeable);
	bool eventDel(CSocket *socket);
	int getEvents(int timeout);
	struct epoll_event events[MaxEvents];
private:
	int epfd;
	CSocketEvent(const CSocketEvent&);
	CSocketEvent& operator=(const CSocketEvent&);
};

CSocketEvent::CSocketEvent()
{
	if((epfd = epoll_create(EpollSize)) < 0)
	{
		std::cout << "create epoll handle err\n";
		return;
	}
	memset(&events, 0, sizeof(struct epoll_event)*MaxEvents);
}

CSocketEvent::~CSocketEvent()
{
	if(epfd > 0)
	{
		close(epfd);
		epfd = -1;
	}
}

bool CSocketEvent::eventSet(CSocket *socket, bool readable, bool writeable)
{
	int	sockfd = socket->getSockfd();
	if(sockfd < 0)
	{
		std::cout << "eventSet the socket fd err\n";
		return false;
	}
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events |= EPOLLET; //ET modle as default	
	if(readable)
		ev.events |= EPOLLIN;
	if(writeable)
		ev.events |= EPOLLOUT;
	//ev.data.ptr = (void*)sockfd;
	return ( 0 == epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev));
}

bool CSocketEvent::eventAdd(CSocket *socket, bool readable, bool writeable)
{
	int sockfd = -1;
	if(socket->setNonBlock(true))
		sockfd = socket->getSockfd();
	if(sockfd < 0)
	{
		std::cout << "eventAdd the socket fd err\n";
		return false;
	}
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events |= EPOLLET; //ET modle as default
	if(readable)
		ev.events |= EPOLLIN;
	if(writeable)
		ev.events |= EPOLLOUT;
	//ev.data.ptr = sockfd;
	return ( 0 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev));
}

bool CSocketEvent::eventDel(CSocket *socket)
{
	int sockfd = socket->getSockfd();
	if(sockfd < 0)
	{
		std::cout << "eventDel the socket fd err\n";
		return false;
	}
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = 0;
	return (0==epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev));
}

int CSocketEvent::getEvents(int timeout)
{
	int ret = epoll_wait(epfd, events, MaxEvents, timeout);
	return ret;
}

