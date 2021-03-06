#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9002

int main() {

    //initialize message "HELLO, WORLD!" ASCII art
    char server_message[256] = "|  | |-- |   |    |-|     \\      /  |-|   |->   |   |-\\   |\n|--| |-- |   |   |   |     \\    /  |   |  |  >  |   |  |  |\n|  | |-- |__ |__  |-|  ,    \\/\\/    |-|   | \\   |__ |-/   o";
    
    //create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //initialize attributes for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    //listening for client
    listen(server_socket, 5);

    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1){
        printf("There was an error accepting remote socket.\n");
    }
    //send the message
    send(client_socket, server_message, sizeof(server_message), 0);

    //close socket
    close(server_socket);

    return 0;
}
