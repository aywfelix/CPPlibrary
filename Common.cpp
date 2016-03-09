#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <utime.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <wchar.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <cerrno>
#include <ctime>
#include <climits>
#include <cstdarg>
#include <cassert>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

//stdint.h
typedef signed char           int8_t;
typedef signed short          int16_t;
typedef signed int            int32_t;
typedef signed long long      int64_t;
typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int          uint32_t;
typedef unsigned long long    uint64_t;

typedef long int               intptr_t;
typedef unsigned long int    uintptr_t;
typedef int                    intptr_t;
typedef unsigned int        uintptr_t;

#define MAX(a, b) (a)>(b)?(a):(b);
#define MIN(a, b) (a)<(b)?(a):(b);

#define CLR(a, b) memset((a), 0, (b))  
const static int MAX_LEN = 256;
#define SYSSLEEP(stop, interval) ({int count=interval*5; while(count-->0 && !stop) usleep(200000);})

//the log level
enum { LOG_ERROR = 0, LOG_INFO, LOG_WARN, LOG_DEBUG};

//把第pos位设置为1：
//#define SETBIT(Number, pos) ((Number) |= (1 << (pos)))
inline void SETBIT( int &Number, int pos)
{
	Number |= 1 << pos;
}
//把第pos位设置为0：
//#define CLRBIT(Number, pos) ((Number) &= ~(1 << (pos)))
inline void CLRBIT(int &Number,int pos)
{
	Number &= ~(1 << (pos));
}

//获取第pos位的值：
//#define GETBIT(Number, pos) ((Number) >> (pos) & 1))
inline int GETBIT(int Number,int pos)
{
	return Number >> (pos) & 1;
}
