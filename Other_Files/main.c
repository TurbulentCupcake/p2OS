#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char * argv []) { 

//	printf("1: %d\n",(int) getpid());
	int rc =  fork();
//	printf("2: %d\n", (int) getpid());

// this is useful in building a shell. When you use a command.
// Shell takes an input from you, it then calls fork, creates a new child, and runs the program 
// that you specified. 

	if(rc == 0) { 
		//child
//		sleep(5);


		close(STDOUT_FILENO);
		(void)	open("outfile.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);


		char * argv[4]; 

		argv[0] = strdup("/bin/ls");
		argv[1] = strdup("-l");
		argv[2] = NULL;
		execvp(argv[0], argv);
		printf("exec failed \n");
	} else if (rc > 0) { 
		//parent
		(void) wait(NULL);
//		printf("I am a parent\n");

	} else { 
		printf("this is an error\n");
	}




	return 0;
}
