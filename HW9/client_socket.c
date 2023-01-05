#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <string.h>

#define MESSAGE1 "Wrong text format"

int main(int argc, const char* argv[]) {
    //create a socket
    int network_socket, bytes_received, send_status, total_bytes = 0;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    //check for connection_status
    if (connection_status == -1)
    {
        printf("The connection has error\n\n");
    }

    if (connection_status == 0)
    {
        char response[256];
        memset(response, '\0', sizeof(response));
        //receive data from the server
        recv(network_socket, response, sizeof(response), 0);
        printf("%s\n", response);
        while (1) {
            char response1[256];
            memset(response, '\0', sizeof(response1));
            printf("Enter a message to echo\n");
            fgets(response1, 256, stdin);
            response1[strlen(response1) - 1] = '\0';
            //printf("strlen: %d\n", strlen(response1));
            send_status = send(network_socket, response1, strlen(response1), 0);
            total_bytes += strlen(response1);
            if (send_status < 0) {
                perror("\nError: ");
            }
            if ((strcmp(response1, "q") == 0) || (strcmp(response1, "Q") == 0)) {
                printf("Total bytes send: %d\n", total_bytes);
                break;
            }
            //print out the server's response
            bytes_received = recv(network_socket, response1, sizeof(response1), 0);
            if (bytes_received < 0) {
                perror("\nError: ");
            }
            else if (bytes_received == 0) {
                printf("Connection closed.");
            }
            response1[bytes_received] = '\0';
            printf("Here is the echo message from the server: %s\n\n", response1);
        }
    }
    //close the socket
    close(network_socket);

    return 0;
}
