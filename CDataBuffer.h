#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// typedef signed char           int8_t;  
// typedef signed short          int16_t;  
// typedef signed int            int32_t;  
// typedef signed long long      int64_t;  
// typedef unsigned char         uint8_t;  
// typedef unsigned short        uint16_t;  
// typedef unsigned int          uint32_t;  
// typedef unsigned long long    uint64_t;

static bool LittleEndian = false;

class CDataBuffer
{
public:
    CDataBuffer()
	{
		pstart = pend = pdata = pfree = NULL;
	}
    virtual ~CDataBuffer()
	{
		if(pstart)
			free(pstart);
		pstart = pend = pdata = pfree = NULL;
	}

    int getDataLen()
	{
		return (pfree - pdata);
	}
	char* getData()
	{
		return (char*)pdata;
	}
	char* getFree()
	{
		return (char*)pfree;
	}
    int getFreeLen()
	{
		return (pend - pfree);
	}
    void pourData(int len)
	{
		pfree += len;
	}
	void stripData(int len)
	{
		pfree -= len;
	}
	void clear()
	{
	    pdata = pfree = pstart;
	}

	void writeInt8(uint8_t n)
	{
		expend(1);
		if(LittleEndian)
		{
			memcpy(pfree, &n, sizeof(n));
		}
		else
		{
			pfree[0] = n & 0xff;			
		}
		pfree += 1;
	}
	void writeInt16(uint16_t n)
	{
		expend(2);
		if(LittleEndian)
		{
			memcpy(pfree, &n, sizeof(n));
		}
		else
		{
			pfree[0] = n & 0xff;
			pfree[1] = (n >> 8) & 0xff;
		}
		pfree += 2;
	}
	void writeInt32(uint32_t n)
	{
		expend(4);
		if(LittleEndian)
		{
			memcpy(pfree, &n, sizeof(n));	
		}else
		{
			pfree[0] = n & 0xff;
			pfree[1] = (n >> 8) & 0xff;
			pfree[2] = (n >> 16) & 0xff;
			pfree[3] = (n >> 24) & 0xff;
		}
		pfree += 4;
	}
	void writeInt64(uint64_t n)
	{
		expend(8);
		if(LittleEndian)
		{
			memcpy(pfree, &n, sizeof(n));	
		}else
		{
			pfree[0] = n & 0xff;
			pfree[1] = (n >> 8) & 0xff;
			pfree[2] = (n >> 16) & 0xff;
			pfree[3] = (n >> 24) & 0xff;
			pfree[4] = (n >> 32) & 0xff;
			pfree[5] = (n >> 40) & 0xff;
			pfree[6] = (n >> 48) & 0xff;
			pfree[7] = (n >> 56) & 0xff;
		}
		pfree += 8;
	}

	void writeVarInt32(uint32_t n)
	{
		expend(4);
		static const int B = 128;
		if(n < (1<<7))
		{
			*pfree++ = n;
		}
		else if(n < (1<<14))
		{
			*pfree++ = n | B;
			*pfree++ = n >> 7;
		}
		else if(n < (1<< 21))
		{
			*pfree++ = n | B;
			*pfree++ = (n >> 7) | B;
			*pfree++ = n >> 14;
		}
		else if(n < (1<<28))
		{
			*pfree++ = n |B;
			*pfree++ = (n >> 7) | B;
			*pfree++ = (n >> 14) | B;
			*pfree++ = n >> 21;
		}
		else
		{
			*pfree++ = n | B;  
			*pfree++ = (n>>7) | B;  
			*pfree++ = (n>>14) | B;  
			*pfree++ = (n>>21) | B;  
			*pfree++ = n>>28;  
		}
	}
	void writeVarInt64(uint64_t n)
	{
		expend(8);
		while(n >= 128)
		{
			*pfree++ = (n & 127) | 128;  
			n >>= 7;  
		}
		*pfree++ = n;
	}
	uint8_t readInt8()
	{
		return (*pdata++);
		uint8_t res;
		if(LittleEndian)
		{
			memcpy(&res, pdata, sizeof(res));
		}
		else{
			res = static_cast<uint8_t>(static_cast<unsigned char>(pdata[0]));
		}
		pdata += 1;
		return res;
	}
	uint16_t readInt16()
	{
		uint16_t res;
		if(LittleEndian)
		{
			memcpy(&res, pdata, sizeof(res));
		}
		else{
			res = ((static_cast<uint16_t>(static_cast<unsigned char>(pdata[0])))
				   |(static_cast<uint16_t>(static_cast<unsigned char>(pdata[1]))<<8));				
		}
		pdata += 2;
		return res;
	}
	uint32_t readInt32()
	{
		uint32_t res;
		if(LittleEndian)
		{
			memcpy(&res, pdata, sizeof(res));
		}else{
			res = ((static_cast<uint32_t>(static_cast<unsigned char>(pdata[0])))
				   |(static_cast<uint32_t>(static_cast<unsigned char>(pdata[1]))<<8)
				   |(static_cast<uint32_t>(static_cast<unsigned char>(pdata[2]))<<16)
				   |(static_cast<uint32_t>(static_cast<unsigned char>(pdata[3]))<<24));
		}
		pdata += 4;
		return res;
	}
	uint64_t readInt64()
	{
		uint64_t res;
		if(LittleEndian)
		{
			memcpy(&res, pdata, sizeof(res));
		}else{
			uint64_t lo = readInt32();
			uint64_t hi = readInt32();
			res = (hi << 32) | lo;
		}
		return res;
	}

