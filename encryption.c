//Andrew Wolstenholme 3600 - 002
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	char encryptionKey;
	char fileName[128];
	char buffer;
	char c;
	int i = 0;
	int fileDescriptor;
	
	//take file name from standard input
	//account for newline char by reading each char individually into name array
	write(STDOUT_FILENO, "Enter file name: ", 17);
	int flag1 = 1;
	while(flag1 == 1){
		if(read(STDIN_FILENO, &c, 1) > 0){	
			if(c == '\n'){
				fileName[i] = '\0';
				break;
			}
			else{
				fileName[i] = c;
				i++;
			}
		}
		else
			flag1 = 0;
	}
	
	//take key from standard input and put into variable
	write(STDOUT_FILENO, "Enter encryption key: ", 22);
	read(STDIN_FILENO, &encryptionKey, sizeof(encryptionKey));

	//open file and do error check
	fileDescriptor = open(fileName, O_RDWR | O_CREAT, 0666);
	if (fileDescriptor < 0) {
		printf("Error opening file. Quitting...");
        	return -1;
    	}

	//read each char individually from standard in
	int flag = 1;
	while(flag == 1){
		if (read(STDIN_FILENO, &buffer, 1) > 0){
			//XOR each byte and write to file
			buffer = buffer ^ encryptionKey;
			write(fileDescriptor, &buffer, 1);
		}
		else{
			flag = 0;
		}
	}
	return 0;
}
