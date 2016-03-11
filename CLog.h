#include "Mutex.h"
#include "CTime.h"
#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <fstream>
#include <sys/types.h> 
#include <sys/stat.h>
//#define LOGPATH "./common.log"
#define BUFSIZE 2048
class CLog
{
public:
	CLog()
	{
		ctime.updateTime();
		ctime.setMday(ctime.getMday() + 1);
		ctime.setHour(0);
		ctime.setMin(0);
		ctime.setSec(0);
		starttime = ctime.timeSec();
	}
	virtual ~CLog()
	{
	}
	static CLog* getInstance();
	bool Init(const char* path);

	void Error(char *pFmt, ...);
    void Info(char  *pFmt, ...);
    void Debug(char *pFmt, ...);
    void Warn(char  *pFmt, ...);     

    void Log(int level, const char * loginfo);
private:
	bool checkLogTime();
	static CLog *m_log;
	CLog(const CLog& clog);
	CLog& operator=(const CLog& clog);

	Mutex m_mutex;
	int m_logLevel;
	char m_logpath[100];
	char m_logfile[200];
	int logHandle;
	enum { LOG_ERROR = 0, LOG_INFO, LOG_WARN, LOG_DEBUG};
	CTime ctime;
	time_t starttime;
};
CLog* CLog::m_log = NULL;
CLog* CLog::getInstance()
{
	if(m_log == NULL)
	{
		m_log = new CLog();
	}
	return m_log;
}

bool CLog::Init(const char* path)
{
	if(mkdir(path, 0755) == -1)
	{
		printf("create log path failed\n");
		return false;
	}
	strcpy(m_logpath, path);
	ctime.updateTime();
	string timestr = ctime.timeToDay();
	sprintf(m_logfile, "%s/%s.log", m_logpath, timestr.c_str());
	logHandle = open(m_logfile, O_CREAT|O_RDWR|O_APPEND, 644);
	if(logHandle < 0)
	{
		printf("create log file failed\n");
		return false;
	}
	return true;
}

void CLog::Log(int level, const char * loginfo)
{
	const char *infos[4] = {"ERROR", "INFO", "WARN", "DEBUG"};
	m_mutex.Lock();
	checkLogTime();
	char logbuf[256];
	if(logHandle > 0)
	{
		ctime.updateTime();
		sprintf(logbuf, "[%s]%s :%s\n", ctime.timeToStr().c_str(), infos[level], loginfo);
		int handle = write(this->logHandle, logbuf, strlen(logbuf));
        if(handle < 0)
		{
			printf("write log file err\n");
		}
	}
	m_mutex.UnLock();
}

void CLog::Error(char *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_ERROR, Buf);
}

void CLog::Info(char  *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_INFO, Buf);	
}

void CLog::Debug(char *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_DEBUG, Buf);	
}

void CLog::Warn(char  *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);      
	Log(LOG_WARN, Buf);	
}


bool CLog::checkLogTime()
{
	if(difftime(time(NULL), starttime) >= 0)
	{
		memset(this->m_logfile, 0, sizeof(this->m_logfile));
		ctime.updateTime();
		string timestr = ctime.timeToDay();
		sprintf(m_logfile, "%s/%s.log", m_logpath, timestr.c_str());
		int handle = open(m_logfile, O_CREAT | O_RDWR | O_APPEND, 644);
        if(handle > 0)
		{
			close(logHandle);
			this->logHandle = handle;
		}
		ctime.setMday(ctime.getMday()+1);
		ctime.setHour(0);
		ctime.setMin(0);
		ctime.setSec(0);
		this->starttime = ctime.timeSec();
		return true;
	}

	return false;
}
