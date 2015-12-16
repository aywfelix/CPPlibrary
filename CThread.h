#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>

class Runnable
{
public:
    virtual ~Runnable() {}
	virtual void* Run(void * arg) = 0;
};

class CThread
{
public:
    CThread();
    virtual ~CThread();
	void Start(Runnable *r, void* arg);
	void Stop();   
	int getpid() { return pid;}
	int getthreadid() {return tid;}
	void Sleep();
	void Yield();
private:
	int CreateThread(pthread_t *threadid, void *pfunction, void *arg)
	{
		int ret = 0;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		{
			ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			if(ret < 0)
			{
				pthread_attr_destroy(&attr);
				return ret;
			}
		}
		pthread_create(threadid, &attr, (void*(*)(void*))pfunction, arg);
		pthread_attr_destroy(&attr);

		return ret;
	}
	static void* hook(void *arg) //global function
	{
		CThread *thread = (CThread*)arg;
		thread->pid = gettid();
		if(thread->runnable)
		{
			thread->runnable->Run(arg);
		}
		return (void*)NULL;
	}
private:
	static pid_t gettid(){return syscall(__NR_gettid);}
    pthread_t tid; //thread id
	int pid; //process id
	void *data;
	Runnable *runnable;
	CThread(const CThread&);
	CThread& operator=(const CThread& );
};

CThread::CThread()
{
	tid = 0;
	pid = 0;
}

CThread::~CThread()
{}

void CThread::Start(Runnable *r, void* arg)
{
	runnable = r;
	data = arg;
	if(CreateThread(&tid, (void*)CThread::hook, (void*)this) < 0)
	{
		std::cout << "create pthread err\n";
	}
	else
	{
		std::cout << "pthread start:"<<tid << "\n";
	}
}

void CThread::Stop()
{
	if(tid)
	{
		pthread_join(tid, NULL);
		tid = 0; pid = 0;
	}
}
void Sleep(struct timeval& timeout)
{
	struct timeval tv = timeout;
	struct timespec ts;
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec*1000L;
	nanosleep(&ts, 0);
}
void Yield()
{
	sched_yield();
}

class CDefultRunnable : public Runnable
{
public:
	CDefultRunnable(){ threadcount = 1;} 
	CDefultRunnable(int num):threadcount(num){}
	~CDefultRunnable(){}

	void SetThreadsCount(int num)
	{
		if(threads != NULL)
		{
			std::cout << "threads is running, can't set thread count\n";
			return;
		}
		threadcount = num;
	}
	void Start();
	void Stop();
	void Wait();
private:
	int threadcount; //
	CThread* threads;
};

void CDefultRunnable::Start()
{
	threads = new CThread[threadcount];
	if(threads)
	{
		for(int i=0; i<threadcount; ++i)
		{
			threads[i].Start(this, (void*)i);
		}
	}
}
void CDefultRunnable::Stop()
{	
}
void CDefultRunnable::Wait()
{
	if(threads != NULL)
	{
		for(int i=0; i< threadcount; ++i)
		{
			threads[i].Stop();
		}
	}
}

class T : public CDefultRunnable
{
public:
    T()
	{
		pthread_mutex_init(&mutex, NULL);
		d = 0;
	}
	T(int num):CDefultRunnable(num)
	{
		pthread_mutex_init(&mutex, NULL);
		d = 0;
	}
    virtual ~T(){d=0;}
	void* Run(void * arg);
private:
	T(const T& t);
	T& operator=(const T& t);
	int d;
	pthread_mutex_t mutex;
};

void* T::Run(void* arg)
{
	do
	{
		pthread_mutex_lock(&mutex);
		++d;		
		if(d > 1000) break;
		std::cout << d << "\n";
		pthread_mutex_unlock(&mutex);
	} while (d < 1000);
}

