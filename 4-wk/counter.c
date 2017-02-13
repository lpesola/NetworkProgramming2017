#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "lock_fcntl.c"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) /* default permissions for new files */
#define LOCKFILE "/tmp/MKLCKXXXXXX" 
#define MAPPED_FILE "temp.txt"
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

void my_lock_init(char *pathname);
void my_lock_wait();
void my_lock_release();

/* Ex. 3 & 4.
 * Like wk3/ex3 but there are only two processes, A & B
 * and they must take turns in writing their name in the character table
 * so that the same letter never appears two times in a row.
 */

struct counter {
	int c;
	char names[];
};

void print_chars(struct counter *ct);

int main(int argc, char **argv)
{
	if (argc != 3) {
		puts("usage: incr2 <#loops> <#name>"); 
		exit(1); 
	}
	int nloop = atoi(argv[1]);
	char name = argv[2][0];

	int fd = open(MAPPED_FILE, O_RDWR | O_CREAT, FILE_MODE);
	if (fd < 0) {
		perror("open failed"); 
		exit(1); 
	}

	struct counter *ct = {0};
	// just some size for testing purposes	
	size_t size = sizeof(struct counter) + 100 * sizeof(char);
	ftruncate(fd, size);
	ct = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ct == MAP_FAILED) {
		perror("mmap failed"); 
		exit(1); 
	}
	memset(ct, 0, size);
	close(fd);

	int readfd, writefd;
	char buf[1];
	buf[0] = name;
	if (name == 'A') {
		mkfifo(FIFO1, 0666);
		mkfifo(FIFO2, 0666);
		readfd = open(FIFO1, O_RDONLY);
		writefd = open(FIFO2, O_WRONLY);

		for (int i = 0; i < nloop; i++) {
			if (read(readfd, buf, sizeof buf) < 0) {
				perror("read");
				exit(1);
			}
			int tmp = ct->c++;
			printf("%d\n", tmp);	
			ct->names[tmp] = name;		
			if (write(writefd, buf, sizeof buf) < 0) {
				perror("write");
				exit(1);
			}
		}
	} else {
		writefd = open(FIFO1, O_WRONLY);
		readfd = open(FIFO2, O_RDONLY);
		for (int i = 0; i < nloop; i++) {			
			if (write(writefd, buf, sizeof buf) < 0) {
				perror("write");
				exit(1);
			}	
			if (read(readfd, buf, sizeof buf) < 0) {
				perror("read");
				exit(1);
			}
			int tmp = ct->c++;
			printf("%d\n", tmp);	
			ct->names[tmp] = name;			
		}
	}


	close(writefd);
	close(readfd);

	if (name == 'A') {
		unlink(FIFO1);
		unlink(FIFO2);
	}
	
	print_chars(ct);

	exit(0);
}

void print_chars(struct counter *ct) {
	puts("char table");
	char n = ct->names[0];
	char *ptr = &ct->names[0];
	int i = 0;
	
	while (n != '\0') {
		printf("%d: %c\n", i, n);
		ptr++;
		n = *ptr;
		i++;
	}
}
