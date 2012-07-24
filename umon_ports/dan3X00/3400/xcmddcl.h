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
