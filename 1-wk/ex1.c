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
	void (*fptr)();
	fptr = get_pid_info;
	struct timeval result = measure_time(fptr);
	struct timespec result_clock = measure_time_clock(fptr);
	
	printf("This process' PID %d \n", pi.pid);	
	printf("Parent's PID %d \n", pi.ppid);	
	printf("Process group's ID %d \n", pi.pgrpid);	
	printf("(Real) UID %d \n", pi.uid);	

	printf("Time elapsed getting pid info: %ld s and %ld micros\n", result.tv_sec, result.tv_usec);
	printf("Time elapsed according to clock_: %ld and %ld \n\n", result_clock.tv_sec, result_clock.tv_nsec);

	fptr = print_env;
	result = measure_time(fptr);
	result_clock = measure_time_clock(fptr);
	printf("Time elapsed printing environment variables: %ld s and %ld micros\n", result.tv_sec, result.tv_usec);
	printf("Time elapsed according to clock_:");
	printf("%ld sec and %ld nanosec\n", result_clock.tv_sec, result_clock.tv_nsec);
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
	// assuming here execution will not take seconds; it should not
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
	// assuming here execution will not take seconds; it should not
	after.tv_nsec = after.tv_nsec - before.tv_nsec;
	after.tv_sec = after.tv_sec - before.tv_sec;
	return after;
}

