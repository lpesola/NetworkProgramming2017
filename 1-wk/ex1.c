#define _XOPEN_SOURCE 700
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

void print_env();
void get_pid_info();
struct timeval measure_time(void (*func)());
struct timespec measure_time_clock(void (*func)());

struct pid_info {
	int pid;
	int ppid;
	int pgrpid;
	int uid;
}; 
struct pid_info pi = {0};

// assuming information about _this_ program is requested
int main(int argc, char **argv) 
{
	struct timeval result = measure_time(print_env);
	struct timespec result_c = measure_time_clock(print_env);

	printf("Time elapsed printing environment variables:\n");
	printf("- gettimeofday: %ld s, %ld us\n", result.tv_sec, result.tv_usec);
	printf("- clock_gettime: %ld s, %ld ns\n", result_c.tv_sec, result_c.tv_nsec);
	
	result = measure_time(get_pid_info);
	result_c = measure_time_clock(get_pid_info);
	
	printf("This process' PID %d \n", pi.pid);	
	printf("Parent's PID %d \n", pi.ppid);	
	printf("Process group's ID %d \n", pi.pgrpid);	
	printf("(Real) UID %d \n", pi.uid);	

	printf("Time elapsed getting pid info:\n");
	printf("- gettimeofday: %ld s, %ld us\n", result.tv_sec, result.tv_usec);
	printf("- clock_gettime: %ld s, %ld ns \n\n", result_c.tv_sec, result_c.tv_nsec);


	
	return 0;
}


void get_pid_info() 
{	
	pi.pid = getpid();
	pi.ppid = getppid();
	pi.pgrpid = getpgrp();
	// assuming real uid is wanted
	pi.uid = getuid();
}

// part b)
void print_env() 
{
	extern char **environ;
	printf("Environment variables: \n");
	int i = 0;
	while(environ[i]) {
		printf("%s\n", environ[i++]);
	}
}

/*
 * Using gettimeofday() as per the assignment. 
 * However: 
 * gettimeofday() is obsolete (POSIX.1-2008).
 * gettimeofday()'s results are affected by jumps in system time.
 * The man page recommends clock_gettime() 
 */

struct timeval measure_time(void (*func)())
{
	struct timeval before;
	struct timeval after;
	gettimeofday(&before, NULL);
	func();
	gettimeofday(&after, NULL);
	after.tv_sec -= before.tv_sec;
	after.tv_usec -= before.tv_usec;
	return after;
}

struct timespec measure_time_clock(void (*func)())
{
	struct timespec before;
	struct timespec after;
	clock_gettime(CLOCK_MONOTONIC, &before);
	func();
	clock_gettime(CLOCK_MONOTONIC, &after);
	after.tv_nsec = after.tv_nsec - before.tv_nsec;
	after.tv_sec = after.tv_sec - before.tv_sec;
	return after;
}

