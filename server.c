//Andrew Wolstenholme server.c

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */ 
#include <string.h> /* memset() */
#include <stdlib.h>
#include <fcntl.h>
#define LOCAL_SERVER_PORT 1234 
#define MAX_MSG 100

int main(int argc, char *argv[]) {
	int sd, rc, n, cliLen;
	struct sockaddr_in cliAddr, servAddr; 
	char msg[MAX_MSG];
	char *tempLine;	
	int fileDescriptor;

	/* check command line args */ 
	if(argc<2) {
	printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
	exit(1); 
	}

	/* socket creation */
	sd=socket(AF_INET, SOCK_DGRAM, 0); 
	if(sd<0) {
		printf("%s: cannot open socket \n",argv[0]); 
		exit(1);
	}

	/* bind local server port */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int portNo = atoi(argv[1]);
	servAddr.sin_port = htons(portNo);
	rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr)); 
	if(rc<0) {
		printf("%s: cannot bind port number %d \n", 
		argv[0], portNo);
		exit(1); 
	}
	printf("%s: waiting for data on port UDP %u\n", argv[0],portNo);
	
	/* init buffer */ 
	memset(msg,0x0,MAX_MSG);
	/* receive message */
	cliLen = sizeof(cliAddr);
	n = recvfrom(sd, msg, MAX_MSG, 0,
	(struct sockaddr *) &cliAddr, (socklen_t *)&cliLen);
	if(n<0) {
		printf("%s: cannot receive data \n",argv[0]); 
		//insert error for getting line again
	}
	printf("%s: from %s:UDP%u filename: %s \n", argv[0],
		inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port),msg);
	
	fileDescriptor = open(msg, O_RDWR | O_CREAT, 0666);
	if (fileDescriptor < 0) {
		printf("Error opening file. Quitting...");
        	return -1;
    	}

	/* server infinite loop */ 
	while(1) {
//take n off the front or say in readme that it is because the reverse
		/* init buffer */ 
		memset(msg,0x0,MAX_MSG);
		/* receive message */
		cliLen = sizeof(cliAddr);
		n = recvfrom(sd, msg, MAX_MSG, 0,
		(struct sockaddr *) &cliAddr, (socklen_t *)&cliLen);
		if(n<0) {
			printf("%s: cannot receive data \n",argv[0]); 
			continue;
		}
		//client should tell server when to quit
		if (msg[0] == EOF)
			break;

		/* write line to file in reverse*/ 
		int strLen = 0;
		tempLine = (char*)malloc(MAX_MSG);
		while(msg[strLen] != '\0'){
			tempLine[strLen] = msg[strLen];
			strLen++;
		}			
		//reverse function, step forward in line and backwards in tempLine
		int k = strLen-2;		
		for (int j = 0; j < strLen; j++){
			msg[j] = tempLine[k];
			k--;
		}
		msg[strLen-1] = '\n';
		printf("Writting to file in reverse.\n");
		write(fileDescriptor, msg, strLen);
		free(tempLine);
	}
	/* end of server infinite loop */ 
	return 0;
}
