#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024

typedef struct Message {
    char opcode;
    int length;
    char payload[255];
} Message;

typedef struct node {
    Message message;
    struct node* next;
} node;

void processMessage(Message message, char* buff) {
    if ((message.opcode == '0') || (message.opcode == '1')) {
        buff[0] = message.opcode;
        buff[1] = '0';
        buff[2] = '0';
        if (strlen(message.payload) == 1) {
            buff[3] = '0';
            buff[4] = message.payload[0];
        }
        else {
            buff[3] = message.payload[0];
            buff[4] = message.payload[1];
        }
    }
}

int main() {
    Message message;
    FILE* file;
    int client_sock;
    char buff[BUFF_SIZE + 1];
    char buff1[BUFF_SIZE + 1];
    struct sockaddr_in server_addr; /* server's address information */
    int bytes_sent, bytes_received;

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

    //send message
    memset(buff, '\0', (strlen(buff) + 1));
    memset(buff1, '\0', (strlen(buff1) + 1));

    printf("0. Ma hoa \n");
    printf("1. Giai ma: \n");
    printf("Chon 0 hoac 1: \n");
    scanf("%c", &message.opcode);
    fflush(stdin);
    if ((message.opcode == '0') || (message.opcode == '1')) {
        printf("Nhap key(0 - 26): \n");
        scanf("%s", message.payload);
        fflush(stdin);
        processMessage(message, buff);
        bytes_sent = send(client_sock, buff, strlen(buff), 0);
        //printf("%d bytes sent\n", strlen(buff1));
        if (bytes_sent < 0) {
            perror("\nError: ");
        }

        //receive echo reply
        bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_received < 0) {
            perror("\nError: ");
        }
        else if (bytes_received == 0) {
            printf("Connection closed.\n");
        }
        buff[bytes_received] = '\0';
        printf("Reply from server: %s\n", buff);
        if (buff[0] == '3') {
            printf("Error\n");
        }
        else {
            file = fopen("./client_folder/text.txt", "r");
            if (file == NULL)
            {
                fprintf(stderr, "\nCouldn't Open File'\n");
                exit(1);
            }
            while (fgets(buff, 99, file) != NULL)
            {
                memset(buff1, '\0', (strlen(buff1) + 1));
                int length = (int)strlen(buff);
                printf("length: %d %d %d\n", length, length / 10, length % 10);
                buff1[0] = '2';
                buff1[1] = (length / 10) + '0';
                buff1[2] = (length % 10) + '0';
                strcat(buff1, buff);

                bytes_sent = send(client_sock, buff1, strlen(buff) + 3, 0);
                printf("buff1 = %s\n", buff1);
                if (bytes_sent < 0) {
                    perror("\nError: ");
                }
                sleep(1);
            }
            fclose(file);
            bytes_sent = send(client_sock, "200", strlen("200") + 3, 0);
            if (bytes_sent < 0) {
                perror("\nError: ");
            }
            remove("./client_folder/text.txt");
            while (1) {
                //receive echo reply
                bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
                if (bytes_received < 0) {
                    perror("\nError: ");
                }
                else if (bytes_received == 0) {
                    printf("Connection closed.\n");
                }
                buff[bytes_received] = '\0';
                printf("Reply from server: %s\n", buff);
                if (buff[0] == '2') {
                    message.length = (buff[1] - '0') * 10 + (buff[2] - '0');
                    if (strncmp(buff, "200", 3) == 0) {
                        break;
                    }
                    else {
                        for (int i = 3, j = 0; i < message.length + 3; i++, j++) {
                            message.payload[j] = buff[i];
                        }
                        message.payload[message.length] = '\0';

                        printf("Opcode: %c\n", message.opcode);
                        printf("Length: %d\n", message.length);
                        printf("Payload: %s\n", message.payload);

                        file = fopen("./client_folder/text.txt", "a");
                        if (file == NULL)
                        {
                            fprintf(stderr, "\nCouldn't Open File'\n");
                            exit(1);
                        }
                        fprintf(file, "%s", message.payload);
                        fclose(file);
                    }
                }
            }
        }
    }
    else {
        printf("Lua chon khong ton tai\n");
    }
    //Step 4: Close socket
    close(client_sock);
    return 0;
}
