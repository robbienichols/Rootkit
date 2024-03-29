#include <netdb.h> 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
  
// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
	char buff[MAX]; 
    	bzero(buff, MAX);   
   	 // read the message from client and copy it in buffer 
   	read(sockfd, buff, sizeof(buff)); 
    	// print buffer which contains the client contents 
    	printf("%c", buff[0]); 
    	bzero(buff, MAX);
} 
  
// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(PF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    }
    bzero(&servaddr, sizeof(servaddr)); 
    // assign IP, PORT 
    servaddr.sin_family = PF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } else {
    	printf("Bind complete\n");
    }
    for(;;){
    // Now server is ready to listen and verification 
    	if ((listen(sockfd, 5)) != 0) { 
    	    printf("Listen failed...\n"); 
    	    exit(0); 
    	} else printf("Server listening..\n"); 
    	
    	len = sizeof(cli); 
    	// Accept the data packet from client and verification 
    	connfd = accept(sockfd, (SA*)&cli, &len); 
    	if (connfd < 0) { 
    	    printf("server acccept failed...\n"); 
    	    exit(0); 
    	} 
    	//else
    	  //  printf("server acccept the client...\n");  
    	func(connfd); 
    }
    // After chatting close the socket 
    close(sockfd); 
} 
