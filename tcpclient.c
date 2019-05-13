//Andrew Wolstenholme tcpclient.c
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

void error(char *msg) {
	perror(msg);
	exit(0); 
}

//layer 1
int l1_write(int sockfd, char b){
	//Writes the byte specified by b to the socket specified by sockfd
	int n = write(sockfd, &b, 1);
	if (n < 0){
		error("ERROR writing to socket");
		return n;
	}
	//return bytes written
	return n;
}

//layer 2
int l2_write(int sockfd, char *line){
	//Sends a line starting at the address specified by the 
	//second parameter (line) to the socket specified by sockfd
	int i = 0;
	//loop through line, sending each character to layer 1
	//stop on the end of line
	while (l1_write(sockfd, line[i]) > 0){
		if(line[i] == '\0'){
			break;
		}
		i++;
	}
	return i;
}

//layer 3
int l3_write(int sockfd, char *filename){
	//Sends the content of a file specified by filename 
	//to the socket specified by sockfd
	int lineCount = 0;
	char buffer[256];
	//open file for reading only
	FILE *filepointer = fopen(filename, "r");
	//get each line from file, send to layer 2 
	while(fgets(buffer, 256, filepointer) != NULL){
		l2_write(sockfd, buffer);
		lineCount++;
		//clear buffer each time
		bzero(buffer, 256);
	}
	//return lines reversed
	return lineCount;
}

int main(int argc, char *argv[]) {

	//code for connection provided by sample TCPclient and server code 
	int sockfd, portno;
	struct sockaddr_in serv_addr; 
	struct hostent *server;
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0); 
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0)
		error("ERROR opening socket"); 
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n"); exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,server->h_length); 
	serv_addr.sin_port = htons(portno); 
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");


	int lines = l3_write(sockfd, argv[3]);
	printf("%d lines read and reversed by server.\n", lines);

	//close connection
	close(sockfd);
	return 0;
}
