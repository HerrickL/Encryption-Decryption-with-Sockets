/***********************************************************************************************************
* Name: Program 4 - opt_enc.c
* Class: CS 344-400
* Written By: L. Herrick 
* Description:  program connects to opt_enc_d and requests one-time-pad style encryption of a user-input
* 		message.  Takes a plaintext filename, key filename, and port number as arguments. When
* 		opt_enc recieves the ciphered texts from opt_enc_d, it outputs to stdout.   
***********************************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	//struct for sockaddir_in
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char *argv[]){
	int sockfd;
	int port;
	int val = 1;
	struct sockaddr_in server_addr;
	int maxSize = 2000;
	char message[maxSize];		//array will hold plaintext
	char key[maxSize];		//array will hold key
	char endmess[maxSize];		//array will hold encrypted plaintext

	//error check usage
	if(argc < 4){
		printf("Usage: opt_enc plaintext_fileName key_fileName portNum [&]\n");
		exit(1);
	}
	
	//create network endpoint with socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

	//put together the port and address for server
	port = atoi(argv[3]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int messagefd, keyfd;
	messagefd = open(argv[1], O_RDONLY);
	keyfd = open(argv[2], O_RDONLY);

	//check that key is larger than message
	if(lseek(messagefd, 0, SEEK_END) > lseek(keyfd, 0, SEEK_END)){
		perror("key is too short");
		exit(1);
	} 

	//connect
	if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect in main");
		exit(2);//set exit value to 2 as per instructions
	}

	//write blocks of data until complete, error check each one
	snprintf(message, maxSize, "%s", argv[1]);
	if((write(sockfd, message, maxSize)) == -1){//send/write message
		perror("write 1");
		//exit(1);
	}
	snprintf(key, maxSize, "%s", argv[2]);
	if((write(sockfd, key, maxSize)) == -1){//send/write key
		perror("write 2");
		//exit(1);
	}

	//read returned data
	if((read(sockfd, message, maxSize)) < 0){
		perror("read");
		//exit(1);
	}
	
	//output message returned
	snprintf(endmess, maxSize, message);
	printf("%s\n", endmess);
	close(sockfd);
	
	return 0;
}
