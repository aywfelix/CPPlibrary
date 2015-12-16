#include "CDataBuffer.h"
using namespace std;

int main(int argc, char *argv[])
{
    CDataBuffer databuffer;
	uint64_t a = 12345678;
	databuffer.writeInt64(a);
	uint32_t b = 1234;
	databuffer.writeInt32(b);

	int len = databuffer.getDataLen();
	cout << len << endl;
	uint64_t aa = databuffer.readInt64();
	uint32_t bb = databuffer.readInt32();

	cout << aa << " :" <<bb <<"\n";

	//不能连续存储在读取,因为没有记录具体存储的数据长度
	databuffer.writeVarInt32(b);
	bb = databuffer.readVarInt32();
	databuffer.writeVarInt64(a);
	aa = databuffer.readVarInt64();

	cout << bb <<":"  << aa << endl;
    return 0;
}
