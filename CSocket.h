#include <iostream>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/tcp.h>

class CSocket
{
public:
    CSocket();
    virtual ~CSocket();
	std::string  getLocalAddr();
	std::string getCliIp();
	int getSockfd() { return sockfd;}
	bool Close();
	bool Connect();
	bool Send(char* sendbuf, int len);
	bool Recv(char* recvbuf, int len, int timeout);

	bool setSoLinger(bool dolinger, int seconds);
	bool setIntOptions(int option, int value);
	bool setTimeout(int option, int milliseconds);
	bool setNonBlock(bool isnonsocket);
	bool setNoDelay(bool nodelay);

	bool setUp(int sockfd, struct sockaddr_in* addr);
	bool setAddr(const char* addr, int port);
protected:
	int sockfd;
	char ip[20];
	struct sockaddr_in sockAddr;	
	CSocket(const CSocket&);
	CSocket& operator=(const CSocket&);
};

CSocket::CSocket()
{
	sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd <= 0)
	{
		std::cout << "create socket err\n";
	}
	memset(ip, 0, sizeof(ip));
}

CSocket::~CSocket()
{
	Close();
}

bool CSocket::Close()
{
	if(sockfd > 0)
		close(sockfd);
	else
		return false;
	sockfd = -1;
	return true;
}

std::string  CSocket::getLocalAddr()
{
	char ipaddr[20]={'\0'};
	const char* shellstr = "ifconfig | sed -n '2p' | awk -F'[ :]+' '{printf $4}'";  
    FILE *fp = popen(shellstr, "r");
	fread(ipaddr, sizeof(char), sizeof(ipaddr), fp);
	if(ipaddr)
	{
		strcpy(ip, ipaddr);
	}
	pclose(fp);
	return std::string(ip);
}

std::string CSocket::getCliIp()
{
	char cliIp[16];
	struct sockaddr_in cliAddr;
	socklen_t size = sizeof(cliAddr);
	if(getpeername(sockfd, (sockaddr*)&cliAddr, &size))
	{
		strcpy(cliIp, "0.0.0.0");
	}
	else
	{
        sprintf(cliIp, "%d.%d.%d.%d", ((unsigned char*)&cliAddr.sin_addr)[0],
				((unsigned char*)&cliAddr.sin_addr)[1],
				((unsigned char*)&cliAddr.sin_addr)[2],
				((unsigned char*)&cliAddr.sin_addr)[3]);
	}
	return cliIp;
}

bool CSocket::setIntOptions(int option, int value)
{
	bool res = false;
	if(sockfd)
	{
		res = (setsockopt(sockfd, SOL_SOCKET, option, (const void*)&value, sizeof(value)) == 0);
	}
	return res;
}

bool CSocket::setSoLinger(bool dolinger, int seconds)
{
	bool res = false;
	if(sockfd)
	{
		struct linger ling;
		ling.l_onoff = dolinger?1:0;
		ling.l_linger = seconds;
		res = (setsockopt(sockfd,SOL_SOCKET, SO_LINGER, (const char *)&ling, sizeof(struct linger)) == 0);
		res = true;		
	}
	return res;
}

bool CSocket::setTimeout(int option, int milliseconds)
{
	bool res = false;
	if(sockfd)
	{
		struct timeval timeout;
		timeout.tv_sec = milliseconds/1000;
		timeout.tv_usec = (milliseconds%1000)*1000000;
		res = (setsockopt(sockfd, SOL_SOCKET, option, (const void*)&timeout, sizeof(timeout)) == 0);
		res = true;		
	}
	return res;
}

bool CSocket::setNonBlock(bool isnonsocket)
{
	bool res = false;
	if(sockfd)
	{
		int oldfd = fcntl(sockfd, F_GETFL);
		res = (fcntl(sockfd, F_SETFL, oldfd | O_NONBLOCK) <0);
		res = true;		
	}
	return res;
}

bool CSocket::setNoDelay(bool nodelay)
{
	bool res = false;
	if(sockfd)
	{
		int ndelay = nodelay?1:0;
		res = (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,(const void*)&ndelay, sizeof(ndelay)) == 0);
		res =true;		
	}
	return res;
}

bool CSocket::Send(char *sendbuf, int len)
{
	if(sockfd <0 || sendbuf==NULL || len < 0)
		return false;
	int dataleft = len, total = 0, ret =0;
	for(;dataleft >0;)
	{
		ret = ::send(sockfd, sendbuf+total, dataleft, 0);
		if(ret < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
				usleep(50000);
				ret = 0;
			}
		}
		total += ret;
		dataleft = len-total;
	}
	return total == len;
}

bool CSocket::Recv(char *recvbuf, int len, int timeout)
{
	if(sockfd <0 || recvbuf==NULL || len < 0)
		return false;
	fd_set fds;
	struct timeval interval;
	interval.tv_sec = timeout;
	interval.tv_usec = 0;
	int recvlen = 0, total = 0;
	for(;;)
	{
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		int res = ::select(sockfd+1, &fds, NULL, NULL, &interval);
		if(res == 0)
			continue;
		if(res < 0)
		{
			::close(sockfd);
			sockfd = -1;
			return false;
		}			
		else
		{
			if(FD_ISSET(sockfd, &fds))
			{
				recvlen = ::recv(sockfd, recvbuf+total, len-total, 0);
				if(recvlen >0 && recvlen < len)
				{
					total += recvlen;
					recvlen = 0;
				}					
				else if((recvlen<0) && (errno == EAGAIN || errno == EWOULDBLOCK))
				{
					recvlen = 0;
					continue;
				}
				else
					break;
			}
		}
	}
	return total == len;
}

