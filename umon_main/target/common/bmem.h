struct bmem {
	int id;				
	char *name;
	int pgsize;
	int spsize;
	int ppb;
	int btot;
	int	(*bmemerase)(int bkno, int verbose);
	int	(*bmemread)(char *dest, int bkno, int pgno, int size, int verbose);
	int	(*bmemwrite)(char *src, int bkno, int pgno, int size, int verbose);
};

extern struct bmem bmemtbl[];
