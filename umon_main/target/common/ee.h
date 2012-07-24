/* ee.h
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
 *	Original author:	Ed Sutter	(see comments in ee.c)
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#ifndef _EE_H_
#define _EE_H_


#define TYPE			int				/* Type of numbers to work with */

#define VARLEN          15              /* Max length of variable names */
#define MAXVARS         50              /* Max user-defined variables */

#define VAR             1
#define DEL             2
#define NUM             3
#define HEXNUM          4

#define HEX_PRINT		1
#define DEC_PRINT		2

typedef struct
{
   char* name;                          /* Function name */
   int   args;                          /* Number of arguments to expect */
   TYPE  (*func)();                     /* Pointer to function */
} FUNCTION;

/* The following macros are ASCII dependant, no EBCDIC here! */
#define iswhite(c)  (c == ' ' || c == '\t')
#define isnumer(c)  ((c >= '0' && c <= '9') || c == '.')
#define ishexnum(c)  ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
#define isalpha(c)  ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') \
                    || c == '_')
#define isdelim(c)  (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' \
                    || c == '^' || c == '(' || c == ')' || c == ',' \
		    || c == '=' || c == '&' || c == '|' \
		    || c == '<' || c == '>')

/* Codes returned from the evaluator */
#define E_OK           0        /* Successful evaluation */
#define E_SYNTAX       1        /* Syntax error */
#define E_UNBALAN      2        /* Unbalanced parenthesis */
#define E_DIVZERO      3        /* Attempted division by zero */
#define E_UNKNOWN      4        /* Reference to unknown variable */
#define E_MAXVARS      5        /* Maximum variables exceeded */
#define E_BADFUNC      6        /* Unrecognised function */
#define E_NUMARGS      7        /* Wrong number of arguments to funtion */
#define E_NOARG        8        /* Missing an argument to a funtion */
#define E_EMPTY        9        /* Empty expression */
#define E_OPNOSUPPORT  10       /* Operation not supported */


extern int setEE(char *);

#endif
