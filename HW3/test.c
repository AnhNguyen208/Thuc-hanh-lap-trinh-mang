#define MAXBUF 256       /*****  Demo of Important Functions  *****/
#include <sys/types.h>
#include <sys/socket.h>  /* for AF_INET constant */
#include <netdb.h>       /* for struct hostent   */
#include <netinet/in.h>  /* For struct in_addr */
#include <arpa/inet.h>   /* for inet_ntoa()    */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* struct in_addr{ unsigned long s_addr}; */
int main()
{
    std::string interface_name = "eth0";
    std::vector<std::string> finded_aliases;
    for (int i = 0; i < 10; i++)
    {
        std::string searching_interface_alias = interface_name + ":" + std::to_string(i);
        struct ifreq ifr;
        int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, searching_interface_alias.c_str());
        ioctl(sock, SIOCGIFFLAGS, &ifr);
        close(sock);
        if (!!(ifr.ifr_flags & IFF_UP))
        {
            fd = socket(AF_INET, SOCK_DGRAM, 0);
            char name[searching_interface_alias.length() + 1];
            strcpy(name, searching_interface_alias.c_str());
            strncpy(ifr.ifr_name, name, IF_NAMESIZE - 1);
            /* Get ip address */
            ioctl(fd, SIOCGIFADDR, &ifr);
            strncpy(buffor, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr), sizeof(buffor));
            std::cout << "Finded alias: " << searching_interface_alias << ", IP: " << buffor << std::endl;
        }
    }
}