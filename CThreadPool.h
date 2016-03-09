#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

typedef void*(*FunPtr)(void* arg);

class CThreadPool;
typedef struct worker
{
	pthread_t pid;
    FunPtr fun_cb;
	void* arg;
	CThreadPool* parent;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}CThread_worker;

class CThreadPool
{
public:
	CThreadPool(int maxThreads);
    virtual ~CThreadPool();

	static CThreadPool* getInstance(int threadNum);
	int addTask(FunPtr fun_cb, void *arg);
	FunPtr fun_cb;

private:
	pthread_mutex_t mainMutex;
	pthread_cond_t IdleCond;
	pthread_cond_t FullCond;
	pthread_cond_t EmptyCond;
	
	CThread_worker **threadList;

	int shutdown;

	int maxThreads;
	int index;
	int total;

	static void* wrapperFunc(void* arg);
	int saveThread(CThread_worker* thread);
	
	static CThreadPool *threadPool;
};

CThreadPool* CThreadPool::threadPool = NULL;

CThreadPool::CThreadPool(int threadNum)
{
	if(threadNum <=0)
		maxThreads = 2;
	else
		maxThreads = threadNum;
	pthread_mutex_init(&mainMutex, NULL);
	pthread_cond_init(&IdleCond, NULL);
	pthread_cond_init(&FullCond, NULL);
	pthread_cond_init(&EmptyCond, NULL);

	shutdown = index  = total = 0;
	threadList = (CThread_worker**)calloc(maxThreads, sizeof(void*));
}

CThreadPool::~CThreadPool()
{
	pthread_mutex_lock(&mainMutex);
	if(index <= total)
	{
		pthread_cond_wait(&FullCond, &mainMutex);
	}
	shutdown = 1;
	for(int i=0;i<index; ++i)
	{
		CThread_worker* thread = threadList[i];
		pthread_mutex_lock(&thread->mutex);
		pthread_cond_signal(&thread->cond);
		pthread_mutex_unlock(&thread->mutex);
	}
	if(total>0)
	{
		pthread_cond_wait(&EmptyCond, &mainMutex);
	}
	for(int i=0;i<index; ++i)
	{
		CThread_worker* thread = threadList[i];
		pthread_mutex_destroy(&thread->mutex);
		pthread_cond_destroy(&thread->cond);
		free(thread);
		thread = NULL;
	}
	
	pthread_mutex_unlock(&mainMutex);
	index = 0;
	pthread_mutex_destroy(&mainMutex);
	pthread_cond_destroy(&EmptyCond);
	pthread_cond_destroy(&IdleCond);
	pthread_cond_destroy(&FullCond);

	free(threadList);
	threadList = NULL;
	
}

CThreadPool* CThreadPool::getInstance(int num)
{
	if(threadPool == NULL)
	{
		threadPool = new CThreadPool(num);
	}
	return threadPool;
}

int CThreadPool::addTask(FunPtr fun_cb, void *arg)
{
	int ret = 0;
	pthread_attr_t attr;
	CThread_worker* thread = NULL;
	pthread_mutex_lock(&mainMutex);
    if(index <=0 && total >= maxThreads)
		pthread_cond_wait(&IdleCond, &mainMutex);
	if(index <=0)
	{
		CThread_worker *thread = (CThread_worker*)malloc(sizeof(CThread_worker));
		thread->pid = 0;
		pthread_mutex_init(&thread->mutex, NULL);
		pthread_cond_init(&thread->cond, NULL);
		thread->fun_cb = fun_cb;
		thread->arg= arg;
		thread->parent = this;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
		if(pthread_create(&(thread->pid), &attr, wrapperFunc, (void*)thread) == 0)
		{
			++total;
		}
		else
		{
			ret = -1;
			pthread_mutex_destroy(&thread->mutex);
			pthread_cond_destroy(&thread->cond);
			free(thread);
			thread =NULL;
		}
	}
	else
	{
		--index;
		thread = threadList[index];
		threadList[index] = NULL;
		thread->fun_cb = fun_cb;
		thread->arg = arg;
		thread->parent = this;

		pthread_mutex_lock(&thread->mutex);
		pthread_cond_signal(&thread->cond);
		pthread_mutex_unlock(&thread->mutex);
	}
	
	pthread_mutex_unlock(&mainMutex);
}

void* CThreadPool::wrapperFunc(void* arg)
{
	CThread_worker *thread = (CThread_worker*)arg;
	while(1)
	{
		if(thread->parent->shutdown == 0)
		{
			thread->fun_cb(thread->arg);
			pthread_mutex_lock(&thread->mutex);
			if(thread->parent->saveThread(thread) == 0)
			{
				pthread_cond_wait(&thread->cond, &thread->mutex);
				pthread_mutex_unlock(&thread->mutex);
			}
			else
			{
				pthread_mutex_unlock(&thread->mutex);
				pthread_mutex_destroy(&thread->mutex);
				free(thread);
				thread = NULL;
				break;
			}
		}
		else
			break;
	}
	
	if(thread != NULL)
	{
		pthread_mutex_lock(&thread->parent->mainMutex);
		--(thread->parent->total);
		if(thread->parent->total <=0)
		{
			pthread_cond_signal(&thread->parent->EmptyCond);
		}
		pthread_mutex_unlock(&thread->parent->mainMutex);
	}	
}

int CThreadPool::saveThread(CThread_worker* thread)
{
	int ret = -1;
	pthread_mutex_lock(&mainMutex);
	if(index <= maxThreads)
	{
		threadList[index] = thread;
        ++index;
		ret = 0;
		pthread_cond_signal(&IdleCond);
		
		if(index >= total)
			pthread_cond_signal(&FullCond);
	}

	pthread_mutex_unlock(&mainMutex);
    return ret;
}
