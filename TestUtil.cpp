#include "CUtil.h"

int main(int argc, char *argv[])
{
	std::string str = "    haha  dd  ";
	std::cout << str.size() << "\n";
	std::cout << Ltrim(str) << std::endl;
	std::cout << str.size() << "\n";
	std::cout << Rtrim(str) << std::endl;
	std::cout << str.size() << "\n";

	std::string str2 = "haha=[hehe]lala";
	// int start = str2.find_first_of('[');
	// int end = str2.find_first_of (']');
	// if(start !=std::string::npos && end > start)
	// {
	//        str2 = str2.substr(start+1,end -start-1);
		
	// 	std::cout << start << " "<< end <<" "<<str2 << "\n";
	// }
    int pos = str2.find("=");
	std::string str1 = str2.substr(0, pos);
	std::cout << str1 <<"\n";
	
	std::string v1 = CConfig::getValueString("./config", "test1", "key1", "sd");
	int v2 = CConfig::getValueInt("./config", "test2", "key2", 9);
	bool v3 = CConfig::getValueBool("./config", "test3","key3", true);

	std::cout << v1 << " "<< v2 << " " << v3 << "\n";
    return 0;
}



