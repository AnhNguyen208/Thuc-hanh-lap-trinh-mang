#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

#define PORT 5500   /* Port that will be opened */ 
#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

typedef struct Message {
    char opcode;
    int length;
    char payload[255];
} Message;

/* The processData function copies the input string to output */
void processData(char* in, char* out, int* key, int* request);

/* The recv() wrapper function*/
int receiveData(int s, char* buff, int size, int flags);

/* The send() wrapper function*/
int sendData(int s, char* buff, int size, int flags);

int main()
{
    int i, maxi, maxfd, listenfd, connfd, sockfd, key = -1, request;
    int nready, client[FD_SETSIZE];
    ssize_t	ret;
    fd_set	readfds, allset;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    //Step 1: Construct a TCP socket to listen connection request
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    //Step 3: Listen request from client
    if (listen(listenfd, BACKLOG) == -1) {  /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    maxfd = listenfd;			/* initialize */
    maxi = -1;				/* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;			/* -1 indicates available entry */
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    //Step 4: Communicate with clients
    while (1) {
        readfds = allset;		/* structure assignment */
        nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nready < 0) {
            perror("\nError: ");
            return 0;
        }

        if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
                perror("\nError: ");
            else {
                printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0) {
                        client[i] = connfd;	/* save descriptor */
                        break;
                    }
                if (i == FD_SETSIZE) {
                    printf("\nToo many clients");
                    close(connfd);
                }

                FD_SET(connfd, &allset);	/* add new descriptor to set */
                if (connfd > maxfd)
                    maxfd = connfd;		/* for select */
                if (i > maxi)
                    maxi = i;		/* max index in client[] array */

                if (--nready <= 0)
                    continue;		/* no more readable descriptors */
            }
        }

        for (i = 0; i <= maxi; i++) {	/* check all clients for data */
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &readfds)) {
                ret = receiveData(sockfd, rcvBuff, BUFF_SIZE, 0);
                if (ret <= 0) {
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client[i] = -1;
                }

                else {
                    rcvBuff[ret] = '\0';
                    printf("rcvBuff = %s\n", rcvBuff);
                    processData(rcvBuff, sendBuff, &key, &request);
                    printf("key = %d, request = %d\n", key, request);
                    if (key != -1) {
                        if (strcmp(sendBuff, "Gui ket qua") == 0) {
                            char buff[BUFF_SIZE + 1];
                            char buff1[BUFF_SIZE + 1];
                            memset(buff, '\0', (strlen(buff) + 1));
                            FILE* file;

                            file = fopen("./server_folder/text.txt", "r");
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
                                printf("buff1 = %s\n", buff1);
                                sendData(sockfd, buff1, strlen(buff) + 3, 0);
                                sleep(3);
                            }
                            fclose(file);
                            sendData(sockfd, "200", strlen("200"), 0);
                            remove("./server_folder/text.txt");
                        }
                        else {
                            sendData(sockfd, sendBuff, strlen(sendBuff), 0);
                            if (ret <= 0) {
                                FD_CLR(sockfd, &allset);
                                close(sockfd);
                                client[i] = -1;
                            }
                        }

                    }
                }
                if (--nready <= 0)
                    break;		/* no more readable descriptors */
            }
        }
    }

    return 0;
}

void processData(char* in, char* out, int* key, int* request) {
    Message message;
    FILE* file;
    message.opcode = in[0];
    //printf("opcode = %c\n", message.opcode);
    if ((message.opcode == '0') || (message.opcode == '1')) {
        *request = message.opcode - '0';
        *key = (in[3] - '0') * 10 + (in[4] - '0');
        if ((*key < 0) || (*key > 26)) {
            strcpy(out, "3");
            *key = -1;
        }
        else {
            strcpy(out, in);
        }
    }
    else if (message.opcode == '2') {
        message.length = (in[1] - '0') * 10 + (in[2] - '0');

        if (message.length == 0) {
            strcpy(out, "Gui ket qua");
        }
        else {
            for (int i = 3, j = 0; i < message.length + 3; i++, j++) {
                message.payload[j] = in[i];
            }
            message.payload[message.length] = '\0';
            if (*request == 0) {
                maHoa(message.payload, *key);
            }
            else {
                giaiMa(message.payload, *key);
            }
            printf("Opcode: %c\n", message.opcode);
            printf("Length: %d\n", message.length);
            printf("Payload: %s\n", message.payload);

            file = fopen("./server_folder/text.txt", "a");
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

void maHoa(char* in, int key) {
    char ch;
    for (int i = 0; in[i] != '\0'; ++i) {
        ch = in[i];
        // check for valid characters
        if (isalnum(ch)) {
            // lower case characters
            if (islower(ch)) {
                ch = (ch - 'a' + key) % 26 + 'a';
            }
            // uppercase characters
            if (isupper(ch)) {
                ch = (ch - 'A' + key) % 26 + 'A';
            }
            // numbers
            if (isdigit(ch)) {
                ch = (ch - '0' + key) % 10 + '0';
            }
        }
        in[i] = ch;
    }
}

void giaiMa(char* in, int key) {
    char ch;
    for (int i = 0; in[i] != '\0'; ++i) {
        ch = in[i];
        // check for valid characters
        if (isalnum(ch)) {
            // lower case characters
            if (islower(ch)) {
                ch = (ch - 'a' - key + 26) % 26 + 'a';
            }
            // uppercase characters
            if (isupper(ch)) {
                ch = (ch - 'A' - key + 26) % 26 + 'A';
            }
            // numbers
            if (isdigit(ch)) {
                ch = (ch - '0' - key + 10) % 10 + '0';
            }
        }
        in[i] = ch;
    }
}

int receiveData(int s, char* buff, int size, int flags) {
    int n;
    n = recv(s, buff, size, flags);
    if (n < 0)
        perror("Error: ");
    return n;
}

int sendData(int s, char* buff, int size, int flags) {
    int n;
    n = send(s, buff, size, flags);
    if (n < 0)
        perror("Error: ");
    return n;
}

