#include <iostream>  
#include <cstdio>  
#include <cstdlib>  
#include <cstring>  
#include <string>  
  
class CString  
{  
public:  
    CString();  
    CString(const char *cstr);  
    CString(const std::string& sstr);  
      
    CString(const CString& cstr);  
    CString& operator=(const CString& cstr);  
    CString& operator=(const char* cstr);  
    CString& operator=(const std::string& sstr);  
  
    operator const char*()  
    {  
        return m_str.c_str();  
    }  
    const CString operator+=(const CString& cstr)  
    {  
        m_str = m_str + cstr.m_str;  
        return CString(m_str);  
    }  
    const CString operator+=(const char *cstr)  
    {  
        m_str = m_str + cstr;  
        return CString(m_str);  
    }  
friend CString operator+(const CString& cstr1, const CString& cstr2)  
    {  
        std::string str = cstr1.m_str + cstr2.m_str;  
        return CString(str);  
    }  
   
    virtual ~CString();  
  
    //void SetParam()  
    void print()  
    {  
        std::cout << m_str << std::endl;  
    }  
private:  
    std::string m_str;  
};  

CString::CString():m_str(NULL)  
{}  
  
CString::CString(const char *cstr):m_str(cstr)  
{}  
  
CString::CString(const std::string& sstr):m_str(sstr)  
{}  
  
CString::CString(const CString& cstr)  
{  
    m_str = cstr.m_str;  
}  
  
CString& CString::operator=(const CString& cstr)  
{  
    if(this == &cstr)  
        return *this;  
    else  
    {  
        m_str = cstr.m_str;  
        return *this;         
    }  
}  
  
CString& CString::operator=(const char *cstr)  
{  
    m_str = cstr;  
    return *this;  
}  
  
CString& CString::operator=(const std::string& sstr)  
{  
    m_str = sstr;  
    return *this;  
}  
  
CString::~CString()  
{  
    if(!m_str.empty())  
    {  
        m_str.clear();  
    }  
          
}  
