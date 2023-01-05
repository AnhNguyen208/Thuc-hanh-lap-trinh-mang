/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024

int checkMessage(char* buff, char* a, char* b, int num) {
    int check = 0;
    for (int i = 0; i < strlen(buff) - 1; i++) {
        // printf("ki tu thu %d: %c\n", i, buff[i]);
        if (isalnum(buff[i]) == 0) {
            check = 1;
            // printf("%c khong hop le\n", buff[i]);
            num = 0;
            return num;
        }
    }
    if (check == 0) {
        int m = 0, n = 0;
        for (int i = 0; i < strlen(buff); i++) {
            if (isalpha(buff[i]) != 0) {
                a[m++] = buff[i];
            }
            else if (isdigit(buff[i]) != 0)
            {
                b[n++] = buff[i];
            }

        }
    }
    return num;
}

int main(int argc, char const* argv[])
{
    if (argc == 2)
    {
        int server_sock; /* file descriptors */
        char buff[BUFF_SIZE];
        char buff1[BUFF_SIZE];
        int bytes_sent, bytes_received;
        struct sockaddr_in server; /* server's address information */
        struct sockaddr_in client; /* client's address information */
        struct sockaddr_in client1;
        int sin_size;

        //Step 1: Construct a UDP socket
        if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {  /* calls socket() */
            perror("\nError: ");
            exit(0);
        }

        //Step 2: Bind address to socket
        server.sin_family = AF_INET;
        server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
        server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */
        bzero(&(server.sin_zero), 8); /* zero the rest of the structure */


        if (bind(server_sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1) { /* calls bind() */
            perror("\nError: ");
            exit(0);
        }

        //Step 3: Communicate with clients
        int i = 0;
        while (1) {
            sin_size = sizeof(struct sockaddr_in);
            bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr*)&client, &sin_size);
            if (strcmp(buff, "client2\0") == 0) {
                client1 = client;
                i = 1;
            }
            if (bytes_received < 0) {
                perror("\nError: ");
            }
            buff[bytes_received] = '\0';
            printf("[%s:%d]: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
            if (strcmp(buff, "client2\0") != 0) {
                strcpy(buff1, buff);
            }
            // printf("buff1: %s\n", buff1);
            if (i == 1) {
                char a[BUFF_SIZE];
                char b[BUFF_SIZE];
                if (checkMessage(buff1, a, b, 1) == 1) {
                    bytes_sent = sendto(server_sock, a, sizeof(a), 0, (struct sockaddr*)&client1, sin_size); /* send to the client welcome message */
                    if (bytes_sent < 0) {
                        perror("\nError: ");
                    }
                    bytes_sent = sendto(server_sock, b, sizeof(b), 0, (struct sockaddr*)&client1, sin_size); /* send to the client welcome message */
                    if (bytes_sent < 0) {
                        perror("\nError: ");
                    }
                }
                else {
                    bytes_sent = sendto(server_sock, "Error", sizeof("Error"), 0, (struct sockaddr*)&client1, sin_size); /* send to the client welcome message */
                    if (bytes_sent < 0) {
                        perror("\nError: ");
                    }
                }
            }
        }
        close(server_sock);
        return 0;
    }
}
