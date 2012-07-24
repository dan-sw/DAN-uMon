/*
 *
 * EE.C         Expression Evaluator
 *				I got this from SIMTEL...
 *				http://www.simtel.net/pub/simtelnet/msdos/c/c-eval.zip
 *				Modified for use in monitor.
 *
 * AUTHOR:      Mark Morley
 * COPYRIGHT:   (c) 1992 by Mark Morley
 * DATE:        December 1991
 * HISTORY:     Jan 1992 - Made it squash all command line arguments
 *                         into one big long string.
 *                       - It now can set/get VMS symbols as if they
 *                         were variables.
 *                       - Changed max variable name length from 5 to 15
 *              Jun 1992 - Updated comments and docs
 *              May 1995 - ELS additions:
 *				* changed global naming a bit.
 *				* added hex (0x) number type to accept hex input.
 *				* added hex() function to print result in hex.
 *				* added '>' '<' '|' and '&' to EE_level2() to 
 *			      support shifts and masking. 
 *				* additional arg to Evaluate so that it can 
 *			      return the string equivalent of the result.
 *
 * You are free to incorporate this code into your own works, even if it
 * is a commercial application.  However, you may not charge anyone else
 * for the use of this code!  If you intend to distribute your code,
 * I'd appreciate it if you left this message intact.  I'd like to
 * receive credit wherever it is appropriate.  Thanks!
 *
 * I don't promise that this code does what you think it does...
 *
 * Please mail any bug reports/fixes/enhancments to me at:
 *      morley@camosun.bc.ca
 * or
 *      Mark Morley
 *      3889 Mildred Street
 *      Victoria, BC  Canada
 *      V8Z 7G1
 *      (604) 479-7861
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
#if INCLUDE_EE
#include "stddefs.h"
#include "ee.h"
#include "genlib.h"
#include "cli.h"

static void	EE_error(int);
static int	EE_level1(), EE_level2(), EE_level3(), EE_level4();
static int	EE_level5(), EE_level6(), EE_power();

static int		EE_printmode;
static uchar	*EE_expression;				/* Pointer to user's expression */
static uchar	EE_token[CMDLINESIZE + 1];	/* Holds the current token */
static int		EE_type;					/* Type of the current token */


TYPE
EE_hex(TYPE v)
{
	EE_printmode = 1;
	return(v);
}

/* Add any math functions that you wish to recognise here...  The first
 *	field is the name of the function as it would appear in an expression.
 *	The second field tells how many arguments to expect.  The third is
 *	a pointer to the actual function to use.
 */
FUNCTION EE_funcs[] =
{
	{ "hex",     1,    EE_hex },
	{ 0 }
};


/* strlwr( char* s ):
 *	Internal use only. Routine converts a string to lowercase.
 *	This one is ASCII specific!
 */
static void
strlwr(char *s)
{
	while( *s )
	{
		if( *s >= 'A' && *s <= 'Z' )
			*s += 32;
		s++;
	}
}

/* EE_parse():
 *	This function is used to grab the next token from the expression that
 *	is being evaluated.
 */
static void
EE_parse(void)
{
	char* t;

	EE_type = 0;
	t = (char *)EE_token;
	while( iswhite( *EE_expression ) )
		EE_expression++;
	if(isdelim(*EE_expression)) {
		EE_type = DEL;
		*t++ = *EE_expression++;
	}
	else if(!strncmp((char *)EE_expression,(char *)"0x",2)) {
		EE_type = HEXNUM;
		EE_expression+=2;
		while( ishexnum( *EE_expression ) )
			*t++ = *EE_expression++;
	}
	else if(isnumer(*EE_expression)) {
		EE_type = NUM;
		while( isnumer( *EE_expression ) )
			*t++ = *EE_expression++;
	}
	else if(isalpha(*EE_expression)) {
		EE_type = VAR;
		while( isalpha( *EE_expression ) )
			*t++ = *EE_expression++;
		EE_token[VARLEN] = 0;
	}
	else if(*EE_expression) {
		*t++ = *EE_expression++;
		*t = 0;
		EE_error( E_SYNTAX );
	}
	*t = 0;
	while( iswhite( *EE_expression ) )
		EE_expression++;
}


