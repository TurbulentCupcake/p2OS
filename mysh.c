#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFFERSIZE 1256
char buffer[BUFFERSIZE];
int count_commands = 1; // keeps count of commands
char CWDBUFFER[1000];
pid_t  PIDLIST[1000];
int INPUTBUFFERSIZE = 100000;
char command[BUFFERSIZE];
	char * split_cmds_2[BUFFERSIZE];




void printArgs(char ** commandArgs);
// function to iteratively print prompt
// and retrieve user command
char * prompt() { 
	//fflush(stdin);
	printf("mysh (%d)> ", count_commands);
	if(fgets(buffer, BUFFERSIZE, stdin) == NULL) {
		// throw error if user doesnt enter correctly
	}
	if(strlen(buffer) > 129) { 
		// throw error it command line has more chars than
		// allowed buffer
		char error_message[30] = "An error has occurred\n";
        	write(STDERR_FILENO, error_message, strlen(error_message));
        	return NULL;
	}
	
	//char * command = malloc(sizeof(char)*BUFFERSIZE);
	if(!command) { 
		// throw error if malloc failed 

	}
	
	strcpy(command, buffer);
	
	return command;
}


// returns an array with parsed commands
char ** parseCommand(char * command) { 

	char *token;
	char * saveptr = command;
	//char ** split_cmds = malloc(sizeof(char*)*BUFFERSIZE);
	char *split_cmds[BUFFERSIZE];
	int i = 1;	

	// tokenize the command line
	split_cmds[0] = strtok_r(command, "\t ", &saveptr);
	while((token = strtok_r(NULL,"\t ", &saveptr))) { 
		split_cmds[i] = token;
		i++;
	}

//	char **	split_cmds_2 = malloc(sizeof(char*)*(i+1));
	for(int j = 0 ; j < i ;  j++) { 
		split_cmds_2[j] = split_cmds[j];
		split_cmds_2[j] = strtok(split_cmds_2[j],"\n");
	}
	

	split_cmds_2[i++] = NULL;
	//free(split_cmds);
		

	

	
	return split_cmds_2;	




}

// function that prints the current working directory
void pwd(char ** commandArgs) { 	

	int size = 0;
	while(commandArgs[size] != NULL) {
			size++;
	}

	if(size > 1) { 
				char error_message[30] = "An error has occurred\n";
		write(STDERR_FILENO, error_message, strlen(error_message));
		return;

	}
	
	if(getcwd(CWDBUFFER, sizeof(char)*1000) == NULL) { 
		// handle error if unable to print current wd
		char error_message[30] = "An error has occurred\n";
		write(STDERR_FILENO, error_message, strlen(error_message));
	}	
	printf("%s\n", CWDBUFFER);
}


// function that changes directory according to destination
// specifided by the user 
void wd(char ** commandArgs){ 


	if(commandArgs[1] == NULL) { 
		if(chdir(getenv("HOME")) < 0) { 
			// handle error if unable to go to home
			char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
		}
	} else {
		if(chdir(commandArgs[1]) < 0)  {
			// handle error if unable to reach specified directoryh
			char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
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
	snprintf(exec_command, sizeof(exec_command), "%s%s", "", commandArgs[0]);
	//printf("command exec = %s\n", exec_command);	
	// place new value in argv[0];
	argv[0] = strdup(exec_command);		
//	printf("%s - ", exec_command);	
	// fill in the rest of the commands in the arguments 
	int i = 1;
	while(commandArgs[i] != NULL) { 
		argv[i] = commandArgs[i]; 
		i++;
	}
	
		argv[i] = NULL;
		
	// call execvp with the arguments created
	if(execvp(argv[0], argv) < 0) { 

		char error_message[30] = "An error has occurred\n";
        	write(STDERR_FILENO, error_message, strlen(error_message));

	}
	exit(1);

}

// function to deal with output redirection
void output_redirect(char ** commandArgs, int charPosition) { 


	if(commandArgs[charPosition+1] == NULL) { 
		  char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
		exit(0);
	} 
	
	if(commandArgs[charPosition+2] != NULL) { 
		char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
	}

	// null out the position where the redirection is present
	commandArgs[charPosition] = NULL;
//	printArgs(commandArgs);


	
	// open an input file
	int out = open(commandArgs[charPosition+1], O_WRONLY|O_CREAT|O_TRUNC, 0600);
	if(out == -1) { 
			 char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
	}
	dup2(out, 1);	
		
	commandArgs[charPosition+1] = NULL;	
	executor(commandArgs);

	
	close(out);
}

void input_redirect(char ** commandArgs, int charPosition) {

        if(commandArgs[charPosition+1] == NULL) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
		exit(0);
        }

        if(commandArgs[charPosition+2] != NULL) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
        	exit(0);
	}
	



	// null out the character position
	commandArgs[charPosition] = NULL;

	// open in read only mode
	int in = open(commandArgs[charPosition+1], O_RDONLY);

	if(-1 == in) { 
		 char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
	}
		
	// duplicate the input file descriptor
	dup2(in, STDIN_FILENO);	

	close(in);

	int size = 0;
	while(commandArgs[size]!=NULL)  {	
			size++;
	}
	char * argv[size+1];
	for(int i = 0 ; i < size; i++) {
				argv[i] = commandArgs[i];
	}
	argv[size]=NULL;

	if(execvp(argv[0], argv) < 0) { 
		printf("error at input redirect");
	}

	return;	

}

