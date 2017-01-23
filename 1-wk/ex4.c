#define _XOPEN_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void handle_interrupt()
{
	char msg[] = "SIGINT received\n";
	write(STDERR_FILENO, msg, sizeof msg);
	exit(1);
}

int sigaction();
struct sigaction act, oact;

int main(int argc, char *argv[])
{
	act.sa_handler=handle_interrupt;

	if (sigaction(SIGINT, &act, &oact) < 0) {
		perror("sigaction"); 
		exit(1);
	}

	if(argc==1) {
		char msg[] = "no file to read\n";
		write(STDOUT_FILENO, msg, sizeof msg);
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	char *buf[10];
	while(read(fd, buf, sizeof buf)>0) {
		write(STDOUT_FILENO, buf, sizeof buf);
		sleep(1); // so there's time to press ctrl-C
	}

	exit(0);
}

