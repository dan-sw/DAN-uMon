#ifndef _BBC_H_
#define _BBC_H_
#define BBC()	bbc(0,__LINE__)
#define BBCF()	bbc(__FILE__,__LINE__)

void bbc(char *file, int line);
#endif
