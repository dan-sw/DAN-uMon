#include "picoc.h"

/* a source file we need to clean up */
static char *CleanupText = NULL;

/* deallocate any storage */
void PlatformCleanup()
{
    if (CleanupText != NULL)
        mon_free(CleanupText);
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen)
{
    mon_getline(Buf, MaxLen, 0);
    return(Buf);
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    return mon_getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    mon_putchar(OutCh);
}

/* read a file into memory */
char *PlatformReadFile(const char *FileName)
{
	TFILE *tfp;
    char *ReadText;
    int BytesRead;
    
    tfp = mon_tfsstat((char *)FileName);
    if (tfp == (TFILE *)0)
        ProgramFail(NULL, "can't read file %s\n", FileName);
    
    ReadText = mon_malloc(TFS_SIZE(tfp) + 1);
    if (ReadText == NULL)
        ProgramFail(NULL, "out of memory\n");
        
	BytesRead = TFS_SIZE(tfp);
    memcpy(ReadText,(char *)TFS_BASE(tfp),BytesRead);

    ReadText[BytesRead] = '\0';
    
    return ReadText;    
}

/* read and scan a file for definitions */
void PlatformScanFile(const char *FileName)
{
    char *SourceStr = PlatformReadFile(FileName);
    char *OrigCleanupText = CleanupText;
    if (CleanupText == NULL)
        CleanupText = SourceStr;

    Parse(FileName, SourceStr, strlen(SourceStr), TRUE);
    mon_free(SourceStr);

    if (OrigCleanupText == NULL)
        CleanupText = NULL;
}

/* exit the program */
void PlatformExit()
{
    mon_appexit(1);
}

