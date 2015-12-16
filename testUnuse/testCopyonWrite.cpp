#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[])
{
    string str1 = "hello world\n";
	string str2 = str1;

	printf("str1:%x  str2:%x\n", str1.c_str(), str2.c_str());
	str2.clear();
	str2 = "hello\n";
	printf("str1:%x  str2:%x\n", str1.c_str(), str2.c_str());
    return 0;
}
