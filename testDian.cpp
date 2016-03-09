#include <iostream>
using namespace std;

void IsBigEndian()
{
    short int a = 0x1122;//十六进制，一个数值占4位
    char b =  *(char *)&a;  //通过将short(2字节)强制类型转换成char单字节，b指向a的起始字节（低字节）
    if( b == 0x11)//低字节存的是数据的高字节数据
    {
        //是大端模式
		cout << "big" << "\n";
    }
    else
    {
        //是小端模式
		cout << "little\n";
    }
}

int main(int argc, char *argv[])
{
    IsBigEndian();
    return 0;
}
