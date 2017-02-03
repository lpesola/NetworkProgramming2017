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


void read_lines(int writefd) 
{
	puts("type in text to be doubled, ctrl-D ends");
	char readbuf[100];
	char writebuf[100];
	ssize_t nbytes;
	int len = 0;
	int toolong = 0;
	int j = 0;
	while ((nbytes = read(STDIN_FILENO, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) { 
			perror ("read from stdin");
			exit(1);
		}
		
		// Lines over 100 bytes must be ignored
		for (int i = 0; i < nbytes ; i++) {
			writebuf[j] = readbuf[i];
			len +=1;
			j += 1;
			if (readbuf[i] == '\n') {
				if (toolong == 1) {
					toolong = 0;
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
				toolong = 1;
			}

		}
	}
}


