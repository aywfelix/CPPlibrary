#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

class CFile
{
public:
	CFile(const std::string& filename);
    virtual ~CFile();

	bool openFile(const char* filemode);
    int getFd() { return fd;}
	int getFileSize();
	bool writeFile(const std::string& data);
	void closeFile();
	bool unlinkFile();
	std::string getFileName(){ return filename;}
	bool checkFile();
	bool reName(const std::string& newname);	
	void readFile(void* buf, int size, int offset);
	void* getMapPtr() { return mapptr;}
	void syncFile(int size) {msync((char*)mapptr+mapsize-size, size, MS_ASYNC);}
	int getFileCreatTime();
    int flushFile()
	{
		if(fd > 0)
			fsync(fd);
	}
	int flushData()
	{
		if(fd > 0)
			fdatasync(fd);
	}
	int pread(void* buffer, size_t size, off_t offset)
	{
		return::pread(fd, buffer, size, offset);
	}
	int pwriteFile(void* buffer, size_t size, off_t offset)
	{
		return::pwrite(fd, buffer, size, offset) == (ssize_t) size;
	}
	static bool createMkdir(std::string& dirpath);
	static bool isDir(std::string& dirpath);
	
private:	
   	bool unmapFile()
	{
		if(mapptr)
		{
			if(munmap(mapptr, maxsize) !=0)
			{
				return false;
			}			
		}
		return true;
    }
	bool mmapFile()
	{
		mapptr = mmap(0, maxsize, PROT_READ|PROT_WRITE, MAP_SHARED,fd, 0);
		if(mapptr == NULL)
		{std::cout<< "map file err\n"; return false;}
		return true;
    }
	off_t getPosition()
	{
		return lseek(fd, 0, SEEK_END);
	}
private:
	int fd;
	FILE* file;
	void* mapptr;
	unsigned long int offset;
	size_t mapsize, maxsize;
	const std::string filename;
    CFile(const CFile&);
	CFile& operator=(const CFile&);
};

CFile::CFile(const std::string& filename):filename(filename),mapsize(0),maxsize(65535),file(NULL)
{}
CFile::~CFile()
{
	if(file)
		closeFile();
}

void CFile::closeFile()
{
	if(fd <0) return;
	if (!unmapFile()) {
		std::cout << "unmap err\n";
	}   
    if(fd >0) close(fd);
	fd = -1;  
	file = NULL;
}

bool CFile::openFile(const char* filemode) //"at+"
{
	if(!checkFile())
	{
		std::cout << "file not exists\n";
		return false;
	}
	file = fopen(filename.c_str(), filemode);
	if(file == NULL)
	{
		std::cout << "open file err\n";
		return false;
	}
	else
	{
		fd = fileno(file);
		mapsize = getPosition();
	}
	return true; 
}

int CFile::getFileSize()
{
	struct stat filestat;
	fstat(fd, &filestat);
	openflag = filestat.st_mode;
	return filestat.st_size;
	// long int curpos,length;
	// curpos = ftell(file);
	// fseek(file, 0L,SEEK_END);
	// length = ftell(file);
	// fseek(file,curpos, SEEK_SET);
	// return length;
}

bool CFile::writeFile(const std::string& data)
{
	size_t datasize = data.size();
	int fileLen = mapsize;
	int offset = datasize + fileLen;
	if(offset >= maxsize)
	{
	    closeFile();
		reName(filename+std::string(".bk"));
		if(!checkFile())
			return false;
	}
	if(fd < 0)
	{	std::cout<< "no open the file\n"; return false;}
	if(ftruncate(fd, offset) < 0)
	{
		return false;
	}		

	mmapFile();
	char *ptr = reinterpret_cast<char*>(mapptr);
	memcpy(ptr+fileLen, data.c_str(), datasize);
	mapsize = offset;
	return true;  
}

bool CFile::unlinkFile()
{
	if(!checkFile())
	{
		return false;
	}
	closeFile();
	return remove(filename.c_str())==0?true:false;
}

bool CFile::checkFile()
{
	if(access(filename.c_str(), F_OK)==0)
		return true;
	else
	{
		int fd = open(filename.c_str(), O_CREAT);
		if(fd < 0)
		{
			std::cout << "create file err\n";
			return false;
		}
		close(fd);
	}
	return true;
}

bool CFile::reName(const std::string& newname)
{
	if(rename(filename.c_str(), newname.c_str())==0)
		return true;
	else
		return false;
}

void CFile::readFile(void* buf, int size, int offs)
{
	if(!checkFile())
	{
		std::cout << "read file err\n";
		return;
	}
	int length = getFileSize();
	if( offs > length)
	{
		std::cout << "offset number too big\n";
		return;
	}
	else
	{
		int readavail = 0;
	    if(size + offs > length)
		{
            readavail = length - offs;		
		}
		else
			readavail = size;
		void* ptr = mmap(NULL, readavail, PROT_READ | PROT_WRITE, MAP_SHARED,  
                         fd, 0);
		char *p = reinterpret_cast<char*>(ptr);		
		memcpy(buf, (void*)(p+offs), readavail);
	}
}

int CFile::getFileCreatTime()
{
	struct stat info;
	stat(filename.c_str(), &info);
	return info.st_ctime;
}

bool CFile::createMkdir(std::string& dirpath)
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

bool CFile::isDir(std::string& dirpath)
{
	struct stat stats;
	if(lstat(dirpath.c_str(), &stats) && S_ISDIR(stats.st_mode))
		return true;
	else
		return false;
}
