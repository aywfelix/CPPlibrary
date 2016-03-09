#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/time.h>
#include <net/if.h>
#include <inttypes.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
class CNetUtil
{
public:
	/**
     * 得到本机ip
     */
    static uint32_t getLocalAddr(const char *dev_name);
    /**
     * ip是本机ip地址, true - 是, false - 不是
     */
    static bool isLocalAddr(uint32_t ip, bool loopSkip = true);
    /**
     * 把字符串的ip转成int
     * 如 10.0.100.89 => 1499725834
     */
    static uint32_t getAddr(const char *ip);
    /**
     * 把uint64转成字符串
     */
    static std::string addrToString(uint64_t ipport);
    /**
     * 把ip,port转成uint64_t
     */
    static uint64_t strToAddr(const char *ip, int port);
    /**
     * 把ip,port转成uint64_t
     */
    static uint64_t ipToAddr(uint32_t ip, int port);
};

uint32_t CNetUtil::getLocalAddr(const char* dev_name)
{
	int             fd, intrface;
    struct ifreq    buf[16];
    struct ifconf   ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        return 0;
    }
  
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (ioctl(fd, SIOCGIFCONF, (char *) &ifc)) {
        close(fd);
        return 0;
    }
    
    intrface = ifc.ifc_len / sizeof(struct ifreq);
    while (intrface-- > 0)
    {
        if(ioctl(fd,SIOCGIFFLAGS,(char *) &buf[intrface])) {
            continue;
        }
        if(buf[intrface].ifr_flags&IFF_LOOPBACK) continue;
        if (!(buf[intrface].ifr_flags&IFF_UP)) continue;
        if (dev_name != NULL && strcmp(dev_name, buf[intrface].ifr_name)) continue;
        if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[intrface]))) {
            close(fd);
            return ((struct sockaddr_in *) (&buf[intrface].ifr_addr))->sin_addr.s_addr;
        }
    }
    close(fd);
    return 0;
}

bool CNetUtil::isLocalAddr(uint32_t ip, bool loopSkip)
{
    int             fd, intrface;
    struct ifreq    buf[16];
    struct ifconf   ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        return false;
    }
  
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (ioctl(fd, SIOCGIFCONF, (char *) &ifc)) {
        close(fd);
        return false;
    }
    
    intrface = ifc.ifc_len / sizeof(struct ifreq);
    while (intrface-- > 0)
    {
        if(ioctl(fd,SIOCGIFFLAGS,(char *) &buf[intrface])) {
            continue;
        }
        if(loopSkip && buf[intrface].ifr_flags&IFF_LOOPBACK) continue;
        if (!(buf[intrface].ifr_flags&IFF_UP)) continue;
        if (ioctl(fd, SIOCGIFADDR, (char *) &buf[intrface])) {
            continue;
        }
        if (((struct sockaddr_in *) (&buf[intrface].ifr_addr))->sin_addr.s_addr == ip) {
            close(fd);
            return true;
        }
    }
    close(fd);
    return false;
}

/**
 * 把字符串的ip转成int
 * 如 10.0.100.89 => 1499725834
 */
uint32_t CNetUtil::getAddr(const char *ip)
{
    if (ip == NULL) return 0;
    int x = inet_addr(ip);
    if (x == (int)INADDR_NONE) {
        struct hostent *hp;
        if ((hp = gethostbyname(ip)) == NULL) {
            return 0;
        }
        x = ((struct in_addr *)hp->h_addr)->s_addr;
    }
    return x;
}

/**
 * 把uint64转成字符串
 */
std::string CNetUtil::addrToString(uint64_t ipport)
{
    char str[32];
    uint32_t ip = (uint32_t)(ipport & 0xffffffff);
    int port = (int)((ipport >> 32 ) & 0xffffffff);
    unsigned char *bytes = (unsigned char *) &ip;
    if (port > 0) {
        sprintf(str, "%d.%d.%d.%d:%d", bytes[0], bytes[1], bytes[2], bytes[3], port);
    } else {
        sprintf(str, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    }
    return str;
}

uint64_t CNetUtil::strToAddr(const char *ip, int port)
{
    uint32_t nip = 0;
    const char *p = strchr(ip, ':');
    if (p != NULL && p>ip) {
        int len = p-ip;
        if (len>64) len = 64;
        char tmp[128];
        strncpy(tmp, ip, len);
        tmp[len] = '\0';
        nip = getAddr(tmp);
        port = atoi(p+1);
    } else {
        nip = getAddr(ip);
    }
    if (nip == 0) {
        return 0;
    }
    uint64_t ipport = port;
    ipport <<= 32;
    ipport |= nip;
    return ipport;
}


uint64_t CNetUtil::ipToAddr(uint32_t ip, int port)
{
    uint64_t ipport = port;
    ipport <<= 32;
    ipport |= ip;
    return ipport;
}

int main(int argc, char *argv[])
{
    uint32_t ip = CNetUtil::getAddr("127.0.0.1");
	std::cout << ip << "\n";
	uint64_t ipport = CNetUtil::strToAddr("127.0.0.1", 9999);
	std::cout << ipport << "\n";
	std::string ipportstr = CNetUtil::addrToString(ipport);
	std::cout << ipportstr << "\n";
    return 0;
}

