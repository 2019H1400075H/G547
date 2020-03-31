#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<linux/ioctl.h>


#define MAGIC_NUMBER 'A'
#define CHANNEL_SELECT_SEQNO 0x01
#define RESULT_ALIGNMENT_SEQNO 0x02

#define CHANNEL_SELECT _IOW(MAGIC_NUMBER, CHANNEL_SELECT_SEQNO, unsigned long)
#define RESULT_ALIGNMENT _IOW(MAGIC_NUMBER, RESULT_ALIGNMENT_SEQNO, unsigned long)


unsigned int chl;
unsigned int align;
unsigned int res;

int main(int arg,char *files[])
{
	int devfil, usrfil;
	if(arg != 3)
	{
		printf("Usage : copy <oldfile> <newfile>\n");
		exit(1);
	}

	devfil = open(files[1], O_RDONLY);

	if(devfil == -1)
	{
		printf("Can't open the source file\n");
		exit(1);
	}

	printf("Required Channel number of ADC(0-7) : \n");
        scanf("%d",&chl);       
        ioctl(devfil, CHANNEL_SELECT, &chl); 
 
        printf("Required Allignment(0/1) : \n");
	scanf("%d",&align); 
        ioctl(devfil, RESULT_ALIGNMENT, &align);
       
 
        printf("Reading value from ADC...\n");
	read(devfil, &res, 2);
	printf("Value read from ADC - %d \n", res);
        close(devfil);
	exit(0);
}

