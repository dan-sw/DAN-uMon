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
 * Modified significantly to run as a uMon application:
 *	Ed Sutter <esutter@alcatel-lucent.com>
 */

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "monlib.h"

struct http_state {
  char *file;
  u16_t left;
  u8_t retries;
};

char *http_respptr;
int  http_respsiz, http_respidx;
int	 http_verbose;

#ifndef CMDRESPBOT
char *cmdrespbot = "</pre>\n</body>\n</html>";
#endif

int
http_putchar(int c)
{
	char *mp;

	/* If http_respptr is -1, then we've aborted the response (probably
	 * because of a memory allocation failure) so just return.
	 */
	if (http_respptr == (char *)0xffffffff)
		return(0);

	/* If http_respptr is 0, then it is the start of a new response.
	 */
	if (http_respptr == 0) {
		TFILE *tfp;
		char *name, *prefix;
		static char namebuf[TFSNAMESIZE+1];

		name = "cmdresptop.html";
		prefix = mon_getenv(HTTP_PREFIX_VARNAME);
		if (prefix) {
			if ((strlen(name) + strlen(prefix) + 1) < sizeof(namebuf)) {
				mon_sprintf(namebuf,"%s%s",prefix,name);
				name = namebuf;
			}
		}
		if ((tfp = mon_tfsstat(name)) != 0) {
			http_respsiz = TFS_SIZE(tfp) + 1024;
		}
		else
			http_respsiz = 1024;
		http_respptr = mon_malloc(http_respsiz);
		if (http_respptr == 0) {
			http_respptr = (char *)0xffffffff;
			return(0);
		}
		if (tfp) {
			memcpy(http_respptr,TFS_BASE(tfp),TFS_SIZE(tfp));
			http_respidx = convert_vars(TFS_BASE(tfp),TFS_SIZE(tfp));
			memcpy(http_respptr,filebuf,http_respidx);
		}
	}
	else if (http_respidx >= http_respsiz - 32) {
		http_respsiz += 1024;
		mp = mon_realloc(http_respptr,http_respsiz);
		if (mp == 0) {
			mon_free(http_respptr);
			http_respptr = (char *)0xffffffff;
			return(0);
		}
		http_respptr = mp;
	}
	http_respptr[http_respidx++] = (char)c;
	return(1);
}

/* httpapp_procnameval():
 *  This function is called by the server as it is processing any
 *  list of name/value pairs in a form reponse.  This is where the application
 *  puts all of the "actions" that must occur as a result of a client
 *  interacting with a form.  The name and value are incoming ascii and
 *  it is 100% application specific code that deals with these name/value
 *  pairs.
 */
int
httpapp_procnameval(char *name, char *val)
{
//	mon_printf("name: %s, val: %s\n",name,val);
	if (strcmp(name,"umoncmd") == 0) {
		http_respptr = 0;
		mon_com(CHARFUNC_PUTCHAR,http_putchar,0,0);
		mon_docommand(val,0);
		if (http_respptr == 0)	// Just in case the message is empty
			http_putchar('\n');	// generate one character of response.
		mon_com(CHARFUNC_PUTCHAR,0,0,0);
	}
    return(0);
}

/* http_process_list():
 *	The input string from the html form has a few ugly details to deal with.
 *	The format of the string is:
 *		"name1=value1&name2=value2"
 *
 *		where "namex" is the name assigned in the form for a 
 *		particular piece of input.
 *		and "valuex" is what was entered by the user of the form.
 *
 *	The ampersand (&) is the delimiter between variable assignments.
 *	Any character can be written as a percent sign followed by a 2-digit
 *	hex value that represents the actual character.
 *	If a '+' sign is on the line, then it should be converted to a space.
 */
static int
http_process_list(char *nvlist, int length)
{
	char	*ipin, *ipout, *eq, *amp, *cp, *namevallist, hval[3];

	http_respptr = (char *)0;

    /* We can't assume that the incoming list is NULL terminated, so
     * to allow us to parse as a standard 'C' string, copy the data
     * to a buffer and null terminate it...
     */
    namevallist = mon_malloc(length + 1);
	if (namevallist == (char *)0) {
      mon_printf("malloc failed\n");
      return(-1);
    }

    memcpy(namevallist,nvlist,length);
    namevallist[length] = 0;

	/* The incoming list may have other HTTPish stuff prepended to it, so
	 * strip it off by searching for the first '=' sign, then backing up to
	 * either the beginning, or the first instance of a CR or LF...
	 */
	cp = strchr(namevallist,'=');
	if (cp) {
		while ((*cp != '\n') && (*cp != '\r') && (cp != namevallist))
			cp--;
		if ((*cp == '\r') || (*cp == '\n'))
			namevallist = cp+1;
		else
			namevallist = cp;
	}

	/* Terminate the incoming name list when a CR or LF is found... */
	cp = strchr(namevallist,'=');
	if (cp) {
		while(*cp) {
			if ((*cp == '\r') || (*cp == '\n')) {
				*cp = 0;
				break;
			}
			cp++;
		}
	}

	ipin = ipout = namevallist;
	
	/* First convert + and %xx... */
	while(*ipin) {
		if (*ipin == '+')
			*ipout = ' ';
		else if (*ipin == '%') {
			hval[0] = ipin[1];
			hval[1] = ipin[2];
			hval[2] = 0;
			ipin += 2;
			*ipout = (unsigned char)strtol(hval,(char **)0,16);
		}
		else
			*ipout = *ipin;
		ipin++;
		ipout++;
	}
	*ipout = 0;

	/* Now parse for the name=var& strings... */
	ipin = namevallist;
	eq = strchr(namevallist,'=');
	if (!eq) {
        mon_free(namevallist);
		return(-1);
    }
	amp = strchr(namevallist,'&');
	while(eq) {
		*eq = 0;
		if (amp)
			*amp = 0;
		httpapp_procnameval(ipin,eq+1);
		if (!amp)
			break;
		ipin = amp+1;
		eq = strchr(ipin,'=');
		amp = strchr(ipin,'&');
	}
    mon_free(namevallist);
	return(0);
}

