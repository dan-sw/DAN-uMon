/* newlib_main.c:
 * This is an example of some of the stuff that "just works" as a result
 * of including the newlib_hooks.c file with a MicroMonitor application.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "monlib.h"

void
input_stuff(void)
{
	char	c;
	int		d1, d2, d3;

	/* getchar():
	 */
	printf("Gimme a character...\n");
	c = getchar();
	printf("You gave me '%c'\n",c);

	/* scanf():
	 */
	printf("Enter 3 whitespace-delimited integers\n");
	scanf("%d %d %d",&d1,&d2,&d3);
	printf("%d %d %d\n",d1,d2,d3);
}

void
env_stuff(void)
{
	printf("IPADD: %s\n",getenv("IPADD"));
	setenv("abc","def",1);
}

void
file_read(char *name)
{
	int		i=1;
	FILE	*fp;
	char	line[256];

	if ((fp = fopen(name,"r")) != (FILE *)NULL) {
		while(fgets(line,sizeof(line),fp) != NULL) {
			printf("%3d: ",i++);
			printf("%s",line);
		}
		fclose(fp);
	}
}

void
file_write(char *name)
{
	FILE	*fp;

	if ((fp = fopen(name,"w")) != (FILE *)NULL) {
		printf("Writing three lines...\n");
		fwrite("this is line1\n",14,1,fp);
		fwrite("this is line2\n",14,1,fp);
		fwrite("this is line3\n",14,1,fp);
		fclose(fp);
	}
}

void
file_append(char *name)
{
	FILE	*fp;

	if ((fp = fopen(name,"a")) != (FILE *)NULL) {
		printf("Writing three lines...\n");
		fwrite("this is line4\n",14,1,fp);
		fwrite("this is line5\n",14,1,fp);
		fwrite("this is line6\n",14,1,fp);
		fclose(fp);
	}
}

void
file_script(void)
{
	FILE	*fp;

	if ((fp = fopen("script,e,test","w")) != (FILE *)NULL) {
		printf("Writing three lines...\n");
		fwrite("echo abcde\n",11,1,fp);
		fwrite("echo fghij\n",11,1,fp);
		fwrite("echo klmno\n",11,1,fp);
		fclose(fp);
	}
}

int
main(int argc, char *argv[])
{
	fprintf(stderr,"Hello errored world\n");
mon_sendenetpkt(0,0);
mon_recvenetpkt(0,0);

//	input_stuff();
//	env_stuff();
	file_read("monrc");
	file_write("filex");
	file_append("filex");
	file_script();
	exit(0);
}
