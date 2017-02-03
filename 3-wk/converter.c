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
	mkfifo(fifo2, 0666);


	int readfd = open(fifo1, O_RDONLY);
	if (readfd == -1){
		perror("open readfd");
		exit(1);
	}

	int writefd = open(fifo2, O_WRONLY);
	if (writefd == -1){
		perror("open writefd");
		close(readfd);
		exit(1);
	} 
	convert_lines(readfd, writefd);
	
	close(readfd);
	close(writefd);
	unlink(fifo2);
	exit(0);
}

/* Read characters from FIFO, convert to uppercase.
 * Send converted text to doubler using FIFO2 
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

}


