#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const* argv[])
{
    if (argc == 2)
    {
        struct hostent* a, * b;
        struct in_addr IP;
        struct in_addr addr;
        inet_aton(argv[1], &addr);
        if ((gethostbyname(argv[1]) == NULL) && (gethostbyaddr(&addr, sizeof(argv[1]), AF_INET) == NULL)) {
            printf("No Information found\n");
        }
        else {
            if (gethostbyaddr(&addr, sizeof(argv[1]), AF_INET) == NULL) {
                a = gethostbyname(argv[1]);
                printf("gethostbyname\n");
            }
            else {
                a = gethostbyaddr(&addr, sizeof(argv[1]), AF_INET);
                printf("gethostbyaddr\n");
            }

            memcpy(&b, &a, sizeof(a));
            printf("HOST: %s\n", b->h_name);
            printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)b->h_addr));
            printf("Alias IP:\n");
            for (int i = 0; b->h_aliases[i] != NULL; i++) {
                printf("%s\n", b->h_aliases[i]);
            }

            // for (int i = 0; b->h_addr_list[i] != NULL; i++) {
            //     IP.s_addr = *((uint32_t*)b->h_addr_list[i]);
            //     printf("IP[%d]: %s\n\n", i, inet_ntoa(IP));
            // }

        }
    }
    else if (argc > 2)
    {
        printf("Qua nhieu tham so duoc cung cap.\n");
    }
    else
    {
        printf("Ban nen cung cap mot tham so.\n");
    }
    return 0;
}
