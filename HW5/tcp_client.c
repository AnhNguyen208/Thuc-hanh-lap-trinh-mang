#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192
#define MESSAGE1 "Login is successful!"
#define MESSAGE2 "Account is blocked or inactive"
#define MESSAGE3 "Password is not correct. Please try again!"
#define MESSAGE4 "Cannot find account"
#define MESSAGE5 "Account is blocked!"

int main() {
    int client_sock;
    char buff[BUFF_SIZE];
    char buff1[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;

    //Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    //Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    //Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {
        printf("\nError!Can not connect to sever! Client exit imediately! ");
        return 0;
    }

    //Step 4: Communicate with server			
    while (1) {
        //send message
        printf("\nInsert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);
        if (msg_len == 0) break;

        bytes_sent = send(client_sock, buff, msg_len, 0);
        if (bytes_sent <= 0) {
            printf("\nConnection closed!\n");
            break;
        }

        //receive echo reply
        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }

        buff[bytes_received] = '\0';
        printf("Reply from server: %s", buff);

        if (strcmp(buff, "Password\0") == 0) {
            int i = 0;
            while (i < 3) {
                printf("\nPlease enter the login password: ");
                memset(buff, '\0', (strlen(buff) + 1));
                fgets(buff, BUFF_SIZE, stdin);
                msg_len = strlen(buff);
                if (msg_len == 0) break;

                bytes_sent = send(client_sock, buff, msg_len, 0);
                if (bytes_sent <= 0) {
                    printf("\nConnection closed!\n");
                    break;
                }

                bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                if (bytes_received <= 0) {
                    printf("\nError!Cannot receive data from sever!\n");
                    break;
                }
                buff[bytes_received] = '\0';
                printf("Reply from server: %s", buff);
                if (strncmp(buff, MESSAGE1, strlen(MESSAGE1)) == 0) {
                    while (1)
                    {
                        printf("\nInsert string to send1:");
                        memset(buff, '\0', (strlen(buff) + 1));
                        fgets(buff, BUFF_SIZE, stdin);
                        msg_len = strlen(buff);
                        if (msg_len == 0) break;

                        bytes_sent = send(client_sock, buff, msg_len, 0);
                        if (bytes_sent <= 0) {
                            printf("\nConnection closed!\n");
                            break;
                        }

                        //receive echo reply
                        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                        if (bytes_received <= 0) {
                            printf("\nError!Cannot receive data from sever!\n");
                            break;
                        }

                        buff[bytes_received] = '\0';
                        if (strncmp(buff, "Goodbye", strlen("Goodbye")) == 0) {
                            printf("Reply from server: %s", buff);
                            break;
                        }
                    }
                    break;
                }
                else if (strncmp(buff, MESSAGE2, strlen(MESSAGE2)) == 0) {
                    break;
                }
                else if (strncmp(buff, MESSAGE3, strlen(MESSAGE3)) == 0) {
                    i++;
                    if (i == 3) {
                        printf("%s\n", MESSAGE5);

                        bytes_sent = send(client_sock, MESSAGE5, strlen(MESSAGE5), 0);
                        if (bytes_sent <= 0) {
                            printf("\nConnection closed!\n");
                            break;
                        }

                        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                        if (bytes_received <= 0) {
                            printf("\nError!Cannot receive data from sever!\n");
                            break;
                        }
                        buff[bytes_received] = '\0';
                        printf("Reply from server: %s", buff);
                        break;
                    }
                }
            }
        }
    }

    //Step 4: Close socket
    close(client_sock);
    return 0;
}
