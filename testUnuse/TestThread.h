#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

using namespace  std;

class CThread
{
public:
    void*(*func)(void* arg);
	pthread_t pid;
	int status;
};
enum emStatus{ leader = 0, processer=1, follower=2};
void* function(void* arg);
