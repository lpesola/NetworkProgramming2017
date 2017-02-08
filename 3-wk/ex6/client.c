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
#include <time.h>
#define PORT 50002

void send_data(int writefd, int datasize, int writesize);

/* ex 6.
 * Measure the time it takes to
 * - establish connection
 * - send all data
 * Optional: calculate and print measured throughput (bytes/s)
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

	struct timespec before, after;
	clock_gettime(CLOCK_MONOTONIC, &before);
	int ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	clock_gettime(CLOCK_MONOTONIC, &after);

	puts("Connection established in");
	printf("%ld s and %ld ns\n", after.tv_sec-before.tv_sec, after.tv_nsec-before.tv_nsec);

	if(ret != 0){
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

	struct timespec before, after;
	clock_gettime(CLOCK_MONOTONIC, &before);
	for (int i = 0; i < datasize; i+= writesize) {
		memset(&data, 2, writesize);
		nbytes = write(writefd, data, writesize);
		if (nbytes < 0) {
			perror("write");
			exit(1);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &after);
	puts("Data sent in");
	printf("%ld s and %ld ns\n", after.tv_sec-before.tv_sec, after.tv_nsec-before.tv_nsec);


}


