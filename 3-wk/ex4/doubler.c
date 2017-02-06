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

void double_lines(int readfd);

/* ex 4.
 * Modify reader & doubler so that TCP sockets are used for ICP
 * Doubler acts as the server.
 * Reader connects to the server.
 * Server IP address given as a command line argument to the client.
 * Client and server are separate processes: 
 *  server forks a new child process to serve each new client.
 *  Port range: 50000-51999
 */

int main(int argc, char *argv[])
{
	struct sockaddr_in cli_addr, serv_addr;
	int lstnsock = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (lstnsock < 0) {
		perror("socket");
		exit(1);
	}


	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if(bind(lstnsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
		perror("bind");
		exit(1);
	}

	listen(lstnsock, 2);

	for (;;) {
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
			double_lines(newsockfd);
			exit(0);
		}
		close(newsockfd);
	}
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