	void writeBytes(char* src, int len)
	{
		expend(len);
		memcpy(src, pdata, len);
		pfree += len;
	}
	bool readBytes(char* dst, int len)
	{
		if(pdata + len > pfree)
			return false;
		memcpy(dst, pdata, len);
		pdata += len;
		return true;
	}
	bool readString(char* dst, int len)
	{
		if(pdata + sizeof(int) > pfree)
			return false;
		int slen = readInt32();
		if(pfree - pdata < slen)
			slen = pfree - pdata;
		if(dst == NULL && slen > 0)
		{
			dst = (char*)malloc(slen);
			len = slen;
		}
		if(len >0 )
		{
			memcpy(dst, pdata, len);
			dst[len - 1]= '\0';
		}
		pdata += slen;
		return true;
	}

	int findBytes(const char* src, int len)
	{
		int dlen = pfree - pdata -len +1;
		int i= 0;
		while(i++ < dlen)
		{
			if(pdata[i] == src[0] && memcmp(pdata+i, src, len)==0)
				return i;
		}
		return -1;
	}
	
	uint32_t readVarInt32()
	{
		uint32_t res=0;
		for(uint32_t i=0; i<=28 && pdata < pfree; i += 7)
		{
			uint32_t tmp = *pdata;
			pdata++;
			if(tmp & 128)
			{
				res |= ((tmp & 127) << i);
			}
			else{
				res |= (tmp << i);
			}
		}
		return res;
	}

	uint64_t readVarInt64()
	{
		uint64_t res = 0;
		for(uint32_t i = 0; i <= 63 && pdata < pfree; i+= 7)
		{
			uint64_t tmp = *pdata;
			pdata++;
			if(tmp & 128)
			{
				res |= ((tmp & 127) << i);
			}
			else{
				res |= tmp << i;
			}
		}
		return res;
	}
private:
	void expend(int len)
	{
		if(pstart == NULL)
		{
			int tmplen = 256;
			while(tmplen < len)
			{tmplen <<=1;}
			pstart = pfree = pdata = (uint8_t*)malloc(tmplen);
			pend = pstart + tmplen;
		}
		else if(pend - pfree < len)
		{
			int flen = (pend - pfree) + (pdata - pstart);
			int dlen = pfree - pdata;
			if(flen < len || flen*4 < dlen)
			{
				int bufsize = (pend - pstart) <<1;
				while(bufsize-dlen < len) bufsize <<=1;
				uint8_t* pbuf = (uint8_t*)malloc(bufsize);
				if(dlen)
				{
					memcpy(pbuf, pdata, dlen);
				}
				free(pstart);
				pdata = pstart = pbuf;
				pfree = pstart+dlen;
				pend = pstart+bufsize;
			}else
			{
				memmove(pstart, pdata, dlen);
				pfree = pstart+dlen;
				pdata = pstart;
			}
		}	
	}
	
private:
	uint8_t* pstart;
	uint8_t* pend;
	uint8_t* pdata;
	uint8_t* pfree;
};
