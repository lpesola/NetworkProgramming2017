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
#define PORT 13
#define SERVER "ukko127.cs.helsinki.fi"


/* ex 2 b.
 * Use connect with a UDP socket. 
 * What happens in the network? What does connect() do?
 * Send datagrams using connected socket to a host that doesn't
 * accept the messages from the given port. What happens in the network? 
 * Which error is generated?
 * What if the host is not connected to the network at all?
 */

int main(void)
{


	struct hostent *host = gethostbyname(SERVER);
	if (host == NULL) {
		puts("gethostbyname failed");
		exit(1);
	}

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = host->h_addrtype;
	serv_addr.sin_port = htons(PORT);
	//convert IP address to correct form
	inet_aton(host->h_addr_list[0], &serv_addr.sin_addr);
	int sockfd = socket(host->h_addrtype, SOCK_DGRAM, PF_UNSPEC);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("connect");
		exit(1);
	}

 	/* When connect is called on a UDP socket:
	 * Kernel checks for any immediate errors (can the other end be connected to for example).
	 * The peer IP address and port number are recorded (using the sockaddr struct that connect got)
	 * 1. Use write or send: 
	 * 	operations automatically use the address/port defined
	 * 2. Kernel only returns datagrams sent from the address/port conmbination
	 * 	specified when calling connect
	 * 3. The process receives asynchronous errors (unlike from an unconnected socket)
	 *
	 * So connect can only be used on a socket that is used to communicate with a single other host.
	 *
	 * (*) datagrams whose address/port doesnt match, are either discarded with an ICPM port unreachable
	 * 	or delivered to another socket on the host.
	 */

	char *msg = "hi";
	send(sockfd, msg, sizeof(msg), 0);

 	exit(0);

}
