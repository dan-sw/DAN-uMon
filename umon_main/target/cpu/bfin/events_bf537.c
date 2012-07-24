/* events_bf537.c:
 *	This code handles events that are caught by the event vectors
 *	that have been installed by the monitor through vinit().
 *	Note that in some places the term "exception" is used when "event"
 *	is what we really mean.  This is because in general, uMon refers to
 * 	any type of interrupt as an "exception".
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "warmstart.h"
#include "cdefBF537.h"

int		ExceptionType;
ulong	ExceptionAddr;
extern	ulong getRETE(void);
extern	ulong getRETN(void);
extern	ulong getRETX(void);
extern	ulong getRETI(void);

void
event(int type)
{
	ExceptionType = type;
	switch(type) {
		case 0:
			ExceptionAddr = getRETE();
			break;
		case 2:
			ExceptionAddr = getRETN();
			break;
		case 3:
			ExceptionAddr = getRETX();
			break;
		default:
			ExceptionAddr = getRETI();
			break;
	}
			
	flushRegtbl();
	flushDcache((char *)&ExceptionType,sizeof(int));
	flushDcache((char *)&ExceptionAddr,sizeof(ulong));
	monrestart(EXCEPTION);
}

void
vinit(void)
{
	extern void evt0(), evt1(), evt2(), evt3();
	extern void evt4(), evt5(), evt6(), evt7();
	extern void evt8(), evt9(), evt10(), evt11();
	extern void evt12(), evt13(), evt14(), evt15();

	*pEVT0 = evt0;		/* Emulation	(reg=RETE, instr=RTE) */
	*pEVT1 = evt1;		/* Reset		(reg=RETI, instr=RTI) */
	*pEVT2 = evt2;		/* NMI			(reg=RETN, instr RTN) */
	*pEVT3 = evt3;		/* Exception	(reg=RETX, instr RTX) */
	*pEVT4 = evt4;		/* reserved							  */
	*pEVT5 = evt5;		/* Hardware err	(reg=RETI, instr RTI) */
	*pEVT6 = evt6;		/* Core timer	(reg=RETI, instr RTI) */
	*pEVT7 = evt7;		/* Interrupt7	(reg=RETI, instr RTI) */
	*pEVT8 = evt8;		/* Interrupt8	(reg=RETI, instr RTI) */
	*pEVT9 = evt9;		/* Interrupt9	(reg=RETI, instr RTI) */
	*pEVT10 = evt10;	/* Interrupt10	(reg=RETI, instr RTI) */
	*pEVT11 = evt11;	/* Interrupt11	(reg=RETI, instr RTI) */
	*pEVT12 = evt12;	/* Interrupt12	(reg=RETI, instr RTI) */
	*pEVT13 = evt13;	/* Interrupt13	(reg=RETI, instr RTI) */
	*pEVT14 = evt14;	/* Interrupt14	(reg=RETI, instr RTI) */
	*pEVT15 = evt15;	/* Interrupt15	(reg=RETI, instr RTI) */
}

char *etypes[] = {
	"Emulation", "Reset", "NMI", "Exception",
	"Reserved", "Hardware Error", "Core Timer", "Interrupt 7",
	"Interrupt 8", "Interrupt 9", "Interrupt 10", "Interrupt 11",
	"Interrupt 12", "Interrupt 13", "Interrupt 14", "Interrupt 15",
};

char *
ExceptionCause(int cause)
{
	switch(cause) {
		case 0x25:
			return("Unrecoverable event");
		case 0x2d:
			return("I-fetch multiple CPLB hits");
		case 0x2a:
			return("I-fetch misaligned access");
		case 0x2b:
			return("I-fetch protection violation");
		case 0x2c:
			return("I-fetch CPLB miss");
		case 0x29:
			return("I-fetch access exception");
		case 0x28:
			return("Watchpoint match");
		case 0x21:
			return("Undefined instruction");
		case 0x2e:
			return("Illegal use of protected resource");
		case 0x27:
			return("DAGN multiple CPLB hits");
		case 0x24:
			return("DAGN misaligned access");
		case 0x23:
			return("DAGN protection violation");
		case 0x26:
			return("DAGN CPLB miss");
		case 0x10:
			return("blah");
		case 0x11:
			return("Single step");
		case 0x22:
			return("Trace buffer");
	}
	if ((cause >= 0) && (cause <= 15))
		return("Excpt instruction");

	return("exception cause unrecognized");
}

char *
ExceptionType2String(int type)
{
	char	*string;
	static char	buf[64];

	if (type == 3) {
		unsigned long seqstat;

		getreg("SEQSTAT",&seqstat);
		sprintf(buf,"Exception (%s)",ExceptionCause(seqstat & 0x3f));
		string = buf;
		return(string);
	}

	if ((type >= 0) && (type <= 15))
		return(etypes[type]);
	
	sprintf(buf,"Event type %d",type);
	string = buf;
	return(string);
}
