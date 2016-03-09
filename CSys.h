#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

static bool g_stop = false;

void sign_handler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		log_error("catch sig %d", sig);
		g_stop = true;
		break;
	}
}

class CSys
{
public:
	static int startDeamon(std::string& pidfile);
	static void writePidFile(std::string& pidfile);
	static bool isExist(std::string& pidfile);
	static bool setResource();
	static int getpagesize()
};

int CSys::startDeamon(std::string& pidfile)
{
	if(pidfile.empty())
		return -1;
	int pid = fork();
	if(pid < 0) return -1;
	else if(pid > 0)
		return pid;
	writePidFile(pidfile);
	int fd = open("/dev/null", 0);
	if(fd != -1)
	{
		dup2(fd, 0);
		close(fd);
	}
	return pid;
}

void CSys::writePidFile(std::string& pidfile)
{
	char buf[30];
	int fd = open(pidfile.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	if(fd < 0) exit(-1);
	sprintf(buf, "%d", getpid());
	if(write(fd, buf, 30) <=0)
	{
		std::cout << "write pid file err\n";
		exit(-1);
	}
	close(fd);
}

int CSys::isExist(std::string& pidfile)
{
	if(pidfile.empty())
		exit(-1);
	int fd, pid;
	char buf[32];
	fd = open(pidfile, O_RDONLY, 0);
	read(fd, buf, 32);
	buf[31] = '\0';
	pid = atoi(buf);
	if(pid >0 && kill(pid, 0) !=0 )
	{
		pid = 0;
	}
	return pid;
}

bool CSys::setResource()
{
	bool ret = true;
	struct rlimit rlim;
	struct rlimit rlim_new;
	if(getrlimit(RLIMIT_CORE, &rlim) == 0)
	{
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if(setrlimit(RLIMIT_CORE, &rlim_new) !=0)
		{
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);
		}
	}

	if((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_max == 0)
	{
		perror( "Set Resource limit failed\n");
		ret = false;
	}
	return ret;
}

//获取系统页面大小
int CSys::getpagesize()
{
	return ::getpagesize();
}









