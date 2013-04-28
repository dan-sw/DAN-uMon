/* Copyright 2013, Qualcomm Atheros, Inc. */
/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/******************************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  dancli.h
 Purpose    :  CLI functions for DAN3400 micro-applications infrastructure 
 Owner		:  Sergey Krasnitsky
 Created    :  1.11.2011
 Note		:  This module data structure/addresses may depend on DAN3400 
               micro-apps. Modify it carefully.
\******************************************************************************/

#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "common.h"
#include "cli.h"
#include "trc.h"
#include "tmr.h"
#include "dancli.h"


/**********************************************************************\
				Common helper functions & data for DANCLI
	Note: all types & defines are copied from API\I_node_cfg.h
\**********************************************************************/

#define NODE_NAME_LEN			 8		// For NODE_CFG_t
#define NODE_ASSERT_FNAME_LEN	32		// For NODE_ASSERT_t
#define NODE_ASSERT_FUNC_LEN	32		// For NODE_ASSERT_t
#define NODE_TRACEBACK_MAX		16		// For NODE_TRACEBACK_t

/*
 * All Node IDs (logical).
 * Note: taken from API\I_node_cfg.h
 */
typedef enum
{
	 NODE_NONE	   = -1,
	 NODE_RX_CPU0  =  0,
	 NODE_RX_CPU1  =  1,
	 NODE_RX_DSP0  =  2,
	 NODE_RX_DSP1  =  3,
	 NODE_RX_DSP2  =  4,
	 NODE_RX_DSP3  =  5,
	 NODE_TX_CPU0  =  6,
	 NODE_TX_CPU1  =  7,
	 NODE_TX_CPU2  =  8,
	 NODE_TX_CPU3  =  9,
	 NODE_TX_DSP0  = 10,
	 NODE_TX_DSP1  = 11,
	 NODE_NPU_CPU0 = 12,
	 NODE_NPU_CPU1 = 13,
	 NODE_NPU_CPU2 = 14,
	 NODE_NPU_CPU3 = 15,
	 NODE_NPU_CPU4 = 16,
	 NODE_NPU_CPU5 = 17,
	 NODE_NPU_ARM0 = 18,
	 NODE_NPU_ARM1 = 19,
	 NODE_NPU_ARM2 = 20,
	 NODE_NPU_ARM3 = 21,

	 NODE_NUMNODES = 21,
	 NODE_NUM_ALL_NODES = 22
} NODE_ID_t;

/*
 * Node Startup stages (corresponds to PLATFROM_e_startup_stage)
 */
typedef enum
{
	NODE_STAGE_IDLE,
	NODE_STAGE_PRE_INIT,
	NODE_STAGE_INIT,
	NODE_STAGE_POST_INIT,
	NODE_STAGE_CUST,
	NODE_STAGE_PRE_START,
	NODE_STAGE_START,
	NODE_STAGE_POST_START,
	NODE_STAGE_POST_RUN,
	NODE_NUM_STAGES
} NODE_STAGE_t;


typedef struct 
{
	UINT32	line;
	char	file[NODE_ASSERT_FNAME_LEN];
	char	func[NODE_ASSERT_FUNC_LEN];
} NODE_ASSERT;


typedef struct
{
	UINT32	cause;
	UINT32	epc1;
	UINT32	addr;
} NODE_EXC;


typedef struct
{
	UINT32	addreses[NODE_TRACEBACK_MAX];
} NODE_TRACEBACK;


/*
 * Debug Streams Node context. 
 * Should be placed in local fast memory.
 */
typedef struct {
	volatile UINT32	 SqnNum;					// DS message sequence number 
	UINT32			 FifoAddr;					// FIFO buffer, actually DSMSG*
	UINT32			 FifoSize;					// FIFO buffer size (number of DSMSG)
	volatile UINT32	 FifoIn;					// FIFO write index
	volatile UINT32	 FifoOut;					// FIFO read index
	volatile UINT32  Lost;						// Number of lost log messages
} NODE_DS_CNXT;


