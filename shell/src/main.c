#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include "builtinFuncs.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

int lsh_launch(char **args);

int lsh_execute(char **args)
{
	int i;

	if(args[0]==NULL)
	{
		return 1;
	}

	for(i=0;i<lsh_num_builtins();i++)
	{
		if(strcmp(args[0], builtin_str[i])==0) 
		{
			return(*builtin_func[i])(args);
		}
	}
	
	return lsh_launch(args);
}

int lsh_launch(char **args)
{
        pid_t pid,wpid;
        int status;

        pid = fork();

        if(pid==0){
                if(execvp(args[0], args) == -1)
                {
                        perror("lsh execution error");
                }
                exit(EXIT_FAILURE);
        } 
        else if(pid<0){
                perror("lsh fork error");
        }
        else
        {
                do{
                        wpid = waitpid(pid, &status, WUNTRACED);
                } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 1;
}


char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize*sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	
	while(token != NULL)
	{
		tokens[position] = token;
		position++;

		if(position >= bufsize){
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize*sizeof(char*));

			if(!tokens)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

char *lsh_read_line(void)
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char)*bufsize);
	int c;

	if(!buffer)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		c = getchar();

		if(c==EOF || c=='\n')
		{
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = c;
		}
	
		position++;

		if(position>=bufsize)
		{
			bufsize+=LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);

			if(!buffer){
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void lsh_loop(void)
{
	char *line;
	char **args;
	int status;
	
	char file_name[] = "history.txt";

	FILE *file = fopen(file_name,"a"); 

	do{
		printf("> ");
		line = lsh_read_line();

		if(file!=NULL)
		{
			fprintf(file,"%s\n",line);
		}

		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	}
	while(status);
	
	fclose(file);
}

int main(int argc,char **argv)
{
	lsh_loop();

	return 0;
}
