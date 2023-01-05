/*UDP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERV_PORT 5550
#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int main(int argc, char const* argv[]) {
    if (argc == 3)
    {
        int client_sock, client;
        char buff[BUFF_SIZE];
        struct sockaddr_in server_addr;
        int bytes_sent, bytes_received, sin_size;

        //Step 1: Construct a UDP socket
        if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {  /* calls socket() */
            perror("\nError: ");
            exit(0);
        }

        //Step 2: Define the address of the server
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);

        printf("1. Client 1\n");
        printf("2. Client 2\n");
        printf("Chon client: \n");
        fgets(buff, BUFF_SIZE, stdin);
        sscanf(buff, "%d", &client);
        printf("client: %d\n", client);
        if (client == 1)
        {
            while (1)
            {
                //Step 3: Communicate with server
                printf("\nInsert string to send:");
                memset(buff, '\0', (strlen(buff) + 1));
                fgets(buff, BUFF_SIZE, stdin);

                if (strcmp(buff, "\n") == 0) {
                    printf("Out \n");
                    break;
                }

                sin_size = sizeof(struct sockaddr);

                bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr*)&server_addr, sin_size);
                if (bytes_sent < 0) {
                    perror("Error: ");
                    close(client_sock);
                    return 0;
                }
            }
            close(client_sock);
        }
        else if (client == 2) {
            memset(buff, '\0', (strlen(buff) + 1));
            strcpy(buff, "client2");

            sin_size = sizeof(struct sockaddr);
            bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr*)&server_addr, sin_size);
            if (bytes_sent < 0) {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            while (1)
            {
                bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr*)&server_addr, &sin_size);
                if (bytes_received < 0) {
                    perror("Error: ");
                    close(client_sock);
                    return 0;
                }
                buff[bytes_received] = '\0';
                printf("Reply from server: %s\n", buff);
            }
            close(client_sock);
        }
    }
    else {
        printf("Error:Wrong number of arguments\n");
    }
    return 0;
}
