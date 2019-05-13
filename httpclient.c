//Andrew Wolstenholme httpclient.c 

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

#define MAX_MSG 1024
#define REMOTE_SERVER_PORT 80 

void error(char *msg) {
	perror(msg);
	exit(0); 
}

int main(int argc, char *argv[]) {

	//check for 3 arguments
	if(argc<3) {
		printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
		exit(1); 
	}
	
	//process for cutting URI for host and resource
	char slash[] = "/";
	char *path = (char *)malloc (MAX_MSG);
	//returns substring from / and after into path variable
	path = strstr (argv[1], "/");
	if (path == NULL){
		path = slash;
	}
	char* host;
	host = (char *) malloc (MAX_MSG);	
	//set everything before slash into host
	for(int i = 0; i < strlen(argv[1]); i++){
		if (argv[1][i] != '/')
			host[i] = argv[1][i];
		else 
			break;
	}
	
	//code for connection provided by sample TCPclient and server code 
	int sockfd;
	struct sockaddr_in serv_addr; 
	struct hostent *server;

	//create socet
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0)
		error("ERROR opening socket"); 
	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n"); exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,server->h_length); 
	serv_addr.sin_port = htons(REMOTE_SERVER_PORT); 
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	//request line set using host and path
	char* getRequest;
	getRequest = (char *) malloc (MAX_MSG);
	bzero(getRequest, MAX_MSG);
	sprintf(getRequest, "GET %s HTTP/1.1\r\nHost: %s \r\nConnection: close\r\n\r\n", path, host);
	printf("%s", getRequest);

	//send request to socket
	if (send(sockfd,getRequest, strlen(getRequest),0) < 0)
		error("ERROR sending request");
	
	//open file to save data
	int filedes = open(argv[2], O_WRONLY | O_CREAT, 0666);
	char buffer;
	int count = 0;
	bool header = true;
	//recieve one byte at a time
	while (recv(sockfd, &buffer, 1, 0) > 0){
		if (header){
			printf("%c", buffer);
			//counting until \r\n\r\n is found in succession 
			if(buffer == '\r' || buffer == '\n'){
				count ++;}
			else if (count == 4)
				header = false;
			else
				count = 0;
		}
		//once end of header, begin to write data into file
		if (!header)
			write(filedes, &buffer, 1);
	}

	//close connection
	close(sockfd);
	return 0;
}
