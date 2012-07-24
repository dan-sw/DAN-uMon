/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * Modified (July 30, 2008): Ed Sutter (esutter@alcatel-lucent.com)
 * Changed to support interface with uMon's TFS file system.
 *
 */
#include "lwip/def.h"
#include "fs.h"
#include "httpd.h"
#include "monlib.h"
#include "tfs.h"

#define NORMAL 1
#define GOTDOLLAR 2
#define GOTOBRACE 3
#define GOTOBRACKET 4
#define OBRACE '{'
#define CBRACE '}'
#define OBRACKET '['
#define CBRACKET ']'

/* This buffer is used for modification of a .html file so that ${XXX}
 * syntax can be replaced with the content of the uMon shell variable 'XXX'.
 * If the .html file is larger than this buffer, or if the expanded file
 * will be larger than this buffer, the conversion is incomplete.
 */
char filebuf[0x1000];

/*-----------------------------------------------------------------------------------*/

char *nofile = "<html>\n<head><title>Page Not Found</title></head>\n<body bgcolor=\"white\" text=\"black\">\n<body>\n<center><h1><br><br><b>Sorry!</h1></b><br>The page you're looking for is not available<br></center></body></html>\n";

/*-----------------------------------------------------------------------------------*/

/* convert_vars():
 * Take base and len as pointers to a file's data in TFS.
 * Parse the file looking for ${xxxx}, if found and if xxxx
 * is an environment variable, replace it with the content of
 * the variable.
 * As of Nov 2009, added support for $[xxxx], to be used as
 * an indicator that 'xxxx' is a command to be run through
 * mon_docommand().
 */
int
convert_vars(char *base, int len)
{
  char varbuf[64];
  char *from = base;
  char *fend = base+len;
  char *to = filebuf;
  char *tend = to+sizeof(filebuf);
  char state = NORMAL;
  char *varname = 0, *env;

  while(from < fend) {
    switch(state) {
      case NORMAL:
    	if (*from == '$')
          state = GOTDOLLAR;
        else
          *to++ = *from;
        break;
      case GOTDOLLAR:
        if (*from == OBRACE)
          state = GOTOBRACE;
        else if (*from == OBRACKET)
          state = GOTOBRACKET;
        else {
          *to++ = '$';
          *to++ = *from;
          state = NORMAL;
        }
        break;
      case GOTOBRACKET:
        if (varname == 0) {
          varname = from;
        }
        else if (*from == CBRACKET) {
          memset(varbuf,0,sizeof(varbuf));
          if ((from-varname) < sizeof(varbuf))
            strncpy(varbuf,varname,from-varname);
          else {
            mon_printf("convert_vars(): overflow protection_1\n");
            return(to-filebuf);
          }
          mon_docommand(varbuf,http_verbose);
          varname = 0;
          state = NORMAL;
        }
        break;
      case GOTOBRACE:
        if (varname == 0) {
          varname = from;
        }
        else if (*from == CBRACE) {
          memset(varbuf,0,sizeof(varbuf));
          if ((from-varname) < sizeof(varbuf))
            strncpy(varbuf,varname,from-varname);
          else {
            mon_printf("convert_vars(): overflow protection_2\n");
            return(to-filebuf);
          }
          env = mon_getenv(varbuf);
          if (to + strlen(env) >= tend) {
            mon_printf("convert_vars(): overflow protection_3\n");
            return(to-filebuf);
          }
          if (env) {
            strcpy(to,env);
            to += strlen(env);
          }
          else {
            strcpy(to,varname);
            to += strlen(varname);
            *to++ = CBRACE;
          }
          varname = 0;
          state = NORMAL;
        }
        break;
    }
    from++;
  }
  return(to-filebuf);
}

int
fs_open(char *name, struct fs_file *file)
{
  TFILE *tfp;
  char *prefix, namebuf[TFSNAMESIZE+1];
  char *origname;

  /* Look for the specified file in TFS.  If found, then
   * return it; else return the nofile string above.
   */
  origname = name;
  prefix = mon_getenv(HTTP_PREFIX_VARNAME);
  if (prefix) {
    if ((strlen(name) + strlen(prefix) + 1) < sizeof(namebuf)) {
      mon_sprintf(namebuf,"%s%s",prefix,name);
      name = namebuf;
    }
  }
  tfp = mon_tfsstat(name);
  if (!tfp) {
    file->data = (char *)nofile;
    file->len = strlen(nofile);
  }
  else {
    /* If the file ends with ".html", then parse it for ${XXX} tokens.
     * and whenver XXX is a valid shell variable in uMon, replace the
     * ${XXX} with the content of the variable (see convert_vars() above).
     */
    if (strstr(name,".html")) {
      file->len = convert_vars(TFS_BASE(tfp),TFS_SIZE(tfp));
      file->data = filebuf;
    }
    else {
      file->data = TFS_BASE(tfp);
      file->len = TFS_SIZE(tfp);
    }
  }
  return(1);
}
/*-----------------------------------------------------------------------------------*/
