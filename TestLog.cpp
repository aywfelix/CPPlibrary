#include "CLog.h"

CLog *cLog = CLog::getInstance();
int main(int argc, char *argv[])
{	
	//cLog->Init("./common.log", 0);
	// cLog->setLogFile("./common.log");
	// cLog->Info("test the program");
	// cLog->Error("err");
	// cLog->Info("hahaha");
	cLog->Init("./log");
	cLog->Info("hahaha");
	cLog->Error("hehehe");
    return 0;
}
