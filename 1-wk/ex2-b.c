/* 
 * Doubler version b: use system calls for read & write
 * Read text lines, write each line twice to stdout
 * so that the copied appears immediately after the original
 */

#include <fcntl.h>
#include <unistd.h>

void double_lines(int fd);

int main(int argc, char *argv[])
{ 
	if(argc==1) {
		double_lines(STDIN_FILENO);
	}else {
		int fd = open(argv[1], O_RDONLY);
		double_lines(fd);
		close(fd);
	} 
	return 0;
}

// we may assume that lines are not arbitrarily/extremely long
void double_lines(int fd) 
{
	char buf[1000];
	int i = 0;
	while(read(fd, &buf[i], 1) == 1) {
		if (buf[i] == '\n'){
			write(STDOUT_FILENO, &buf[0], i+1);
			write(STDOUT_FILENO, &buf[0], i+1);
			buf[0]='\0';
			i = 0;
		}
		i+=1;	
	}	


}



