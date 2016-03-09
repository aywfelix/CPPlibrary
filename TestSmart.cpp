#include "SmartPtr.h"
#include <iostream>

class Test :public CRef
{
public:
	Test():data(0)
	{
		std::cout << "test::test invoke\n";
	}
    Test(int a):data(a)
	{
		std::cout << "test::test invoke\n";
	}
    virtual ~Test()
	{
		std::cout << "test::~test() invoked\n";
	}
private:
	int data;
};

int main(int argc, char *argv[])
{
    // Test *t = new Test(10);
	// SmartPtr<Test> p(t);
	// SmartPtr<Test> p2(p);
	// SmartPtr<Test> p3 = p2;
	SmartPtr<Test> t;
    return 0;
}

