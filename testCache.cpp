#include "CCache.h"

int main(int argc, char *argv[])
{

	LRUCache cache;
	const char* key1 = "aaa";
	const char* value1 = "bbbb";
	const char* key2 = "bbb";
	const char* value2 = "cccc";
	const char* key3 = "bbb";
	const char* value3 = "dddd";
	const char* key4 = "eee";
	const char* value4 = "fff";
	const char* key5 = "111";
	const char* value5 = "fff11";
	const char* key6 = "121";
	const char* value6 = "ff211";
	cache.insert(key1, (void*)value1);
	cache.insert(key2, (void*)value2);
	cache.insert(key3, (void*)value3);
	cache.insert(key4, (void*)value4);
	cache.insert(key5, (void*)value5);
	cache.insert(key6, (void*)value6);

	
	if(cache.search("121", (void*)"fff211"))
	{
		cout << "OK\n";
	}
	else
		cout <<"not exist\n";
	cache.print();
    cout<< "================\n";
	cache.erase(key2, (void*)value2);
    cache.print();


	return 0;
}