// function that handles multiple redirection 
void multiple_redirect(char ** commandArgs, int input_redir_position, int output_redir_position) {

 		// NOTE: input_redir_position and output_redir_position can never the same 
		
		int in = open(commandArgs[input_redir_position+1], O_RDONLY);
		dup2(in, STDIN_FILENO);
		close(in);
		int out = open(commandArgs[output_redir_position+1], O_WRONLY|O_CREAT|O_TRUNC, 0600);
		dup2(out, STDOUT_FILENO);
		close(out);


		commandArgs[input_redir_position] = NULL;
		commandArgs[output_redir_position] = NULL;

		int size = 0;
		while(commandArgs[size] != NULL) { 
			size++;
		}
		char * argv[size+1];

		for(int i = 0 ; i < size; i++) { 
			argv[i] = commandArgs[i];
		}
		argv[size]=NULL;
		argv[0] = strdup(commandArgs[0]);

		if(execvp(argv[0], argv)) { 
			printf("error at the multiple redirect\n");
		}
}



//function that handles pipes
void pipe_execution(char ** commandArgs, int pipe_position){

	pid_t pid1, pid2;
	int status1, status2;
	int fd[2];
	commandArgs[pipe_position] = NULL;

	int arg1_size = 0 ;
	while(commandArgs[arg1_size] != NULL)  {	 
			arg1_size++;
	} 

	int arg2_size = 0;
	while(commandArgs[arg2_size+pipe_position+1] != NULL) { 
			arg2_size++;
	}

	char * argv1[arg1_size+1];
	char * argv2[arg2_size+1];

	for(int i = 0; i < arg1_size; i++) { 
		argv1[i] = commandArgs[i];
	} 

	for(int i = 0; i < arg2_size; i++) {
		argv2[i] = commandArgs[i+pipe_position+1];
	}

	argv1[arg1_size] = NULL;
	argv2[arg2_size] = NULL;

	char exec_command_1[256];	
	char exec_command_2[256];

        snprintf(exec_command_1, sizeof(exec_command_1), "%s%s", "", argv1[0]);
        snprintf(exec_command_2, sizeof(exec_command_2), "%s%s", "", argv2[0]);

	argv1[0] = strdup(exec_command_1);
	argv2[0] = strdup(exec_command_2);
	// create pipe 	
	if(pipe(fd)) { 
		// deal with error
		char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));

	}

	fflush(stdout);
	pid1 = fork();

	if(pid1 == 0)  {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		if(execvp(argv1[0], argv1) == -1) { 
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));

		}
		exit(0);



	}

	fflush(stdout);
	pid2 = fork();

	if(pid2 == 0) { 
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);	
		if(execvp(argv2[0], argv2) == -1) {
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		exit(0);
	}

	free(argv1[0]);
	free(argv2[0]);
	close(fd[0]);	
	close(fd[1]);	
	waitpid(pid2, &status2, 0);	
	waitpid(pid1, &status1, 0);
	
}

