#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192
#define MESSAGE1 "File Already Exists!"
#define MESSAGE2 "File Transfer Is Completed..."
#define MESSAGE3 "Error: File Not Found!"
#define MESSAGE4 "Error: Wrong File Format!"

int main(int argc, char** argv) {
    if (argc == 2) {
        int client_sock;
        char buff[BUFF_SIZE], buff1[BUFF_SIZE];
        struct sockaddr_in server_addr; /* server's address information */
        int msg_len, bytes_sent, bytes_received;

        memset(buff, '\0', (strlen(buff) + 1));
        memset(buff1, '\0', (strlen(buff1) + 1));
        strcpy(buff, "./folder_client/");
        strcat(buff, argv[1]);
        struct stat temp;

        if (stat(buff, &temp) != -1)
        {
            printf("\nFile \"%s\" exists\n", buff);
            return 0;
        }
        else {

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

            argv[1][strlen(argv[1])] = '\0';
            bytes_sent = send(client_sock, argv[1], strlen(argv[1]), 0);
            if (bytes_sent <= 0) {
                printf("\nConnection closed!\n");
            }
            bytes_received = recv(client_sock, buff1, BUFF_SIZE - 1, 0);
            if (bytes_received <= 0) {
                printf("\nError!Cannot receive data from sever!\n");
                return 0;
            }

            buff1[bytes_received] = '\0';
            printf("Reply from server: %s\n", buff1);
            if ((strncmp(buff1, MESSAGE3, strlen(MESSAGE3)) == 0) || (strncmp(buff1, MESSAGE4, strlen(MESSAGE4)) == 0)) {
                return 0;
            }

            FILE* image;
            if ((image = fopen(buff, "w")) == NULL)
            {
                perror("Error opening file image");
                exit(1);
            }
            while ((bytes_received = recv(client_sock, buff1, BUFF_SIZE - 1, 0)) > 0) {
                buff1[bytes_received] = '\0';
                //printf("Reply from server: %s, Length: %d\n", buff1, bytes_received);
                //printf("Reading Picture Byte Array\n");
                //Convert it Back into Picture
                //printf("Converting Byte Array to Picture\n");
                fwrite(buff1, 1, bytes_received, image);
            }
            fclose(image);

        }
        //Step 4: Close socket
        close(client_sock);
        return 0;
    }
    else {
        printf("Error!\n");
    }

}