/* EE_level1( TYPE* r )
 *	This function handles any variable assignment operations.
 *	It returns a value of 1 if it is a top-level assignment operation,
 *	otherwise it returns 0.
 */
static int
EE_level1(TYPE *r)
{
	char t[VARLEN + 1];

	if( EE_type == VAR ) {
		if( *EE_expression == '=' )
		{
			strcpy( (char *)t, (char *)EE_token );
			EE_parse();
			EE_parse();
			if( !*EE_token )
			{
				setenv(t,"");
				return(1);
			}
			EE_level2( r );
			shell_sprintf(t,"%d",*r);
			return( 1 );
		}
	}
	EE_level2( r );
	return( 0 );
}


/* EE_level2( TYPE* r )
 *   This function handles any addition and subtraction operations.
 */
static int
EE_level2(TYPE *r)
{
	TYPE t = 0;
	char o;

	EE_level3( r );
	while((o = *EE_token) == '+' || o == '-' ||
			o == '&' || o == '|'  ||
			o == '>' || o == '<' )
	{
		EE_parse();
		EE_level3( &t );
		if( o == '+' )
			*r = *r + t;
		else if( o == '-' )
			*r = *r - t;
		else if( o == '&' )
			*r = (TYPE)((long)*r & (long)t);
		else if( o == '|' )
			*r = (TYPE)((long)*r | (long)t);
		else if( o == '>' )
			*r = (TYPE)((long)*r >> (long)t);
		else if( o == '<' )
			*r = (TYPE)((long)*r << (long)t);
	}
	return(0);
}

/* EE_level3( TYPE* r )
 *	This function handles any multiplication, division, or modulo.
 */

static int
EE_level3(TYPE *r)
{
	TYPE t;
	char o;

	EE_level4( r );
	while( (o = *EE_token) == '*' || o == '/' || o == '%' )
	{
		EE_parse();
		EE_level4( &t );
		if( o == '*' ) {
			*r = *r * t;
		}
		else if( o == '/' )
		{
			if( t == 0 )
				EE_error( E_DIVZERO );
#ifdef DIVIDE_NOT_SUPPORTED
			EE_error(E_OPNOSUPPORT);
#else
			*r = *r / t;
#endif
		}
		else if( o == '%' )
		{
			if( t == 0 )
				EE_error( E_DIVZERO );
			*r = *r % t;
		}
	}
	return(0);
}

/* EE_level4( TYPE* r )
 *	This function handles any "to the power of" operations.
 */

static int
EE_level4(TYPE *r)
{
	TYPE t;

	EE_level5( r );
	if( *EE_token == '^' )
	{
		EE_parse();
		EE_level5( &t );
		*r = EE_power( *r, t );
	}
	return(0);
}

static int
EE_power(TYPE base,TYPE power)
{
	int i, val;

	val = 1;
	for(i=0;i<power;i++)
		val *= base;

	return(val);
}


/* EE_level5(TYPE* r)
 *	This function handles any unary + or - signs.
 */

static int
EE_level5(TYPE *r)
{
	char o = 0;

	if( *EE_token == '+' || *EE_token == '-' )
	{
		o = *EE_token;
		EE_parse();
	}
	EE_level6( r );
	if( o == '-' )
		*r = -*r;
	return(0);
}


/* EE_level6(TYPE* r)
 *	This function handles any literal numbers, variables, or functions.
 */
