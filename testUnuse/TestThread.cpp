#include "TestThread.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int count = 0;
pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t init_cond = PTHREAD_COND_INITIALIZER;
volatile int flag = 0;

void* function(void* arg)
{
	while(1)
	{
		// pthread_mutex_lock(&init_lock);
		// pthread_cond_wait(&init_cond, &init_lock);
		// pthread_mutex_unlock(&init_lock);
		if(flag)
		   break;
		else
			continue;
	}
	CThread *thread = (CThread*)arg;
	do
	{
		if(thread->status == leader)
		{
			thread->status = processer;
			pthread_mutex_lock(&mutex);
			++count;
			cout << thread->pid << ": "<<count << "\n";
			pthread_mutex_unlock(&mutex);
			sleep(8);
            thread->status = follower;
		}
		sleep(1);
	} while (1);
}

int Random(int low, int high)  
{  
    return low + rand() % (high - low + 1);  
}  

int main(int argc, char *argv[])
{
	srand((int)time(NULL));  
	int ncnt = 5;
    CThread thread[ncnt];
	for(int i=0; i<ncnt; ++i)
	{
		thread[i].func = function;
		thread[i].pid = -1;
		thread[i].status = follower;
	}

	for(int i =0; i<ncnt; ++i)
	{
		pthread_create(&(thread[i].pid), NULL, thread[i].func, &thread[i]);
	}
	
	// pthread_cond_signal(&init_cond);
	flag = 1;
	do
	{
		//select
        for(int i=0; i< ncnt; ++i)
		{
			if((thread[i].status == follower))
			{
				thread[i].status = leader;
				cout << thread[i].pid<< ":"<<"become the leader\n";
				break;
			}			
		}
		sleep(1);
	} while (1);

	for(int i =0; i<ncnt; ++i)
	{
		pthread_join(thread[i].pid, NULL);
	}
	pthread_mutex_destroy(&init_lock);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&init_cond);
	
    return 0;
}



