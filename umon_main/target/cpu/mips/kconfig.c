/* kconfig.c:
 * Configure target for linux kernel.
 * This scheme is used with the Cadenux linux port used on the CSB350.
 * This command (along with the kernel that it configures) assumes there
 * is a block of space (KCONFIG_BASE) that is formatted as follows:
 *
 *    -----------------------
 *    | int argc            |
 *    -----------------------
 *    | char **argv         |
 *    -----------------------
 *    | char **envp         |
 *    -----------------------
 *    | uint init_address   |
 *    -----------------------
 *    | uint init_size      |
 *    -----------------------
 *    | data area           |
 *    -----------------------
 *
 * The data area is used to store the argument list array and the
 * strings that the array points to...
 *
 *    -----------------------
 *    | char *argv[ARGCMAX] |
 *    -----------------------
 *    | argv strings        |
 *    -----------------------
 *
 * So, if we assume ARGCMAX is 4, the base of the KCONFIG_BASE is zero,
 * and the argv list was "hello" "world", the data area would look like this...
 *
 *             -----------------------
 * 0x00000000  |  0x00000010         |       <--
 *             -----------------------         |
 * 0x00000004  |  0x00000016         |         | Space for the char *argv[]
 *             -----------------------         | array.
 * 0x00000008  |  0x00000000         |         |
 *             -----------------------         |
 * 0x0000000c  |  0x00000000         |       <--
 *             -----------------------
 * 0x00000010  |  hello\0            |       <--
 *             -----------------------         | Space for the strings pointed
 * 0x00000016  |  world\0            |         | to by the char *argv[] array.
 *             -----------------------         |
 * 0x0000001c  |  \0                 |       <--
 *             -----------------------
 *
 */

#include "config.h"
#include "cli.h"
#include "genlib.h"
#include "stddefs.h"
#include "tfs.h"
#include "tfsprivate.h"

#define ARGCMAX				16
#define ARGC_OFFSET			0
#define ARGV_OFFSET			1
#define ENVP_OFFSET			2
#define IADDR_OFFSET		3
#define ISIZE_OFFSET		4
#define ARGVARRAY_OFFSET	5
#define ARGVDATA_OFFSET		(ARGVARRAY_OFFSET+ARGCMAX)

#ifndef KCONFIG_BASE
#define KCONFIG_BASE	0xa0002000
#define KCONFIG_SIZE	0x00000fff
#define KCONFIG_END		(KCONFIG_BASE+KCONFIG_SIZE)
#endif

static int k_initialized;
static char k_file[TFSNAMESIZE+1];

int
kconfigInitrd(char *filename)
{
	TFILE	*tfp;
	long *space;

	if (strlen(filename) <= TFSNAMESIZE) {
		strcpy(k_file,filename);
	}
	else {
		printf("Filename (%s) too long\n",filename);
		return(-1);
	}
	tfp = tfsstat(filename);
	if (!tfp) {
		printf("%s: not found\n",filename);
		return(-1);
	}
	space = (long *)KCONFIG_BASE;
	space[IADDR_OFFSET] = (long)TFS_BASE(tfp);
	space[ISIZE_OFFSET] = (long)TFS_SIZE(tfp);
	return(0);
}

void
kconfigInit(void)
{
	long *space;

	space = (long *)KCONFIG_BASE;
	memset((char *)space,0,KCONFIG_SIZE);
	space[ARGV_OFFSET] = (long)&space[ARGVARRAY_OFFSET];
	k_file[0] = 0;
	k_initialized = 1;
}

void
kconfigList(void)
{
	int i, argc;
	long *space;
	char **argv;
	
	if (k_initialized) {
		space = (long *)KCONFIG_BASE;
		argc = space[ARGC_OFFSET];
		argv = (char **)space[ARGV_OFFSET];

		printf("Kconfig Base: 0x%lx\n",(long)space);

		printf("Arg count: %d%s\n",argc,argc > 0 ? " ..." : "");
		for(i=0;i<argc;i++)
			printf(" Arg %d: <%s>\n",i,argv[i]);

		if (k_file[0]) {
			printf("Initrd file: %s, addr: 0x%lx, size: %ld bytes\n",
				k_file,space[IADDR_OFFSET],space[ISIZE_OFFSET]);
		}
	}
	else {
		printf("kconfig not initialized\n");
	}
}

int
kconfigParam(char *param)
{
	int argc;
	long *space;
	char **argv, *data, *end;

	if (!k_initialized)
		kconfigInit();

	space = (long *)KCONFIG_BASE;
	end = (char *)KCONFIG_END;

	argc = (int)space[ARGC_OFFSET];
	if (argc >= ARGCMAX) {
		printf("Kconfig arg count is maxed out\n\n");
		return(-1);
	}

	/* Find the end of the last string stored in the data area:
	 */
	data = (char *)&space[ARGVDATA_OFFSET];
	if (*data != 0) {
		do {
			while(*data != 0)
				data++;
			data++;
		} while (*data != 0);
	}

	if ((data + strlen(param) + 1) >= end) {
		printf("Kconfig data area full\n");
		return(-1);
	}
	strcpy(data,param);
	argv = (char **)space[ARGV_OFFSET];
	argv[argc] = data;
	space[ARGC_OFFSET]++;
	return(0);
}

char *kconfigHelp[] = {
	"Kernel config",
	"-[f:ilp:]",
	" -f {file}   initrd file name",
	" -i          clear parameter list",
	" -l          show current paramter list",
	" -p {param}  add new parameter to list",
	0,
};

int
kconfig(int argc,char **argv)
{
	int	opt;

	while ((opt=getopt(argc,argv,"f:ilp:")) != -1) {
		switch(opt) {
		case 'f':
			kconfigInitrd(optarg);
			break;
		case 'i':
			kconfigInit();
			break;
		case 'l':
			kconfigList();
			break;
		case 'p':
			kconfigParam(optarg);
			break;
		default:
			return(CMD_SUCCESS);
		}
	}

	/* If no args or options, list the current kconfig data:
	 */
	if (argc == 1) {
		kconfigList();
		return(CMD_SUCCESS);
	}

	if (argc != optind)
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
