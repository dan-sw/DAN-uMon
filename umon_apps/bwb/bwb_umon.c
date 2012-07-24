/***************************************************************
 *
 * bwb_umon.c:
 *	MicroMonitor-specific additions to BywaterBasic.
 *
 * Contact: Ed Sutter	esutter@lucent.com
 *
 ***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bwbasic.h"
#include "monlib.h"

/***************************************************************

        FUNCTION:       fnc_inkey()

        DESCRIPTION:    This C function implements the BASIC INKEY$
        		function.  It is implementation-specific.

***************************************************************/

extern struct bwb_variable *
fnc_inkey( int argc, struct bwb_variable *argv , int id)
   {
   static struct bwb_variable nvar;
   char tbuf[ MAXSTRINGSIZE + 1 ];
   static int init = FALSE;

   /* initialize the variable if necessary */

   if ( init == FALSE )
      {
      init = TRUE;
      var_make( &nvar, STRING );
      }

   /* check arguments */

#if PROG_ERRORS
   if ( argc > 0 )
      {
      sprintf( bwb_ebuf, "Two many arguments to function INKEY$()" );
      bwb_error( bwb_ebuf );
      return &nvar;
      }

#else
   if ( fnc_checkargs( argc, argv, 0, 0 ) == FALSE )
      {
      return NULL;
      }
#endif

   /* body of the INKEY$ function */

   if ( mon_gotachar() == 0 )
      {
      tbuf[ 0 ] = '\0';
      }
   else
      {
      tbuf[ 0 ] = (char) mon_getchar();
      tbuf[ 1 ] = '\0';
      }

   /* assign value to nvar variable */

   str_ctob( var_findsval( &nvar, nvar.array_pos ), tbuf );

   /* return value contained in nvar */

   return &nvar;

   }
