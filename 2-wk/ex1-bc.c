/* simple fork example 1 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Modify "simple fork program"
 * a) add necessary error checking
 * 	use write() for output
 * 	output includes pid of current process
 *
 * b) exactly 5 processes will run, each of them run the loop 
 *
 * c) number of processes executing the loop is given as an argument            
 * 	default 5, max 50
 */
void print_lines();

int main(int argc, char **argv)
{
	int times;
	if (argc == 1){ 
		times = 5;
	}
	else { 
		times = atoi(argv[1]);	
		if (times > 50 || times < 1) {
			puts("permitted values 1-50");
			exit(0);
		}
	}

	int pid;
	for (int i = 1; i < times; i++){
		if((pid = fork()) < 0){
			perror("fork");
       			exit(1);	       
		} else if (pid == 0) {
		       break;	
		}
	}
	print_lines();
	
	waitpid(-1, NULL, WNOHANG);
	exit(0);
}
void print_lines() 
{
	int pid = getpid();
	for  (int i = 1; i <= 5; i++){
		char line[25];
		int written = sprintf(line, "process %d, line %d\n", pid, i);
		if ((write(STDOUT_FILENO, &line[0], written)) < 0) {
			perror("write");
			exit(1);
		}
	}
}

