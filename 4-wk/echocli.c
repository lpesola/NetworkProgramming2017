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
#define BUFSIZE 1000
#define SERV_ADDR "ukko128.cs.helsinki.fi"

void echo(int fd);

int main(int argc, char **argv) {

	/* create socket
	 * check the service echo
	 *  -> correct port / host
	 *  -> ukko128.cs.helsinki.fi
	 * connect to server
	 * read line from stdin
	 * write line in socket
	 * read line from socket
	 * writ line to stdout
	 * repeat until EOF
	 * test on 
	 * ukko126.cs.helsinki.fi
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
/*	if (bind(sockfd, (struct sockaddr *)&addr, sizeof addr) < 0) {
		perror("bind");
		exit(1);
	}*/

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof addr) < 0) {
		perror("bind");
		exit(1);
	}

	echo(sockfd);

	close(sockfd);

	exit(0);
}

void echo(int fd) {

	/*
	 * read BUFSIZE from stdin until '\n'
	 * write BUFSIZE in socket until \n
	 * read BUFSIZE  from socket until \n
	 * write BUFSIZE to stdout until \n
	 * repeat until EOF
	 */

	char buf[BUFSIZE];
	char wrbuf[BUFSIZE];
	int nbytes;
	while(fgets(wrbuf, BUFSIZE, STDIN_FILENO) != NULL) {
		write(fd, wrbuf, strlen(wrbuf));
		nbytes = read(fd, buf, BUFSIZE);
		write(STDOUT_FILENO, buf, nbytes);

	}



	
}




