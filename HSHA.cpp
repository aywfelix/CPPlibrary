#include <iostream>
#include <vector>
#include <sys/time.h>
#include <string.h>
// for file operation.  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
using namespace std;

//队列层
class TaskQueue
{
public:
    struct TaskItem
	{
		char* buffer;
		int startoffset;
		int size;

		TaskItem& operator=(const TaskItem& item)
		{
			if(this == &item)
				return *this;
			buffer = item.buffer;
			size = item.size;
			startoffset = item.startoffset;
			return *this;	
		}
	};
    TaskQueue():refstart(0),refend(0)
	{
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}
    virtual ~TaskQueue()
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);

		if(getSize()>0)
		{
			vector<TaskItem*>::iterator iter = m_items.begin();
			for(;iter != m_items.end(); ++iter)
			{
				if((*iter)->buffer != NULL)
					delete (*iter)->buffer;
				delete (*iter);
			}
		}
	}

	void push(TaskItem *item)
	{
		pthread_mutex_lock(&mutex);
		m_items.push_back(item);
		++refend;
		if(refend > refstart)
		{
			pthread_cond_signal(&cond);
		}
		pthread_mutex_unlock(&mutex);
	}
	TaskItem* pop()
	{
		pthread_mutex_lock(&mutex);
		if(m_items.empty() || refstart > refend)
		{
            pthread_cond_wait(&cond, &mutex);
		}
        TaskItem* item = m_items[refstart++];
		pthread_mutex_unlock(&mutex);
        return item;
	}
	int getSize()
	{
		pthread_mutex_lock(&mutex);
		int size = m_items.size();
		pthread_mutex_unlock(&mutex);
		return size;
	}

	void print()
	{
		if(!m_items.empty())
		{
			vector<TaskItem*>::iterator iter = m_items.begin();
			for(;iter != m_items.end(); ++iter)
			{
				cout << (*iter)->buffer<< endl;
			}
		}
	}
private:
	vector<TaskItem*> m_items;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int refstart;
	int refend;
};
//异步任务层
class AioProcessor
{
public:
	AioProcessor(int fd, TaskQueue* queue):m_fd(fd), m_queue(queue)
	{
		isRun = false;
	    req.tv_sec = 0;
		req.tv_nsec = 1000*1000*1000;
	}
	~AioProcessor()
	{
		isRun = false;
		vector<pthread_t>::iterator iter = m_tids.begin();
		for(;iter !=m_tids.end(); ++iter)
		{
			pthread_join(*iter, NULL);
		}
	}
	void startUP(int count)
	{
		isRun = true;
		for(int i=0;i<count; ++i)
		{
			pthread_t tid;
			pthread_create(&tid, NULL, reinterpret_cast<void*(*)(void*)>(process), this);
			m_tids.push_back(tid);
		}

	}
	static void* process(void* arg)
	{
		AioProcessor *proc = (AioProcessor*)arg;
		TaskQueue::TaskItem *item;
		while(proc->isRun)
		{
			if((item = proc->m_queue->pop()) != NULL)
			{
				if(pread(proc->m_fd, item->buffer, item->size, item->startoffset) < 0)
				{
					cout << "read err\n";
					break;
				}
			}
			else
			{
				while(nanosleep(&(proc->req), &(proc->rem))!=0)
				{
					if(errno == EINTR)
					{
						proc->req = proc->rem;
					}
					else
						break;
				}
			}
		}
	}

private:
	TaskQueue *m_queue;
	vector<pthread_t> m_tids;
	int m_fd;
	struct timespec req, rem;
	bool isRun;
};
//应用层
class AppExcutor
{
public:
    AppExcutor(int fd):m_fd(fd), queue() 
	{
		proc = new AioProcessor(fd, &queue);
		proc->startUP(3);		
	}
    virtual ~AppExcutor()
	{
		if(proc)
		{
		    delete proc;
			proc = NULL;
		}
	}
	void reader(const int startoffset, int size, char* buf)
	{
		item = new TaskQueue::TaskItem();
		item->buffer = buf;
		item->size = size;
		item->startoffset = startoffset;
		queue.push(item);        
	}

	void print()
	{
		queue.print();
	}
private:
	TaskQueue queue;
	AioProcessor *proc;
	int m_fd;
	TaskQueue::TaskItem *item;
};

int main(int argc, char *argv[])
{
    int fd = open("./testfile", O_RDONLY);
	if(fd < 0)
		return -1;
	AppExcutor excutor(fd);
	int size = 50;
	for(int i=0;i<10; ++i)
	{
		char *buf = new char[size+1];
		memset(buf, 0, sizeof(buf));
		excutor.reader(i*size, size, buf);
	}
	excutor.print();
    return 0;
}

