#include "CBase64.h"    
#include <iostream>
#include <string.h>

using namespace std;  
  
int main()  
{  
    unsigned long len = 0;  
  
    unsigned char pIn[100];  
    unsigned char pOut[200];  

	char  str[] = "hello world";
    memcpy(pIn,str, sizeof(str));  
  
    string strout;  
  
    cout<<(char*)pIn<<endl;  
    if(CBase64::Encode(pIn, sizeof(str), strout))  
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
  
    return 0;  
}  
