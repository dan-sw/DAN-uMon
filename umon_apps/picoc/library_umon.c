#include "picoc.h"

#define INCLUDE_SHOWCOMPLEX 0
#define INCLUDE_SYSTEM 1

void PlatformLibraryInit()
{
}

void Crandom(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = rand() % Param[0]->Val->Integer;
}

void
Csystem(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
	mon_docommand(Param[0]->Val->NativePointer,0);
}

void
Csetenv(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
	char *name, *value;

	name = (char *)Param[0]->Val->NativePointer;
	value = (char *)Param[1]->Val->NativePointer;
	mon_setenv(name,value);
}

void
Cgetenv(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
	char *name;

	name = Param[0]->Val->NativePointer;
	ReturnValue->Val->NativePointer = mon_getenv(name);
}

void
Cptr(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
	int i;

	i = Param[0]->Val->Integer;
	ReturnValue->Val->NativePointer = (void *)i;
}

void
Cstrtol(struct ParseState *Parser, struct Value *ReturnValue,
struct Value **Param, int NumArgs)
{
    char *str = (char *)Param[0]->Val->NativePointer;
    char **cp = (char **)Param[1]->Val->NativePointer;
    int base = Param[2]->Val->Integer;

	ReturnValue->Val->Integer = (int)strtol(str,cp,base);
}

void
Ctfsadd(struct ParseState *Parser, struct Value *ReturnValue,
	struct Value **Param, int NumArgs)
{
	char *name, *info, *flags;
	unsigned char *src;
	int size, rc;

	name = Param[0]->Val->NativePointer;
	info = Param[1]->Val->NativePointer;
	flags = Param[2]->Val->NativePointer;
	src = Param[3]->Val->NativePointer;
	size = Param[4]->Val->Integer;

	rc = mon_tfsadd(name,info,flags,src,size);
	ReturnValue->Val->Integer = rc;
}

/* list of all library functions and their prototypes */
struct LibraryFunction PlatformLibrary[] =
{
    { Cstrtol,		"int strtol(char *,char **,int)" },
    { Cgetenv,      "char *getenv(char *)" },
    { Csetenv,      "void setenv(char *,char *)" },

    { Ctfsadd,      "int tfsadd(char *,char *,char *,char *,int)" },

    { Csystem,      "void system(char *)" },
    { NULL,         NULL }
};

