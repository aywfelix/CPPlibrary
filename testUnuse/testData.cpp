#include <iostream>
#include <cstdio>
#include <stdint.h>
using namespace std;

int main(int argc, char *argv[])
{
	// void* data = "85899320189261291454";
	// char* p = static_cast<char*>(data);
	char* data = "858993201";
	char* p = data;
	int32_t* t1 = (int32_t*)p;
	p += sizeof(int32_t);
	int32_t* t2 = (int32_t*)p;
	p += sizeof(int32_t);
	int32_t* t3 = (int32_t*)p;
	p += sizeof(int32_t);

	cout << t1 << " "<< t2 << " "<< t3<< endl;
    return 0;
}
