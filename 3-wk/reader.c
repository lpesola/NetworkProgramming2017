#define _XOPEN_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void read_lines(int writefd);

int main(int argc, char *argv[])
{	
    	char *fifo1 = "/tmp/fifo1";
	mkfifo(fifo1, 0666);
	
	int writefd = open(fifo1, O_WRONLY);
	if (writefd == -1) {
		perror("open writefd");
		exit(1);
	}
	
	read_lines(writefd);
	close(writefd);

	unlink(fifo1);
	exit(0);
}

/*
 * Read text from stdin. 
 * Write all lines 100 bytes or less to fifo1 for further processing.
 * Longer lines are ignored.
 */

void read_lines(int writefd) 
{
	puts("type in text to be doubled, ctrl-D ends");
	char readbuf[100]; 
	char writebuf[100]; 
	ssize_t nbytes;
	// number of characters read before a newline
	int len = 0; 
	// non-zero if last character in readbuf wasn't a newline
	int longline = 0; 
	// the current position in writebuf
	int j = 0; 

	while ((nbytes = read(STDIN_FILENO, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) { 
			perror ("read from stdin");
			exit(1);
		}
		
/* 
 * To ignore too long lines keep track of the number of characters read (len).
 * When >100 characters have been read, set longline to be true (non-zero) 
 * to mark that next segments first characters are part of a too long line.
 * 
 * When a newline is read, if the line was not too long (= longline is false (0)),
 * write everything up to here to writefd for further processing.
 * 
 * If longline is true, the end of a too long line has been reached so 
 * characters up to here can be ignored. 
 */

		for (int i = 0; i < nbytes ; i++) {
			writebuf[j] = readbuf[i];
			len +=1;
			j += 1;
			if (readbuf[i] == '\n') {
				if (longline) {
					longline = 0;
					len = 0;
					j = 0;	
				} else {
					write(writefd, writebuf, len);
					j = 0;
					len = 0;
			       	}
			}
			if (len > 100) {
				j = 0;
				len = 0;
				longline = 1;
			}

		}
	}
}


