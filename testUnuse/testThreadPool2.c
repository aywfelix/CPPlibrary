#include "ThreadPool2.h"

void *
myprocess (void *arg)
{
	printf ("threadid is 0x%x, working on task %d\n", pthread_self(),*(int *) arg);
	sleep (1);/*休息一秒，延长任务的执行时间*/
    return NULL;
}

int
main (int argc, char **argv)
{
	pool_init(3);/*线程池中最多三个活动线程*/

    /*连续向池中投入10个任务*/
    int *workingnum = (int *) malloc (sizeof (int) * 10);
    int i;
    for (i = 0; i < 10; i++)
	{
		workingnum[i] = i;
		pool_add_worker (myprocess, &workingnum[i]);
	}

	/*等待所有任务完成*/
	sleep (5);

	/*销毁线程池*/
	pool_destroy ();

	free (workingnum);
    return 0;
}
