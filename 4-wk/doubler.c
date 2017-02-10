#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 50002

void double_lines(int readfd, struct sockaddr *pcliaddr, int clilen);

/* ex 1.
 * Modify wk3/ex4 reader & doubler so that UDP sockets are used for ICP
 */

int main(int argc, char *argv[])
{
	int sockfd = socket(PF_INET, SOCK_DGRAM, PF_UNSPEC);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}


	struct sockaddr_in serv_addr = {0};
	struct sockaddr_in cli_addr = {0};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
		perror("bind");
		exit(1);
	}
	
	double_lines(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
	exit(0);
}

void double_lines(int readfd, struct sockaddr *pcliaddr, int clilen) 
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

