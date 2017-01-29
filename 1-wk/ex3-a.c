/*
 * Retrieve the type, owner and size of a given file and print them.
 */
#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <stdio.h>
#include <pwd.h>

int main(int argc, char *argv[]) 
{ 
	if (argc == 1) {
		perror("no file specified");
		return 1;
	} else {
		char *fn = argv[1];
		struct stat finfo;
		struct passwd *pinfo;
		stat(fn, &finfo);
		pinfo = getpwuid(finfo.st_uid);
		struct passwd pwd;
		pwd = *pinfo;
	
		printf("Owner: %s\n", pwd.pw_name);	
		printf("Size: %ld B\n", finfo.st_size);

		char *type;
		if (S_ISDIR(finfo.st_mode)!=0)
			type = "directory";
		else if (S_ISCHR(finfo.st_mode)!=0)
			type = "character special file";
		else if (S_ISBLK(finfo.st_mode)!=0)
			type = "block special file";
		else if (S_ISREG(finfo.st_mode)!=0)
			type = "regular file";
		else if (S_ISFIFO(finfo.st_mode)!=0)
			type = "pipe or FIFO special file";
		else if (S_ISLNK(finfo.st_mode)!=0)
			type = "symbolic link";
		else if (S_ISSOCK(finfo.st_mode)!=0)
			type = "socket";
		else type = "N/A";

		printf("Type: %s\n", type);
	}

	return 0;
}


