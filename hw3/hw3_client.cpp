#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 17932

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname, port\n", argv[0]);
       exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
 
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);
    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ 
        error("ERROR connecting");
    }

    while(1){
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");

        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("Server: %s\n", buffer);

        if (strncmp("quit", buffer, 4) == 0){
            break;
        }
    }

    close(sockfd);
    return 0;
}

// #define PORT 9002

// int main(int argc, char *argv[]) {

//  	if ((argc != 2) && (argc != 3)){
//  		printf("Usage: %s hostname [port]\n", argv[0]);
//  		exit(1);
//  	}

//  	int port = PORT;

//  	if (argc == 3){
//  		port = atoi(argv[2]);
//  	}

//      create a socket
//     int client_socket = socket(AF_INET, SOCK_STREAM, 0);

//     //specify address for the socket
//     struct sockaddr_in server_address;
//     server_address.sin_family = AF_INET;
//     server_address.sin_port = htons(port);
//     server_address.sin_addr.s_addr = INADDR_ANY;

//     //connect the socket to the server
//     int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));

//     //check for error with connection
//     if (connection_status == -1){
//         printf("There was an error making connection to remote socket.\n");
//     }

//     //receive data from the server
//     char server_response[256];
//     recv(client_socket, &server_response, sizeof(server_response), 0);
//     printf("%s", server_response);

//     //-----------------------------send message---------------------------------

//     bind(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));

//     //listening for client
//     listen(client_socket, 5);

//     printf("client listening...\n");
//     int server_socket = accept(client_socket, NULL, NULL);
//     printf("client accepted server socket\n");
    
//     char client_message[256] = "You have reached the client!\n";
//     //send the message
//     send(server_socket, client_message, sizeof(client_message), 0);

//     printf("\n\n\nClient is sending a message to the server.\n");

//     char client_message[256] = "\n\nRANDOM MESSAGE\n";
//     send(network_socket, client_message, sizeof(client_message), 0);

//     //close socket
//     close(client_socket);

//     return 0;
// }
