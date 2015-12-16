#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include "CLog.h"

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

const static int MAX_LEN = 256;
//the log level
enum { LOG_ERROR = 0, LOG_INFO, LOG_WARN, LOG_DEBUG};
#define LogInfo(...) CLog::getInstance()->Info(LOG_INFO, __VA_ARGS__);
#define LogWarn(...) CLog::getInstance()->Warn(LOG_WARN, __VA_ARGS__);
#define LogDebug(...) CLog::getInstance()->Debug(LOG_DEBUG, __VA_ARGS__);
#define LogErr(...) CLog::getInstance()->Error(LOG_ERROR, __VA_ARGS__);
