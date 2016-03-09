#include <iostream>
#include <string>
#include <sys/stat.h> 

bool isDir(std::string& dirpath);
bool createMkdir(std::string& dirpath);

bool isDir(std::string& dirpath)
{
	struct stat stats;
	if(lstat(dirpath.c_str(), &stats) && S_ISDIR(stats.st_mode))
		return true;
	else
		return false;
}


bool createMkdir(std::string& dirpath)
{
	mode_t umask_value = umask (0);
	umask (umask_value);
	mode_t mode = ((S_IRWXU | S_IRWXG | S_IRWXO) & (~ umask_value)) | S_IWUSR | S_IXUSR;
	std::string path;
	int index;
	int pos = dirpath.find("/");
	path = dirpath.substr(0, ++pos);
	std::cout << path << "\n";
	while(!isDir(path))
	{
		if(!mkdir(path.c_str(), mode))
			return false;
		index = dirpath.find("/", pos);
		if(index == std::string::npos)
		{
			break;
		}
		path += dirpath.substr(pos, index-pos+1);
		//std::cout << path << "\n";
		pos = ++index;
	}
	return true;
}

int main(int argc, char *argv[])
{
	std::string path = "haha/hehe/xinxi/log";
	std::string file = path.substr(path.find_last_of('/')+1);
	std::cout << file << "\n";
	createMkdir(path);
	
    return 0;
}
