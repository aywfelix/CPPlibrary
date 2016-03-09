#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <vector>

#ifdef MAP
#include <map>
using std::map;

typedef std::map<std::string, std::string> STR_STR_MAP;
typedef std::map<std::string, STR_STR_MAP*> STR_MAP;
typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
typedef STR_MAP::iterator STR_MAP_ITER;
#else
//#include "CHash.h"
#include <ext/hash_map>
using __gnu_cxx::hash_map;
struct str_hash{
    unsigned int operator()(const std::string& str)const
	{
		return __gnu_cxx::__stl_hash_string(str.c_str());
	}
};
// struct str_hash{
// 	unsigned int operator()(const std::string& str) const
// 	{
// 		return CHash::HashA(const_cast<char*>(str.c_str()), str.size());
// 	}
// };
typedef __gnu_cxx::hash_map<std::string, std::string, str_hash> STR_STR_MAP;
typedef __gnu_cxx::hash_map<std::string, STR_STR_MAP*, str_hash> STR_MAP;
typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
typedef STR_MAP::iterator STR_MAP_ITER;

#endif

struct char_equal{
    bool operator()(const char* s1, const char* s2) const
	{
		return (strcmp(s1, s2) == 0);
	}
};

struct string_equal{
	bool operator()(const std::string& s1, const std::string& s2) const
	{
		return (s1 == s2);
	}
};



#define Config CConfig::getCConfig()
class CConfig
{
public:
    CConfig(){}
    virtual ~CConfig()
	{
	    for(STR_MAP_ITER it = m_config.begin(); it != m_config.end(); ++it)
		{
			delete it->second;			
		}

	}

	bool Load(std::string configFile);
	std::string getValueString(std::string section, std::string key, std::string value);
	int getValueInt(std::string section, std::string key, int value);
	bool getValueBool(std::string section, std::string key, bool value);
	void getStringList(std::string section, std::string key, std::vector<std::string>& vec);
	void getIntList(std::string section, std::string key, std::vector<int>& vec);

	void toString();
	static CConfig& getCConfig();
private:
	std::string Trim(std::string& str)
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ")+1);
		return str;
	}
	bool isSection(std::string& str)
	{
		int start = str.find_first_of('[');
		int end = str.find_first_of(']');
		if(start !=std::string::npos && end > start)
		{
			str = str.substr(start+1, end-start-1);
			Trim(str);
			if(!str.empty())
				return true;
		}
		return false;
	}
	bool parseValue(std::string& str, std::string& key, std::string& value)
	{
		int pos = str.find("=");
		if(pos == std::string::npos)
			return false;
		key = str.substr(0, pos);
		Trim(key);
		value = str.substr(pos+1, str.size()-pos -1);
		Trim(value);
		if(!key.empty() && !value.empty())
		{
			return true;
		}
		return false;
	}
    
private:
	STR_MAP m_config;
};

static CConfig config;
CConfig& CConfig::getCConfig()
{
	return config;
}

bool CConfig::Load(std::string configFile)
{
	if(configFile.empty())
	{
		return false;
	}
	std::fstream fin(configFile.c_str(), std::ios::in);
	if(!fin)
	{
		std::cout << "open file err\n";
		return false;
	}

	char buf[256];
	STR_STR_MAP *m = NULL;
	std::string key, value;
	std::string strLine;
	while(!fin.eof())
	{
		fin.getline(buf, 256);
		if(buf == NULL || buf[0]=='#' || buf[0]==';' || buf[0]=='\0')
			continue;
		strLine = buf;
		Trim(strLine);
		if(isSection(strLine))
		{
		    STR_MAP_ITER it = m_config.find(strLine);
			if(it == m_config.end())
			{
				m = new STR_STR_MAP();
				m_config.insert(std::make_pair<std::string, STR_STR_MAP*>(strLine, m));
			}
			else
			{
			    m = it->second;
			}
			continue;
		}
		if(!parseValue(strLine, key, value))
			continue;
		if(m == NULL)
		{
			std::cout << "don't config the section\n";
			fin.close();
			return false;
		}
		STR_STR_MAP_ITER it2 = m->find(key);
		if(it2 == m->end())
		{
			m->insert(std::make_pair<std::string, std::string>(key, value));
		}
		else
		{
			it2->second += '\0';
			it2->second += value;
		}
		
	}
	fin.close();
	return true;
}

std::string CConfig::getValueString(std::string section, std::string key, std::string value)
{
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return value;
	STR_STR_MAP* m = it->second;
	if(m == NULL)
		return value;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return value;
	
	value = it2->second;
	return value;
}

int CConfig::getValueInt(std::string section, std::string key, int value)
{
	char buf[256];
	sprintf(buf, "%d", value);
	return atoi(getValueString(section, key, buf).c_str());
}

bool CConfig::getValueBool(std::string section, std::string key, bool value)
{
	std::string res;
	res = getValueString(section, key, value?"true":"false");
	if(res == "true")
		return true;
    else
		return false;
}

void CConfig::getStringList(std::string section, std::string key, std::vector<std::string>& vec)
{
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return;
	STR_STR_MAP *m = it->second;
	if(m == NULL)
		return;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return;
	else
	{
		std::string str = it2->second;
		int pos = str.find('\0');
		while( pos != std::string::npos)
		{
			vec.push_back(str.substr(0, pos));
			str = str.substr(pos+1);
			pos = str.find('\0');
		}
		vec.push_back(str);
	}
	
}
void CConfig::getIntList(std::string section, std::string key, std::vector<int>& vec)
{
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return;
	STR_STR_MAP *m = it->second;
	if(m == NULL)
		return;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return;
	else
	{
		std::string str = it2->second;
		int pos = str.find('\0');
		while( pos != std::string::npos)
		{
			vec.push_back(atoi((str.substr(0, pos).c_str())));
			str = str.substr(pos+1);
			pos = str.find('\0');
		}
		vec.push_back(atoi(str.c_str()));
	}
}

void CConfig::toString()
{
	STR_MAP_ITER it = m_config.begin();
	for(;it != m_config.end(); ++it)
	{
		std::cout <<"["<< it->first << "]"<<"\n";
		if(it->second != NULL)
		{
			STR_STR_MAP_ITER it2 = it->second->begin();
			for(; it2 != it->second->end(); ++it2)
			{
				std::cout << it2->first << " = " << it2->second << "\n";
			}
		}
	}	
}

