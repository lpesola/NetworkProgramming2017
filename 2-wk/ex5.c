/*
 * Use an unnamed pipe for ipc.
 * Parent process reads lines from stdin (ignores too long lines)
 * Child process prints all the lines twice
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#

void double_lines(int fd);
void read_lines(int fd);

int main(int argc, char *argv[])
{
	int fd[2];

	if(pipe(fd)==-1){
		perror("pipe");
		exit(1);
	}

	int pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		//close the end parent uses for writing
		// stop when read returns 0: parent has closed write end
		close(fd[1]);
		double_lines(fd[0]);	
		close(fd[0]);
	} else {
		//close the end child uses for reading
		close(fd[0]);
		read_lines(fd[1]);
		close(fd[1]);
		wait(NULL);

	}

	exit(0);
}

void double_lines(int fd) 
{
	char buf[100];
	int nbytes;
	while ((nbytes = read(fd, buf, sizeof buf)) != 0) {
		if (nbytes == -1 && errno == EINTR) {
			continue;
		}
		else if (nbytes == -1) {
			perror("doubler read");
			exit(1);	
		}
		
		if ((write(STDOUT_FILENO, buf, nbytes)) == -1)
			perror("1st write");
		if ((write(STDOUT_FILENO, buf, nbytes)) == -1)
			perror("2nd write");
	}
}

void read_lines(int fd) 
{
	char buf[100];
	ssize_t nbytes;
	while ((nbytes = read(STDIN_FILENO, buf, sizeof buf))) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) {
			perror ("read stdin");
			exit(1);
		}
		/*  discard lines that are over 100 bytes long
		 *  -> if last character in buf is not \n, 
		 *  line is too long, do nothing	
		 */

		if (buf[nbytes-1] =='\n') {
			write(fd, buf, nbytes);
		}
	}
}
