#include "CThreadPool.h"
#include <time.h>
void *myprocess (void *arg)
{
	printf ("threadid is %lu, working on task(%s)\n", pthread_self(), (char*)arg);
	sleep (3);
}

void *myprocess2 (void *arg)
{
	printf ("threadid is %lu, working on task(%s)\n", pthread_self(), (char*)arg);
	sleep (2);
}

int main(int argc, char *argv[])
{

	CThreadPool* pool = CThreadPool::getInstance(10);

	for(int i=0;i<5; ++i)
	{
		pool->addTask(myprocess, (void*)"process1");
	}

	for(int i=0; i<5; i++)
	{
		pool->addTask(myprocess2, (void*)"process2");	
	}

	sleep(5);
    return 0;
}
