#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 128
char buffer[BUFFERSIZE];
int count_commands = 1; // keeps count of commands



// function to iteratively print prompt
// and retrieve user command
char * prompt() { 
	
	printf("mysh (%d)> ", count_commands);
	if(fgets(buffer, BUFFERSIZE, stdin) == NULL) {
		// throw error if user doesnt enter correctly
	}
	if(strlen(buffer) > BUFFERSIZE) { 
		// throw error it command line has more chars than
		// allowed buffer
	}
	
	char * command = malloc(sizeof(char)*BUFFERSIZE);
	if(!command) { 
		// throw error if malloc failed 
	}
	
	strcpy(command, buffer);
	count_commands++;
	
	return command;
}


// returns an array with parsed commands
char * parseCommand(char * command) { 

	char * saveptr = command;
	char ** split_cmds = malloc(sizeof(char*)*BUFFERSIZE);
	int i = 0;	

	while((token = strtok))r



}








int main(int argc, char * argv [])  {
	
// create a while loop that iteratively takes in 
// an input, parses it and executes the commands accordingly

for(;;) { 
	
	char * command = prompt(); // get the command from user
	printf("%s\n", command);				
	free(command);
		
}











}




