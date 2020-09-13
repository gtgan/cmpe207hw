#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXBUF 1000
#define PORT 23

int main() {

    //create socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //initialized socket attributes
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("94.142.241.111");

    //connect the socket to the server
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    //check for error with connection
    if (connection_status == -1){
        printf("ERROR: unable to connect to socket.\n");
    }

    //continuously receive data from socket endhost
    char server_response[MAXBUF];
    while(true){
        
        //receive data from the server
        int numbytes = recv(network_socket, server_response, MAXBUF, 0);

        //validates that data was received correctly
        if (numbytes == -1){
            perror("recv");
            exit(1);
        }

        else{
            //printf("count: %d\n", numbytes);
            server_response[numbytes] = '\0';
        }
        //print the data received from server_response
        printf("%s\n", server_response);

    }
    //close the socket
    close(network_socket);

    return 0;
}
    