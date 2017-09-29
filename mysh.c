#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BUFFERSIZE 128
char buffer[BUFFERSIZE];
int count_commands = 1; // keeps count of commands
char CWDBUFFER[1000];


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
		split_cmds_2[j] = strtok(split_cmds_2[j],"\n");
	}
	

	split_cmds_2[i++] = NULL;
	free(split_cmds);
	
	return split_cmds_2;	




}

// function that prints the current working directory
void pwd() { 	
	
	getcwd(CWDBUFFER, sizeof(char)*1000);
	printf("%s\n", CWDBUFFER);
}


// function that changes directory according to destination
// specifided by the user 
void wd(char ** commandArgs){ 

	if(commandArgs[1] == NULL) { 
		chdir(getenv("HOME"));
	} else {
	}
		
		
	
}


// test function to print arguments
void printArgs(char ** commandArgs) {
	
	int i = 0;
	while(commandArgs[i] != NULL) { 
		fwrite(commandArgs[i],1, 8, stdout);
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
	
	// the parsed command args must now be executed accordingly
//	printf("%s", commandArgs[0]);	
	if(strcmp(commandArgs[0], "pwd") == 0) { 
		pwd(); // if the user calls pwd, then pwd
	} else if (strcmp(commandArgs[0], "cd") == 0) {
		wd(commandArgs);		
	} else {
	//	printf("command not regonized\n");
	}
	
	free(command);
		
}











}




