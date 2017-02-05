#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
                        /* default permissions for new files */

static struct flock	lock_it, unlock_it;
static int		lock_fd = -1;
			/* fcntl() will fail if my_lock_init() not called */

/*
 * ex 2.
 * modify programs shm_incr.c and lock_fcntl.c so that 
 * - there's no fork: separate programs are run instead
 * - the programs access the same shared memory area
 * => modify 
 * - file lock init
 * - shared memory init (processes need to use the same path name)
 */


void my_lock_init(char *pathname)
{
    char	lock_file[1024];

		/* 4must copy caller's string, in case it's a constant */
    strncpy(lock_file, pathname, sizeof(lock_file));
    if (mktemp(lock_file) == NULL) {
	perror("mktemp failed"); exit(1); };

    lock_fd = open(lock_file, O_CREAT | O_WRONLY, FILE_MODE);
    if (lock_fd < 0) {
    	perror("lock file open failed"); exit(1); }
    unlink(lock_file);			/* but lock_fd remains open */

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = 0;
	lock_it.l_len = 0;

	unlock_it.l_type = F_UNLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = 0;
	unlock_it.l_len = 0;
}

void my_lock_wait()
{
    int		rc;
    
    while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
	if (errno == EINTR)
		continue;
    	else {
		perror("fcntl error for my_lock_wait"); exit(1); }
    }
}

void my_lock_release()
{
    if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0) {
		perror("fcntl error for my_lock_release"); exit(1); }
}
/* end my_lock_wait */
