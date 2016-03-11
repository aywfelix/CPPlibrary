/** 
 * (C) Copyright 2009, asiainfo 
 * @version  v1.0 
 * @author    
 * @brief    base64编码和解码 
 * 
 * history: 
 * <p> 2009-02-17 1.0 build this moudle</p> 
 */  
  
#ifndef ___BASE64_H___  
#define ___BASE64_H___  
   
#include <string>  
  
using namespace std;  
  
class CBase64  
{  
public:  
    CBase64();  
    ~CBase64();  
  
    /********************************************************* 
	 * 函数说明：将输入数据进行base64编码 
	 * 参数说明：[in]pIn      需要进行编码的数据 
                [in]uInLen  输入参数的字节数 
                [out]strOut 输出的进行base64编码之后的字符串 
				* 返回值  ：true处理成功,false失败  
				**********************************************************/  
    bool static Encode(const unsigned char *pIn, unsigned long uInLen, string& strOut);  
  
    /********************************************************* 
	 * 函数说明：将输入数据进行base64编码 
	 * 参数说明：[in]pIn          需要进行编码的数据 
                [in]uInLen      输入参数的字节数 
                [out]pOut       输出的进行base64编码之后的字符串 
                [out]uOutLen    输出的进行base64编码之后的字符串长度 
				* 返回值  ：true处理成功,false失败 
				**********************************************************/  
    bool static Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);  
      
    /********************************************************* 
	 * 函数说明：将输入数据进行base64解码 
	 * 参数说明：[in]strIn        需要进行解码的数据 
                [out]pOut       输出解码之后的节数数据 
                [out]uOutLen    输出的解码之后的字节数长度 
				* 返回值  ：true处理成功,false失败 
				**********************************************************/  
    bool static Decode(const string& strIn, unsigned char *pOut, unsigned long *uOutLen) ;  
  
    /********************************************************* 
	 * 函数说明：将输入数据进行base64解码 
	 * 参数说明：[in]strIn        需要进行解码的数据 
                [out]pOut       输出解码之后的节数数据 
                [out]uOutLen    输出的解码之后的字节数长度 
				* 返回值  ：true处理成功,false失败  
				**********************************************************/  
    bool static Decode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen) ;  
};  
  
#endif // ___BASE64_H___  

/* 
 * 
 *Base64? 
 * 
 * 
 * 
 */  
  
//#include "Base64.h"  
  
static const char *g_pCodes =  
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";  
  
static const unsigned char g_pMap[256] =  
	{  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
		52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
		255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
		7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
		19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
		255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
		37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
		49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
		255, 255, 255, 255  
	};  
  
CBase64::CBase64()  
{  
}  
  
CBase64::~CBase64()  
{  
}  
  
bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen)  
{  
    unsigned long i, len2, leven;  
    unsigned char *p;  
  
    if(pOut == NULL || *uOutLen == 0)  
        return false;  
  
    //ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));  
  
    len2 = ((uInLen + 2) / 3) << 2;  
    if((*uOutLen) < (len2 + 1)) return false;  
  
    p = pOut;  
    leven = 3 * (uInLen / 3);  
    for(i = 0; i < leven; i += 3)  
    {  
        *p++ = g_pCodes[pIn[0] >> 2];  
        *p++ = g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];  
        *p++ = g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];  
        *p++ = g_pCodes[pIn[2] & 0x3f];  
        pIn += 3;  
    }  
  
    if (i < uInLen)  
    {  
        unsigned char a = pIn[0];  
        unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;  
        unsigned char c = 0;  
  
        *p++ = g_pCodes[a >> 2];  
        *p++ = g_pCodes[((a & 3) << 4) + (b >> 4)];  
        *p++ = ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';  
        *p++ = '=';  
    }  
  
    *p = 0; // Append NULL byte  
    *uOutLen = p - pOut;  
    return true;  
}  
  
bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, string& strOut)   
{  
    unsigned long i, len2, leven;  
    strOut = "";  
  
    //ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));  
  
    len2 = ((uInLen + 2) / 3) << 2;  
    //if((*uOutLen) < (len2 + 1)) return false;  
  
    //p = pOut;  
    leven = 3 * (uInLen / 3);  
    for(i = 0; i < leven; i += 3)  
    {  
        strOut += g_pCodes[pIn[0] >> 2];  
        strOut += g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];  
        strOut += g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];  
        strOut += g_pCodes[pIn[2] & 0x3f];  
        pIn += 3;  
    }  
  
    if (i < uInLen)  
    {  
        unsigned char a = pIn[0];  
        unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;  
        unsigned char c = 0;  
  
        strOut += g_pCodes[a >> 2];  
        strOut += g_pCodes[((a & 3) << 4) + (b >> 4)];  
        strOut += ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';  
        strOut += '=';  
    }  
  
    //*p = 0; // Append NULL byte  
    //*uOutLen = p - pOut;  
    return true;  
}  
  
bool CBase64::Decode(const string& strIn, unsigned char *pOut, unsigned long *uOutLen)   
{  
    unsigned long t, x, y, z;  
    unsigned char c;  
    unsigned long g = 3;  
  
    //ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));  
  
    for(x = y = z = t = 0; x < strIn.length(); x++)  
    {  
        c = g_pMap[strIn[x]];  
        if(c == 255) continue;  
        if(c == 254) { c = 0; g--; }  
  
        t = (t << 6) | c;  
  
        if(++y == 4)  
        {  
            //if((z + g) > *uOutLen) { return false; } // Buffer overflow  
            pOut[z++] = (unsigned char)((t>>16)&255);  
            if(g > 1) pOut[z++] = (unsigned char)((t>>8)&255);  
            if(g > 2) pOut[z++] = (unsigned char)(t&255);  
            y = t = 0;  
        }  
    }  
  
    *uOutLen = z;  
    return true;  
}

