#include "CHash.h"
#include <iostream>
#include <cstdio>
#include <cstring>

int main(int argc, char *argv[])
{
	char* haha = "ssaaerawegfwqe";
	unsigned int hashcode = CHash::HashB(haha, strlen(haha));
	std::cout << hashcode << "\n";
    return 0;
}
