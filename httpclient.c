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

        if(argc<3) {
                printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
                exit(1);
        }
        //code for connection provided by sample TCPclient and server code
        int sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        //create socet
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
        serv_addr.sin_port = htons(REMOTE_SERVER_PORT);
        if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
                error("ERROR connecting");

        //write into connection http query
        //read from link, head
        //print error message
        //if 200 find content, save into file
        char* getRequest;
        getRequest = (char *) malloc (MAX_MSG);
        bzero(getRequest, MAX_MSG);

        sprintf(getRequest, "GET / HTTP/1.1\r\nHost: %s \r\nConnection: close\r\n\r\n", argv[1]);
        if (send(sockfd,getRequest, strlen(getRequest),0) < 0)
                error("ERROR sending request");

        int filedes = open(argv[2], O_WRONLY | O_CREAT, 0666);
        char* buffer;
        buffer = (char *) malloc (MAX_MSG);
        bzero(buffer, MAX_MSG);
        char tempBuffer[4];
        bool header = true;
while (recv(sockfd, buffer, 1, 0) > 0){
        /*if (header){
                tempBuffer[0] = tempBuffer[1];
                tempBuffer[1] = tempBuffer[2];
                tempBuffer[2] = tempBuffer[3];
                tempBuffer[3] = *buffer;
                if (tempBuffer[0] == '\r' && tempBuffer[1] == '\n' && tempBuffer[2] == '\r' && tempBuffer[3] == '\n')
                        header = false;
        }
        else{*/
                //printf("in if loop\n");
                write(filedes, buffer, MAX_MSG);
        //}
}


//close connection
close(sockfd);
return 0;

//convert to binary

//socket creation
//connect to port 80
//
//      HEAD /
}
                                                       1,1           Top
