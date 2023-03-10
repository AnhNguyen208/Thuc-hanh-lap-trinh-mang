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

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024

/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client
*/
void echo(int sockfd);

int main() {

    int listen_sock, conn_sock; /* file descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    pid_t pid;
    int sin_size;

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  /* calls socket() */
        printf("socket() error\n");
        return 0;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */

    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("\nError: ");
        return 0;
    }

    if (listen(listen_sock, BACKLOG) == -1) {
        perror("\nError: ");
        return 0;
    }

    /* Establish a signal handler to catch SIGCHLD */
    signal(SIGCHLD, sig_chld);

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr*)&client, &sin_size)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                perror("\nError: ");
                return 0;
            }
        }

        /* For each client, fork spawns a child, and the child handles the new client */
        pid = fork();

        /* fork() is called in child process */
        if (pid == 0) {
            close(listen_sock);
            printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
            echo(conn_sock);
            exit(0);
        }

        /* The parent closes the connected socket since the child handles the new client */
        close(conn_sock);
    }
    close(listen_sock);
    printf("Connection closed.\n");
    return 0;
}

void sig_chld(int signo) {
    pid_t pid;
    int stat;

    /* Wait the child process terminate */
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("\nChild %d terminated\n", pid);
    }
}

void echo(int sockfd) {
    char buff[BUFF_SIZE], buff1[BUFF_SIZE], buff2[BUFF_SIZE];
    int bytes_sent, bytes_received;

    bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); //blocking

    if (bytes_received < 0) {
        perror("\nError: ");
    }
    else if (bytes_received == 0) {
        printf("Connection closed.");
    }
    //printf("received: %s\n", buff);
    for (int i = 0; i < bytes_received - 1; i++) {
        buff[i] = toupper(buff[i]);
    }

    memset(buff1, '\0', (strlen(buff1) + 1));
    memset(buff2, '\0', (strlen(buff2) + 1));
    strncpy(buff2, buff, bytes_received - 1);
    strcpy(buff1, "./folder_server/");
    strncat(buff1, buff, bytes_received - 1);
    //printf("file: %s msg_len: %d\n", buff, bytes_received);
    FILE* file;

    if ((file = fopen(buff1, "w")) == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    while (1) {
        bytes_received = recv(sockfd, buff, BUFF_SIZE, 0);
        if (strncmp("[ENDFILE]", buff, sizeof("[ENDFILE]")) == 0) {
            break;
        }
        for (int i = 0; i < bytes_received; i++)
        {
            buff[i] = toupper(buff[i]);
        }
        //printf("received: %s\n", buff);
        fprintf(file, "%s", buff);
        bzero(buff, BUFF_SIZE);
    }
    fclose(file);
    //printf("Close file\n");

    if ((file = fopen(buff1, "r")) == NULL) {
        perror("Error opening file");
        exit(1);
    }
    char data[BUFF_SIZE] = { 0 };
    while (fgets(data, BUFF_SIZE, file) != NULL) {
        //printf("%s\n", data);
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, BUFF_SIZE);
    }
    fclose(file);
    //printf("Close file\n");
    send(sockfd, "[ENDFILE]", sizeof("[ENDFILE]"), 0);
    close(sockfd);
}