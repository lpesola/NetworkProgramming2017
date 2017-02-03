#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

void double_lines(int readfd);

int main(int argc, char *argv[])
{
	char *fifo2 = "/tmp/fifo2";
	int readfd = open(fifo2, O_RDONLY);
	if (readfd == -1) {
		perror("open readfd");
		exit(1);
	}

	double_lines(readfd);
	close(readfd);
	exit(0);
}

void double_lines(int readfd) 
{
	char readbuf[100];
	char writebuf[100];
	int nbytes;
	int j = 0;
	int len = 0;
	while ((nbytes = read(readfd, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR) {
			continue;
		}
		else if (nbytes == -1) {
			perror("doubler read");
			exit(1);	
		}
	 	// print one line at a time	
		for (int i = 0; i < nbytes ; i++){
			writebuf[j] = readbuf[i];
			len += 1;
			j += 1;
			if (readbuf[i] == '\n') {
				if ((write(STDOUT_FILENO, writebuf, len)) == -1)
					perror("1st write");
				if ((write(STDOUT_FILENO, writebuf, len)) == -1)
					perror("2nd write");
				j = 0;
				len = 0;
			}


		}
	


	}
}

