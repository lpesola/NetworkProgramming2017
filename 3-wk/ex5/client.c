#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 50002

void send_data(int writefd, int datasize, int writesize);

/* ex 5.
 * Modify ex 4. so that
 * - client doesn't read stdin: instead it generates random data itself
 * - total amount of bytes to generate is given as an argument
 * - maximum amount of data to write is given as an argument
 * - close TCP connection to inform server that all data has been sent
 */
int main(int argc, char *argv[])
{
	if (argc != 4) {
		puts("usage: ./reader <ipaddr> datasize writesize");
		exit(1);
	}
	int datasize = atoi(argv[2]);
	printf("data: %d\n", datasize);
	int writesize = atoi(argv[3]);
	printf("writesize: %d\n", writesize);

	int sockfd = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	//convert IP address to correct form
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
	serv_addr.sin_port = htons(PORT);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("connect");
		exit(1);
	}
		
	send_data(sockfd, datasize, writesize);
	close(sockfd);
	shutdown(sockfd, SHUT_WR);

 	exit(0);
}

void send_data(int writefd, int datasize, int writesize) 
{
	char data[writesize]; 
	ssize_t nbytes;

	// generate some data to send
	// data doesn't need to be truly random or in any way meaningful

	for (int i = 0; i < datasize; i += writesize) {
		memset(&data, 2, writesize);
		nbytes = write(writefd, data, writesize);
		if (nbytes < 0) {
			perror("write");
			exit(1);
		}
	}
}


