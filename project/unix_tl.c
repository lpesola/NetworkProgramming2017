#define _POSIX_C_SOURCE 200112L // for getaddrinfo
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h> // for memeset
#include <errno.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h> // for uppercase conversion
#define LSTNPORT 22047 
#define SOURCECODE "kurssit/netwprog/project/unix_tl.c\n"

void serve();
void create_servers();
void send_file(char *line);

int main (void) {

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
	// in all likelyhood the list only contains one entry
	// but just in case: loop through all the entries to make sure
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

	// result struct no longer needed
	freeaddrinfo(result);

	// chat with the server	
	char buf[1000];
	int rb = read(sockfd, buf, sizeof buf);
	write(STDOUT_FILENO, buf, rb); 
	
	char msg[] = "hello\nlpesola\n";
	write(sockfd, msg, sizeof msg);
	printf("sent: %s\n", msg);

	while( (rb = read(sockfd, buf, sizeof buf)) != 0 ) {
		if (rb == -1 && errno == EINTR) 
			continue;
		
		if (rb == -1) {
			perror("1st chat");
			exit(1);
		}
		// find out the port number
		// and fork a child process that listens on that port
		// the child process takes care of creating actual servers 
		// (explained below) 
		// this is NOT a realiable way to do it but works for this assignment
		// since we can expect the only line containg the word port is 
		// the one that defines the port number
		write(STDOUT_FILENO, buf, rb);
		const char *port = "port";
		char *p_port = strstr(buf, port);
		if (p_port != NULL) {
			int port_no = atoi(p_port+5);	
			printf("the port number is: %d\n", port_no);
			int pid;
			if((pid = fork()) < 0) {
				perror("1st fork");
				exit(1);
			}
			if (pid == 0) {
				// this is a child
				create_servers(port_no);
				// if we return, exit this process
				exit(0);
			} else {
				printf("forked a child server: %d\n", pid);
			}


		}
		// make sure the buffer is zeroed before reading again
		memset(buf, 0, sizeof buf); 
	}

	close(sockfd);
	// we can block here until everything is done
	wait(NULL);
	return(0);
}


/* 
 * Listen to the port specified.
 * For a new child server to serve each new client.
 * todo: This child needs to be terminated somehow  
 */
void create_servers(int port) {

	int listnsock = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (listnsock < 0) {
		perror("create socket for listening: ");
		exit(1);
	}

	struct sockaddr_in our_serv = {0};
	our_serv.sin_family = AF_INET;
	our_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	our_serv.sin_port = htons(port);
	int b = bind(listnsock, (struct sockaddr *)&our_serv, sizeof(our_serv));
	if (b < 0) {
		perror("bind");
		exit(1);
	}
	listen(listnsock, 2);

	// for testing purposes only accept once
	for (int i = 0; i < 3; i++) {
		struct sockaddr_in cli_addr;
		socklen_t clilen = sizeof(cli_addr);
		int clisock = accept(listnsock, (struct sockaddr*)&cli_addr, &clilen);
		if (clisock < 0) {
			perror("accept error (not quitting) ");
			continue;
		}
		int pid = fork();
		// fork errors are rare, not checking that now
		if (pid == 0) {
			close(listnsock);
			serve(clisock);
			exit(0);
		}	
		// parent closes the socket only the child needs
		close(clisock);
	}

}
/* read a line (ends in \n) into the buffer
 * then figure out the correct command
 * and call the appropriate function
 * expecting that the lines are not horribly long > 10k characters
 * this of course is not a safe assumption under normal conditions
 */
void serve(int sockfd) {
	int nbytes;
	char readbuf[10000];
	char writebuf[10000];
	char line[10000];
	// current position in writebuf; stored here in case the whole message
	// is not read in one read
	int j = 0;
	// if this is 0, the previous read didn't contain a newline
	// otherwise contains a command
	char cmd = 0;
	char *newline = NULL;
	while((nbytes = read(sockfd, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;
		if (nbytes == -1) {
			perror("this child stops (can be intentional) : ");
			break;
		}
	

		/* this still expects that after a newline there is nothing!!!
		 * that seemed to be the case but cant be sure..
		 */

		strncpy(&line[j], readbuf, nbytes); 
		char *n_exists = strchr(readbuf, '\n');
		j += nbytes;
		if (n_exists != NULL) {
			// a line ended during this read
			// check the command
			puts("HERE ENDED A LINE!!!!!!!!");
			cmd = line[0];
		} else {
			continue;
		}	


		switch(cmd) {
			case '#':
				// this is a comment: ignore but print
				// expect everything to be read in one read...
				puts("THE FOLLOWING LINE WAS IGNORED");
				write(STDOUT_FILENO, line, j);
				puts("THAT WAS AN IGNORED LINE");
				break;
			case 'E':
				for (int i = 0; i < j; i++) {
					writebuf[i] = toupper(line[i]);
				}
				puts("ECHO THIS LINE:");
				write(STDOUT_FILENO, writebuf, j);
				write(sockfd, writebuf, j);
				puts("THAT LINE WAS ECHOED TO THE SENDER");
				break;
			case 'C':
			        write(sockfd, SOURCECODE, sizeof SOURCECODE);
				break;
			case 'F':
				// maybe check that the file requested makes some sense
				// =>the program source code and not /etc/passwd
				// or some other silly thing
				send_file(readbuf);
				break;
			case 'A':
				puts("Everything done; can quit");
				break;
			case 'Q':
				// quit this connection?
				close(sockfd);
				break;
			default:
				printf("incorrect command: %c \n", cmd);
				exit(1);

		}
		// if we are here, we've finished reading and handling a line
		// make sure readbuf and line are empty
		memset(readbuf, 0, sizeof readbuf);
		memset(line, 0, sizeof line);
		memset(writebuf, 0, sizeof writebuf);
		j = 0;
		
	}


}


void send_file(char *line) {

	/*
	 * find out name of file
	 * find out port
	 * reply with size
	 * connect to port
	 * send file
	 * close socket
	 */

}
