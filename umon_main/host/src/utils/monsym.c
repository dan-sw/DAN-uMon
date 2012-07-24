#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "version.h"
#include "utils.h"

#define	LINESIZE 256

extern	int optind;
extern	char *optarg;
extern	long strtol();

char	*getword(char *,int,int);

#define MAXSYMSIZE 63

struct symline {
	char			symbol[MAXSYMSIZE+1];
	unsigned long	value;
};

int		debug, symTot, truncTot, verbose;
FILE	*symfp = (FILE *)NULL;

/* getword():
	return the "word"th whitespace delimited character string found on
	the "line"th line of "string".
	String is assumed to be null terminated.
*/

char *
getword(char *string, int word, int line)
{
	int		i;
	char	*retword, *p;

	while (isspace((int)*string))
		string++;
	for (i=1;i<line;i++) {
		while (*string != '\n') {
			if (*string++ == '\0')
				return("");
		}
		while (isspace((int)*string))
			string++;
	}
	for (i=1;i<word;i++) {
		while ((*string != ' ') && (*string != '\t')) {
			if (*string++ == '\0')
				return("");
		}
		while (isspace((int)*string))
			string++;
	}
	retword = Malloc(strlen(string));
	p = retword;
	while (!isspace((int)*string)) {
		if (*string == '\0')
			break;
		*p++ = *string++;
	}
	*p = '\0';
	if (strlen(retword) > MAXSYMSIZE) {
		if (verbose)
			fprintf(stderr,"Symbol truncated: %s\n",retword);
		truncTot++;
	}
	return(retword);
}

char *usage_txt[] = {
	"Usage: monsym [options] {infile}",
	" Convert a tabulated file of symbol/value combinations to the format",
	" used by MicroMonitor's command line symbol retrieval...",
	"",
	" Options:",
	" -d{data_col}     column in input file that contains data (default=1)",
	" -s{symbol_col}   column in input file that contains symbol (default=3)",
	" -l{max_ln_size}  maximum accepted line size (default=64)",
	" -p{data_prefix}  prefix added to data column",
	" -P{sym_prefix}   prefix added to symbol column",
	" -S{x|d}          sort numerically based on data column (hex or decimal)",
	" -V               display version of tool",
	" -v               verbose mode",
	0,
};

int
main(int argc,char *argv[])
{
	char *dataPrefix, *symbolPrefix, *fmt, line[LINESIZE];
	int	opt, i, j, sortbase;
	int	symbolColumn, dataColumn, lineMax;
	struct	stat	finfo;
	struct symline	*symLines, *slp;

	truncTot = symTot = 0;
	symLines = (struct symline *)0;
	sortbase = debug = verbose = 0;
	symbolColumn = 3;
	dataColumn = 1;
	dataPrefix = "";
	symbolPrefix = "";
	lineMax = 64;
	while((opt=getopt(argc,argv,"d:l:P:p:s:S:vV")) != EOF) {
		switch(opt) {
		case 'd':
			dataColumn = atoi(optarg);
			break;
		case 'l':
			lineMax = atoi(optarg);
			break;
		case 'P':
			symbolPrefix = optarg;
			break;
		case 'p':
			dataPrefix = optarg;
			break;
		case 'S':
			if (*optarg == 'x')
				sortbase = 16;
			else if (*optarg == 'd')
				sortbase = 10;
			else
				usage(0);
			break;
		case 's':
			symbolColumn = atoi(optarg);
			break;
		case 'V':
			showVersion();
			break;
		case 'v':  
			verbose = 1;
			break;
		default:
			usage(0);
		}
	}

	if (argc != optind+1)
		usage(0);

	/* Open the symbol file (built by nm): */
	if ((symfp = fopen(argv[optind],"r")) == (FILE *)NULL) {
		perror(argv[optind]);
		usage(0);
	}

	if (stat(argv[optind],&finfo) == -1) {
		perror(argv[optind]);
		usage(0);
	}

	if (sortbase) {
		int	line_cnt;
		char *symbol, *data;

		line_cnt = 0;
		while(1) {
			if (fgets(line,LINESIZE,symfp) == NULL)
				break;
			line_cnt++;
		}

		symLines = (struct symline *)Malloc(line_cnt * sizeof(struct symline));
		slp = symLines;
		rewind(symfp);

		while(1)
		{
			if (fgets(line,LINESIZE,symfp) == NULL)
				break;
	
			/* Get the symbolname in column "symbolColumn": */
			symbol = getword(line,symbolColumn,1);
			if (*symbol == '\0')
				continue;
	
			/* Get the data in column "dataColumn": */
			data = getword(line,dataColumn,1);
			if (*data == '\0') {
				free(symbol);
				continue;
			}
			
			symTot++;
			slp->value = strtoul(data,0,sortbase);
			strncpy(slp->symbol,symbol,MAXSYMSIZE);
			slp->symbol[MAXSYMSIZE] = 0;
			free(symbol);
			free(data);
			slp++;
		}

		/* Run a bubble sort on that list based on slp->value, then print
		 * out the list...
		 */
		for(i=1;i<line_cnt;++i) {
			struct symline tmpsymline;

			for(j=line_cnt-1;j>=i;--j) {
				if ((symLines[j-1].value > symLines[j].value) > 0) {
					tmpsymline = symLines[j-1];
					symLines[j-1] = symLines[j];
					symLines[j] = tmpsymline;
				}
			}
		}

		if (sortbase == 16)
			fmt = "%s%s %s%x\n";
		else
			fmt = "%s%s %s%d\n";

		for(i=0;i<line_cnt;i++) {
			printf(fmt,
				symbolPrefix,symLines[i].symbol,
				dataPrefix,symLines[i].value);
		}
		
		free(symLines);
	}
	else {
		while(1)
		{
			int	lsize;
			char *symbol, *data, outline[LINESIZE];
	
			if (fgets(line,LINESIZE,symfp) == NULL)
				break;
	
			/* Get the symbolname in column "symbolColumn": */
			symbol = getword(line,symbolColumn,1);
			if (*symbol == '\0')
				continue;
	
			/* Get the data in column "dataColumn": */
			data = getword(line,dataColumn,1);
			if (*data == '\0') {
				free(symbol);
				continue;
			}
			symTot++;
			lsize = sprintf(outline,"%s%s %s%s",
				symbolPrefix,symbol,dataPrefix,data);
			if (lsize <= lineMax)
				puts(outline);
			free(symbol);
			free(data);
		}
	}
	fprintf(stderr,"%d symbols processed\n",symTot);
	if (truncTot)
		fprintf(stderr,"%d symbols truncated\n",truncTot);
	if (symfp)
		fclose(symfp);
	return(0);
}

