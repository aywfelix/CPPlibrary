#include "CQueue.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    CList *list = new CList(5);
	list->putItem((void*)"haha");
	list->putItem((void*)"sda");
	list->putItem((void*)"sgaga");
	for(int i=0;i<list->getCount(); ++i)
		cout << (char*)list->getItem(i)<<" ";
	cout << "\n";

	CCircleQ *CQ = new CCircleQ(5);
	CQ->push((void*)"gaadasda");
	CQ->push((void*)"egargara");
	CQ->push((void*)"efdgzdrgs");
	for(int i=0;i<CQ->getLength(); ++i)
		cout << (char*)CQ->pop()<<" ";
	cout << "\n";
    return 0;
}
