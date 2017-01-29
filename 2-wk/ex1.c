/* simple fork example 1 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Modify "simple fork program"
 * a) add necessary error checking
 * 	use write() for output
 * 	make program print which process prints which line
 *
 * b) exactly 5 processes will run, each of them run the loop 
 *
 * c) number of processes executing the loop is given as an argument 
 * 	default 5, max 50
 */

int main(int argc, char **argv)
{
	int pid;
	if((pid = fork()) < 0){
		perror("fork");
       		exit(1);	       
	}
	for  (int i = 1; i <= 5; i++){
		// string should be: process 'pid', line 'i'
		char line[40];
		// int sprintf(char *restrict s, const char *restrict format, ...);
		sprintf(line, "process %d, line %d\n",pid, i);
		write(STDOUT_FILENO, &line[0], sizeof line);
	}
	exit(0);
}
