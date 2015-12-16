#include <cstdlib>  
#include <pthread.h>  
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
class Mutex  
{  
public:  
    Mutex();  
    virtual ~Mutex();  
  
    void Lock();  
    void UnLock();  
private:  
friend class CondVar;  
    pthread_mutex_t m_mutex;  
    Mutex(const Mutex&);  
    void operator=(const Mutex&);  
};  
  
class CondVar  
{  
public:  
    explicit CondVar(Mutex* mu);  
    virtual ~CondVar();  
      
    void Wait();  
    void Signal();  
    void SignalAll();  
private:  
    pthread_cond_t m_cond;  
    Mutex *m_mu;  
};  

class CASLock
{
public:
    CASLock(void* lock);
    virtual ~CASLock();
    void Lock();
	void UnLock();
	bool TryLock();
private:
	void* m_lock;
};

static void MutexErr(const char* err, int res)  
{  
    if(res != 0)  
    {  
        fprintf(stderr, "pthread %s: %s\n", err, strerror(res));  
        return;  
    }  
}  
  
Mutex::Mutex()  
{  
    MutexErr("init mutex", pthread_mutex_init(&m_mutex, NULL));  
}  
  
Mutex::~Mutex()  
{  
    MutexErr("destroy mutex", pthread_mutex_destroy(&m_mutex));  
}  
  
void Mutex::Lock()  
{  
    MutexErr("lock mutex", pthread_mutex_lock(&m_mutex));  
}  
  
void Mutex::UnLock()  
{  
    MutexErr("unlock mutex", pthread_mutex_unlock(&m_mutex));  
}  
  
CondVar::CondVar(Mutex* mu):m_mu(mu)  
{  
    MutexErr("init cond", pthread_cond_init(&m_cond, NULL));  
}  
  
CondVar::~CondVar()  
{  
    MutexErr("destroy cond", pthread_cond_destroy(&m_cond));  
}  
  
void CondVar::Wait()  
{  
    MutexErr("wait cond", pthread_cond_wait(&m_cond, &m_mu->m_mutex));  
}  
  
void CondVar::Signal()  
{  
    MutexErr("signal cond", pthread_cond_signal(&m_cond));  
}  
  
void CondVar::SignalAll()  
{  
    MutexErr("broadcast cond", pthread_cond_broadcast(&m_cond));  
}

CASLock::CASLock(void* lock):m_lock(lock){}
CASLock::~CASLock(){}

void CASLock::Lock()
{
	while(!__sync_bool_compare_and_swap((int*)m_lock, 0, 1))
	{
		sched_yield();
	}
}

void CASLock::UnLock()
{
	*((int*)m_lock) = 0;
}

bool CASLock::TryLock()
{
	if(__sync_bool_compare_and_swap((int*)m_lock, 0, 1))
		return 0;
	else
		return -1;
}


