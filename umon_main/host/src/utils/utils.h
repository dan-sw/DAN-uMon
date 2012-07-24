/* Header file used with getopt.c. */
extern int	optind;
extern char *optarg;
extern int verbose;

#ifdef BUILD_WITH_VCC
extern int getopt(int,char **,char *);
#endif
extern int fileexists(char *);
extern unsigned short otherEnd16(int,unsigned short);
extern unsigned long otherEnd32(int,unsigned long);
extern char * StrAllocAndCopy(char *);
extern char * Malloc(int);
extern int Read(int,char *,int);
extern int Write(int,char *,int);
extern int Lseek(int,int,int);
extern void usage(char *);
