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

void read_lines(int fd);
void write_lines(int fd);

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
	
	read_lines(writefd);
	write_lines(readfd);
	close(readfd);

	unlink(fifo1);
	unlink(fifo2);
	exit(0);
}


void read_lines(int fd) 
{
	puts("type in text to be doubled, ctrl-D ends");
	char buf[100];
	ssize_t nbytes;
	while ((nbytes = read(STDIN_FILENO, buf, sizeof buf))) {
		// discard lines that are over 100 bytes long
		//  -> if last character in buf is not \n, 
		//  line is too long, do nothing	
		//  -> else write to fd
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) 
			perror ("read from pipe");
		if (buf[nbytes-1] =='\n') {
			write(fd, buf, nbytes);
		}
		// add some error handling, at least EINTR
	}
	puts("that's all!");
	close(fd);
}

void write_lines(int fd) 
{
	char buf[100];
	ssize_t nbytes;
	while ((nbytes = read(fd, buf, sizeof buf))) {
		write(STDOUT_FILENO, buf, nbytes);
		// add some error handling, at least EINTR
	}
}
