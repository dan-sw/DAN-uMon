/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

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
#include "dancli.h"


/**********************************************************************\
				Common helper functions & data for DANCLI
\**********************************************************************/

#define CHECKERROR(cond,printfparams)	if (cond) { printf printfparams; return; }

int dancli_GetEnvInt(char * name)
{
	char * str = getenv(name);
	if (!str) {
		printf ("Environment variable %s not found", name);
		return -1;
	}
	return (int)strtol(str,0,0);
}


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
