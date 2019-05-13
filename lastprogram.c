//Andrew Wolstenholme lastprogram.c

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>

#define MALLOC_SIZE 1024
#define MAX_SD 25
#define PORT_NO 9000

typedef struct chat_client_t
{
	char domain[MALLOC_SIZE];
	int sockfd;
}chat_client_t;

//A global array to hold all the programs socket descriptors
int sd_arr[MAX_SD];

//The fd_set. I made it global altough it's not needed for this example
fd_set socket_set;

//Rebuilding an fd_set using the global descriptor array
void rebuild_fd_set(fd_set* socks)
{
    FD_ZERO(socks);
    for(int i=0 ; i<MAX_SD; i++)
    {
        if(sd_arr[i]!=0)
        {
            FD_SET(sd_arr[i], socks);
        }
    }
}

/*
Adds a descriptor to the global array. There might be a better way to do this.
*/
void add_sd(int sda[MAX_SD], int sd)
{
    for(int i=0 ; i<MAX_SD; i++)
    {
        if(sda[i]==0 || sda[i]==sd)
        {
            sda[i] = sd;
            return;
        }
    }
    printf("FATAL ERROR : Maximum number of socket descriptors (MAX_SD %d) exceeded\n", MAX_SD);
    exit(1);
}

int guard(int n, char* err) { if (n==-1) {perror(err); exit (1); } return n;}

int konnect(char* domainName)
{
	struct sockaddr_in serv_addr;
	int sockdes = socket(AF_INET, SOCK_STREAM, 0);
	if(sockdes<0) {
		printf("ERROR opening socket\n");
	}
	else
		printf("Created a Connection to '%s'\n\n", domainName);
	struct hostent *h = gethostbyname(domainName);
	if(h==NULL) {
		fprintf(stderr,"ERROR, no such host\n"); exit(0);
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)h->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,h->h_length);
	serv_addr.sin_port = htons(PORT_NO);
	if (connect(sockdes, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		printf("ERROR connecting\n");
	return sockdes;
}



void* threadF(void* sockfd){
	int r;
	int socketdes = *((int*)sockfd);
	char buff[MALLOC_SIZE];

	while(1){
		struct sockaddr_in cli_addr;
		socklen_t clilen = sizeof(cli_addr);
		bzero(buff, MALLOC_SIZE);
		r = read(socketdes, buff, MALLOC_SIZE);

		if(r > 0){
				getpeername(socketdes, (struct sockaddr *)&cli_addr, &clilen);
				printf("%s sent the message: %s\n\n", inet_ntoa(cli_addr.sin_addr), buff);
		}
		else{
			printf("ERROR reading\n");
			break;
		}
	}
	return NULL;
}



chat_client_t clientInfo[MAX_SD];
int counter = 0;

int main(int argc, char** argv)
{
    int listen_socket_fd = guard (socket(AF_INET, SOCK_STREAM, 0), "Could not create TCP listening socket\n");
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    guard (bind(listen_socket_fd, (struct sockaddr*) &addr, sizeof(addr)), "Could not bind\n");
    guard( listen (listen_socket_fd, 100), "Could not listen!\n");
    int maxsd = listen_socket_fd;
    //Server's infinite loop!
    while(1)
    {
				struct sockaddr_in cli_addr;
				socklen_t clilen = sizeof(cli_addr);
				FD_ZERO(&socket_set);
		    FD_SET( listen_socket_fd, &socket_set);
		    FD_SET(STDIN_FILENO, &socket_set);

        printf("Use 'i' to initate a connection.\n");
				printf("Use 's' to send a message to an exisiting connection.\n");
				printf("Use 'x' to terminate a connection.\n");
				printf("Waiting for a connection.\n\n");
        int rc = select (maxsd+1, &socket_set,NULL, NULL, NULL);
        guard(rc, "select() returned with an error!\n");
        //Let us check all other socket descriptors as well to see if any of them has also become available
            //Does this socket has data ready?
	      if( FD_ISSET(listen_socket_fd, &socket_set) )
	      {
		        int client_socket_fd = accept (listen_socket_fd, (struct sockaddr *)&cli_addr, &clilen);
						if(client_socket_fd > 0)
							printf("Accepted a Connection from: %s\n\n", inet_ntoa(cli_addr.sin_addr));
		        guard(client_socket_fd, "Failed To accept\n");
		        //Add the new socket to our global array. Note: You should also remove it when the connection terminates.
		        //update maxsd to the latestes opened socket.
		        maxsd = client_socket_fd;

						pthread_t pthread;
						pthread_create(&pthread, 0, threadF, &client_socket_fd);
	      }
				if ( FD_ISSET(STDIN_FILENO, &socket_set) ) //STDIN is ready
				{
						char* buff = (char*)malloc(MALLOC_SIZE);
						bzero(buff, MALLOC_SIZE);
						fgets(buff, MALLOC_SIZE, stdin);

						buff [ strlen(buff) - 1] = '\0';

						if(buff[0] == 'i')//initiate connection
						{
							int exists = 0;
							for (int y = 0; y < MAX_SD; y++){
								if(strcmp(buff + 2, clientInfo[y].domain) == 0)
									exists = 1;
							}
							if(exists == 0){
								int des = konnect(buff + 2);
								//struct chat_client s;
								strcpy(clientInfo[counter].domain, buff + 2);
								clientInfo[counter].sockfd = des;
								//add struct to the array
								counter++;
							}
							else
								printf("Connection already exists\n");
						}
						else if(buff[0] == 's')//send
						{
								char* msgStart = (char*)malloc(MALLOC_SIZE);
								bzero(msgStart, MALLOC_SIZE);
								msgStart = strstr(buff + 2, " ") + 1;
								//loop throgh array of structs and match using strcmp to domain in buff

								char* domainName = strtok(buff+2, " ");
								for(int y = 0; y < MAX_SD; y++)
								{
									if (strcmp(domainName, clientInfo[y].domain) == 0){
										//send message to clientInfo[y].sockfd;
											if(0 > send(clientInfo[y].sockfd, msgStart, strlen(msgStart), 0))
											{
												printf("Cannot send message\n");
											}
									}
								}
							}
							else if(buff[0] == 'x')//quit
							{//loop to find domain name
									strcpy(clientInfo[counter].domain, buff + 2);
									for(int j = 0; j < MAX_SD; j++){
										if (strcmp(buff+2, clientInfo[j].domain) == 0)
										{
											close (clientInfo[j].sockfd);
											printf("Closed Connection (socketfd=%d) to '%s'\n\n", clientInfo[j].sockfd, clientInfo[j].domain );
											strcpy ( clientInfo[j].domain, "");
											clientInfo[j].sockfd = 0;
											break;
										}
									}
							}
							else if(buff[0] == 'q')//quit all
							{//loop to find domain name
									int openConnection = 0;
									for(int j = 0; j < MAX_SD; j++){
										if (clientInfo[j].sockfd != 0)
										{
											openConnection = 1;
											break;
										}
									}
								if(openConnection == 0){
									printf("No connections open. Quiting Program.\n");
									exit(1);
								}
								else
									printf("Open Connections. Cannot quit.\n");
							}
							else
							{
								printf("No command found.\n");
							}
				}//closes if STDIN is ready
    }//closes while
}
