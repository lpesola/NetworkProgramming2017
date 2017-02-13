#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 50800
#define ADDR "127.0.0.1"
#define BUFSIZE 1000

void echo(int fd);

int main(void) {

	int sockfd = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	inet_aton(ADDR, &addr.sin_addr); 
	addr.sin_port = htons(PORT);
	if (bind(sockfd, (struct sockaddr*) &addr, sizeof addr) < 0) {
		perror("bind");
		exit(1);
	}	
	listen(sockfd, 5);
      
	while (1) {
		struct sockaddr_in cliaddr = {0};
		socklen_t len = sizeof(cliaddr);
		int cli = accept(sockfd, (struct sockaddr*) &cliaddr, &len);
		if (cli < 0) {
			perror("accept");
			exit(1);
		}
		int pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		} else if(pid == 0) {
			close(sockfd);
			echo(cli);
			exit(0);
		}
		close(cli);
	}
	

}


void echo(int fd) {

	char buf[BUFSIZE];
	int nbytes;
	while ((nbytes = read(fd, buf, BUFSIZE)) != 0) {
		if (nbytes < 0 && errno == EINTR) 
			continue;
		if (nbytes < 0) {
			perror("read");
			exit(1);
		}
		if(write(fd, buf, nbytes) < 0) {
			if (errno == EINTR) 
				continue;
			else {
				perror("write");
				exit(1);
			}
		}

	}
	close(fd);

}
