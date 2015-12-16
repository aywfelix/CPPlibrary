#include "CFile.h"
int main(int argc, char *argv[])
{
    std::string filename("./log");
	CFile *cFile = new CFile(filename);
	// cFile->openFile("a+");
	// char buf[20] = {'\0'};
	// cFile->readFile(buf, 10, 3);
	// std::cout <<buf <<"\n";
	// cFile->closeFile();
	cFile->openFile("at+");
	std::string writestr("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	cFile->writeFile(writestr);
	std::string writestr2("sdgagagaergrefzrew\n");
	cFile->writeFile(writestr2);
	cFile->syncFile(writestr.size() + writestr2.size());
	// std::cout << cFile->getFileSize() << "\n";
	cFile->closeFile();
	//const std::string newname("./clog");
	//cFile->reName(newname);
	//cFile->unlinkFile();

    return 0;
}


// #include "CThread.h"
// int main(int argc, char *argv[])
// {
//     // T t;
// 	// CThread *thread = new CThread();
// 	// thread->Start(&t, NULL);
// 	// std::cout << "OK\n";
// 	// thread->Stop();
// 	// sleep(1);
// 	// T t2;
// 	// thread->Start(&t2, NULL);
// 	// thread->Stop();
// 	// sleep(1);

// 	T *t = new T(5);
//     //t->SetThreadsCount(5);
// 	t->Start();
// 	t->Stop();
// 	sleep(2);
//     return 0;
// }

// #include "CTime.h"

// int main(int argc, char *argv[])
// {
//     CTime time;
// 	std::cout << time;
// 	time.Start();
// 	sleep(5);
// 	time.Stop();
// 	time.costTime();	
//     return 0;
// }
