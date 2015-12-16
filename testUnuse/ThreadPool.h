#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <algorithm>
#include "Mutex.h"

using namespace std;
using std::vector;

class CJob;
class CThread;
class CWorkerThread;
class CThreadPool;

class CThreadManage
{
private:
    CThreadPool*    m_Pool;
    int          m_NumOfThread;
protected:
public:

    void  SetParallelNum(int num);
    CThreadManage();
    CThreadManage(int num);

    virtual ~CThreadManage();
    void    Run(CJob* job,void* jobdata);
    void    TerminateAll(void);
};

CThreadManage::CThreadManage(){

    m_NumOfThread = 10;
    m_Pool = new CThreadPool(m_NumOfThread);

}

CThreadManage::CThreadManage(int num){

    m_NumOfThread = num;

    m_Pool = new CThreadPool(m_NumOfThread);

}

CThreadManage::~CThreadManage(){

    if(NULL != m_Pool)

		delete m_Pool;

}

void CThreadManage::SetParallelNum(int num){

    m_NumOfThread = num;

}

void CThreadManage::Run(CJob* job,void* jobdata){

    m_Pool->Run(job,jobdata);

}

void CThreadManage::TerminateAll(void){

    m_Pool->TerminateAll();

}

class CThread
{
private:
    int          m_ErrCode;
    Semaphore    m_ThreadSemaphore;  //the inner semaphore, which is used to realize
    unsigned     long m_ThreadID;   
    bool         m_Detach;       //The thread is detached
    bool         m_CreateSuspended;  //if suspend after creating
    char*        m_ThreadName;
    ThreadState m_ThreadState;      //the state of the thread
protected:
    void     SetErrcode(int errcode){m_ErrCode = errcode;}
    static void* ThreadFunction(void*);
public:
    CThread();
    CThread(bool createsuspended,bool detach);
    virtual ~CThread();
    virtual void Run(void) = 0;
    void     SetThreadState(ThreadState state){m_ThreadState = state;}
    bool     Terminate(void);    //Terminate the threa
    bool     Start(void);        //Start to execute the thread
    void     Exit(void);
    bool     Wakeup(void);

    ThreadState  GetThreadState(void){return m_ThreadState;}
    int      GetLastError(void){return m_ErrCode;}
    void     SetThreadName(char* thrname){strcpy(m_ThreadName,thrname);}
    char*    GetThreadName(void){return m_ThreadName;}
    int      GetThreadID(void){return m_ThreadID;}

    bool     SetPriority(int priority);
    int      GetPriority(void);
    int      GetConcurrency(void);
    void     SetConcurrency(int num);
    bool     Detach(void);
    bool     Join(void);
    bool     Yield(void);
    int      Self(void);
};

class CThreadPool
{
friend class CWorkerThread;
private:
    unsigned int m_MaxNum;   //the max thread num that can create at the same time
    unsigned int m_AvailLow; //The min num of idle thread that shoule kept
    unsigned int m_AvailHigh;    //The max num of idle thread that kept at the same time
    unsigned int m_AvailNum; //the normal thread num of idle num;
    unsigned int m_InitNum;  //Normal thread num;

protected:
    CWorkerThread* GetIdleThread(void);
    void    AppendToIdleList(CWorkerThread* jobthread);
    void    MoveToBusyList(CWorkerThread* idlethread);
    void    MoveToIdleList(CWorkerThread* busythread);
    void    DeleteIdleThread(int num);
    void    CreateIdleThread(int num);

public:
    CThreadMutex m_BusyMutex;    //when visit busy list,use m_BusyMutex to lock and unlock
    CThreadMutex m_IdleMutex;    //when visit idle list,use m_IdleMutex to lock and unlock
    CThreadMutex m_JobMutex; //when visit job list,use m_JobMutex to lock and unlock
    CThreadMutex m_VarMutex;
	
    CCondition       m_BusyCond; //m_BusyCond is used to sync busy thread list
    CCondition       m_IdleCond; //m_IdleCond is used to sync idle thread list
    CCondition       m_IdleJobCond;  //m_JobCond is used to sync job list
    CCondition       m_MaxNumCond;

    vector<CWorkerThread*>   m_ThreadList;
    vector<CWorkerThread*>   m_BusyList;     //Thread List
    vector<CWorkerThread*>   m_IdleList; //Idle List

    CThreadPool();
    CThreadPool(int initnum);
    virtual ~CThreadPool();

    void    SetMaxNum(int maxnum){m_MaxNum = maxnum;}
    int     GetMaxNum(void){return m_MaxNum;}
    void    SetAvailLowNum(int minnum){m_AvailLow = minnum;}
    int     GetAvailLowNum(void){return m_AvailLow;}
    void    SetAvailHighNum(int highnum){m_AvailHigh = highnum;}
    int     GetAvailHighNum(void){return m_AvailHigh;}
    int     GetActualAvailNum(void){return m_AvailNum;}
    int     GetAllNum(void){return m_ThreadList.size();}
    int     GetBusyNum(void){return m_BusyList.size();}
    void    SetInitNum(int initnum){m_InitNum = initnum;}
    int     GetInitNum(void){return m_InitNum;}
    void    TerminateAll(void);

