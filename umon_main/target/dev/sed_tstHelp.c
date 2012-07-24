char *sed_tstHelp[] = {
	"Run SED135x video controller test",
	USAGE_STRING,
    "Options:",
    " -c     force display to CRT mode",
	" -d{N}  force a depth of N-bits/pixel (N=4|8|16)",
#ifdef SED_I_OPTION
    " -i     init only, do not run frame buffer tests",
#endif
    " -l     force display to LCD mode",
    " -n     no keycheck - CAUTION: RESET SYSTEM TO STOP!",
#ifdef SED_S_OPTION
	" -s     scope loop, continously write 0 to SED and read",
#endif
	"",
	"Notes:",
	" * This command allows the user to test the SED135x Video Controller",
	"   found on some Cogent Single Boards.  The user may set the mode to",
	"   CRT or LCD and also set the color depth to run the test at.",
	" * With no options specified, default to current mode and depth",
	" * The frame buffer R/W test will test all of the frame",
	"   buffer regardless of depth.",
	(char *)0
};
