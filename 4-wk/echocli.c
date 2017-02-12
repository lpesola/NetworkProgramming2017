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
#define BUFSIZE 100
#define SERV_ADDR "ukko128.cs.helsinki.fi"

void echo(int fd);

int main(void) {

	/* Wk 4 / ex 2. 
	 * Create echoclient
	 * Check correct port and host for service "echo"
	 */ 

	struct servent *service = getservbyname("echo", "tcp");
	if (service == NULL) {
		puts("getservbyname failed");
		exit(1);
	}

	struct hostent *host = gethostbyname(SERV_ADDR);
	if (host == NULL) {
		puts("gethostbyname failed");
		exit(1);
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = host->h_addrtype;
	addr.sin_port = service->s_port;
	struct in_addr iaddr = {0};
	inet_aton(host->h_addr_list[0], &iaddr);
	addr.sin_addr = iaddr;
	int sockfd = socket(host->h_addrtype, SOCK_STREAM, PF_UNSPEC);

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof addr) < 0) {
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
		write(fd, rbuf, strlen(rbuf));
		ssize_t rbytes = read(fd, wbuf, sizeof wbuf);
		write(STDOUT_FILENO, wbuf, rbytes);
	}
}

