#include "ThreadPool.h"

int main(int argc, char *argv[])
{
	CThreadManage* manage = new CThreadManage(10);
    for(int i=0;i<40;i++)
    {
        CXJob*   job = new CXJob();
        manage->Run(job,NULL);
    }
    sleep(2);
    CYJob* job = new CYJob();
    manage->Run(job,NULL);
    manage->TerminateAll();
	
    return 0;
}
