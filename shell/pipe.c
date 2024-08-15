#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
	char *args[] = {"sort","file.txt","|","uniq","|","wc"};
	char **ptr = args;
	
	for(int i=0;i<sizeof(args)/sizeof(char*);i++)
	{
		printf("%s\n",*ptr);
		ptr = ptr + 1;
	}

	int pipefd[2];
    	pid_t cpid;
    	char buf[1024];
	ssize_t bytesRead;


	if (pipe(pipefd) == -1) {
		perror("pipe");
        	exit(EXIT_FAILURE);
    	}

    	cpid = fork();
    	if (cpid == -1) {
        	perror("fork");
        	exit(EXIT_FAILURE);
    	}

	if (cpid == 0) {    // Child process
        	close(pipefd[1]);          // Close unused write end

        	printf("Child process reading from pipe...\n");
		
		bytesRead = read(pipefd[0],buf,sizeof(buf)-1);

		if(bytesRead>0){
			buf[bytesRead] = '\0';
			write(STDOUT_FILENO,buf,bytesRead);
		}

        	close(pipefd[0]);  // Close read end
        	printf("Child process done reading.\n");

		//printf("%s\n",buf);

        	exit(EXIT_SUCCESS);

    	} else {            // Parent process
        	close(pipefd[0]);          // Close unused read end

        	const char *msg = "Hello from parent!\n";
        	printf("Parent process writing to pipe...\n");
        	write(pipefd[1], msg, strlen(msg)); // Write to pipe
        	close(pipefd[1]);          // Close write end, which sends EOF to child

        	wait(NULL);                // Wait for child process to finish
        	printf("Parent process done.\n");
        	exit(EXIT_SUCCESS);
    	}

	return 0;
}