    void    Run(CJob* job,void* jobdata);
};

CThreadPool::CThreadPool()
{
    m_MaxNum = 50;
    m_AvailLow = 5;
    m_InitNum=m_AvailNum = 10 ;  
    m_AvailHigh = 20;
    m_BusyList.clear();
    m_IdleList.clear();

    for(int i=0;i<m_InitNum;i++){
		CWorkerThread* thr = new CWorkerThread();
		thr->SetThreadPool(this);
		AppendToIdleList(thr);
		thr->Start();
    }
}


CThreadPool::CThreadPool(int initnum)
{
    assert(initnum>0 && initnum<=30);
    m_MaxNum   = 30;
    m_AvailLow = initnum-10>0?initnum-10:3;
    m_InitNum=m_AvailNum = initnum ;  
    m_AvailHigh = initnum+10;

    m_BusyList.clear();
    m_IdleList.clear();

    for(int i=0;i<m_InitNum;i++){
		CWorkerThread* thr = new CWorkerThread();
		AppendToIdleList(thr);
		thr->SetThreadPool(this);
		thr->Start();       //begin the thread,the thread wait for job
    }

}


CThreadPool::~CThreadPool()
{

	TerminateAll();

}

void CThreadPool::TerminateAll()
{

    for(int i=0;i < m_ThreadList.size();i++) {

		CWorkerThread* thr = m_ThreadList[i];

		thr->Join();

    }

    return;

}

CWorkerThread* CThreadPool::GetIdleThread(void)
{

    while(m_IdleList.size() ==0 )

		m_IdleCond.Wait();

    m_IdleMutex.Lock();

    if(m_IdleList.size() > 0 )

    {

		CWorkerThread* thr = (CWorkerThread*)m_IdleList.front();

		printf("Get Idle thread %dn",thr->GetThreadID());

		m_IdleMutex.Unlock();

		return thr;

    }

    m_IdleMutex.Unlock();

    return NULL;
}

 

//add an idle thread to idle list

void CThreadPool::AppendToIdleList(CWorkerThread* jobthread)
{

    m_IdleMutex.Lock();

    m_IdleList.push_back(jobthread);

    m_ThreadList.push_back(jobthread);

    m_IdleMutex.Unlock();

}

 

//move and idle thread to busy thread

void CThreadPool::MoveToBusyList(CWorkerThread* idlethread)
{

    m_BusyMutex.Lock();

    m_BusyList.push_back(idlethread);

    m_AvailNum--;

    m_BusyMutex.Unlock();

  

    m_IdleMutex.Lock();

    vector<CWorkerThread*>::iterator pos;

    pos = m.IdleList.find(m_IdleList.begin(),m_IdleList.end(),idlethread);

    if(pos !=m_IdleList.end())

		m_IdleList.erase(pos);

    m_IdleMutex.Unlock();

}

 

void CThreadPool::MoveToIdleList(CWorkerThread* busythread)
{

    m_IdleMutex.Lock();

    m_IdleList.push_back(busythread);

    m_AvailNum++;

    m_IdleMutex.Unlock();

 

    m_BusyMutex.Lock();

    vector<CWorkerThread*>::iterator pos;

    pos = find(m_BusyList.begin(),m_BusyList.end(),busythread);

    if(pos!=m_BusyList.end())

		m_BusyList.erase(pos);

    m_BusyMutex.Unlock();

 

    m_IdleCond.Signal();

    m_MaxNumCond.Signal();

}

//create num idle thread and put them to idlelist

void CThreadPool::CreateIdleThread(int num)
{

    for(int i=0;i<num;i++){

		CWorkerThread* thr = new CWorkerThread();

		thr->SetThreadPool(this);

		AppendToIdleList(thr);

		m_VarMutex.Lock();

		m_AvailNum++;

		m_VarMutex.Unlock();

		thr->Start();       //begin the thread,the thread wait for job

    }

}

 

void CThreadPool::DeleteIdleThread(int num)
{

    printf("Enter into CThreadPool::DeleteIdleThreadn");

    m_IdleMutex.Lock();

    printf("Delete Num is %dn",num);

    for(int i=0;i<num;i++)
	{

		CWorkerThread* thr;

		if(m_IdleList.size() > 0 )
		{

            thr = (CWorkerThread*)m_IdleList.front();

            printf("Get Idle thread %dn",thr->GetThreadID());

		}

		vector<CWorkerThread*>::iterator pos;

		pos = find(m_IdleList.begin(),m_IdleList.end(),thr);

		if(pos!=m_IdleList.end())

			m_IdleList.erase(pos);

		m_AvailNum--;

		printf("The idle thread available num:%d n",m_AvailNum);

		printf("The idlelist num:%d n",m_IdleList.size());

    }

    m_IdleMutex.Unlock();

}