/*-----------------------------------------------------------------------------------*/
static void
conn_err(void *arg, err_t err)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(err);

  hs = arg;
  mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
static void
close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{
  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  mem_free(hs);
  tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
static void
send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;

  /* We cannot send more data than space available in the send
     buffer. */     
  if (tcp_sndbuf(pcb) < hs->left) {
    len = tcp_sndbuf(pcb);
  } else {
    len = hs->left;
  }

  do {
    err = tcp_write(pcb, hs->file, len, 0);
    if (err == ERR_MEM) {
      len /= 2;
    }
  } while (err == ERR_MEM && len > 1);  
  
  if (err == ERR_OK) {
    hs->file += len;
    hs->left -= len;
    /*  } else {
    mon_printf("send_data: error %s len %d %d\n", lwip_strerr(err), len, tcp_sndbuf(pcb));*/
  }
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  struct http_state *hs;

  hs = arg;
  
  /*  printf("Polll\n");*/
  if (hs == NULL) {
    /*    printf("Null, close\n");*/
    tcp_abort(pcb);
    return ERR_ABRT;
  } else {
    ++hs->retries;
    if (hs->retries == 4) {
      tcp_abort(pcb);
      return ERR_ABRT;
    }
    send_data(pcb, hs);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(len);

  hs = arg;

  hs->retries = 0;
  
  if (hs->left > 0) {    
    send_data(pcb, hs);
  } else {
    close_conn(pcb, hs);
    if (http_respptr) {
      mon_free(http_respptr);
      http_respptr = 0;
      http_respidx = 0;
    }
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  int i;
  char *data;
  struct fs_file file;
  struct http_state *hs;

  hs = arg;

  if (err == ERR_OK && p != NULL) {

    /* Inform TCP that we have taken the data. */
    tcp_recved(pcb, p->tot_len);
    
    if (hs->file == NULL) {
      data = p->payload;
      
      if (strncmp(data, "GET ", 4) == 0) {
        for(i = 0; i < 40; i++) {
          if (((char *)data + 4)[i] == ' ' ||
             ((char *)data + 4)[i] == '\r' ||
             ((char *)data + 4)[i] == '\n') {
            ((char *)data + 4)[i] = 0;
          }
        }

        if (*(char *)(data + 4) == '/' && *(char *)(data + 5) == 0) {
          fs_open("index.html", &file);
        } else if (!fs_open((char *)data + 5, &file)) { // skip leading slash
          fs_open("404.html", &file);
        }

        hs->file = file.data;
        hs->left = file.len;

        pbuf_free(p);
        send_data(pcb, hs);

        /* Tell TCP that we wish be to informed of data that has been
           successfully sent by a call to the http_sent() function. */
        tcp_sent(pcb, http_sent);
      } else if (strncmp(data,"POST ",5) == 0) {
        char *cp = data+p->tot_len;
        char gotequal = 0;

        for(cp = data+p->tot_len;cp>data;cp--) {
          if (*cp == '=') {
            gotequal = 1;
            continue;
          }
          if (gotequal && ((*cp == 0x0d) || (*cp == 0x0a))) {
              break;
          }
        }
        if (cp == data) {
          pbuf_free(p);
          close_conn(pcb, hs);
        }
        else {
          cp++;
          http_process_list(cp,p->tot_len - (cp-data));
		  if ((http_respptr != 0) && (http_respptr != (char *)0xffffffff)) {
			strcpy(http_respptr+http_respidx,cmdrespbot);
			http_respidx += strlen(cmdrespbot);
            hs->file = http_respptr;
            hs->left = http_respidx;
            pbuf_free(p);
            send_data(pcb, hs);
        	tcp_sent(pcb, http_sent);
          }
        }
        //pbuf_free(p);
      } else {
        mon_printf("HTTP NOT POST OR GET...\n");
		mon_printmem(data,p->tot_len,1);
        pbuf_free(p);
        close_conn(pcb, hs);
      }
    } else {
      pbuf_free(p);
    }
  }

  if (err == ERR_OK && p == NULL) {
    close_conn(pcb, hs);
  }
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

//  tcp_setprio(pcb, TCP_PRIO_MIN);
  
  /* Allocate memory for the structure that holds the state of the
     connection. */
  hs = (struct http_state *)mem_malloc(sizeof(struct http_state));

  if (hs == NULL) {
    mon_printf("http_accept: Out of memory\n");
    return ERR_MEM;
  }
  
  /* Initialize the structure. */
  hs->file = NULL;
  hs->left = 0;
  hs->retries = 0;
  
  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  tcp_recv(pcb, http_recv);

  tcp_err(pcb, conn_err);
  
  tcp_poll(pcb, http_poll, 4);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
httpd_init(void)
{
  struct tcp_pcb *pcb;

  if (mon_getenv("HTTP_VERBOSE"))
    http_verbose = 1;
  else
    http_verbose = 0;
  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, 80);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, http_accept);
}
/*-----------------------------------------------------------------------------------*/