/* 
 * Node status flags
 */
enum {
	NODE_STATUS_ASSERT		= 0x00001,
	NODE_STATUS_EXCEPTION	= 0x00002,
};


typedef struct {
	char				Node[NODE_NAME_LEN];	// Node name
	UINT32				NodeId;					// Logical Node ID
	UINT32				Stage;					// Node stage
	UINT32				Status;					// Status flags bitmask
	UINT32				Timestamp;				// Timestamp of last crash (Assert or Exception)
	NODE_DS_CNXT		DsContext;				// DS context
	NODE_ASSERT			Assert;					// Last assert information
	NODE_EXC			Exception;				// Last exception information
	NODE_TRACEBACK		Traceback;				// Last traceback information
	void*				ipc_cfg_p;
	void*				osa_cfg_p;
	void*				GMAC_cfg_p;
} NODE_CFG_t;



#define CHECKERROR(cond,printfparams)	if (cond) { printf printfparams; return; }



/**********************************************************************\
						CLI command "checkcrc"
\**********************************************************************/

int dancli_Checkcrc(int argc, char *argv[])
{
	signed char		opt;
	bool			dump = false;
	uchar		   *buffer;
	ulong			nbytes;
	ulong			crc1, crc2;
	char		   *node, *segnum;

	while ((opt = getopt(argc,argv,"d")) != -1) {
		switch(opt) {
		case 'd':	dump = true;	break;
		default:
			return CMD_PARAM_ERROR;
		}
	}

	if (argc < optind+2)
		return CMD_PARAM_ERROR;

	buffer = (uchar*) strtoul(argv[optind],0,0);
	nbytes = strtoul(argv[optind+1],0,0);
	crc1   = strtoul(argv[optind+2],0,0);
	node   = (argc >= optind+3) ? argv[optind+3] : 0;
	segnum = (argc >= optind+4) ? argv[optind+4] : 0;

	// Check crc32 for the uncached memory (bit #31 is ON)
	// crc2 = crc32((uchar*)((unsigned)buffer|(1<<31)), nbytes);
	crc2 = crc32(buffer, nbytes);

	if (crc1 != crc2) {
		if (node) {
			printf (node);
			if (segnum) {
				printf (" seg#");
				printf (segnum);
			}
			printf (" : ");
		}
		printf ("CRC32 ERROR @ 0x%08X (0x%X bytes) = 0x%X instead 0x%X\n", buffer, nbytes, crc2, crc1);
	}

	return 0;
}


char *dancli_Checkcrc_help[] = {
    "Check CRC32 for a block of memory",
    "-[d] addr size crc32 [nodename] [segnum]\n"
	"Options:\n"
	"\t-d\tdump memory if CRC32 is incorrect\n",
    0,
};


/**********************************************************************\
						CLI command "ds"
	Note: data structures & code duplicate DS_CLN & DS_SVR modules
\**********************************************************************/

#define DS_SRV_MAX_LOG_LINE_LEN		   120
#define DS_SRV_LINE_PREFIX_LEN			35
#define DS_SRV_NODE_NAME_LEN			 7
#define DS_SRV_TIMESTMP_LEN				 8
#define DS_SRV_NAME_LEN					 5
#define DS_SRV_MAX_LINE_LEN				 (DS_SRV_LINE_PREFIX_LEN + DS_SRV_MAX_LOG_LINE_LEN)



/*
 * DS message structure. 
 * It limits number of print arguments for all commands up to 4 (param0..param3).
 * Note: this is internal DS structure, should be used only by DS macro & funcs
 */
