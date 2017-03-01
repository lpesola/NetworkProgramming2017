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


/* ex 2 a.
 * Find out the biggest UDP datagram that can be sent 
 * (presumably: sent without fragmentation, which is turned off by default..)
 */
int main(int argc, char *argv[])
{
	if (argc != 3) {
		puts("usage: <program> <ip address> <starting size>");
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

	// start from message size, which should be a positive integer
	// find out first if that is too large: if yes, halve it until it's not
	// then increase the size by 1 until sending fails due to size
	// we could just increase by 1 from 0 but that's not really necessary since we can guess at the maximum size
	
	size_t bufsize = atoi(argv[2]);
	
	int sent = 0;
	do {	
		char buffer[bufsize++];
		printf("size: %ld \n", bufsize);
		sent = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (sent == -1 && errno == EINTR)
			continue;
	} while (sent > 0);


	if (sent == -1 && errno == EMSGSIZE) {
		printf("maxsize = %ld \n", bufsize--);	
	}

 	exit(0);

}
