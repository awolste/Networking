//Andrew Wolstenholme tcpsever.c
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

void error(char *msg) {
	perror(msg);
	exit(1); 
}

//layer 1
int l1_read(int sockfd, char *b){
	//Reads one byte and copies the byte to the address 
	//specified by b from the socket specified by sockfd
	int n = read(sockfd, b, 1);
	if (n < 0){
		error("ERROR reading from socket in l1 read");
		return n;
	}
	//returns bytes read
	return n;
}

//layer 2
int l2_read(int sockfd, char *line){
	//Reads a line from the socket specified by sockfd, 
	//and reverses the received line and stores it in memory 
	//starting at the address specified by the pointer *line
	bzero(line, 256);
	int strLen = 0;
	char buffer;
	char *tempLine;	
	tempLine = (char*)malloc(256);
	int n;	

	//loop through line, send each character to layer 1 and create tempLine
	//for reverse. When end is reached, add /0 to the end
	while((n = l1_read(sockfd, &buffer)) > 0){
		tempLine[strLen] = buffer;
		if (buffer == '\0'){
			tempLine[strLen] = '\0';
			break;
		}
		strLen++;
	}
	//reverse function, step forward in line and backwards in tempLine
	int k = strLen - 1;		
	for (int j = 0; j < strLen; j++){
		line[j] = tempLine[k];
		k--;
	}
	free(tempLine);
	return strLen;
}

//layer 3
int l3_read(int sockfd){
	//Reads lines from the socket and print out the reversed lines
	char *line;
	int lineCount = 0;
	int x;
	line = (char*)malloc(256);
	//loop through file, sending a line at a time to layer 2
	while ((x = l2_read(sockfd, line)) > 0){
			printf("%s", line);
			lineCount++;
	}
	printf("\n");
	free(line);
	return lineCount;
} 

int main(int argc, char *argv[]) {
	//code for connection provided by sample TCPclient and server code 
	int sockfd, newsockfd, portno;
	socklen_t clilen; 
	struct sockaddr_in serv_addr, cli_addr; 
	if (argc < 2) { 
		fprintf(stderr,"ERROR, no port provided\n"); 
		exit(1);
	}
	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr)); 
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = INADDR_ANY; 
	serv_addr.sin_port = htons(portno);
	//bind to address
	if (bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	printf("Waiting for connection from client...\n");
	//listen for connection call
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr); 
	//accept connection call
	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
	printf("Accepted connection!\n\n") ;
	if (newsockfd < 0)
		error("ERROR on accept"); 
	//call layer 3, sending the accecpted socket descriptor
	l3_read(newsockfd);

    printf("Done with connection - closing\n\n\n");
    close(sockfd);
	
}
