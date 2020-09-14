#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>



#define PORT 9002

int main() {

    //create a socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //connect the socket to the server
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    //check for error with connection
    if (connection_status == -1){
        printf("There was an error making connection to remote socket.\n");
    }

    //receive data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    printf("%s", server_response);

    //close socket
    close(network_socket);

    return 0;
}
