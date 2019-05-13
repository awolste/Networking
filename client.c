//Andrew Wolstenholme client.c

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */
#include <stdlib.h>

#define REMOTE_SERVER_PORT 1234
#define MAX_MSG 100

int main(int argc, char *argv[]) {
	int sd, rc;
	struct sockaddr_in remoteServAddr;
	struct hostent *h;

	/* check command line args */
	if(argc<3) {
	printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
	exit(1);
	}

	/* get server IP address (no check if input is IP address or DNS name */
	h = gethostbyname(argv[1]);
	if(h==NULL) {
		printf("%s: unknown host '%s' \n", argv[0], argv[1]);
		exit(1);
	}
	printf("%s: sending data to '%s' (IP : %s) \n", argv[0],
		 h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
	remoteServAddr.sin_family = h->h_addrtype;
	memcpy((char *) &remoteServAddr.sin_addr.s_addr,
		h->h_addr_list[0], h->h_length);
	int portNo = atoi(argv[2]);
	remoteServAddr.sin_port = htons(portNo);

	/* socket creation */
	sd = socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0) {
		printf("%s: cannot open socket \n",argv[0]); exit(1);
	}

	/* send file name */
	rc = sendto(sd, argv[3], strlen(argv[3])+1, 0,
		(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
		if(rc<0) {
			printf("cannot send data\n");
			close(sd);
			exit(1);
		}

	/* send data */
	char buffer[MAX_MSG];
	FILE *filepointer = fopen(argv[3], "r");
	while(fgets(buffer, MAX_MSG, filepointer) != NULL){
		rc = sendto(sd, buffer, strlen(buffer)+1, 0,
		(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
		//clear buffer each time
		if(rc<0) {
			printf("cannot send data");
			close(sd);
			exit(1);
		}
		bzero(buffer, MAX_MSG);
	}
	//send end of file
	char eof = EOF;
	rc = sendto(sd, &eof, strlen(buffer)+1, 0,
		(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
	return 1;
}
