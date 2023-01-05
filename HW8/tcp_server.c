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
#include <stdbool.h>

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024
#define MESSAGE1 "Login is successful!"
#define MESSAGE2 "Account is blocked or inactive"
#define MESSAGE3 "Password is not correct. Please try again!"
#define MESSAGE4 "Cannot find account"
#define MESSAGE5 "Account is blocked!"
#define MESSAGE6 "Display message!"

typedef struct Account {
    char username[20];
    char password[20];
    int status;
} Account;

typedef struct node
{
    Account infor;
    int SignIn;
    struct node* next;
} node;

struct node* head = NULL;
struct node* current = NULL;

void printList() {
    struct node* ptr = head;
    printf("\n[ ");

    while (ptr != NULL) {
        printf("(%s,%s,%d) \n", ptr->infor.username, ptr->infor.password, ptr->infor.status);
        ptr = ptr->next;
    }

    printf(" ]");
}

void insertFirst(Account account) {
    struct node* link = (struct node*)malloc(sizeof(struct node));

    strcpy(link->infor.username, account.username);
    strcpy(link->infor.password, account.password);
    link->infor.status = account.status;
    link->SignIn = 0;

    link->next = head;
    head = link;
}

struct node* deleteFirst() {
    struct node* tempLink = head;
    head = head->next;
    return tempLink;
}

bool isEmpty() {
    return head == NULL;
}

void wrileFile()
{
    FILE* outfile;
    outfile = fopen("taikhoan.txt", "w");
    struct node* temp = head;
    while (temp != NULL)
    {
        fprintf(outfile, "%s %s %d\n", temp->infor.username, temp->infor.password, temp->infor.status);
        temp = temp->next;
    }
    fclose(outfile);
}

void readFile()
{
    FILE* file;
    file = fopen("taikhoan.txt", "r");
    Account temp;
    if (file == NULL)
    {
        fprintf(stderr, "\nCouldn't Open File'\n");
        exit(1);
    }
    while (fscanf(file, "%s %s %d", temp.username, temp.password, &temp.status) != EOF)
    {
        insertFirst(temp);
    }
    fclose(file);
}

node* checkUsername(char* username)
{
    node* temp = head;
    while (temp != NULL)
    {
        if (strncmp(temp->infor.username, username, strlen(temp->infor.username)) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void displayMessage(int sockfd) {
    FILE* file;
    int bytes_sent;
    if ((file = fopen("groupchat.txt", "r")) == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char data[BUFF_SIZE] = { 0 };
    while (fgets(data, BUFF_SIZE, file) != NULL) {
        //printf("%s\n", data);
        bytes_sent = send(sockfd, data, sizeof(data), 0);
        if (bytes_sent == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, BUFF_SIZE);
    }


    bytes_sent = send(sockfd, "[ENDFILE]", sizeof("[ENDFILE]"), 0);
    fclose(file);
}

void insertMessage(char* message) {
    FILE* file;
    if ((file = fopen("groupchat.txt", "a")) == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fprintf(file, "%s", message);
    fclose(file);
}

/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client
*/
void echo(int sockfd);

int main() {
    readFile();
    Account account;
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
    memset(buff1, '\0', (strlen(buff1) + 1));
    memset(buff2, '\0', (strlen(buff2) + 1));
    Account account;

    bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); //blocking

    if (bytes_received < 0) {
        perror("\nError: ");
    }
    else if (bytes_received == 0) {
        printf("Connection closed.");
    }
    printf("received: %s\n", buff);
    sscanf(buff, "%s %s", account.username, account.password);
    if (checkUsername(account.username) != NULL) {
        if ((strncmp(checkUsername(account.username)->infor.password, account.password, strlen(checkUsername(account.username)->infor.password)) == 0) && (checkUsername(account.username)->infor.status == 1)) {
            bytes_sent = send(sockfd, MESSAGE1, strlen(MESSAGE1), 0);
            if (bytes_sent < 0) {
                perror("\nError: ");
            }

            bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); //blocking

            if (bytes_received < 0) {
                perror("\nError: ");
            }
            else if (bytes_received == 0) {
                printf("Connection closed.");
            }
            printf("received: %s\n", buff);

            if (strncmp(MESSAGE6, buff, strlen(MESSAGE6)) == 0)
            {
                displayMessage(sockfd);
                bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); //blocking

                if (bytes_received < 0) {
                    perror("\nError: ");
                }
                else if (bytes_received == 0) {
                    printf("Connection closed.");
                }
                printf("received: %s\n", buff);
                buff[bytes_received] = '\0';
                insertMessage(buff);
                displayMessage(sockfd);
            }

        }
        else if ((strncmp(checkUsername(account.username)->infor.password, account.password, strlen(checkUsername(account.username)->infor.password)) == 0) && (checkUsername(account.username)->infor.status == 0)) {
            bytes_sent = send(sockfd, MESSAGE2, strlen(MESSAGE2), 0); /* send to the client welcome message */
            if (bytes_sent <= 0) {
                printf("\nConnection closed");
            }
        }
        else {
            bytes_sent = send(sockfd, MESSAGE3, strlen(MESSAGE3), 0); /* send to the client welcome message */
            if (bytes_sent <= 0) {
                printf("\nConnection closed");
            }
        }
    }
    close(sockfd);
}