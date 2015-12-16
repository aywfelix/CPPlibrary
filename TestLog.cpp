#include "CLog.h"

CLog *cLog = CLog::getInstance();
int main(int argc, char *argv[])
{	
	cLog->Init("./common.log", 0);
	cLog->Info("test the program");
	cLog->Error("err\n");
    return 0;
}
