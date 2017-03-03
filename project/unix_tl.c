#define _POSIX_C_SOURCE 200112L // for getaddrinfo
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h> // for memeset
#include <errno.h> 
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

	struct addrinfo hints;
	// use these to specify what kind of information we want
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	// get the port and address for the UNIX_TL service on the specified host
	struct addrinfo *result;
	if (getaddrinfo("whx-10.cs.helsinki.fi", "UNIX_TL", &hints, &result)  < 0) {
		perror("getaddrinfo");
		exit(1);
	}


	// the result is a linked list: 
	// probably we would only get one result that would be the correct one
	// but just in case, loop through all the entries to make sure
	// the correct one is found and connect to that 
	int sockfd;
	for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("connect");
			close(sockfd);
			continue;
		}
		puts("connected!");	
	}


	
	
	char buf[1000];
	int rb = read(sockfd, buf, sizeof buf);
	write(STDOUT_FILENO, buf, rb); 
	
	char msg[] = "HELLO\n";
	write(sockfd, msg, sizeof msg);

	rb  = read(sockfd, buf, sizeof buf);
	write(STDOUT_FILENO, buf, rb); 

	char name[] = "this is LPESOLA client\n";
	int sb = send(sockfd, name, sizeof name, 0);

	rb  = read(sockfd, buf, sizeof buf);
	write(STDOUT_FILENO, buf, rb); 

	rb  = read(sockfd, buf, sizeof buf);
	write(STDOUT_FILENO, buf, rb); 
	freeaddrinfo(result);


	return(0);
}