void CThreadPool::Run(CJob* job,void* jobdata)
{

    assert(job!=NULL);

    //if the busy thread num adds to m_MaxNum,so we should wait

    if(GetBusyNum() == m_MaxNum)

        m_MaxNumCond.Wait();

    if(m_IdleList.size()<m_AvailLow)
    {

		if(GetAllNum()+m_InitNum-m_IdleList.size() < m_MaxNum )

			CreateIdleThread(m_InitNum-m_IdleList.size());

		else

			CreateIdleThread(m_MaxNum-GetAllNum());

    }

 

    CWorkerThread*  idlethr = GetIdleThread();

    if(idlethr !=NULL)
    {

		idlethr->m_WorkMutex.Lock();

		MoveToBusyList(idlethr);

		idlethr->SetThreadPool(this);

		job->SetWorkThread(idlethr);

		printf("Job is set to thread %d n",idlethr->GetThreadID());

		idlethr->SetJob(job,jobdata);

    }

}

class CWorkerThread:public CThread
{
private:
    CThreadPool*  m_ThreadPool;
    CJob*    m_Job;
    void*    m_JobData;
   
    CThreadMutex m_VarMutex;
    bool      m_IsEnd;
protected:
public:
    CCondition   m_JobCond;
    CThreadMutex m_WorkMutex;
    CWorkerThread();
    virtual ~CWorkerThread();
    void Run();
    void    SetJob(CJob* job,void* jobdata);
    CJob*   GetJob(void){return m_Job;}
    void    SetThreadPool(CThreadPool* thrpool);
    CThreadPool* GetThreadPool(void){return m_ThreadPool;}
};
CWorkerThread::CWorkerThread()
{
    m_Job = NULL;
    m_JobData = NULL;
    m_ThreadPool = NULL;
    m_IsEnd = false;
}
CWorkerThread::~CWorkerThread()
{
    if(NULL != m_Job)
		delete m_Job;
    if(m_ThreadPool != NULL)
		delete m_ThreadPool;
}
 
void CWorkerThread::Run()
{
    SetThreadState(THREAD_RUNNING);
    for(;;)
    {
		while(m_Job == NULL)
			m_JobCond.Wait();
 
		m_Job->Run(m_JobData);
		m_Job->SetWorkThread(NULL);
		m_Job = NULL;
		m_ThreadPool->MoveToIdleList(this);
		if(m_ThreadPool->m_IdleList.size() > m_ThreadPool->GetAvailHighNum())
		{
			m_ThreadPool->DeleteIdleThread(m_ThreadPool->m_IdleList.size()-m_ThreadPool->GetInitNum());
		}
		m_WorkMutex.Unlock();
    }
}
void CWorkerThread::SetJob(CJob* job,void* jobdata)
{
    m_VarMutex.Lock();
    m_Job = job;
    m_JobData = jobdata;
    job->SetWorkThread(this);
    m_VarMutex.Unlock();
    m_JobCond.Signal();
}
void CWorkerThread::SetThreadPool(CThreadPool* thrpool)
{
    m_VarMutex.Lock();
    m_ThreadPool = thrpool;
    m_VarMutex.Unlock();
}


class CJob
{
private:
    int      m_JobNo;        //The num was assigned to the job
    char*    m_JobName;      //The job name
    CThread  *m_pWorkThread;     //The thread associated with the job
public:
    CJob( void );
    virtual ~CJob();
       
    int      GetJobNo(void) const { return m_JobNo; }
    void     SetJobNo(int jobno){ m_JobNo = jobno;}
    char*    GetJobName(void) const { return m_JobName; }
    void     SetJobName(char* jobname);
    CThread *GetWorkThread(void){ return m_pWorkThread; }
    void     SetWorkThread ( CThread *pWorkThread )
	{
        m_pWorkThread = pWorkThread;
    }
    virtual void Run ( void *ptr ) = 0;
};

CJob::CJob(void):m_pWorkThread(NULL),m_JobNo(0),m_JobName(NULL)
{}
CJob::~CJob(){
    if(NULL != m_JobName)
		free(m_JobName);
}
void CJob::SetJobName(char* jobname)
{
    if(NULL !=m_JobName)
	{
        free(m_JobName);
        m_JobName = NULL;
    }
    if(NULL !=jobname)
	{
        m_JobName = (char*)malloc(strlen(jobname)+1);
        strcpy(m_JobName,jobname);
    }
}


class CXJob:public CJob
{
public:
    CXJob(){}
    ~CXJob(){}
    void Run(void* jobdata)
	{
        printf("The Job comes from CXJOB/n");
        sleep(2);
    }
};
 
class CYJob:public CJob
{
public:
    CYJob(){}
    ~CYJob(){}
    void Run(void* jobdata)    {
        printf("The Job comes from CYJob/n");
    }
};
