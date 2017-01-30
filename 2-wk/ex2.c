#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void print_lines();
int do_child(char *path, char *arg);

int main(int argc, char *argv[])
{

	if (argc < 2 || argc > 3) { 
		puts("incorrect number of arguments");
		exit(1);
	}

	int pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		if((do_child(argv[1], argv[2])==-1))
			perror("execv");
	} else {
		print_lines();
		int status;
		wait(&status);
		// todo find out exit status of child process
		if (WIFEXITED(status)!=0)
			puts("normal exit");
		if (WIFSIGNALED(status)!=0)
			puts("terminated by a signal not caught");
	} 
	
	exit(0);
}

void print_lines() 
{
	int pid = getpid();
	for  (int i = 1; i <= 5; i++){
		char line[25];
		int written = sprintf(line, "process %d, line %d\n", pid, i);
		write(STDOUT_FILENO, &line[0], written);
	}
}

int do_child(char *path, char *arg)
{	
	char *cmd[] = {path, arg, (char *)0};
	int ret = execv(path, cmd);
	return ret;
		
}

