#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define MESSAGE1 "Login is successful!"
#define MESSAGE2 "Account is blocked or inactive"
#define MESSAGE3 "Password is not correct. Please try again!"
#define MESSAGE4 "Cannot find account"
#define MESSAGE5 "Account is blocked!"


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
    outfile = fopen("account.txt", "w");
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
    file = fopen("account.txt", "r");
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

int main()
{
    readFile();
    Account account;
    int listen_sock, conn_sock; /* file descriptors */
    char recv_data[BUFF_SIZE], temp1[BUFF_SIZE], temp2[BUFF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;

    //Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */
    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == -1) { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    //Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1) {  /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    //Step 4: Communicate with client
    while (1) {
        //accept request
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr*)&client, &sin_size)) == -1) {
            perror("\nError: ");
        }

        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

        //start conversation
        while (1) {
            //receives message from client
            bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); //blocking
            if (bytes_received <= 0) {
                printf("\nConnection closed");
                break;
            }
            else {
                recv_data[bytes_received] = '\0';
                strcpy(temp1, recv_data);
                printf("\nReceive1: %s \ntemp1: %s\n", recv_data, temp1);
                if (checkUsername(temp1) != NULL) {
                    bytes_sent = send(conn_sock, "Password\0", strlen("Password\0"), 0); /* send to the client welcome message */
                    if (bytes_sent <= 0) {
                        printf("\nConnection closed");
                        break;
                    }
                    else {
                        while (1)
                        {
                            bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); //blocking
                            if (bytes_received <= 0) {
                                printf("\nConnection closed");
                                break;
                            }
                            else {
                                recv_data[bytes_received] = '\0';
                                printf("\nReceive2: %s ", recv_data);
                                if ((strncmp(checkUsername(temp1)->infor.password, recv_data, strlen(checkUsername(temp1)->infor.password)) == 0) && (checkUsername(temp1)->infor.status == 1)) {
                                    bytes_sent = send(conn_sock, MESSAGE1, strlen(MESSAGE1), 0); /* send to the client welcome message */
                                    if (bytes_sent <= 0) {
                                        printf("\nConnection closed");
                                        break;
                                    }
                                    while (1)
                                    {
                                        bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); //blocking
                                        if (bytes_received <= 0) {
                                            printf("\nConnection closed");
                                            break;
                                        }
                                        else {
                                            recv_data[bytes_received] = '\0';
                                            printf("\nReceive3: %s ", recv_data);
                                            if (strncmp(recv_data, "Bye", strlen("Bye")) == 0) {
                                                strcpy(temp2, "Goodbye ");
                                                strcat(temp2, checkUsername(temp1)->infor.username);
                                                temp2[strlen(temp2)] = '\0';
                                                bytes_sent = send(conn_sock, temp2, strlen(temp2) - 1, 0); /* send to the client welcome message */
                                                if (bytes_sent <= 0) {
                                                    printf("\nConnection closed");
                                                }
                                                break;
                                            }
                                            else {
                                                bytes_sent = send(conn_sock, recv_data, bytes_received, 0); /* send to the client welcome message */
                                                if (bytes_sent <= 0) {
                                                    printf("\nConnection closed");
                                                    break;
                                                }
                                            }
                                        }
                                    }

                                }
                                else if ((strncmp(checkUsername(temp1)->infor.password, recv_data, strlen(checkUsername(temp1)->infor.password)) == 0) && (checkUsername(temp1)->infor.status == 0)) {
                                    bytes_sent = send(conn_sock, MESSAGE2, strlen(MESSAGE2), 0); /* send to the client welcome message */
                                    if (bytes_sent <= 0) {
                                        printf("\nConnection closed");
                                        break;
                                    }
                                    break;
                                }
                                else if (strncmp(recv_data, MESSAGE5, strlen(MESSAGE5)) == 0) {
                                    checkUsername(temp1)->infor.status = 0;
                                    wrileFile();
                                    bytes_sent = send(conn_sock, "END", strlen("END"), 0); /* send to the client welcome message */
                                    if (bytes_sent <= 0) {
                                        printf("\nConnection closed");
                                        break;
                                    }
                                    break;
                                }
                                else {
                                    bytes_sent = send(conn_sock, MESSAGE3, strlen(MESSAGE3), 0); /* send to the client welcome message */
                                    if (bytes_sent <= 0) {
                                        printf("\nConnection closed");
                                        break;
                                    }

                                }
                            }
                        }
                    }
                }
                else {
                    bytes_sent = send(conn_sock, MESSAGE4, strlen(MESSAGE4), 0); /* send to the client welcome message */
                    if (bytes_sent <= 0) {
                        printf("\nConnection closed");
                        break;
                    }
                }
            }

            //echo to client

        }


    }//end conversation
    close(conn_sock);


    close(listen_sock);
    return 0;
}
