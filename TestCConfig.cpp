#include "CConfig.h"

int main(int argc, char *argv[])
{
	//std::string configfile = "./config";
    Config.Load("./config");
#ifdef MAP
	Config.toString();
#endif
	// std::string v1 = Config.getValueString("test1", "key1", "sd");
	// std::cout << v1 << "\n";
	int v2 = Config.getValueInt("test2", "key2", 10);
	std::cout << v2 << "\n";
	std::vector<std::string> vec;
	Config.getStringList("test1", "key1", vec);
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		std::cout << *it <<" "<< "\n";
	}
    return 0;
}
