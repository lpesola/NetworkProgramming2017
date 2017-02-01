/*
 * Modify ex. 5:
 * - use FIFOs for ipc.
 * - change case from lower to upper case instead of doubling
 * - converter does not print: sends converted lines back
 * - separate programs; no fork
 */
#define _XOPEN_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void read_lines(int readfd, int writefd);

int main(int argc, char *argv[])
{
	
	/*
	 * create fifos 1, 2
	 *
	 * read line from stdin to buf
	 * write buf to fifo1 until eof
	 *
	 * read line from fifo2
	 * write to stdout
	 * until fifo2 closed
	 */

    	char *fifo1 = "/tmp/fifo1";
	char *fifo2 = "/tmp/fifo2";
	mkfifo(fifo1, 0666);
	mkfifo(fifo2, 0666);
	
	int readfd = open(fifo1, O_RDONLY);
	if (readfd == -1) {
		perror("open readfd");
		exit(1);
	}
	int writefd = open(fifo2, O_WRONLY);
	if (writefd == -1) {
		perror("open writefd");
		close(writefd);
		unlink(fifo2);
		exit(1);
	}
	
	read_lines(readfd, writefd);
	close(readfd);
	close(writefd);

	unlink(fifo1);
	unlink(fifo2);
	exit(0);
}


void read_lines(int readfd, int writefd) 
{
	puts("type in text to be doubled, ctrl-D ends");
	char buf[100];
	ssize_t nbytes;
	while ((nbytes = read(STDIN_FILENO, buf, sizeof buf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) { 
			perror ("read from pipe");
			exit(1);
		}
		
		/* Lines over 100 bytes must be ignored
		 * If last read character was \n, line is short enough
		 * -> this works because small (<PIPE_BUF) writes/reads
		 *  are atomic on FIFOs
		 */
		
		if (buf[nbytes-1] =='\n') {
			int bsent = write(writefd, buf, nbytes);
			
			int bread = read(readfd, buf, bsent);
			write(STDOUT_FILENO, buf, bread);
		} else {
			while(buf[nbytes-1] != '\n'){
				nbytes = read(STDIN_FILENO, buf, sizeof buf);
			}
		} 

	}
}


