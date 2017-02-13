#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define BUFSIZE 100
#define SERV_ADDR "127.0.0.1"
#define PORT 50800

void echo(int fd);

int main(void) {

	/* Wk 4 / ex 6. 
	 * Test echo server.
	 */ 

	signal(SIGPIPE, SIG_IGN);
	int sockfd = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (sockfd < 0) {	
		perror("socket");
		exit(1);								
	}

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_ADDR, &serv_addr.sin_addr);
	serv_addr.sin_port = htons(PORT);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof serv_addr) < 0) {
		perror("connect");
		exit(1);
	}

	echo(sockfd);

	close(sockfd);

	exit(0);
}


void echo(int fd) 
{
	
	
	puts("type in text to be echoed, ctrl-D ends");
	char rbuf[BUFSIZE];
	char wbuf[BUFSIZE];	

	while (fgets(rbuf, BUFSIZE, stdin) != NULL) {
		if (write(fd, rbuf, strlen(rbuf)) < 0) {
			if (errno == EINTR) 
				continue;
			if (errno == EPIPE) 
				perror("communication terminated");
			else
				perror("write socket");
			exit(1);
		} 

		ssize_t rbytes = read(fd, wbuf, sizeof wbuf);
		if (rbytes < 0) {
			perror("read socket");
			exit(1);
		}
		write(STDOUT_FILENO, wbuf, rbytes);
	}
}

