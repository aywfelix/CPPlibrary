#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>

std::string Ltrim(std::string& str)
{
	if(str.empty())
	{
		return str;
	}
	
	return str.erase(0, str.find_first_not_of(" "));
}

std::string& Rtrim(std::string& str)
{
	if(str.empty())
		return str;

	return str.erase(str.find_last_not_of(" ")+1);
}

std::string& Trim(std::string& str)
{
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ")+1);
	return str;
}

void split_str(const char* str, const char* delim, std::vector<std::string>& values)
{
	if (NULL == str)
	{
		return;
	}
	if (NULL == delim)
	{
		values.push_back(str);
	}
	else
	{
		const char* pos = str + strspn(str, delim);
		size_t len = 0;
		while ((len = strcspn(pos, delim)) > 0)
		{
			values.push_back(std::string(pos, len));
			pos += len;
			pos += strspn(pos, delim);
		}
	}
}

char *conv_show_string(char *str, int size, char *ret = NULL, int msize = 0) 
{
	int index = 0;
	if (ret == NULL) {
		msize = size*3+5;
		ret = (char*) malloc(msize);
	}
	unsigned char *p = (unsigned char *)str;
	while (size-->0 && index<msize-4) {
		index += sprintf(ret+index, "\\%02X", *p);
		p ++;
	}
	ret[index] = '\0';
	return ret;
}

char *bin2ascii(char *str, int size, char *ret = NULL, int msize = 0) {
	if (ret == NULL) {
		msize = size * 3 + 5;
		ret = (char*) malloc(msize);
	}
	unsigned char *p = (unsigned char*)str;
	int i = 0;
	while (size-- > 0 && i < msize - 4) {
		if (*p >= '!' && *p <= '~') { //~ printable, excluding space
			i += sprintf(ret + i, "%c", *p++);
		} else {
			i += sprintf(ret + i, "\\%02X", *p++);
		}
	}
	ret[i] = '\0';
	return ret;
}
void conv_raw_string(const char *str, char *result, int *size)
{
	int index = 0;
	const unsigned char *p = (const unsigned char *)str;
	while(*p && index<(*size)-1) {
		if (*p == '\\') {
			int c1 = *(p+1);
			int c2 = *(p+2);
			if (c1==0 || c2==0) break;
			if (isupper(c1)) c1 = tolower(c1);
			int value = (c1 >= '0' && c1 <= '9' ? c1 - '0' : c1 - 'a' + 10) * 16;
			if (isupper(c2)) c2 = tolower(c2);
			value += c2 >= '0' && c2 <= '9' ? c2 - '0' : c2 - 'a' + 10;
			result[index++] = (char)(value&0xff);
			p += 2;
		} else {
			result[index++] = *p;
		}
		p ++;
	}
	*size = index;
}

class CConfig
{
public:
	static std::string getValueString(std::string configFile, std::string section, std::string key, std::string defaultvalue);
	static int getValueInt(std::string configFile, std::string section, std::string key, int defaultvalue);
	static bool getValueBool(std::string configFile, std::string section, std::string key, bool defaultvalue);
};

std::string CConfig::getValueString(std::string configFile, std::string section, std::string key, std::string defaultvalue)
{
	if(configFile.empty() || section.empty() || key.empty())
	{
		return defaultvalue;
	}

	std::fstream fin(configFile.c_str(), std::ios::in);
	if(!fin)
	{
		return defaultvalue;
	}
	char buf[256];
	bool isFind = false;
	int index = 0;
	std::string strline, value, cursecton, curkey;
    while(!fin.eof())
	{
		fin.getline(buf, 256);
		strline = buf;
		Trim(strline);
		switch(strline[0])
		{
		case '#':
		case ';':
			continue;
		case '[':
			index = strline.find_first_of(']');
			if(index == std::string::npos)
				continue;
            cursecton = strline.substr(1, index -1);
			Trim(cursecton);
			break;
		default:
			if(cursecton != section)
				continue;
			index = strline.find_first_of('=');
			curkey = strline.substr(0, index);
			Trim(curkey);
			if(curkey == key)
			{
				value = strline.substr(index+1);
				Trim(value);
				isFind = true;
			}
		}
	}
	fin.close();
	if(isFind)		
		return value;
	else
		return defaultvalue;
} 

int CConfig::getValueInt(std::string configFile, std::string section, std::string key, int defaultvalue)
{
	char buf[256];
	sprintf(buf, "%d", defaultvalue);
	return atoi(getValueString(configFile, section, key, buf).c_str());
}

bool CConfig::getValueBool(std::string configFile, std::string section, std::string key, bool defaultvalue)
{
	std::string res;
	res = getValueString(configFile, section, key, defaultvalue?"true":"false");
	if(res == "true")
		return true;
    else
		return false;
}
