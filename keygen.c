/***********************************************************************************************************
* Name: Program 4 - keygen.c
* Class: CS 344-400
* Written By: L. Herrick 
* Description:  program creates a file with a key string inside it, matching the length of a given 
* 		parameter size.  Keys are only allowed to use randomized A-Z and space characters. 
*
***********************************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	//struct for sockaddir_in
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]){
	int length, random, i;	
//	char key[80000]; //longest file is actually 6438	
	
	//length of current file
	length = atoi(argv[1]);	
	char key[length+1];

	//create a random seed
	srand(time(NULL));

	for(i=0; i < length; i++){
		random = rand() % 27;
		if(random == 26){
			key[i] = 32;		//translate to ascii space
		}
		else{
			key[i] = random + 65;	//translate to ascii A-Z
		} 
	}
	//add newline to show end for later fgets
	key[length] = 0;


	//output key to stdout
	printf("%s\n", key);

	return 0;
}
