/* Copyright 2013, Qualcomm Atheros, Inc. */
/**********************************************************************\
 Library     :  uMON (Micro Monitor)
 Filename    :  trc.c
 Purpose     :  Trace functions and macros for uMON
 Author      :  Sergey Krasnitsky
 Created     :  27.6.2012
\**********************************************************************/

#include "config.h"
#include "genlib.h"
#include "common.h"
#include "cpu.h"
#include "trc.h"


#ifdef TRACE_ENABLED

// printf/puts macro was defined in config.h
// We want call the real functions from this file
#undef printf
#undef puts


TRC_HDR*	trcHdr;			// Tracer Header: points to start of tracer buffer
TRC_ELEM*	trcFifo;		// Tracer FIFO buffer: follows *trcHdr
int			trcNesting;     // Functions nesting counter


// Copy string discarding all '\n'. Add the termination '\0' if truncated
static inline int strcpy_non(char* dst, char * src, int maxlen)
{
	int len = 0;
	while (1) {
		if (*src != '\n'  &&  *src != '\r') {
			if (len == maxlen-1) {
				dst[len++] = '\0';
				break;
			}
			dst[len++] = *src;
			if (!*src)
				break;
		}
		src++;
	}
	return len;
}

void trcInit (uint32 base, uint32 size)
{
	unsigned fifosize;
	char     str[16];

	if (base) {
		sprintf(str,"%d",base);
		setenv ("UMON_TRACER_ADDR", str);
	}
	if (size) {
		sprintf(str,"%d",size);
		setenv ("UMON_TRACER_SIZE", str);
	}

	base = getenv_int_def ("UMON_TRACER_ADDR", UMON_TRACER_ADDR);
	size = getenv_int_def ("UMON_TRACER_SIZE", UMON_TRACER_SIZE);

	trcHdr   = (TRC_HDR*) base;
	fifosize = (size - sizeof(TRC_HDR)) / sizeof(TRC_ELEM);

	if (!trcHdr || size <= sizeof(TRC_HDR) || !fifosize) {
		puts ("Error initializing trcInit");
		return;
	}

	memset ((void*)trcHdr, 0, size);

	trcFifo = (TRC_ELEM*) (trcHdr + 1);	// next after trcHdr[0]

	trcHdr->FifoSize = fifosize;
	trcHdr->Magic	 = TRC_MAGIC;
}


int trcPrint (char* sformat, ...)
{
	TRC_ELEM * newelem = trcFetchNextElem();

	va_list  argptr;
	va_start (argptr, sformat);

	vsnprintf (newelem->sbuf, TRC_MAXSTRLEN-1, sformat, argptr);
	trcTraces_(newelem->sbuf, newelem);

	return printf (newelem->sbuf);
}


void trcPuts (char* str)
{
	trcTraces (str);
	puts      (str);
}


char* trcFormatLine (TRC_ELEM* te)
{
	// *******************************************************************************************************
	// Static data for formating
	#define TRC_INFO_TEMPL			"|%08X|%8X|"	// Template for using by modules performing external formating
	#define TRC_INFO_TEMPL_LEN      19				// Length of formated TRC_INFO_TEMPL string without terminal 0
	#define TRC_ARROW_LEN           16   
	#define TRC_ARROW_START			(TrcArrowStart+ TRC_ARROW_LEN)
	#define TRC_ARROW_END			(TrcArrowEnd  + TRC_ARROW_LEN)
	#define TRC_BLANK				(TrcBlank     + TRC_ARROW_LEN)

	static char TrcBlank	  [TRC_ARROW_LEN+3] = "                ";
	static char TrcArrowStart [TRC_ARROW_LEN+3] = "--------------->";
	static char TrcArrowEnd   [TRC_ARROW_LEN+3] = "<---------------";
	// *******************************************************************************************************

	static char strbuf [TRC_INFO_TEMPL_LEN + TRC_ARROW_LEN + TRC_MAXSTRLEN + 5];
	
	char * snest;

	uint32 flags = te->nest;
	uint16 nest  = (uint16) te->nest;

	if (flags & TRC_FLG_FUNCS) {
        snest = TRC_ARROW_START - nest;
	}
	else if (flags & TRC_FLG_FUNCE) {
        * (TRC_ARROW_END - nest) = '<';
        snest = TRC_ARROW_END - nest;
	}
	else {
        snest = TRC_BLANK - nest;
	}

	sprintf (strbuf, TRC_INFO_TEMPL, te->time, te->nseq);
	memcpy  (strbuf + TRC_INFO_TEMPL_LEN, snest, nest);

	char str2[TRC_MAXSTRLEN];
	strcpy_non(str2, te->str, TRC_MAXSTRLEN);	// copy te->str to str2, terminating on '\n'

	if (flags & TRC_FLG_FORMATTED)
		strncpy (strbuf + TRC_INFO_TEMPL_LEN + nest, str2, TRC_MAXSTRLEN);
	else
		snprintf (strbuf + TRC_INFO_TEMPL_LEN + nest, TRC_MAXSTRLEN, str2, te->par0);

	return strbuf;
}


void trcShow (uint32 base, int nlines)
{
	TRC_HDR *	thdr;
	TRC_ELEM*	tfifo, *te;
	int			i, ind, ifirst;


	if (base) {
		thdr = (TRC_HDR*) base;
	}
	else {
		if (!trcHdr) {
			puts("Tracer was not initiated");
			return;
		}
		thdr = trcHdr;
	}

	tfifo = (TRC_ELEM*) (thdr + 1);

	if (thdr->Magic != TRC_MAGIC) {
		puts ("ERROR: Tracer buffer magic number doesn't match");
		return;
	}

    if (nlines && nlines<thdr->FifoSize) {
		ifirst = thdr->FifoSize - nlines;
	}
	else {
		ifirst = 0;
	}

	printf ("Tracer buffer: 0x%X elements @ 0x%X (next=0x%X)\n\n", thdr->FifoSize, thdr, thdr->NextIdx);
	if (ifirst)
		printf ("Show last %d lines\n\n", nlines);
	puts ("  Time      Sqn#        Message\n");

	for (i = 0, ind = thdr->NextIdx /* next may be the first */;   i < thdr->FifoSize;   i++)
    {
        te = &tfifo[ind];
		
        if (!te->str /* empty msg, the buffer is not full */ || i < ifirst /* user wants only last nlines */) {
            goto next_ind;
        }

		// For debug:
		if (!te->time) {
			printf ("shady: i = %d, ind = %d, te->time = %x, te->nseq = %x\n", i, ind, te->time, te->nseq);
		}

        puts (trcFormatLine(te));
		next_ind:
        ind = CYCLIC_INC (ind, thdr->FifoSize);
    }
}

#endif // TRACE_ENABLED
