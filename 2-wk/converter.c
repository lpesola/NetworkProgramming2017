/*
 * Use an unnamed pipe for ipc.
 * Parent process reads lines from stdin (ignores too long lines)
 * Child process prints all the lines twice
 */


#define _XOPEN_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

void convert_lines(int readfd, int writefd);

int main(int argc, char *argv[])
{
	char *fifo1 = "/tmp/fifo1";
	char *fifo2 = "/tmp/fifo2";

	int writefd = open(fifo1, O_WRONLY);
	if (writefd == -1)
		perror("open writefd");

	int readfd = open(fifo2, O_RDONLY);
	if (readfd == -1)
		perror("open readfd");

	/*
	 * read line from fifo2
	 * convert to uppercase char by char
	 * write to fifo1
	 * until eof
	 * until fifo1 closed
	 */

	convert_lines(readfd, writefd);

	exit(0);
}

void convert_lines(int readfd, int writefd) 
{
	puts("converting");
	char buf[100];
	int nbytes;
	while ((nbytes = read(readfd, buf, sizeof buf)) != 0) {
		if (nbytes == -1 && errno == EINTR) {
			continue;
		} else if (nbytes == -1) {
			perror("doubler read");
			exit(1);	
		}
	// write can fail too
		char uppercase[100]; 	
		for (int i = 0; i < nbytes; i++)
			uppercase[i] = toupper(buf[i]);
		write(writefd, uppercase, nbytes);
	}
	puts("pipe closed; nothing more to convert");
	close(writefd);
	close(readfd);

}


