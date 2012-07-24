// This is an example program that can be put into TFS and
// executed by pcc...
// There are many more examples in the 'test' directory that comes
// with the official picoc distribution.
//
void try1()
{
  int i;
  printf("Please type a character: ");
  i = getchar();
  printf("\nThanks, you typed: '%c'\n\n\n",i);
}

void try2()
{
  int i;
  char *cp;
  short *sp;
  long *lp, lval;

  system("dm $APPRAMBASE");

  cp = getenv("APPRAMBASE");
  lval = strtol(cp,0,16);

  cp = (char *)lval;
  sp = (short *)lval;
  lp = (long *)lval;

  for(i=0;i<5;i++)
    printf("*cp = 0x%x\n",*cp++);

  for(i=0;i<5;i++)
    printf("*sp = 0x%x\n",*sp++);

  for(i=0;i<5;i++)
    printf("*lp = 0x%x\n",*lp++);
}

void try3()
{
  char *cp, *cp1;
  long lval;

  cp = malloc(100);
  lval = (long)cp;
  cp1 = getenv("PLATFORM");
  strcpy(cp,cp1);

  printf("cp: 0x%x\n",lval);
  free(cp);
}

void try4()
{
  char cmd[64];
  char *cp, *cp1;
  long lval;

  cp = getenv("APPRAMBASE");
  lval = strtol(cp,0,16);
  sprintf(cmd,"dm 0x%x",lval);
  printf("%s:\n",cmd);
  system(cmd);

  cp1 = malloc(64);
  sprintf(cp1,"dm 0x%x 16",lval);
  printf("%s:\n",cp1);
  system(cp1);
  free(cp1);
}

void try5()
{
  char *cp1, *cp2, *cp3, *cp4;
  int d1, d2, d3, d4;

  cp1 = getenv("IPADD");
  d1 = strtol(cp1,&cp2,10);
  d2 = strtol(cp2+1,&cp3,10);
  d3 = strtol(cp3+1,&cp4,10);
  d4 = strtol(cp4+1,0,10);

  printf("My IP: %d.%d.%d.%d\n",d1,d2,d3,d4);
}

try1();
try2();
try3();
try4();
try5();
