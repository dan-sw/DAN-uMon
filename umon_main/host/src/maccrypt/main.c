/* maccrypt.c:
 * This tool uses the UNIX password encryption function (crypt()) to encrypt
 * a MAC address.  Encryption of a MAC address is used to provide a back
 * door entry into embedded systems that require login/password to
 * access the system.  The backdoor is provided to support the possibility
 * of a customer losing their password...

 * Each system has some "backdoor" username (for example, 'lucent_su') that
 * violates the acceptable syntax of a normal username.  This eliminates
 * the possibility of a customer attempting to use the same username as
 * our backdoor username.  Each time a login is attempted, the username
 * is first compared to the backdoor username, if it matches, and the
 * password matches the encryption of that system's MAC address, then
 * the login is approved with superuser priveledges.  If the username is
 * NOT the backdoor username, then all normal login restrictions apply.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "version.h"
#ifndef BUILD_WITH_VCC
#ifndef O_BINARY
#define O_BINARY	0
#endif
#include <unistd.h>
#endif

#include "version.h"

extern int optind;
extern char *optarg;

#ifdef UNIQUE_DATATBL
#include "unique_data_file.c"
#else
unsigned char *umon_crypt_datatbl = (unsigned char *)"5gfHHJ^RVGJF#@FNM_NJE#W)(UJH(&T*%Ec54evX6t-098yun-9um9u8yv785rc5e43w3xw543wx43wx54ex7%R*^&RC*&TV_*(U_)(UN_)U*B76rc764ex643wx34ec6t098yn=9ui=n09im[pim[pimo(UB*)&GVB*&^FC$EX^$&*Y_(U*N(_*UN)9h8b087gv87f6cuytfghfvu654dx#SX53q2ax74f0p8u-[0in=-i-9ub098v087gv987g";
#endif

#define UNIX_CRYPT	'u'
#define CHEAP_CRYPT	'c'

int
EtherToBin(char *ascii,unsigned char *binary)
{
	int	i;
	char	*acpy;

	acpy = ascii;
	for(i=0;i<6;i++) {
		binary[i] = (unsigned char)strtol(acpy,&acpy,16);
		if ((i != 5) && (*acpy++ != ':')) {
			printf("Misformed ethernet addr: %s\n",ascii);
			return(-1);
		}
	}
	return(0);
}

/* "cheap" crypt::
	Use 256 bytes of random data as a very simple means of encrypting some
	data.  The result can be a maximum size of twice the incoming string.

	If a character from the incoming string has either '00000110' bits set,
	then use that character twice.  This allows the encrypted string
	to be a varying length based on the content of the string.

	Use the first two characters of "setting" plus the checksum of the 
	characters pointed to by result as the initial offset into the data table.
	This makes is less likely that passwords with similar starting characters
	will have similar starting characters in the encryption.
*/

char *
cheap_crypt(char *string, char *setting, char *result)
{
	int	offset, csum;
	char *cryptresult, *cp;

	csum = 0;
	cp = string;
	while(*cp)
		csum += *cp++;
	cryptresult = result;

	/* Set up an offset into the data table that is based on the checksum	*/
	/* of the incoming string plus the sum of the two setting characters...	*/
	offset = (csum + setting[0] + setting[1]) % 255;

	/* For each character in the incoming string, replace it with a			*/
	/* character in the data table.  If the incoming character has either	*/
	/* 00000110 bit set, then use that character twice.						*/
	while(*string) {
		offset += (int)*string;
		offset = offset % 255;
		*result = umon_crypt_datatbl[offset];
		if (*string & 6) {
			result++;
			offset += (int)*string;
			offset = offset % 255;
			*result = umon_crypt_datatbl[offset];
		}
		string++;
		result++;
	}
	*result = 0;
	return(cryptresult);
}

char *errmsg[] = {
	"Usage:",
	" maccrypt [options] {MAC}",
	"Options:",
	" -f{fname}  override default crypt data table with content of file.",
	" -u     use Unix crypt.",
	" -V     show version of maccrypt.exe.",
	"\n",
	0,
};

void
usage(error)
char	*error;
{
	int	i;

	if (error)
		fprintf(stderr,"ERROR: %s\n",error);
	for(i=0;errmsg[i];i++)
		fprintf(stderr,"%s\n",errmsg[i]);
	exit(1);
}


/* main():
 * The incoming argument is assumed to be a MAC address formatted as
 * six ascii-hex bytes each separated by a colon.  From this we
 * extract an 8-byte "key" and 2-byte "salt" to be passed to crypt().
 * The crypt() function returns an encrypted string that represents
 * that MAC address.
 */
int
main(int argc,char *argv[])
{
	int		i, opt, crypttype;
	char	*rslt, *string, *cryptfile;
	unsigned char salt[3], buffer[64], etherbin[16];
	struct	stat	mstat;
	extern	char *orig_crypt(char *,char *);

	cryptfile = (char *)0;
	crypttype = CHEAP_CRYPT;
	while((opt=getopt(argc,argv,"f:uV")) != EOF) { 
		switch(opt) {
		case 'f':
			cryptfile = optarg;
			break;
		case 'u':
			crypttype = UNIX_CRYPT;
			break;
		case 'V':
			showVersion();
			break;
		}
	}

	if (argc != optind+1)
		usage(0);

	if (cryptfile != (char *)0) {
		unsigned char c;
		int	fd, data_tot;

		/* Open alternate crypt data file: */
		fd = open(cryptfile,O_RDONLY | O_BINARY);
		if (fd == -1) {
			perror(cryptfile);
			usage(0);
		}
		fstat(fd,&mstat);

		/* Read the first 256 printable bytes of the file in and overwrite
		 * the default datatbl[] array...
		 */
		data_tot = 0;
		while(data_tot < 256) {
			if (read(fd,(char *)&c,1) != 1) {
				fprintf(stderr,"%s does not have enough useable data\n",
					cryptfile);
				exit(1);
			}
			if ((c > 0x20) && (c < 0x7f))
				umon_crypt_datatbl[data_tot++] = c;
		}
		close(fd);
	}

	string = argv[optind];

	/* 2 salt characters are the first and last characters of string: */
	salt[0] = string[0];
	salt[1] = string[strlen(string)-1];
	salt[2] = 0;

	if (EtherToBin(string,etherbin) != 0)
		exit(1);

	etherbin[6] = 'A';
	etherbin[7] = 'a';
	etherbin[8] = 0;

	for(i=0;i<6;i++) {
		while (etherbin[i] > 0x7e) {
			etherbin[i] -= 0x20;
			etherbin[6]++;
		}
		while (etherbin[i] < 0x20) {
			etherbin[i] += 0x8;
			etherbin[7]++;
		}
	}

	switch(crypttype) {
		case UNIX_CRYPT:
			rslt = orig_crypt((char *)etherbin,(char *)salt);
			break;
		case CHEAP_CRYPT:
			rslt = cheap_crypt((char *)etherbin,(char *)salt,(char *)buffer);
			break;
		default:
			fprintf(stderr,"Invalid encryption type\n");
			break;	
	}
	
	printf("Encryption for MAC %s: %s\n",string,rslt+2);
	
	exit(0);
}

