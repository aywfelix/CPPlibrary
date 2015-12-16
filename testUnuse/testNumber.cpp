#include <iostream>
#include <stdint.h>
#include <string.h>
using namespace std;

int main(int argc, char *argv[])
{
    uint32_t value = 122342124;
	uint32_t revalue;
	char buf[4];
	memcpy(buf, &value, sizeof(value));

    cout << buf << endl;
	memcpy(&revalue, buf, sizeof(buf));
	cout << revalue << endl;
    return 0;
}
