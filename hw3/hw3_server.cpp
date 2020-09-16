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
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr,"ERROR, no IP provided\n");
        exit(1);
    }

    int sockfd, newsockfd, portno;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    //portno = argv[]
    int n;
     
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ 
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }

    while(1){
        
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if (n < 0){
            error("ERROR reading from socket");
        }
        printf("Client: %s\n", buffer);
        
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0){
            error("ERROR writing to socket");
        }

        if (strncmp("quit", buffer, 4) == 0){
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0; 
}


// void error (const char *msg){
//     perror(msg);
//     exit(1);
// }


// int main(int argc, char *argv[]){
//     int sockfd, newsockfd, portno;
//     socklen_t clilen;
//     char buffer[255];
//     struct sockadder_in serv_addr, cli_addr;

//     int n;
//     if (argc < 2){
//         printf("ERROR: no port provided\n");
//         exit(1);
//     }

//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
// }


// #define PORT 9002

// int main() {

//     //initialize message
//     char server_message[256] = "You have reached the server!\nThis is another example line!\n";
    
//     //create a socket
//     int server_socket = socket(AF_INET, SOCK_STREAM, 0);

//     //initialize attributes for the socket
//     struct sockaddr_in server_address;
//     server_address.sin_family = AF_INET;
//     server_address.sin_port = htons(PORT);
//     server_address.sin_addr.s_addr = INADDR_ANY;

//     //bind the socket to specified IP and port
//     bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

//     //listening for client
//     listen(server_socket, 5);

//     printf("server listening...\n");
//     int client_socket = accept(server_socket, NULL, NULL);
//     printf("server accepted client socket\n");
    
//     //send the message
//     send(client_socket, server_message, sizeof(server_message), 0);

//     printf("Sent message to client.\n");
 
//     //-----------------------------receive message---------------------------------

//     int connection_status = connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

//     //check for error with connection
//     if (connection_status == -1){
//         printf("There was an error making connection to remote socket.\n");
//     }

//     //receive data from the client
//     char client_response[256];
//     recv(server_socket, &client_response, sizeof(client_response), 0);
//     printf("%s", client_response);

//     //close socket
//     close(server_socket);

//     return 0;
// }
