#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BUF_SIZE 14336*10         //for 10Mhz
//#define BUF_SIZE 14336*2*10         //for 20Mhz

int main(int argc, char *argv[])
//int main()
{
    int i=0,j=0;
    char str_in[50], str_out[50];
    FILE *fpin[10], *fpout;
    unsigned int *inptr;
    unsigned char *outptr;
    unsigned int localbuf;
    unsigned int bufferSize, bandWidth, antId;

    unsigned int input_buf[14336*2*10];

    inptr = (unsigned int *)&input_buf;
    outptr = (unsigned char *)&input_buf;

#if 1
    //printf(" value of argc is %d\n",argc);
    if((argc == 1))
    {
printf("--------------------------------------------------------------------------------\n");
        printf("   Use \"%s help\" for help on usage \n",argv[0]);
printf("--------------------------------------------------------------------------------\n");
        return(0);
    }
if((argc == 2))
{
    if(((strcmp(argv[1],"help")) == 0))
    {
	printf("--------------------------------------------------------------------------------\n");
        printf("   Usage of the executiable is \n");
        printf("   %s <bandwidth> <antenna ID> \n\n",argv[0]);
        printf("   Please enter only 2 arguments bandwidth in Mhz\n\
   \"10 or 20\" and antID \"0 or 1\"\n\n");
        printf("   Place all the 10 subframe file in the corresponding\n\
   10Mhz\\Ant0,10Mhz\\Ant0,20Mhz\\Ant0 and 20Mhz\\Ant1 folders\n\n");
        printf("   Input file names should named as  frame0_subframe0_rx_fft_in.txt,\n\
   frame0_subframe1_rx_fft_in.txt... so on\n\n");
        printf("   Output binary files will be generated to the folder outputBinary \n");
	printf("--------------------------------------------------------------------------------\n");
        return(0);
    }
    else
    {
	printf("--------------------------------------------------------------------------------\n");
        printf("   Use \"%s help\" for help on usage \n",argv[0]);
	printf("--------------------------------------------------------------------------------\n");
	return(0);
    }
}	

    if((argc != 3))
    {
printf("--------------------------------------------------------------------------------\n");
        printf("   Usage of the executiable is \n");
        printf("   textToBinary.exe <bandwidth> <antenna ID> \n");
        printf("   Please enter only 2 arguments bandwidth in Mhz \"10 or 20\" and antID \"0 or 1\"\n");
printf("--------------------------------------------------------------------------------\n");
        return(0);
    }

    if((atoi(argv[1]) != 10) && ((atoi(argv[1]) != 20)))
    {
printf("--------------------------------------------------------------------------------\n");
        printf(" Please enter a valid bandwidth value in Mhz \"10 or 20\"\n");
printf("--------------------------------------------------------------------------------\n");
        return(0);
    }

    if((atoi(argv[2]) != 0) && ((atoi(argv[2]) != 1)))
    {
printf("--------------------------------------------------------------------------------\n");
        printf(" Please enter a valid Antenna ID \"0 or 1\"\n");
printf("--------------------------------------------------------------------------------\n");
        return(0);
    }

    //printf(" value of arg 3 is %d\n",atoi(argv[3]));
    antId = atoi(argv[2]);
    bandWidth = atoi(argv[1]);
    bufferSize = (14336 * bandWidth);

    for(j=0;j<10;j++)
    {
        sprintf(str_in, "%s%d%s%d%s", "10Mhz/Ant", antId, "/frame0_subframe", j, "_rx_fft_in.txt");
        fpin[j]=fopen(str_in,"r");
        for(i=0;i<(bufferSize/10);i++)
        {
          fscanf(fpin[j],"%x",&localbuf);
          *inptr++ = localbuf;
        }
        fclose(fpin[j]);
    }

    sprintf(str_out, "%s%d%s%d%s", "outputBinary/10_Subframe_Data_", bandWidth, "Mhz_Ant", antId, ".bin");
    fpout=fopen(str_out,"wb");
    fwrite(input_buf, 1, bufferSize*4, fpout);

    fclose(fpout);

#endif
    return(0);
}
