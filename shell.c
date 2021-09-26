#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char **argv)
     {
     	
     	shell_loop();
     	
     	return EXIT_SUCCESS;
	 }
	 
	 void shell_loop(void)
	 {
	 	//With this function we handle commands with these three steps
	 	/**
	 	=>Read: reads the command from the standard input
	 	=>parse: separate the command string into a command and argument
	 	=>Execut: runs the parsed command
	 	**/
	 	char *line;
	 	char **args;
	 	int status;
	 	
	 	do{
	 		printf(">");
	 		line = shell_read_line();
	 		args = shell_split_line(line);
	 		status = shell_execute(args);
	 		
	 		free(line);
	 		free(args);
		 } while(status);
	 }
	 
	 char *shell_read_line(void)
	 {
	 	int buf_size = = SHELL_RL_BUF_SIZE;
	 	int position = 0;
	 	char *buffer = malloc(sizeof(char) *buf_size);
	 	int shell_c;
	 	char *line = NULL;
	 	ssize_t buf_size = 0;
	 	
	 	if(!buffer)
	 	{
	 		fprintf(stderr, "Shell: An allocation error just occured\n");
	 		exit(EXIT_FAILURE);
		 }
		 
		 while(1)
		 {
		 	//reading a character
		 	shell_c = getchar();
		 	
		 	if(shell_c == EOF || shell_c =='\n')
		 	{
		 		buffer[position] = '\0';
		 		return buffer;
			 }
			 else
			 {
			 	buffer[position] = shell_c;
			 }
			 position++;
			 
			 //iF the buffer is exceeded reallocate
			 if(position >= buf_size)
			 {
			 	buf_size += SHELL_RL_BUF_SIZE;
			 	buffer = realloc(buffer, buf_size);
			 	if(!buffer)
			 	{
			 		fprintf(stderr,"Shell: An allocation error just occured");
			 		exit(EXIT_FAILURE);
				 }
			 }
		 }
		 if(getline(&line,&buf_size,stdin)== -1)
		 {
		 	if(feof(stdin))
		 	{
		 		exit(EXIT_SUCCESS);
			 }
			 else
			 {
			 	perror("readline");
			 	exit(EXIT_FAILURE);
			 }
		 }
		 return line;
	 }
	 
	 char **shell_split_line(char *line)
	 {
	 	int buf_size = SHELL_TOK_BUF_SIZE,position =0;
	 	char **tokens = malloc(buf_size * sizeof(char*));
	 	char *token;
	 	
	 	if(!tokens)
	 	{
	 		fprintf(stderr, "lsh: allocation error\n");
	 		exit(EXIT_FAILURE)
		 }
		 
		 token = strtok(line, SHELL_TOK_DELIM);
		 while(token != NULL)
		 {
		 	tokens[position] = token;
		 	position++;
		 	
		 	if(position >= buf_size) 
		 	{
		 		buf_size += SHELL_TOK_BUF_SIZE;
		 		tokens = realloc(tokens, buf_size * sizeof(char*));
			 }
		 }
	 }
	 
	 int shell_launch(char **args)
	    {
	    	pid_t,wpid;
	    	int status;
	    	
	    	pid = fork();
	    	if(pid == 0)
	    	{
	    		//child process
	    		if(execvp(args[0],args) == -1)
	    		{
	    			perror("Shell");
				}
				exit(EXIT_FAILURE);
			}else if (pid < 0)
			{
				//Error while forking
				perror("Shell");
			}else {
				//parent process
				do{
					wpid = waitpid(pid,&status,WUNTRACED);
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			}
			return 1;
}

//Delcaration for shell built-in functions
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

//list of built-in commands followed by their functions
char *builtin_str[] = {
"cd",
"help",
"exit"
};

int (*builtin_func[]) (char **) = {
&lsh_cd,
&lsh_help,
&lsh_exit
};

int lsh_num_builtins(){
	return sizeof(builtin_str) /sizeof(char *);
}

//builtin function implementation
int lsh_cd(char **args)
{
	if(args[1] == NULL)
	{
		fprintf(stderr,"Shell: expected arguments to \"cd\"\n");
	} 
	else
	{
		if(chdir(args[1]) != 0)
		{
			perror("Shell");
		}
	}
	return 1;
}

int lsh_help(char **args)
{
	int i;
	printf("Denis Muriungi Shell \n");
	printf("Type program names and press enter \n");
	printf("The following are builtin: \n");
	
	for(i =0; i<lsh_num_builtins(); i++)
	{
		printf(" %s \n",builtin_str[i]);
	}
	
	printf("Use the man information to get more information \n about the other commands \n");
	return 1;
}

int lsh_exit(char **args)
{
	return 0;
}

int lsh_execute(char **args)
{
	int i;
	
	if(args[0] == NULL)
	{
		//Empty command entered
		return 1;
	}
	for(i = 0; i<lsh_num_builtins(); i++)
	{
		if(strcmp(args[0],builtin_str[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}
	return lsh_launch(args);
}
