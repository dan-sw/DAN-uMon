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
/* xcmdtbl.h:
 * This file must exist even if it is empty because it is #included in the
 * common file cmdtbl.c.  The purpose is to keep the common comand table
 * file (common/cmdtbl.c) from being corrupted with non-generic commands
 * that may be target specific.
 * This is the declaration portion of the code that must be at the top of
 * the cmdtbl[] array.
 */
extern char *upgrade_umon_help[];
extern int upgrade_umon(int argc, char *argv[]);
extern char *pll_on_help[];
extern int pll_on(int argc, char *argv[]);
extern char *spif_device_help[];
extern int spif_device(int argc, char *argv[]);
extern char *ddr_init_help[];
extern int ddr_init_cli(int argc, char *argv[]);
extern char *dancli_Checkcrc_help[];
extern int dancli_Checkcrc(int argc, char *argv[]);
extern char *dancli_Ds_help[];
extern int dancli_Ds(int argc, char *argv[]);
extern char *dancli_Trc_help[];
extern int dancli_Trc(int argc, char *argv[]);
extern char *dancli_Timer_help[];
extern int dancli_Timer(int argc, char *argv[]);
extern char *dancli_Status_help[];
extern int dancli_Status(int argc, char *argv[]);

