/**********************************************************************************************************
* Name: Program 4 - opt_dec_d.c
* Class: CS 344-400
* Written By: L. Herrick 
* Description:  program runs in the background as a daemon.  Performs decoding using the one time pad 
* 		method.  To accomplish this, it lisens on a particular port for when a connection is 
* 		made to that port.  It then writes back the message to the process that is connected
* 		via the same port. 
**********************************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	//struct for sockaddir_in
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char *argv[]){
	pid_t pid, sid;			//where pid is parent and cpid is child
	int sockfd, client_sockfd;	//for creating socket
	int port, sizeofadd;		//for port
	struct sockaddr_in server_addr, user_addr;//for server address
	socklen_t userSize;
	int maxSize = 2000;
	char fileName[maxSize];		//name of file
	char message[maxSize];	  	//message in - will hold encryption
	char key[maxSize];		//will hold key to decrypt with
	char decryptmess[maxSize];	//message out - will hold decrypted text
	int  s, r, w;			//for read( ) and write( )
	

	//correct usage opt_enc_d [port numb]
	if(argc < 2){
		printf("Usage: opt_enc_d [port number]\n");
		exit(1);	//only startup error terms program
	}	

	//when a connection is made with opt_enc, fork immediatly so as to remain available to connections
	pid = fork();
	
	//error with fork
	if(pid < 0){
		perror("fork");
		exit(1);
	}
	//parent	
	else if(pid > 0){
		//done - exit and remain available for connections
		exit(0);
	}
	//child process
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd <  0){
		error("socket");
		//exit(1);
	}
	port = atoi(argv[1]);
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	s = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(s < 0){
		error("bind");
		//exit(1);
	} 
	userSize = sizeof(user_addr);
	
	//support up to 5 concurrent connections	
	listen(sockfd, 5);
	while(1){
		//client_sockfd = accept(sockfd, (struct sockaddr *) &user, sizeof(user));
		client_sockfd = accept(sockfd, (struct sockaddr *) &user_addr, &userSize);
		if(client_sockfd < 0){
			error("accept");
			/* here you may want to continue or exit - depends */
		}
		

		//read data from client (message)
		bzero(fileName, maxSize);
		r = read(client_sockfd, fileName, maxSize);
		if(r < 0){
			error("read");
			//_exit(1);
		}	
		//open file
		FILE *file;
		file = fopen(fileName, "r");
		if(file == NULL){
			error("fopen message");
			//_exit(1);
		}
		//read from file with message
		bzero(message, maxSize);
		fgets(message, maxSize, file);		
		fclose(file);
			
		bzero(fileName, maxSize);
		//read data from client (key)
		r = read(client_sockfd, fileName, maxSize);
		if(r < 0){
			error("read");
		}	
		//open file with key
		
		if((file = fopen(fileName, "r")) == NULL){
			error("fopen key");
		}
		//read from file with key
		bzero(key, maxSize);
		fgets(key, maxSize, file);

		fclose(file);
							

		//encrypt
		int i, buffer;	//buffer for ascii checking
		bzero(decryptmess, maxSize);
		for(i=0; i<strlen(message); i++){
			//turn mess and key to A=0.., add key then modulo by 26. Spaces need to equal 26.
			if(message[i] == 32){
				//space = 26
				if(key[i] == 32){ //will be 0, A
					buffer = (((message[i]-6)-(key[i]-6))+65);
				}
				else{
					buffer = ((message[i]-6)-(key[i]-65));
					if(buffer < 0){
						buffer = 27 + buffer;
					}
					if(buffer == 26){
						decryptmess[i] = 32;
					}
					else{ //A-Z char
						decryptmess[i] = buffer+65;
					}		
				}
			}
			else{
				if(key[i] == 32){ //space
					//printf("3. %c is a space\n", key[i]);
					buffer = ((message[i]-65)-(key[i]-6));
					if(buffer < 0){
						buffer = 27 + buffer;
					}
					if(buffer == 26){ //space equals 26 in this program
						decryptmess[i] = 32;
					}
					else{
						decryptmess[i] = buffer+65;
					}
				}
				else{ //A-Z message and key
					buffer = ((message[i]-65)-(key[i]-65)); 
					if(buffer < 0){
						buffer = 27 + buffer;
					}
					if(buffer == 26){
						decryptmess[i] = 32;
					}
					else{
						decryptmess[i] = buffer+65;
					}
				}
			}	
		}
		int esize = strlen(decryptmess)-1;
		decryptmess[esize]='\0';		

		//write data to client
		if((w = write(client_sockfd, decryptmess, maxSize)) == -1){
			error("write");
		}
			
	}
	return 0;
}