static int
EE_level6(TYPE *r)
{
	char	*cp;
	int  i;
	int  n;
	TYPE a[3];

	if( *EE_token == '(' )
	{
		EE_parse();
		if( *EE_token == ')' )
			EE_error( E_NOARG );
		EE_level1( r );
		if( *EE_token != ')' )
			EE_error( E_UNBALAN );
		EE_parse();
	}
	else
	{
		if( EE_type == NUM )
		{
			*r = (TYPE) atoi( (char *)EE_token );
			EE_parse();
		}
		else if( EE_type == HEXNUM )
		{
			*r = (TYPE) strtol((char *)EE_token,(char **)0,16);
			EE_parse();
		}
		else if( EE_type == VAR )
		{
			if( *EE_expression == '(' )
			{
				for( i = 0; EE_funcs[i].name; i++ )
				{
					if( ! strcmp( (char *)EE_token, (char *)EE_funcs[i].name ) )
					{
						EE_parse();
						n = 0;
						do
						{
							EE_parse();
							if( *EE_token == ')' || *EE_token == ',' )
								EE_error( E_NOARG );
							a[n] = 0;
							EE_level1( &a[n] );
							n++;
						} while( n < 4 && *EE_token == ',' );
						EE_parse();
						if( n != EE_funcs[i].args )
						{
							strcpy( (char *)EE_token, (char *)EE_funcs[i].name );
							EE_error( E_NUMARGS );
						}
						*r = EE_funcs[i].func( a[0], a[1], a[2] );
						return(0);
					}
				}
				if( ! EE_funcs[i].name )
					EE_error( E_BADFUNC );
			}
			else {
				cp = getenv((char *)EE_token);
				if (cp)
					*r = strtol(cp,(char **)0,0);
				else
					EE_error( E_UNKNOWN );
			}
			EE_parse();
		}
		else
			EE_error( E_SYNTAX );
	}
	return(0);
}


/* Evaluate(char* e, TYPE* result, int* a ) 
 *	This function is called to evaluate the expression E and return the
 *	answer in RESULT.  If the expression was a top-level assignment, a
 *	value of 1 will be returned in A, otherwise it will contain 0.
 *	Returns E_OK if the expression is valid, or an error code. 
*/

int
Evaluate(char *e, char *sresult)
{
	TYPE result;

	EE_printmode = DEC_PRINT;
	EE_expression = (unsigned char *)e;
	strlwr( (char *)EE_expression );
	result = 0;
	EE_parse();
	if( ! *EE_token )
		EE_error( E_EMPTY );
	EE_level1( &result );
	if (EE_printmode == HEX_PRINT)
		sprintf(sresult,"0x%x",result);
	else
		sprintf(sresult,"%d",result);
	return((int)result);
}

void
EE_error(int erno)
{
	printf(" EE Error # %d\n",erno);
}

#if 0
/* As of umon1.0, the 'Let' command functionality is built into
 * the 'set' command...
 */
char *LetHelp[] = {
	"Set shellvar equal to result of expression",
	"{varname=expression}",
	0,
};

int
Let(int argc,char *argv[])
{
	char	*eqsign;
	char	srslt[16];

	if (argc != 2)
		return(CMD_PARAM_ERROR);

	eqsign = strchr(argv[1],'=');

	if (eqsign) {
		*eqsign = 0;
		if (*(eqsign+1) != 0) {
			Evaluate(eqsign+1,srslt);
			setenv(argv[1],srslt);
		}
		return(CMD_SUCCESS);
	}
	return(CMD_PARAM_ERROR);
}
#endif

/* setEE():
 * Called by 'set' command (as of uMon1.0, the 'let' command is removed.
 */
int
setEE(char *string)
{
	int len;
	char srslt[16];
	char *eqsign, vcopy[24];

	eqsign = strchr(string,'=');
	if (!eqsign)
		return(0);

	len = eqsign - string;
	if (len >= (sizeof(vcopy))) {
		printf("Varname too long\n");
		return(-1);
	}
	memcpy(vcopy,string,len);
	vcopy[len] = 0;

	Evaluate(eqsign+1,srslt);
	setenv(vcopy,srslt);
	return(1);
}
#endif
