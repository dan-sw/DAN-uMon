/* i2c.c:
 * This file contains all of the generic code to support a target
 * with one or more I-Squared-C interface masters.
 * It assumes that somewhere in target-specific space there are three
 * i2c interface functions: i2cRead(), i2cWrite() and i2cCtrl().
 */
#include "config.h"
#include "cli.h"
#include "i2c.h"
#include "genlib.h"
#include "stddefs.h"

int	i2cVerbose;

char *I2cHelp[] = {
	"I-Squared-C Interface Access",
	"[options] {init | show | read addr len | write addr data}",
#if INCLUDE_VERBOSEHELP
	"Options:",
	" -b        burst",
	" -i{##}    interface",
	" -w{data}  pre-write data as part of read (uses repeated start)",
	" -v        verbose",
#endif
	0,
};

int
I2cCmd(int argc, char *argv[])
{
	char	*next;
	uchar	buf[256];
	int		i, wtot, opt, interface, len, rslt, addr, burst, ret, rwctrl;

	rslt = 0;
	wtot = 0;
	burst = 0;
	rwctrl = 0;
	interface = 0;
	i2cVerbose = 0;
	ret = CMD_SUCCESS;
	while ((opt=getopt(argc,argv,"bi:vw:")) != -1) {
		switch(opt) {
		case 'i':
			interface = atoi(optarg);
			break;
		case 'b':
			burst = 1;		/* Do multiple accesses within one START/STOP */
			break;
		case 'w':			/* Pre-write in a read */
			burst = 1;
			rwctrl = REPEATED_START;
			for(wtot=1;wtot<sizeof(buf);wtot++) {
				buf[wtot] = (uchar)strtol(optarg,&next,0);
				if (*next == ',')
					optarg = next+1;
				else
					break;
			}
			buf[0] = wtot;
			break;
		case 'v':
			i2cVerbose = 1;
			break;
		default:
			return(CMD_FAILURE);
		}
	}

	if (argc < (optind+1))
		return(CMD_PARAM_ERROR);

	if (strcmp(argv[optind],"read") == 0) {
		if (argc != (optind+3))
			return(CMD_PARAM_ERROR);
		addr = strtol(argv[optind+1],0,0);
		len = strtol(argv[optind+2],0,0);
		if (len > sizeof(buf))
			len = sizeof(buf);
		if (burst) {
			addr |= rwctrl;
			rslt = i2cRead(interface,addr,buf,len);
		}
		else {
			for(i=0;i<len;i++) {
				rslt = i2cRead(interface,addr,buf+i,1);
				if (rslt < 0)
					break;
			}
		}
		if (rslt < 0) {
			printf("i2cRead = %d\n",rslt);
			ret = CMD_FAILURE;
		}
		else
			printMem(buf,len,1);
	}
	else if (strcmp(argv[optind],"write") == 0) {
		if (rwctrl == REPEATED_START) {
			printf("-w applies to read only\n");
			return(CMD_FAILURE);
		}
		if (argc < (optind+3))
			return(CMD_PARAM_ERROR);
		addr = strtol(argv[optind+1],0,0);
		for(len=0,i=optind+2;i<argc;i++,len++) 
			buf[len] = (uchar)strtol(argv[i],0,0);

		if (burst) {
			rslt = i2cWrite(interface,addr,buf,len);
		}
		else {
			for(i=0;i<len;i++) {
				rslt = i2cWrite(interface,addr,buf+i,1);
				if (rslt < 0)
					break;
			}
		}
		if (rslt < 0) {
			printf("i2cWrite = %d\n",rslt);
			ret = CMD_FAILURE;
		}
	}
	else if (strcmp(argv[optind],"init") == 0) {
		i2cCtrl(interface,I2CCTRL_INIT,0,0);
	}
	else if (strcmp(argv[optind],"show") == 0) {
		i2cCtrl(interface,I2CCTRL_SHOW,0,0);
	}
	else
		return(CMD_PARAM_ERROR);

	i2cVerbose = 0;
	return(ret);
}
