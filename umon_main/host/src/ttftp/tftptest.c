#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#ifdef BUILD_WITH_VCC
#include <io.h>
#include <winsock2.h>
typedef unsigned short ushort;
#define sleep(n) Sleep(n*1000)
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "ttftp.h"


int		test_Opcode = 0, test_OpcodePassCount = 0;
int		test_CorruptByte, test_OpcodePass, test_Type, test_SleepTime;
int		test_OpcodePassCount;

/* testTftp():
	This function is called at the top of each opcode reception to
	support the ability to insert various errors into the protocol
	and/or data.
*/
void
testTftp(int opcode,char *msg,int len)
{
	int	i;

	if (opcode != test_Opcode)
		return;

	if (++test_OpcodePassCount != test_OpcodePass)
		return;

	if (test_Type == TFTPTEST_QUIT) {
		exit(1);
	}
	else if (test_Type == TFTPTEST_SLEEP) {
		fprintf(stderr,"TEST: sleeping...");
		for(i=0;i<test_SleepTime;i++) {
			sleep(1);
			fprintf(stderr,".");
		}
		fprintf(stderr,"\n");
	}
	else if (test_Type == TFTPTEST_CORRUPT) {
		fprintf(stderr,"TEST: corruption...");
		msg[test_CorruptByte] = 0;
	}
	else if (test_Type == TFTPTEST_BBNO) {
		fprintf(stderr,"TEST: blockno err...");
		msg[3] += 3;
	}
}

/* testSetup():
	Called at startup to setup tests that are carried out by testTftp().
	The incoming line is formatted...
		OPCODE,OPCODE_PASS,TESTTYPE,ARG1,ARG2
	where...
		OPCODE is RRQ, WRQ, ACK, DAT or ERR;
		OPCODE_PASS indicates what opcode to apply this to, for example,
			if OPCODE_PASS is 3 and OPCODE is DAT, the apply this test to
			the third TFTP_DAT;
		TESTTYPE is SLEEP or CORRUPT
			if SLEEP, then ARG1 is sleep time, ARG2 is not used;
			if CORRUPT, then ARG1 is byte number, ARG2 is not used; 

*/
int
testSetup(char *setupline)
{
	int	 ccnt;
	char *commas[5], *cp;

	/* Determin the opcode: */
	if (!strncmp(setupline,"RRQ,",4))
		test_Opcode = TFTP_RRQ;
	else if (!strncmp(setupline,"WRQ,",4))
		test_Opcode = TFTP_WRQ;
	else if (!strncmp(setupline,"ACK,",4))
		test_Opcode = TFTP_ACK;
	else if (!strncmp(setupline,"DAT,",4))
		test_Opcode = TFTP_DAT;
	else if (!strncmp(setupline,"ERR,",4))
		test_Opcode = TFTP_ERR;
	else {
		fprintf(stderr,"Bad opcode.\n");
		return(-1);
	}

	ccnt = 0;
	cp = setupline;
	while(*cp) {
		if (*cp == ',')
			commas[ccnt++] = cp;
		cp++;
	}
	if ((ccnt < 2) || (ccnt > 4)) {
		fprintf(stderr,"Bad comma count.\n");
		return(-1);
	}
	
	test_OpcodePass = atoi(commas[0] + 1);
	if (test_OpcodePass <= 0) {
		fprintf(stderr,"Bad opcode pass (out of range).\n");
		return(-1);
	}
	if (((test_Opcode == TFTP_RRQ) || (test_Opcode == TFTP_WRQ)) &&
		(test_OpcodePass != 1)) {
		fprintf(stderr,"Bad opcode pass (NA for opcode).\n");
		return(-1);
	}

	if (!strncmp(commas[1]+1,"SLEEP,",6))  {
		test_Type = TFTPTEST_SLEEP;
		test_SleepTime = atoi(commas[2]+1);
		if (test_SleepTime <= 0) {
			fprintf(stderr,"Bad sleep time.\n");
			return(-1);
		}
	}
	else if (!strncmp(commas[1]+1,"BBNO",4))  {
		test_Type = TFTPTEST_BBNO;
	}
	else if (!strncmp(commas[1]+1,"QUIT",4))  {
		test_Type = TFTPTEST_QUIT;
	}
	else if (!strncmp(commas[1]+1,"CORRUPT,",8)) {
		test_Type = TFTPTEST_CORRUPT;
		test_CorruptByte = atoi(commas[2]+1);
		if (test_CorruptByte <= 0) {
			fprintf(stderr,"Bad byte number.\n");
			return(-1);
		}
	}
	else {
		fprintf(stderr,"Bad testtype.\n");
		return(-1);
	}
	
	return(0);
}
