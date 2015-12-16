#include <iostream>
#include <set>
#include <string>
using namespace std;
using std::set;

struct MySetItem
{
	string str;
	MySetItem(const char* cstr):str(cstr)
	{}
	// bool operator < (const MySetItem& item) const //自定义排序函数
	// {
	// 	return (str < item.str);		
	// }
};

struct MyComp //自定义排序函数
{
	bool operator()(const MySetItem& a, const MySetItem& b)
	{
		return (a.str < b.str);
	}
};
int main(int argc, char *argv[])
{
    MySetItem item1("aaaa");
	MySetItem item2("vasfa");
	MySetItem item3("sadf");
	MySetItem item4("ggasfa");
	MySetItem item5("heerga");
	MySetItem item6("g333");
	MySetItem item7("g333");

	// set<MySetItem> iset;
	set<MySetItem, MyComp> iset;
	iset.insert(item1);
	iset.insert(item2);
	iset.insert(item3);
	iset.insert(item4);
	iset.insert(item5);
	iset.insert(item6);
	iset.insert(item7);

	//set<MySetItem>::iterator iter = iset.begin();
	set<MySetItem, MyComp>::iterator iter = iset.begin();
	for(;iter != iset.end(); ++iter)
	{
		cout << (*iter).str << endl;
	}
    return 0;
}


