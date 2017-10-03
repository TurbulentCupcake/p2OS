#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFFERSIZE 128
char buffer[BUFFERSIZE];
int count_commands = 1; // keeps count of commands
char CWDBUFFER[1000];
pid_t  PIDLIST[1000];
char INPUTBUFFERSIZE = 10000;


void printArgs(char ** commandArgs);
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
	
	if(getcwd(CWDBUFFER, sizeof(char)*1000) == NULL) { 
		// handle error if unable to print current wd
	}	
	printf("%s\n", CWDBUFFER);
}


// function that changes directory according to destination
// specifided by the user 
void wd(char ** commandArgs){ 


	if(commandArgs[1] == NULL) { 
		if(chdir(getenv("HOME")) < 0) { 
			// handle error if unable to go to home
		}
	} else {
		if(chdir(commandArgs[1]) < 0)  {
			// handle error if unable to reach specified directoryh
		}
	}
		
	
}



// general executor function that executes command based on the input
void executor(char ** commandArgs)  {
	
	// obtain the size of the command
	int cmdSize = 0;
	while(commandArgs[cmdSize] != NULL) { cmdSize++; } 
		
//	printf("command size = %s \n " , commandArgs[0]);
		
	// construct an argument buffer to send to execvp
	char * argv[cmdSize+1];
	
	// create a string to specify location of process commands 
	char exec_command[256];
	
	// concatenate the user command to bin so it can be called. 
	snprintf(exec_command, sizeof(exec_command), "%s%s", "/bin/", commandArgs[0]);
	//printf("command exec = %s\n", exec_command);	
	// place new value in argv[0];
	argv[0] = strdup(exec_command);		
	
	// fill in the rest of the commands in the arguments 
	int i = 1;
	while(commandArgs[i] != NULL) { 
		argv[i] = commandArgs[i]; 
		i++;
	}
	argv[i+1] = NULL;

	// call execvp with the arguments created
	if(execvp(argv[0], argv) < 0) { 
		// error check if this command didnt execute correctly
	}


}

// function to deal with output redirection
void output_redirect(char ** commandArgs, int charPosition) { 

	// null out the position where the redirection is present
	commandArgs[charPosition] = NULL;
//	printArgs(commandArgs);
	
	// open an input file
	int out = open(commandArgs[charPosition+1], O_WRONLY|O_CREAT|O_TRUNC, 0600);

	dup2(out, 1);	
		
	commandArgs[charPosition+1] = NULL;	
	executor(commandArgs);


	close(out);
	return;
}

void input_redirect(char ** commandArgs, int charPosition) {

	// null out the character position
	commandArgs[charPosition] = NULL;
	
	// open in read only mode
	int in = open(commandArgs[charPosition+1], O_RDONLY, 0600);

	// duplicate the input file descriptor
	dup2(in, 2);	

	// create the input buffer 
	char input_buffer[INPUTBUFFERSIZE];
	
	// copy the input from file to buffer 
	fgets(input_buffer,INPUTBUFFERSIZE, in);
	
	printf("%s\n", input_buffer);
	

}



// seperate command to execute functions that require forking 
void executeCommand(char ** commandArgs, int isBackground) {

	// calculate the number of arguments
	int cmdSize = 0;
	while(commandArgs[cmdSize] != NULL) { cmdSize++; }
	
	// check if either of the redirections 
	// or piping occur
	
	int charPosition = 0;
	while(commandArgs[charPosition] != NULL) { 
			if(strcmp(commandArgs[charPosition], ">")==0 || 
			   strcmp(commandArgs[charPosition], "<")==0) {
				break;
			}
		charPosition++;
	}
	int status;
	// fork child
	pid_t rc = fork();
	if(rc == 0) { // child process
		//fflush(stdout);
		if(commandArgs[charPosition] != NULL && strcmp(commandArgs[charPosition],">") == 0) { 
		//	fprintf(stderr, "reached here\n");	
			output_redirect(commandArgs, charPosition); 
		} else if (commandArgs[charPosition] != NULL && strcmp(commandArgs[charPosition],"<") == 0) {
			// input redirection
			input_redirect(commandArgs, charPosition);			
		} else { 
			executor(commandArgs);
		}
	} else if (rc > 0) { // parent process	
		// if process is not executed in background, wait for it
		if(isBackground == 0) {
			(void) waitpid(rc, &status, 0);
		}
		// record the process so that we can kill it if it is defunct
		int i = 0;
		while(PIDLIST[i] != NULL) { // find a free location to store pid
			i++;
		}
		PIDLIST[i] = rc;
		
	}
		
}
			



// test function to print arguments
void printArgs(char ** commandArgs) {
	
	int i = 0;
	while(commandArgs[i] != NULL) { 
		//fwrite(commandArgs[i],1, 8, stdout);
		printf("%s\n", commandArgs[i]);
		i++;
	}
	

}





int main(int argc, char * argv [])  {
	
// create a while loop that iteratively takes in 
// an input, parses it and executes the commands accordingly
int status;
for(;;) { 
	
	char * command = prompt(); // get the command from user
	char ** commandArgs = parseCommand(command);
	 //printArgs(commandArgs);		


	if(commandArgs[0] == NULL) { 
			continue;
	}
		
	// the parsed command args must now be executed accordingly
//	printf("%s", commandArgs[0]);	
	if(strcmp(commandArgs[0], "pwd") == 0) { 
		pwd(); // if the user calls pwd, then pwd
	} else if (strcmp(commandArgs[0], "cd") == 0) {
		wd(commandArgs);		
	} else {
		// if the command is different and requires to be forked
		
		// check if the program has to run in the background
		int size=0;
		while(commandArgs[size] != NULL) { size++; } // obtain size of commandArgs
		// we need to check if the second-to-last element is an & (last element is always NULL)
		if(strcmp(commandArgs[size-1], "&") == 0) {
			commandArgs[size-1] = NULL; // make position of & into NULL in order to parse easily
			executeCommand(commandArgs, 1); // execute command such that it runs in background process 	
	
		} else { 
			executeCommand(commandArgs, 0); // else execute normally without background exec
		}	
	}

	// iterate through the PIDLIST to find all process that are dead and remove them 
	for(int i; i <  1000; i++) { 
		if(PIDLIST[i] != NULL) { 
			if(waitpid(PIDLIST[i],&status, WNOHANG) == -1) { 
			//	kill(PIDLIST[i], SIGKILL);
			//	PIDLIST[i] = NULL;
			} 
		}	
	}
	free(command);
	


	// handle dead processes	
}











}




