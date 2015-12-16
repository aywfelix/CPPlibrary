#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

typedef unsigned int uint32_t;
uint32_t DecodeFixed32(const char* data)
{
	uint32_t res;
	memcpy(&res, data, sizeof(res));
	return res;
}
uint32_t Hash(const char* data, uint32_t n, uint32_t seed)
{
	const uint32_t m = 0xc6a4a793;
	const uint32_t r = 24;
	const char* limit = data + n;
	uint32_t h = seed ^ (n * m);

	// Pick up four bytes at a time
	while (data + 4 <= limit) {
		uint32_t w = DecodeFixed32(data);
		data += 4;
		h += w;
		h *= m;
		h ^= (h >> 16);
	}

	// Pick up remaining bytes
	switch (limit - data) {
    case 3:
		h += data[2] << 16;
		// fall through
    case 2:
		h += data[1] << 8;
		// fall through
    case 1:
		h += data[0];
		h *= m;
		h ^= (h >> r);
		break;
	}
	return h;
}

int main(int argc, char *argv[])
{
    string str = "aabbgggsefqafdgargaargargargerwfqew";
	uint32_t res = Hash(str.data(), str.size(), 0);
	cout <<(res>>28) <<endl;
	cout << (10&6) << endl;

	cout << sizeof(void*) << endl;
	int aglin = 2*sizeof(void*); 
	for(int n=0; n<20;++n)
	{
		//int n = ;
		int p = (n & (aglin-1));
		p = (p==0)?0:aglin-p;
		cout << p <<" " << n<<" "<<(n &(aglin-1)) << endl;
	}

	//set 8 bit aglin
	
    return 0;
}










