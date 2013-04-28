/* Copyright 2013, Qualcomm Atheros, Inc. */
/**********************************************************************\
 Library     :  uMON (Micro Monitor)
 Filename    :  trc.h
 Purpose     :  Trace functions and macros
 Author      :  Sergey Krasnitsky
 Created     :  27.6.2012
\**********************************************************************/

#ifndef _TRC_H
#define _TRC_H


#ifdef 	TRACE_ENABLED

// Define here the default tracer buffer in the TX CB (1.5Mb)
// The first trcInit call done at the init stage before monrc has started. 
// So, in order to use tracer during initialization we should provide the default buffer. 
// If trcInit will be called again, e.g. from the CLI, it may use the environment variables.
#define UMON_TRACER_ADDR	 0x40400000
#define UMON_TRACER_SIZE	    0xEF000	// 0x180000, - the end of 1st Mb in TX CB contains some data: params & debugger 32-bytes flags 


#define TRC_MAGIC			 0x10081971
#define TRC_MAXSTRLEN     			108   	// Max tracer string length after formating
#define TRC_FLG_FORMATTED		0x10000		// Flags: the string in TRC_ELEM already formatted
#define TRC_FLG_FUNCS			0x20000		// Flags: function start
#define TRC_FLG_FUNCE			0x40000		// Flags: function end


void trcInit	(uint32 base, uint32 size);
void trcShow	(uint32 base, int nlines);
int  trcPrint	(char* sformat, ...);
void trcPuts	(char* str);

// The following funcs are inlines. Defined as a static inline.
// void trcTrace	(char* sformat, int arg);
// void trcTraces 	(char* str);
// void trcFuncs	(char* func);
// void trcFunce	(char* func);


/*
 * Header of the tracer buffer. 
 */
typedef struct {
	uint32	Magic;					// 0x10081971
	uint32	FifoSize;				// Number of elements in the following TRC_ELEM array
	uint32	NextSqnNum;				// Sqn number that will be assigned to the next element
	uint32	NextIdx;				// Index of the next free TRC_ELEM element in the FIFO
} TRC_HDR;


/*
 * Tracing element. There is a cyclic buffer of such elements.
 * Note: the TRC_MAXSTRLEN is chosen in such way that sizeof(TRC_ELEM) == 128
 */
typedef struct
{
    uint32	time;       			// Trace timestamp
    uint32	nseq;       			// Global sequence number
    uint32	nest;       			// Nesting counter (16bit lsb) and flags (msb)
	char*	str;					// Constant trace string / format string / func name (may point to sbuf)
	uint32	par0;					// Parameter 0 (optional)
	char	sbuf[TRC_MAXSTRLEN];	// String may be copied here
} TRC_ELEM;


/*******************************************************************************************\
									Data declaration
\*******************************************************************************************/

extern TRC_HDR*		trcHdr;			// Tracer Header: points to start of tracer buffer
extern TRC_ELEM*	trcFifo;		// Tracer FIFO buffer: follows *trcHdr
extern int			trcNesting;     // Functions nesting counter


/*******************************************************************************************\
									Trace macros
\*******************************************************************************************/

#define TRCINIT()			trcInit(0,0)
#define TRCPUTS(str)		trcPuts(str)
#define TRCPRN(...)			trcPrint(__VA_ARGS__)
#define TRCERR(...)			trcPrint(__VA_ARGS__)
#define TRCSTR(str)			trcTraces(str)
#define TRCTRC(str,arg)		trcTrace(str, (uint32)arg)
#define TRCFUNS()          	trcFuncs((char*)__FUNCTION__)
#define TRCFUNE()          	trcFunce((char*)__FUNCTION__)



/*******************************************************************************************\
										Inline functions
\*******************************************************************************************/

#include "cpu.h"

inline static TRC_ELEM * trcFetchNextElem()
{
	TRC_ELEM * ret = &trcFifo[trcHdr->NextIdx];
	trcHdr->NextIdx = CYCLIC_INC(trcHdr->NextIdx, trcHdr->FifoSize);
	return ret;
}


inline static void trcTraces_ (char* str, TRC_ELEM * newelem)
{
	newelem->time = CPU_GetCurrentClockCount();
	newelem->nest = trcNesting | TRC_FLG_FORMATTED;
	newelem->nseq = trcHdr->NextSqnNum++;
	newelem->str  = str;
}


inline static void trcTraces (char* str)
{
	trcTraces_ (str, trcFetchNextElem());
}


inline static void trcTrace (char* sformat, int arg)
{
	TRC_ELEM * newelem = trcFetchNextElem();

	newelem->time = CPU_GetCurrentClockCount();
	newelem->nest = trcNesting;
	newelem->nseq = trcHdr->NextSqnNum++;
	newelem->str  = sformat;
	newelem->par0 = arg;
}


inline static void trcFuncs (char* func)
{
	TRC_ELEM * newelem = trcFetchNextElem();

	newelem->time = CPU_GetCurrentClockCount();
	newelem->nest = ++trcNesting | TRC_FLG_FUNCS | TRC_FLG_FORMATTED;
	newelem->nseq = trcHdr->NextSqnNum++;
	newelem->str  = func;
}


inline static void trcFunce (char* func)
{
	TRC_ELEM * newelem = trcFetchNextElem();

	newelem->time = CPU_GetCurrentClockCount();
	newelem->nest = trcNesting-- | TRC_FLG_FUNCE | TRC_FLG_FORMATTED;
	newelem->nseq = trcHdr->NextSqnNum++;
	newelem->str  = func;
}



#else	// TRACE_ENABLED

#define TRCINIT()
#define TRCPUTS(str)	puts(str)
#define TRCPRN(...)		
#define TRCERR(...)		printf(__VA_ARGS__)
#define TRCSTR(str)
#define TRCTRC(...)
#define TRCFUNS()
#define TRCFUNE()

#endif // TRACE_ENABLED



#endif // _TRC_H
