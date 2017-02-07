#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "lock_fcntl.c"
#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
			/* default permissions for new files */
#define LOCKFILE "MKLCK.ex6"  
#define MAPPED_FILE "temp.txt"
void my_lock_init(char *pathname);
void my_lock_wait();
void my_lock_release();

/*
 * ex 2.
 * modify programs shm_incr.c and lock_fcntl.c so that 
 * - there's no fork: separate programs are run instead
 * - the programs access the same shared memory area
 * => modify 
 * - file lock init
 * - shared memory init (processes need to use the same path name)
 * 
 * ex 3.
 * modify ex 2. so that there's a shared character array adjacent to the shared
 * integer counter in the shared memory area.
 * Every time a process updates the counter, it writes its name in the table
 * using the counter value as the index
 */

int main(int argc, char **argv)
{
	int fd, i, nloop, zero = 0;
	int *ptr;
	pid_t pid;

	if (argc != 2) {
		puts("usage: incr2 <#loops>"); 
		exit(1); 
	}
	nloop = atoi(argv[1]);

	/* 4open file, initialize to 0, map into memory */
	// first change for ex 2: argv[1] => MAPPED_FILE
	if ((fd = open(MAPPED_FILE, O_RDWR | O_CREAT, FILE_MODE)) < 0) {
		perror("open failed"); 
		exit(1); 
	}
	
	// this is used to initialize counter to 0?
	if (write(fd, &zero, sizeof(int)) <0) {
		perror("write failed"); 
		exit(1); 
	}
	
	ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (ptr == MAP_FAILED) {
		perror("mmap failed"); 
		exit(1); 
	}
	close(fd);

	pid = getpid();
	/* 4create, initialize, and unlink file lock */
	my_lock_init(LOCKFILE);
	setbuf(stdout, NULL);	/* stdout is unbuffered */

	sleep(2);
	for (i = 0; i < nloop; i++) {
		my_lock_wait();
		int temp = (*ptr)++;
		printf("process %d: %d\n", pid, temp);
		usleep(200000);
		my_lock_release();
	}

	exit(0);
}
