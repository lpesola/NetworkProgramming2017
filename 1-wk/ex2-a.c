/* 
 * Doubler version a: use stdio for read & write.
 * Read text lines, write each line twice to stdout.
 * so that the copied appears immediately after the original.
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

// we may assume that lines are not arbitrarily/extremely long
void double_lines(FILE *fp) 
{
	char buf[1000];
	while (fgets(buf, sizeof buf, fp)!=NULL)
		printf("%s%s", buf, buf);

	
}





