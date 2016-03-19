#include "CBase64.h"    
#include <iostream>
#include <string.h>
#include "Base64.h"
using namespace std;  
  
int main()  
{  
    unsigned long len = 0;  
  
    unsigned char pIn[100];  
    unsigned char pOut[200];  

	char  str[] = "laijia200815122840452";
    memcpy(pIn,str, sizeof(str));  
  
    string strout;  
  
    cout<<(char*)pIn<<endl;  
    if(CBase64::Encode(pIn, strlen(str), strout))  
        cout<<strout<<endl;  
    else  
        cout<<"encode err"<<endl;  
  
    string stroIn = strout;  
    cout<<stroIn<<endl;  
  
    memset(pOut,0,sizeof(pOut));  
  
    if(CBase64::Decode(stroIn, pOut, &len))  
    {  
        cout<<(char *)pOut<<endl;  
        cout<<"len="<<strlen((char *)pOut)<<endl;  
    }  
    else  
        cout<<"deconde err"<<endl;  
  
    cout<<len<<endl;  
	const std::string s = "laijia200815122840452" ;  
  
	std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());  
	std::string decoded = base64_decode(encoded);  
  
	std::cout << "encoded: " << encoded << std::endl;  
	std::cout << "decoded: " << decoded << std::endl;  
    return 0;  
}  