// seperate command to execute functions that require forking 
void executeCommand(char ** commandArgs, int isBackground) {

	// calculate the number of arguments
	int cmdSize = 0;
	while(commandArgs[cmdSize] != NULL) { cmdSize++; }
	
	// check if either of the redirections 
	// or piping occur
	int charPosition = 0;
	int input_redir_flag = 0; int output_redir_flag = 0;
	int pipe_flag = 0;
	// this is safe because we should never find a redirection symbol at the first position
	int input_redir_position = 0; int output_redir_position = 0; int pipe_position = 0;

	while(commandArgs[charPosition] != NULL) { 
		
			// get the position of where the input and output 
			// redirection is 
			if(strcmp(commandArgs[charPosition], ">")==0 && commandArgs[charPosition] != NULL) {
				//printf("greater than strcmp value %d - \n", strcmp(commandArgs[charPosition], ">")==0);
				output_redir_flag = 1;
				output_redir_position = charPosition;		
			} else if (strcmp(commandArgs[charPosition], "<")==0 && commandArgs[charPosition] != NULL) { 
				//printf("less than strcmp value %d - \n", strcmp(commandArgs[charPosition], "<")==0);
				input_redir_flag = 1;	
				input_redir_position = charPosition;
			} else if (strcmp(commandArgs[charPosition], "|") == 0 && commandArgs[charPosition] != NULL)  {
				pipe_flag = 1;
				pipe_position = charPosition;
			}
			/*
			if(strcmp(commandArgs[charPosition], ">")==0 || 
			   strcmp(commandArgs[charPosition], "<")==0) {
				break;
			}
			*/
		charPosition++;
	}


	
	if(pipe_flag == 1)  {
		
		pipe_execution(commandArgs, pipe_position);


	} else { 

		int status;
		// fork child
		fflush(stdout);
		fflush(stderr);
		pid_t rc = fork();
		if(rc == 0) { // child process
				if(input_redir_flag == 0 && output_redir_flag == 1 && commandArgs[output_redir_position] != NULL) { 
				// output redirection	
				output_redirect(commandArgs, output_redir_position); 
				return;
			} else if (input_redir_flag == 1 && output_redir_flag == 0 && commandArgs[input_redir_position] != NULL) {
				// input redirection
				input_redirect(commandArgs, input_redir_position);			
				return;
			} else if (input_redir_flag == 1 && output_redir_flag == 1 && commandArgs[input_redir_position]  != NULL && commandArgs[output_redir_position] != NULL) {
				// multiple redirection 
				multiple_redirect(commandArgs, input_redir_position, output_redir_position);		
				return;
			} else { 
				executor(commandArgs);
	
				return;
			}
		} else if (rc > 0) { // parent process	
			// if process is not executed in background, wait for it
	
			if(isBackground == 0) {
				if(waitpid(rc, &status, 0)< 0) { 
					printf("error child did not return");
					}	
			}
			// record the process so that we can kill it if it is defunct
			int i = 0;
				while(PIDLIST[i] != NULL) { // find a free location to store pid
				i++;
			}
			PIDLIST[i] = rc;	 
		}
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

// initialize process id list with NULLS
for(int i = 0 ; i < 1000 ; i++) { 

	PIDLIST[i] = NULL;
}

if(argc > 1)  { 
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
}

	
// create a while loop that iteratively takes in 
// an input, parses it and executes the commands accordingly
int status;
for(;;) { 



	
	char * command = prompt(); // get the command from user
	if(command == NULL) { 
		count_commands++;
		continue;
	}
	char ** commandArgs = parseCommand(command);
	 //printArgs(commandArgs);		

	if(commandArgs[0] == NULL) { 
			continue;
	} else { 
		count_commands++;
	}

	if (strcmp(commandArgs[0], "exit") == 0) { 

		for(int i = 0; i < 1000; i++) { 
			if(PIDLIST[i] != NULL) { 
				kill(PIDLIST[i], SIGKILL);
			}
		}
		exit(0);

	}

			
	// the parsed command args must now be executed accordingly
//	printf("%s", commandArgs[0]);	
	if(strcmp(commandArgs[0], "pwd") == 0) { 
		pwd(commandArgs); // if the user calls pwd, then pwd
	} else if (strcmp(commandArgs[0], "cd") == 0) {
		wd(commandArgs);		
	} else if (strcmp(commandArgs[0], "exit") == 0) { 

		for(int i = 0; i < 1000; i++) { 
			if(PIDLIST[i] != NULL) { 
				kill(PIDLIST[i], SIGKILL);
			}
		}
		exit(0);

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
	for(int i=0; i <  1000; i++) { 
		if(PIDLIST[i] != NULL) { 
			if(waitpid(PIDLIST[i],&status, WNOHANG) != 0) { 
				kill(PIDLIST[i], SIGKILL);
				PIDLIST[i] = NULL;
			} 
		}	
	}
	// handle dead processes	free(commandArgs);
	}




}