#if 0
typedef struct
{
    UINT32       time;       // Trace time
    UINT32       nseq;       // Global sequence number
    UINT32       taskid;     // Task ID
    UINT8        level;      // Severity level >=DSMSG_ERROR
    UINT8        flags;      // Additional flags
    UINT16       nest;       // Nesting counter
    cchar      * dsname;     // Debug Stream Name
    cchar      * sfile;      // Source file (mostly for errors)
    UINT32       sline;      // Source line (mostly for errors)
    cchar      * func;       // Source function name
    cchar      * str;        // Print/format string
    UINT32       param0;     // Parameter 0 (optional)
    UINT32       param1;     // Parameter 1 (optional)
    UINT32       param2;     // Parameter 2 (optional)
    UINT32       param3;     // Parameter 3 (optional)
} DSMSG;
#else

#define DS_MAX_LINE_LEN			120
#define DS_NAME_LEN				8
#define DS_FILE_FUNC_LEN		20

typedef struct
{
    UINT32      time;
    UINT32      nseq;
    UINT32      taskid;
    UINT8       level;
    UINT8       flags;
    UINT16      nest;
    UINT32      sline;
    char        dsname	[DS_NAME_LEN	  + 1];
    char        sfile	[DS_FILE_FUNC_LEN + 1];
    char		func	[DS_FILE_FUNC_LEN + 1];
    char		str		[DS_MAX_LINE_LEN  + 1];
} DSMSG;
#endif




typedef struct {
	char*				 Node;
	volatile NODE_CFG_t* CnxtAddr;
} DSSRV_ELEM;


/*
 * dsContext address of each Node.
 * It should reference to each node TCM. 
 * Notes: 
 *	- this addresses cannot be changed and their local values set in memmap.xmm files.
 *	- bit#31 is raised in order to have uncached access
 */