bool CSocket::setUp(int fd, struct sockaddr_in* addr)
{
	Close();
	this->sockfd = fd;
	memcpy(&sockAddr, addr, sizeof(sockAddr));
	return true;
}

bool CSocket::Connect()
{
	if(sockfd < 0)
	{
		return false;
	}
	return (::connect(sockfd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)));
}

//tcp server
class CServSocket : public CSocket
{
public:
    CServSocket();
    virtual ~CServSocket();

	CSocket* Accept();
    bool Listen(int port);

private:
	bool Close();
private:
};

CServSocket::CServSocket():CSocket::CSocket()
{}

CServSocket::~CServSocket()
{
	if(!Close())
	{
		std::cout << "close err\n";
	}
}

bool CServSocket::Close()
{
	CSocket::Close();
	return true;
}

bool CServSocket::Listen(int port)
{
	if(port < 0)
	{
		std::cout <<"the port err\n";
		return false;
	}
	if(sockfd < 0)
	{
		return false;
	}
	setAddr(getLocalAddr().c_str(), port);
	setSoLinger(false, 0);
	setIntOptions(SO_REUSEADDR,1);
	setIntOptions(SO_KEEPALIVE, 1);
	setIntOptions(SO_SNDBUF, 640000);
	setIntOptions(SO_RCVBUF, 640000);
	setNoDelay(true);
	if(::bind(sockfd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		std::cout << "bind err\n";
		return false;
	}
	if(::listen(sockfd, 0) < 0)
	{
		std::cout << "listen err\n";
		return false;		
	}
	return true;
}

CSocket* CServSocket::Accept()
{
	CSocket* connSocket;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int connsockfd = -1;
	if((connsockfd = ::accept(sockfd, (struct sockaddr*)&addr, &len)) <0)
	{
		return false;
	}
	connSocket = new CSocket();
	connSocket->setUp(connsockfd, &addr);
	return connSocket;
}

bool CSocket::setAddr(const char* addr, int port)
{
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if(addr==NULL || strlen(addr)==0)
	{
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		return true;
	}
	else
	{
		unsigned long inaddr;
		struct hostent* hp;
		inaddr = inet_addr(addr);
		if(inaddr != INADDR_NONE)
		{
			memcpy(&sockAddr.sin_addr, &inaddr, sizeof(inaddr));
		}
		else
		{
			hp = gethostbyname(addr);
			if(hp == NULL)
			{
				return false;
			}
			memcpy(&sockAddr.sin_addr, hp->h_addr, hp->h_length);
		}
		// const char* p = addr;
		// char c;
		// bool isIp = true;
		// while((c=*p++)&& (c !='\0'))
		// {
		// 	if((c !='.') && !(c>='0' && c<='9'))
		// 	{
		// 		isIp = false;
		// 		break;
		// 	}
		// }
		// if(isIp)
		// {
		// 	sockAddr.sin_addr.s_addr = inet_addr(addr);
		// 	return true;
		// }
		// else
		// {
		// 	struct hostent *host = gethostbyname(addr);
		// 	if(host != NULL)
		// 	{
		// 		memcpy(&(sockAddr.sin_addr), *(host->h_addr_list), sizeof(struct in_addr));
		// 	}
		// }
	}

	return true;
	//return false;
}
//tcp client
class CCliSocket
{
public:
    CCliSocket();
    virtual ~CCliSocket();
	bool Connect(const char* ip, int port);
	bool Send(char* sendbuf, int len);
	bool Recv(char* recvbuf, int len);
private:
	bool Close();
private:
	int clisockfd;	
};

CCliSocket::CCliSocket()
{
	if((clisockfd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "client socket err\n";
	}
}

CCliSocket::~CCliSocket()
{
	if(!Close())
	{
		std::cout << "close err\n";
	}
}

bool CCliSocket::Close()
{
	if(clisockfd < 0)
		return false;
	else
	{
		::close(clisockfd);
		clisockfd = -1;
	}
		
	return true;
}

bool CCliSocket::Connect(const char* ip, int port)
{
	if(ip==NULL || ip=="" || port < 0)
		return false;
	struct sockaddr_in sddr;
	sddr.sin_family = AF_INET;
	sddr.sin_addr.s_addr = inet_addr(ip);
	sddr.sin_port = htons(port);
	if(::connect(clisockfd, (struct sockaddr*)&sddr, sizeof(sddr)) < 0)
	{
		std::cout << "connnect to server err\n";
		return false;
	}
	return true;
}

bool CCliSocket::Send(char* sendbuf, int len)
{
	int ret = 0;
	do
	{
		ret = ::send(clisockfd, sendbuf, len, 0);
		
	} while (ret < 0 && (errno == EAGAIN || errno == EINTR));
    return ret == len; 
}

bool CCliSocket::Recv(char* recvbuf, int len)
{
	int ret = 0;
	do
	{
		ret = ::recv(clisockfd, recvbuf, len, 0);
	} while (ret < 0 && (errno == EAGAIN || errno == EINTR));
    return ret == len; 
}
