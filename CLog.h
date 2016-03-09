#include "Mutex.h"
#include "CTime.h"
#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <fstream>

#define LOGPATH "./common.log"
#define BUFSIZE 2048
class CLog
{
public:
	CLog(){}
	virtual ~CLog()
	{
		if(fout)
		    fout.close();
	}
	static CLog* getInstance();
	void Init(const char* path, int level);

	void Error(char *pFmt, ...);
    void Info(char  *pFmt, ...);
    void Debug(char *pFmt, ...);
    void Warn(char  *pFmt, ...);     

    void Log(int level, const char * loginfo);
	void setLogFile(const char* path);
private:

	static CLog *m_log;
	CLog(const CLog& clog);
	CLog& operator=(const CLog& clog);

	Mutex m_mutex;
	int m_logLevel;
	const char* m_logPath;
	std::ofstream fout;	
	enum { LOG_ERROR = 0, LOG_INFO, LOG_WARN, LOG_DEBUG};
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

void CLog::Init(const char* path, int level)
{
	m_logPath = path;
	m_logLevel = level;
}

void CLog::Log(int level, const char * loginfo)
{
	const char *infos[4] = {"ERROR: ", "INFO: ", "WARN : ", "DEBUG: "};
	m_mutex.Lock();
	CTime time;
	//std::ofstream fout(m_logPath, std::ios::app|std::ios::out);
	if(fout)
	{
		fout<<"["<<time<<"]"<<infos[level]<<loginfo<<std::endl;
		if(fout.tellp() > 2*1024*1024)
		{
			fout.close();
			std::string logname = m_logPath + time.getTime(); //back the log file
			rename(m_logPath, logname.c_str());
		}
	}
	else
	{
		std::cout << time << infos[0] <<"open file failed \n"<<strerror(errno) << std::endl;
	}
	fout.flush();
	//if(fout) fout.close();
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

void CLog::setLogFile(const char* path)
{
	m_logPath = path;
	fout.open(m_logPath, std::ios::app|std::ios::out);
}
