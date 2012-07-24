extern char *optarg;
extern int optind, sp;
extern unsigned char	ctypetbl[];

extern int abs(int arg);
extern int getchar(void);
extern int atoi(char *p);
extern int gotachar(void);
extern int putchar(char c);
extern int target_getchar(void);
extern int target_gotachar(void);
extern int target_putchar(char c);
extern int strlen(char *s);
extern int docommand(char *line);
extern int printf(char *fmt, ...);
extern int sprintf(char *buf, char *fmt, ...);
extern int getline(char *buf,int max);
extern int strcmp(char *s1,char * s2);
extern int getopt(int argc,char *argv[],char *opts);
extern int getbytes(char *buf, int tot);
extern int inRange(char *range, int value);

extern char * strcpy(char *s1,char *s2);
extern char *strncpy(char *s1,char *s2,int n);
extern int strncmp(char *s1,char *s2,int n);
extern char *strchr(char *sp,char c);
extern char *memset(char *sp,char c,int n);
extern char *memcpy(char *to,char *from,int n);
extern int memcmp(char *s1,char *s2,int n);
extern void ticktock(void);

extern long strtol(char *str,char **ptr,int base);

extern unsigned long strtoul(char *str, char **ptr,int base);

extern void ioInit(void);
extern void delay(int);
extern void getoptinit(void);
extern void puts(char *string);
extern void CommandLoop(void);

