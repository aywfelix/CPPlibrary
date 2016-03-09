#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

class CList
{
public:
    CList():count(0),maxCount(64)
	{
		List = (void**)malloc(sizeof(void*)*maxCount);
		memset(List, 0, sizeof(void*)*maxCount);
	}
	CList(int size):count(0),maxCount(size)
	{
		List = (void**)malloc(sizeof(void*)*maxCount);
		memset(List, 0, sizeof(void*)*maxCount);
	}
    virtual ~CList()
	{
		if(List)
			free(List);
		List = NULL;
	}

	int getCount()
	{
		return count;
	}
	void* getItem(int index)
	{
		if(index == 0 || index<= maxCount-1)
			return List[index];
		else
			return NULL;
	}
	int putItem(void* item)
	{
		if(item == NULL) return -1;
	    if(count >= maxCount)
		{
			maxCount *= 2;
			List = (void**)realloc(List, sizeof(void*)*maxCount);
			memset(List+count, 0, (maxCount-count)*sizeof(void*));			
		}
		List[count++] = item;
		return count;
	}
	void* takeItem(int index)
	{
		void* ret = NULL;
		if(index >= count || index <0)
			return NULL;
		else if(index <= maxCount-1)
		{
			ret = List[index];
			memmove(List+index, List+index+1, sizeof(void*)*(maxCount-index-1));
			--count;
		}
		return ret;
	}
private:
	int count, maxCount;
	void** List;
};

class CCircleQ
{
public:
    CCircleQ():head(0),tail(0),count(0),maxCount(64)
	{
		CircleQ = (void**)malloc(sizeof(void*)*maxCount);
		memset(CircleQ, 0, sizeof(void*)*maxCount);
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}
	
	CCircleQ(int size):head(0),tail(0),count(0),maxCount(size)
	{
		CircleQ = (void**)malloc(sizeof(void*)*maxCount);
		memset(CircleQ, 0, sizeof(void*)*maxCount);
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}
	
    virtual ~CCircleQ()
	{
		if(CircleQ)
			free(CircleQ);
		CircleQ = NULL;
	}

	void* top()
	{
		pthread_mutex_lock(&mutex);
		return count>0 ? CircleQ[head]:NULL;
		pthread_mutex_unlock(&mutex);
			
	}
	void* pop()
	{
		void* ret = NULL;
		pthread_mutex_lock(&mutex);
		if(count == 0)
		{
			pthread_cond_wait(&cond, &mutex);			
		}
		else
		{
			ret = CircleQ[head++];
			head = head%maxCount;
			--count;
		}

		pthread_mutex_unlock(&mutex);
		return ret;
	}
	
	int push(void* item)
	{
		if(item == NULL) return -1;
		pthread_mutex_lock(&mutex);
		if(count > maxCount)
		{
			maxCount *= 2;
			void **NewCirCleQ = (void**)malloc(sizeof(void*)*maxCount);
			unsigned int headlen=0, taillen=0;
			if(head < tail)
				headlen = tail-head;
			else
			{
				headlen = count-tail;
				taillen = tail;
			}
			memcpy(NewCirCleQ, &(CircleQ[head]), sizeof(void *)*headlen );
			if(taillen)
			{
				memcpy(&(NewCirCleQ[headlen]), CircleQ, sizeof(void *)*taillen);
			}
			head = 0;
			tail = headlen+taillen;

			free(CircleQ);
			CircleQ = NewCirCleQ;
		}
		CircleQ[tail++] = item;
		tail %= maxCount;
		++count;
		if(getLength() >= 0)
		{
			pthread_cond_signal(&cond);
		}
		pthread_mutex_unlock(&mutex);
	}
	
	int getLength()
	{
		pthread_mutex_lock(&mutex);
		return count;	
		pthread_mutex_unlock(&mutex);
	}
private:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	void **CircleQ;
	unsigned int head, tail;
	unsigned int count, maxCount; 
};
