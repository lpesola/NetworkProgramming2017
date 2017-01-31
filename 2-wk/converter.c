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
	if (writefd == -1){
		perror("open writefd");
		exit(1);
	}
	int readfd = open(fifo2, O_RDONLY);
	if (readfd == -1){
		close(writefd);
		perror("open readfd");
		exit(1);
	}

	convert_lines(readfd, writefd);

	exit(0);
}

/* Read lines from FIFO for reading, convert to uppercase.
 * Send converted line back using FIFO for writing.
 * The other process (reader.c) creates the FIFOs.
 */

void convert_lines(int readfd, int writefd) 
{
	char buf[100];
	int nbytes;
	while ((nbytes = read(readfd, buf, sizeof buf)) != 0) {
		if (nbytes == -1 && errno == EINTR) {
			continue;
		} else if (nbytes == -1) {
			perror("doubler read");
			exit(1);	
		}
		char uppercase[100]; 	
		for (int i = 0; i < nbytes; i++)
			uppercase[i] = toupper(buf[i]);
	
		int ret = write(writefd, uppercase, nbytes);
		if (ret == -1) {
			perror("write");
			exit(1);
		} 
	}
	puts("pipe closed; nothing more to convert");
	close(writefd);
	close(readfd);

}


