/* xcmdtbl.c:
 * This file must exist even if it is empty because it is #included in the
 * common file cmdtbl.c.  The purpose is to keep the common comand table 
 * file (common/cmdtbl.c) from being corrupted with non-generic commands
 * that may be target specific.
 * It is the entry in the command table representing the new command being
 * added to the cmdtbl[] array.
 */
#ifdef DAN2400
        { "upgrade_umon", upgrade_umon, upgrade_umon_help, 0 },
#endif
        { "pll_on", pll_on, pll_on_help, 0 },
        { "spif_device", spif_device, spif_device_help, 0 },
        { "ddr_init", ddr_init_cli, ddr_init_help, 0 },
        { "checkcrc", dancli_Checkcrc, dancli_Checkcrc_help, 0 },
