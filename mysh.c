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
char ** parseCommand(char * command) { 

	char *token;
	char * saveptr = command;
	char ** split_cmds = malloc(sizeof(char*)*BUFFERSIZE);
	int i = 1;	

	// tokenize the command line
	split_cmds[0] = strtok_r(command, "\t ", &saveptr);
	while((token = strtok_r(NULL,"\t ", &saveptr))) { 
		split_cmds[i] = token;
		i++;
	}

	char **	split_cmds_2 = malloc(sizeof(char*)*(i+1));
	for(int j = 0 ; j < i ;  j++) { 
		split_cmds_2[j] = split_cmds[j];
		//printf("Command Arg : %s\n", split_cmds_2[j]);
	}
	

	split_cmds_2[i++] = NULL;
	free(split_cmds);
	
	return split_cmds_2;	




}



// test function to print arguments
void printArgs(char ** commandArgs) {
	
	int i = 0;
	while(commandArgs[i] != NULL) { 
		printf("%s-", commandArgs[i]);
		i++;
	}

}





int main(int argc, char * argv [])  {
	
// create a while loop that iteratively takes in 
// an input, parses it and executes the commands accordingly

for(;;) { 
	
	char * command = prompt(); // get the command from user
	char ** commandArgs = parseCommand(command);
	// printArgs(commandArgs);	

	free(command);
		
}











}




