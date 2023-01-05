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

int main() {
	int client_sock;
	char buff[BUFF_SIZE + 1], buff1[BUFF_SIZE + 1], buff2[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received, bytes_total = 0;
	FILE* file;
	FILE* file1;

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
	//send message
	printf("\nInsert file name:");
	fgets(buff, BUFF_SIZE, stdin);
	msg_len = strlen(buff);

	bytes_sent = send(client_sock, buff, msg_len, 0);
	bytes_total += bytes_sent;
	if (bytes_sent < 0) {
		perror("\nError: ");
	}
	strcpy(buff1, "./folder_client/");
	strncat(buff1, buff, msg_len - 1);

	//printf("file: %s msg_len: %d\n", buff1, msg_len);

	if ((file = fopen(buff1, "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}

	strcpy(buff2, "./folder_client/");
	for (int i = 0; i < msg_len; i++)
	{
		buff[i] = toupper(buff[i]);
	}
	strncat(buff2, buff, msg_len - 1);
	//printf("file: %s msg_len: %d\n", buff2, msg_len);

	char data[BUFF_SIZE] = { 0 };
	while (fgets(data, BUFF_SIZE, file) != NULL) {
		//printf("%s\n", data);
		bytes_sent = send(client_sock, data, sizeof(data), 0);
		bytes_total += bytes_sent;
		if (bytes_sent == -1) {
			perror("[-]Error in sending file.");
			exit(1);
		}
		bzero(data, BUFF_SIZE);
	}
	fclose(file);
	//printf("Close file\n");
	bytes_sent = send(client_sock, "[ENDFILE]", sizeof("[ENDFILE]"), 0);
	bytes_total += bytes_sent;
	//receive echo reply
	remove(buff1);
	// if (remove(buff1) == 0)
	// {
	// 	printf("Xoa file thanh cong !!!");
	// }
	// else
	// {
	// 	printf("Error: khong the xoa file tren");
	// }
	if ((file1 = fopen(buff2, "w")) == NULL)
	{
		perror("Error opening file1");
		exit(1);
	}
	while (1) {
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if (strncmp("[ENDFILE]", buff, sizeof("[ENDFILE]")) == 0) {
			break;
		}
		//printf("received: %s\n", buff);
		fprintf(file1, "%s", buff);
		bzero(buff, BUFF_SIZE);
	}
	fclose(file1);
	printf("Số byte đã gửi tới server: %d\n", bytes_total);
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
