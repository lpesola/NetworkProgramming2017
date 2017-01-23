/* 
 * Doubler version a). 
 * Read text lines, write each line twice to stdout
 * so that original and re-written once are right next to each other.
 * File to read from may be defined as an argument: if no file is specified
 * stdin is used.
 * Expecting lines to be at most ????? bytes long. 
 * Use stdio.h for read & write.
 */

#include <stdio.h>
void double_lines(FILE *fp);

int main(int argc, char *argv[])
{ 
	if(argc==1) {
		double_lines(stdin);
	} else {
		FILE *fp; 
		fp = fopen(argv[1], "r");
		if (!fp)
			return 1;

		double_lines(fp);
		fclose(fp);
	}
	return 0;
}

void double_lines(FILE *fp) 
{
	char buf[1000];
	while (fgets(buf, sizeof buf, fp)!=NULL)
		printf("%s%s", buf, buf);

	
}





