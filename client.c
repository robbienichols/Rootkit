#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
void func(int sockfd, char *byte) 
{ 
    char buff[MAX]; 
    int n; 
    bzero(buff, sizeof(buff)); 
    
    n = 0; 
    buff[0] = *byte; 
    printf("buf contents: %c\n", buff[0]);
    write(sockfd, buff, 1); 
} 
  
int main(int argc, char *argv[]) { 
    register int sockfd;
    register int connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } else {
        printf("Socket successfully created..\n"); 
    }
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } else {
        printf("connected to the server..\n"); 
    }
    
    // function for chat 
    func(sockfd, argv[1]); 
  
    // close the socket 
    close(sockfd); 
}