DSSRV_ELEM  dssrvNodeContexts [NODE_NUMNODES] = 
{
	{ "RX_CPU0", (NODE_CFG_t*) (0x54117E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "RX_CPU1", (NODE_CFG_t*) (0x54317E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "RX_DSP0", (NODE_CFG_t*) (0x5442FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "RX_DSP1", (NODE_CFG_t*) (0x5452FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "RX_DSP2", (NODE_CFG_t*) (0x5462FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "RX_DSP3", (NODE_CFG_t*) (0x5472FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "TX_CPU0", (NODE_CFG_t*) (0x44017E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "TX_CPU1", (NODE_CFG_t*) (0x44117E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "TX_CPU2", (NODE_CFG_t*) (0x44217E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "TX_CPU3", (NODE_CFG_t*) (0x44317E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "TX_DSP0", (NODE_CFG_t*) (0x4442FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "TX_DSP1", (NODE_CFG_t*) (0x4452FE78 | 0x80000000) },  // Inside 128k-CTCM 
	{ "NP_CPU0", (NODE_CFG_t*) (0x64017E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_CPU1", (NODE_CFG_t*) (0x64117E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_CPU2", (NODE_CFG_t*) (0x64217E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_CPU3", (NODE_CFG_t*) (0x64317E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_CPU4", (NODE_CFG_t*) (0x64417E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_CPU5", (NODE_CFG_t*) (0x64517E78 | 0x80000000) },  // Inside  32k-CTCM 
	{ "NP_ARM0", (NODE_CFG_t*) (0x64900014 | 0x80000000) },  // Inside   8k-DTCM 
	{ "NP_ARM1", (NODE_CFG_t*) (0x64B00014 | 0x80000000) },  // Inside   8k-DTCM 
	{ "NP_ARM2", (NODE_CFG_t*) (0x64D00014 | 0x80000000) },  // Inside   8k-DTCM 
};



static int DS_SRV_GetMessage (volatile NODE_DS_CNXT* ds, unsigned *fifo_out, DSMSG* msg)
{
	if (*fifo_out == ds->FifoIn) // condition of empty
		return 0;

	*msg = ((volatile DSMSG*)(ds->FifoAddr|0x80000000)) [*fifo_out];	// get from uncached FifoAddr

	*fifo_out = CYCLIC_INC (*fifo_out, ds->FifoSize);
	return 1;
}

// Copy string discarding all '\n'
// Not to add the termination '\0'
static inline int strcpy_non(char* dst, cchar * src, int maxlen, BOOL fill_spaces)
{
	int len = 0;
	while (1) {
		if (!*src) {
			if (fill_spaces) {
				for ( ; len < maxlen; len++)
					dst[len] = ' ';
			}
			break;
		}

		if (*src != '\n'  &&  *src != '\r') {
			if (len == maxlen)
				break;
			dst[len++] = *src;
		}
		src++;
	}
	return len;
}


void DS_SRV_Print2Uart (cchar* node, DSMSG* msg)
{
	char  strbuf[DS_SRV_MAX_LINE_LEN + 1];
	int   len = 0;

	char* dsname = (char*) msg->dsname;
	if (!dsname || (unsigned)dsname>>24 == 0x5f) {
		// if dsname = NULL or points to some unreachable TCM 
		dsname = "";
	}

	strbuf[len++] = '>';
	strbuf[len++] = '>';
	strbuf[len++] = ' ';
	memcpy (&strbuf[len], (char*)node, DS_SRV_NODE_NAME_LEN);		len += DS_SRV_NODE_NAME_LEN;
	strbuf[len++] = ' ';
	strbuf[len++] = '>';
	strbuf[len++] = '>';
	strbuf[len++] = ' ';
	sprintf(&strbuf[len],"%08x",msg->time);							len += DS_SRV_TIMESTMP_LEN;
	strbuf[len++] = ' ';
	strbuf[len++] = '>';
	strbuf[len++] = '>';
	strbuf[len++] = ' ';
	len += strcpy_non(&strbuf[len], dsname, DS_SRV_NAME_LEN, TRUE);
	strbuf[len++] = ' ';
	strbuf[len++] = '>';
	strbuf[len++] = '>';
	strbuf[len++] = ' ';

	if (msg->str) {
		len += strcpy_non(&strbuf[len], msg->str, DS_SRV_MAX_LOG_LINE_LEN, FALSE);
	}

	strbuf[len] = '\0';
	puts (strbuf);
}


int dancli_Ds(int argc, char *argv[])
{
	signed char		opt, cmd = -1;
	int				i;
	int				n1 = 0,  n2 = NODE_NUMNODES-1;		// def range: all nodes
	DSMSG			msg;

	while ((opt = getopt(argc,argv,"spcn:")) != -1)
	{
		switch(opt) {
			case 's':
			case 'p':
			case 'c':	cmd = opt;	
						break;
			case 'n':
						for (i=0; i<NODE_NUMNODES; i++) {
							if (strcmp(optarg,(char*)dssrvNodeContexts[i].Node) == 0) {
								n1 = n2 = i;
								break;
							}
						}
						break;
			default:
				return CMD_PARAM_ERROR;
		}
	}


	if (cmd == -1)
		cmd = 's';	// default 


	switch (cmd)
	{
		case 's':
			printf ("DS FIFOs:\n");
			for (i=n1; i<=n2; i++) {
				DSSRV_ELEM*				node = &dssrvNodeContexts[i];
				volatile NODE_DS_CNXT*	ds   = &node->CnxtAddr->DsContext;

				printf ("\n  %s:\n"
						"   SqnNum\t=%8x\n"
						"   FifoAddr\t=%8x\n"
						"   FifoSize\t=%8x\n"
						"   FifoIn\t=%8x\n"
						"   FifoOut\t=%8x\n",
						node->Node, ds->SqnNum, ds->FifoAddr, ds->FifoSize, ds->FifoIn, ds->FifoOut);
			}
			break;

		case 'c':
			for (i=n1; i<=n2; i++)
				memset_4aligned ((int*) &dssrvNodeContexts[i].CnxtAddr->DsContext, sizeof(NODE_DS_CNXT));
			break;

		case 'p':
			for (i=n1; i<=n2; i++) {
				DSSRV_ELEM* node = &dssrvNodeContexts[i];
				unsigned	fifo_out = 0;	// KS 30.04.2012  Temporary assign fifo_out to 0, change this to find it according to sqn number
				while (DS_SRV_GetMessage (&node->CnxtAddr->DsContext, &fifo_out, &msg)) {
					DS_SRV_Print2Uart (node->Node, &msg);
				}
			}
			break;

		default:
			return CMD_PARAM_ERROR;
	}

	return 0;
}


char *dancli_Ds_help[] = {
    "Show status of DS FIFOs",
    "-[scpn:]\n"
	"Options:\n"
	"\t-s\tshow all nodes DS contexts (default)\n"
	"\t-c\tclear all nodes DS contexts\n"
	"\t-p\tformat and print pending DS strings\n",
	"\t-nNODE\tfilter NODE-relevant information only\n" ,
    0,
};


/**********************************************************************\
						CLI command "trc"
		tracer (trc) services: options, prints, etc.
\**********************************************************************/

#ifdef TRACE_ENABLED

int dancli_Trc(int argc, char *argv[])
{
	signed char	opt;
	unsigned	base = 0, size = 0;
	char		*cmd;
	int			par;

	while ((opt = getopt(argc,argv,"b:s:")) != -1)
	{
		switch(opt) {
		case 'b':  base = strtoul(optarg,0,0);	break;
		case 's':  size = strtoul(optarg,0,0);	break;
		default:
			return CMD_PARAM_ERROR;
		}
	}

	if (argc == optind)
		return CMD_PARAM_ERROR;

	cmd = argv[optind++];
	par = (optind < argc) ? strtoul(argv[optind],0,0) : 0;

	switch (cmd[0])	// cmd == 'set' or 'show'
	{
	case 'i': // init
		printf ("INIT TRACER:");
		if (base)
			printf (" Base=0x%X", base);
		if (size)
			printf (" Size=0x%X", size);
		puts ("");
		trcInit(base, size);
		break;

	case 's': // show
		trcShow (base, par);
		break;

	default:
		return CMD_PARAM_ERROR;
	}

	return 0;
}


char *dancli_Trc_help[] = {
	"Init and Show uMon's Tracer information",
	"[-b<Addr>] [-s<Size>] init | show [nlines]\n"
	"init - init TRC module, set Addr/Size pair as new defaults for the tracer FIFO\n"
	"show - print trace information to console, if nlines set - show only n last lines\n"
	"Options:\n"
	"\t-b<Addr>\tFIFO buffer base address\n"
	"\t-s<Size>\tFIFO buffer size\n",
	0,
};

#endif // TRACE_ENABLED



/**********************************************************************\
						CLI command "timer"
				 May be used from uMon scripts.
\**********************************************************************/

int dancli_Timer (int argc, char *argv[])
{
	signed char	opt;
	unsigned	secs = 5;
	char		*cmd;

	while ((opt = getopt(argc,argv,"i:")) != -1)
	{
		switch(opt) {
		case 'i':  secs = strtoul(optarg,0,0);	break;
		default:
			return CMD_PARAM_ERROR;
		}
	}

	if (argc == optind)
		return CMD_PARAM_ERROR;

	cmd = argv[optind];

	switch (cmd[2])	// cmd == 'start' or 'stop' or 'pinch'
	{
		case 'a': TMR_Start(secs);	break;
		case 'o': TMR_Start(0);		break;
		case 'n': TMR_Pinch();		break;
		default:	return CMD_PARAM_ERROR;
	}

	return 0;
}


char *dancli_Timer_help[] = {
	"Init, pinch and show uMon's timer",
	"[-i<num_seconds>]  start | stop | pinch\n"
	"start - start the timer, used together with -i option\n"
	"stop  - stop the timer\n"
	"pinch - pinch the timer, must be called at least once in 14 seconds\n"
	"Options:\n"
	"\t-i<num_seconds>\tinterval in seconds to print the time (5 by default)\n",
	0,
};


/**********************************************************************\
						CLI command "status"
\**********************************************************************/

char* dancli_NODE_STAGE [NODE_NUM_STAGES] =
{
	"IDLE",
	"PRE_INIT",
	"INIT",
	"POST_INIT",
	"CUST",
	"PRE_START",
	"START",
	"POST_START",
	"POST_RUN"
};


typedef enum {
	DANCLI_PRINT_ERRORSONLY,
	DANCLI_PRINT_TABLE,
	DANCLI_PRINT_USUAL,
	DANCLI_PRINT_FULL
} DANCLI_PRINT_STYLE;


static inline bool dancli_IsNodeAlive(NODE_ID_t id, volatile NODE_CFG_t * ncb)
{
	char s[NODE_NAME_LEN];
	memcpy_4aligned((int*)s,(int*)ncb->Node,NODE_NAME_LEN);
	return 	(memcmp (dssrvNodeContexts[id].Node, s, NODE_NAME_LEN) == 0);
}


void dancli_PrintAssertException (volatile NODE_CFG_t * ncb)
{
      int i;

      if (ncb->Status & NODE_STATUS_ASSERT) 
      {
            // Copy file & func to the local memory because of TCM problem
            char  file[NODE_ASSERT_FNAME_LEN];
            char  func[NODE_ASSERT_FUNC_LEN];
            memcpy_4aligned ((int*)file, (int*)ncb->Assert.file, NODE_ASSERT_FNAME_LEN);
            memcpy_4aligned ((int*)func, (int*)ncb->Assert.func, NODE_ASSERT_FUNC_LEN);
            printf ("ASSERT failure [%s:%d]: %s()\n", file, ncb->Assert.line, func);
      }

      if (ncb->Status & NODE_STATUS_EXCEPTION) {
            printf ("EXCEPTION raised: addr=0x%X, epc1=0x%X, cause = %d\n",
            		ncb->Exception.addr, ncb->Exception.epc1, ncb->Exception.cause);
      }

      printf ("TRACEBACK:\n");

      for (i=0; i<NODE_TRACEBACK_MAX; i++)
            if (ncb->Traceback.addreses[i])
                  printf ("\tfunc[%d] = 0x%X\n", i, ncb->Traceback.addreses[i]);
}

bool dancli_StatusPrintNode (NODE_ID_t id, volatile NODE_CFG_t * ncb, DANCLI_PRINT_STYLE style)
{
	bool trash, errors, err_nodid = false, err_stage = false;
	char *stage;

	trash = !dancli_IsNodeAlive(id, ncb);

	if (trash && style < DANCLI_PRINT_FULL) {
		// Trash in the NCB, skip this node
		// Print nothing 
		goto end_func;
	}

	errors = (ncb->Status);

	if (ncb->NodeId != id)
		errors = err_nodid = true;

	if ((unsigned)ncb->Stage >= NODE_NUM_STAGES)
		errors = err_stage = true;

	if (!errors && style == DANCLI_PRINT_ERRORSONLY)
		goto end_func;

	if (style != DANCLI_PRINT_TABLE) {
		printf ("\n%-7s (%d), NCB 0x%X\n", dssrvNodeContexts[id].Node, id, ncb);
		puts ("--------------------------------------------------");
		if (trash) {
			printf ("*** trash ***\n");
			goto end_func;
		}
	}

	if (err_nodid)
		printf ("ERROR: Node ID is incorrect! (%d)\n", ncb->NodeId);

	if (err_stage) {
		printf ("ERROR: Stage is incorrect! (%d)\n", ncb->Stage);
		stage = "";
	}
	else {
		stage = dancli_NODE_STAGE[ncb->Stage];
	}

	if (style == DANCLI_PRINT_TABLE) {
		printf (" %-8s %-10s  ", dssrvNodeContexts[id].Node, stage);

		if (ncb->Status) {
			printf ("0x%08x", ncb->Timestamp);
		}
		else 
			printf ("     -    ");

		if (ncb->Status & NODE_STATUS_EXCEPTION)
			printf ("0x%08X %4d    0x%08X  ", ncb->Exception.addr, (uint8)ncb->Exception.cause, ncb->Exception.epc1);
		else
			printf ("    -         -         -      ");

		if (ncb->Status & NODE_STATUS_ASSERT) {
			// Copy file & func to the local memory because of TCM problem
			char	file[NODE_ASSERT_FNAME_LEN];
			memcpy_4aligned ((int*)file, (int*)ncb->Assert.file, NODE_ASSERT_FNAME_LEN);
			printf ("%s:%d\n", file, ncb->Assert.line); 
		}
		else {
			printf ("\t-\n");
		}
	}
	else {
		if (style != DANCLI_PRINT_ERRORSONLY) {
			#if 0
			int  i;

			printf (
				"%-23s: 0x%X\n"
				"%-23s: %d\n"
				"%-23s: %d\n"
				"%-23s: %s\n"
				"%-23s: %d\n"
				"%-23s: %d\n"
				"%-23s: %d\n"
				"%-23s: %d\n",
				"HW ID",				ncb->coreid, 
				"Node Gateway",			ncb->gw, 
				"Clock",				ncb->system_clk, 
				"Stage",				stage,
				"IPC msg counter",		ncb->ipc_seqnum, 
				"Prints counter",		ncb->log_seqnum, 
				"Debug Level",			ncb->dbg_level, 
				"Number of IPC agents", ncb->num_agents
			);

			for (i = 0; i < ncb->num_agents; i++)
				printf ("IPC agent #0: %s, id=0x%X\n", ncb->agents[i].name, ncb->agents[i].id);
			#endif
		}
		
		dancli_PrintAssertException(ncb);
	}

	end_func:
	return !trash;
}


void dancli_StatusPrint(DANCLI_PRINT_STYLE style)
{
	int i, n;

	if (style == DANCLI_PRINT_TABLE) {
		puts ("---------------------------------------------------------------------------------");
		puts ("|  Node  |  Stage    |   Time   | ExcAddr  | Cause | ExcData  |      Assert     |");
		puts ("---------------------------------------------------------------------------------");
	}

	for (i = n = 0; i < NODE_NUMNODES; i++)	{
		if (dancli_StatusPrintNode((NODE_ID_t)i, dssrvNodeContexts[i].CnxtAddr, style)) n++;
	}

	if (style == DANCLI_PRINT_TABLE && n) {
		puts ("---------------------------------------------------------------------------------");
		printf (" Total running nodes: %d\n", n);
		puts ("---------------------------------------------------------------------------------");
		puts ("");
	}
}


int dancli_Status(int argc, char *argv[])
{
	signed char			opt;
	DANCLI_PRINT_STYLE	style = DANCLI_PRINT_TABLE; // default
	int					i;

	while ((opt = getopt(argc,argv,"etufc")) != -1) {
		switch(opt) {
		case 'e':	style = DANCLI_PRINT_ERRORSONLY; break;
		case 't':	style = DANCLI_PRINT_TABLE;		 break;
		case 'u':	style = DANCLI_PRINT_USUAL;		 break;
		case 'f':	style = DANCLI_PRINT_FULL;		 break;

		case 'c':	// clear may be not set together with other options
			for (i = 0; i < NODE_NUMNODES; i++)
					memset_4aligned((int*)dssrvNodeContexts[i].CnxtAddr, sizeof(NODE_CFG_t));
			return 0;

		default:
			return CMD_PARAM_ERROR;
		}
	}

	dancli_StatusPrint(style);
	return 0;
}


char *dancli_Status_help[] = {
    "Print information from Nodes NCB memory",
    "[etufc]\n\n"
	"Options:\n"
	"\t-e\tprint errors only\n"
	"\t-t\tprint as a table (default)\n"
	"\t-u\tusual print: full information for running nodes only\n"
	"\t-f\tprint full information for all nodes\n"
	"\t-c\tclear the all nodes NCBs including DS contexts\n",
    0,
};
