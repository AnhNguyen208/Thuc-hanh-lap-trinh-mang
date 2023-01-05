#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024
#define MESSAGE1 "Login is successful!"
#define MESSAGE2 "Account is blocked or inactive"
#define MESSAGE3 "Password is not correct. Please try again!"
#define MESSAGE4 "Cannot find account"
#define MESSAGE5 "Account is blocked!"
#define MESSAGE6 "Display message!"


int main(int argc, char const* argv[]) {
	if (argc == 3) {
		int client_sock;
		char buff[BUFF_SIZE + 1], buff1[BUFF_SIZE + 1], buff2[BUFF_SIZE + 1];
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
			printf("\nError!Can not connect to sever! Client exit imediately! \n");
			return 0;
		}

		//Step 4: Communicate with server			
		memset(buff, '\0', (strlen(buff) + 1));
		memset(buff1, '\0', (strlen(buff1) + 1));
		memset(buff2, '\0', (strlen(buff2) + 1));

		strcpy(buff, argv[1]);
		strcat(buff, " ");
		strcat(buff, argv[2]);
		//send message
		// printf("\nInsert message:");
		// fgets(buff, BUFF_SIZE, stdin);
		msg_len = strlen(buff);

		bytes_sent = send(client_sock, buff, msg_len, 0);

		if (bytes_sent < 0) {
			perror("\nError: ");
		}

		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if (bytes_received < 0) {
			perror("\nError: ");
		}
		else if (bytes_received == 0) {
			printf("Connection closed.");
		}
		printf("Notification: %s\n", buff);

		//printf("strncmp: %d\n", strncmp(buff, MESSAGE1, strlen(MESSAGE1)));
		if (strncmp(buff, MESSAGE1, strlen(MESSAGE1)) == 0) {
			printf("GROUP CHAT: \n");
			printf("--------------------------\n");
			bytes_sent = send(client_sock, MESSAGE6, strlen(MESSAGE6), 0);

			if (bytes_sent < 0) {
				perror("\nError: ");
			}
			while (1)
			{
				bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
				if (bytes_received < 0) {
					perror("\nError: ");
				}
				else if (bytes_received == 0) {
					printf("Connection closed.");
				}
				if (strncmp("[ENDFILE]", buff, strlen("[ENDFILE]")) == 0) {
					break;
				}
				else {
					printf("%s", buff);
				}
			}
			printf("--------------------------\n");
			printf("\nInsert message:");
			fgets(buff1, BUFF_SIZE, stdin);
			strcpy(buff2, argv[1]);
			strcat(buff2, " : ");
			strcat(buff2, buff1);
			bytes_sent = send(client_sock, buff2, strlen(buff2), 0);

			if (bytes_sent < 0) {
				perror("\nError: ");
			}
			printf("GROUP CHAT: \n");
			printf("--------------------------\n");
			while (1)
			{
				bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
				if (bytes_received < 0) {
					perror("\nError: ");
				}
				else if (bytes_received == 0) {
					printf("Connection closed.");
				}
				if (strncmp("[ENDFILE]", buff, strlen("[ENDFILE]")) == 0) {
					break;
				}
				else {
					printf("%s", buff);
				}
			}
			printf("--------------------------\n");

		}
		//Step 4: Close socket
		close(client_sock);
		return 0;
	}
	else {
		printf("./client username password\n");
	}

}
