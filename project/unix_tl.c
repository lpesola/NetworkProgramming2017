#define _POSIX_C_SOURCE 200112L // for getaddrinfo
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h> // for memeset
#include <errno.h> 
#include <stdio.h>
#include <unistd.h>
#define LSTNPORT 22047 
#define SOURCECODE "kurssit/netwprog/project/unix_tl.c"
void serve();
void create_servers();

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
	return(0);
}



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

	for (;;) {
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

/* commands needed
 * # -> comment
 * E -> echo
 * C -> send path name
 * F -> send file to port, also filesize first as reply
 * A -> we're ready, can quit
 * Q -> quit this connection? THIS?
 */

void serve(int sockfd) {
	int nbytes;
	// assuming messages are pretty small
	char readbuf[1000];
	
	while((nbytes = read(sockfd, readbuf, sizeof readbuf)) != 0) {
		if (nbytes == -1 && errno == EINTR)
			continue;

		if (nbytes == -1) {
			perror("reading from client, this child stops");
			break;
		}
		printf("%d, NEW ROUND\n", getpid());
		// write out everything we got
		write(STDOUT_FILENO, readbuf, nbytes);
	}


}
