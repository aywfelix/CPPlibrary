#include<stdio.h>  
#include<iostream>  
#include<signal.h>  
#include <string.h>  
#include<time.h>  
#include <stddef.h>  
#include <sstream>  
#include <iomanip>  
using namespace std;


class CTimeTask
{
public:
	typedef void(*func)(int signo, siginfo_t* info, void* context);
    CTimeTask()
	{
		memset(&evp, 0, sizeof(evp));  
		evp.sigev_notify = SIGEV_SIGNAL;  
		evp.sigev_signo = SIGRTMAX;  
		evp.sigev_value.sival_ptr = &timeID;
		if(::timer_create(CLOCK_REALTIME, &evp, &timeID) ==-1 )  
		{  
			cout << "timer create err" << endl;  
			return;  
		}  
	}
    virtual ~CTimeTask()
	{}

	int timeTask(int sec, int nsec, func onFunc)
	{
		struct sigaction sigac;  
		sigemptyset(&sigac.sa_mask);  
		sigac.sa_flags = SA_SIGINFO;  
		sigac.sa_sigaction = onFunc;  
		sigaction(SIGRTMAX, &sigac, NULL);  
     
		struct itimerspec it;    
		it.it_interval.tv_sec = sec;    
		it.it_interval.tv_nsec = nsec;    
		it.it_value.tv_sec = sec;    
		it.it_value.tv_nsec = nsec;  
  
		if(::timer_settime(timeID, 0, &it, NULL) == -1)  
		{  
			cout << "set time task err" << endl;  
			::timer_delete(timeID);  
			return -1;  
		}  
	}
private:
	timer_t timeID;  
    struct sigevent evp;
};

void testfunc(int signo, siginfo_t* info, void* context)  
{  
    // cnt++;  
    // cout << cnt << endl;  
    struct tm *t;  
    time_t timer = time(NULL);  
    t = localtime(&timer);  
    ostringstream os;  
    os.fill('0');  
    os << setw(4) << t->tm_year + 1900 << '-'  
       << setw(2) << t->tm_mon + 1 << '-'  
       << setw(2) << t->tm_mday << ' '          
       << setw(2) << t->tm_hour << ':'  
       << setw(2) << t->tm_min  << ':'  
       << setw(2) << t->tm_sec  << ends;  
    cout << os.str() << endl;  
}

int main(int argc, char *argv[])
{
	CTimeTask timeTask;
	timeTask.timeTask(2, 0, testfunc);
	while(1);
    return 0;
}
