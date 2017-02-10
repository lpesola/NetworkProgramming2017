#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 50002

void read_lines(int writefd);

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
	if (argc != 2) {
		puts("usage: ./reader <ipaddr>");
		exit(1);
	}


	int sockfd = socket(PF_INET, SOCK_DGRAM, PF_UNSPEC);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	//convert IP address to correct form
	inet_aton(argv[1], &serv_addr.sin_addr);
	serv_addr.sin_port = htons(PORT);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("connect");
		exit(1);
	}
		
	read_lines(sockfd);

 	exit(0);
}

/*
 * Read text from stdin. 
 * Write all lines 100 bytes or less to socket for further processing.
 * Longer lines are ignored.
 */

void read_lines(int writefd) 
{
	puts("type in text to be doubled, ctrl-D ends");
	char readbuf[100]; 
	char writebuf[100]; 
	ssize_t nbytes;
	// number of characters read before a newline
	int len = 0; 
	// non-zero if last character in readbuf wasn't a newline
	int longline = 0; 
	// the current position in writebuf
	int j = 0; 

	while ((nbytes = read(STDIN_FILENO, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) { 
			perror ("read from stdin");
			exit(1);
		}
		
/* 
 * To ignore too long lines keep track of the number of characters read (len).
 * When >100 characters have been read, set longline to be true (non-zero) 
 * to mark that next segments first characters are part of a too long line.
 * 
 * When a newline is read, if the line was not too long (= longline is false (0)),
 * write everything up to here to writefd for further processing.
 * 
 * If longline is true, the end of a too long line has been reached so 
 * characters up to here can be ignored. 
 *
 * strchr could be used for simpler code.
 */

		for (int i = 0; i < nbytes ; i++) {
			writebuf[j] = readbuf[i];
			len +=1;
			j += 1;
			if (readbuf[i] == '\n') {
				if (longline) {
					longline = 0;
					len = 0;
					j = 0;	
				} else {
					write(writefd, writebuf, len);
					j = 0;
					len = 0;
			       	}
			}
			if (len > 100) {
				j = 0;
				len = 0;
				longline = 1;
			}

		}
	}
}


