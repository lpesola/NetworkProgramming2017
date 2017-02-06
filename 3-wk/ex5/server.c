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


/* ex 5.
 * Modify ex 4 so that
 * - Server ignores all data from the client. 
 * - Maximum number of bytes to read with a single read is given as an argument.
 *
 */

void read_lines(int readfd, int maxbytes);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		puts("usage ./server maxbytes");
		exit(0);
	}
	int maxbytes = atoi(argv[1]);

	int lstnsock = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (lstnsock < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if(bind(lstnsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
		perror("bind");
		exit(1);
	}

	listen(lstnsock, 2);

	for (;;) {	
		struct sockaddr_in cli_addr = {0};
		socklen_t clilen = sizeof(cli_addr);
		int newsockfd = accept(lstnsock, (struct sockaddr*)&cli_addr, &clilen);
		if (newsockfd < 0) {
			perror("accept");
			exit(1);
		}
		int pid = fork();
		if(pid < 0){
			perror("fork");
			exit(1);
		}
		if (pid == 0) {
			close(lstnsock);
			read_lines(newsockfd, maxbytes);
			exit(0);
		}
		close(newsockfd);
	}
	exit(0);
}

void read_lines(int readfd, int maxbytes)
{	

	int nbytes;
	char readbuf[maxbytes];
	while ((nbytes = read(readfd, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR) {
			continue;
		}
		else if (nbytes == -1) {
			perror("doubler read");
			exit(1);	
		}

	}
	puts("done reading");
	close(readfd);
}

