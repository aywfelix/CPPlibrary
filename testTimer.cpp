//测试程序
#include "CTime.h"
#include <time.h>
#include <iostream>
#include <stdio.h>    
#include <signal.h>    
#include <string.h>       
#include <stddef.h>    
#include <sstream>    
#include <iomanip>    
using namespace std;
//一个进程只能有一个定时器,下一个会覆盖上一个

void print_timeA(int signo, siginfo_t* info, void* context)
{
	CTime ctime;
	cout << ctime.timeToStr() << endl;
	ctime.setMday(ctime.getMday()+1); 
	cout << ctime.timeToStr() << endl;
	ctime.updateTime();
	cout << ctime.timeToStr() << endl;
}
int main(int argc, char *argv[])
{
	timerTask->excuteTask(5*1000, print_timeA, 0);
	cout << timerTask->getTimeID() << endl;
	while(true)
	{
		sleep(15);
	}
    return 0;
}

