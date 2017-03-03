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
#define PORT 7


/* ex 2 b.
 * Use connect with a UDP socket. 
 * What happens in the network? What does connect() do?
 * Send datagrams using connected socket to a host that doesn't
 * accept the messages from the given port. What happens in the network? 
 * The messages get discarded, server sends ICMP port unreachable.
 *
 * Which error is generated?
 * Connection refused.
 *
 * What if the host is not connected to the network at all?
 * Nothing happens, the address cannot be translated. There is no host that would send the ICMP message.
 */

int main(int argc, char *argv[])
{
	if (argc < 2) {
		puts("usage ./ex2-b <addr>");
		exit(0);
	} 
	char *server = argv[1];

	struct hostent *host;
    	if ((host = gethostbyname(server)) == NULL) {
		herror("gethostbyname");
		return 2;
	}
	struct in_addr **addr_list = (struct in_addr **) host->h_addr_list;
  	  for(int i = 0; addr_list[i] != NULL; i++) {
	            printf("%s ", inet_ntoa(*addr_list[i]));
		        }

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = host->h_addrtype;
	serv_addr.sin_port = htons(PORT);
	bcopy(host->h_addr, &serv_addr.sin_addr, host->h_length);

	int sockfd = socket(host->h_addrtype, SOCK_DGRAM, PF_UNSPEC);

	printf("address resolved to %s \n", inet_ntoa(serv_addr.sin_addr));

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
	 * (*) datagrams whose address/port doesnt match, are either discarded with an ICMP port unreachable
	 * 	or delivered to another socket on the host.
	 */

	char *msg = "hi";
	int sent = send(sockfd, msg, sizeof(msg), 0);
	if (sent < 0) {
		printf("error: %s\n", strerror(errno));
	} else {
		puts("sent msg");
	}
	
	char recvd[10];
	int rb = recv(sockfd, recvd, sizeof(recvd), 0);
	if (rb < 0) {
		printf("error: %s\n", strerror(errno));
	} else {
		printf("%s\n", recvd);
	}
	
	exit(0);


}
