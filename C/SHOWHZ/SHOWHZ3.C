#include <graphics.h>
#include <stdio.h>
#include <dos.h>

long  offset;
int   qh, wh;

#define X 320
#define Y 240

int  i, j, k;
unsigned char  HZ[3] = "苏";
unsigned char  ch;
char far *ptr = (char far*)0xa0000000L;

FILE  *fp;

int main()
 {  int graphdriver=DETECT, graphmode;
	fp = fopen("c:\\ucdos\\hzk16", "rb");
	if(fp==NULL)
	 { printf("Error: Can't open hzk file \"hzk16\"!");
	   exit(1);
	 }
	initgraph(&graphdriver, &graphmode, "c:\\tc");
	cleardevice();

	/***   计算偏移量(内码)   ***/

	qh = HZ[0]-0xa0;   /* 区码 */
	wh = HZ[1]-0xa0;   /* 位码 */
	offset = ((qh-1)*94+(wh-1)) * 32L;
	fseek(fp, offset, SEEK_SET);

	outportb(0x3c4, 2);
	outportb(0x3c5, BLUE);

	ptr += Y*80 + X/8;

	/*for(j=0; j<16; j++)
	  { for(i=0; i<2; i++) 	  *(ptr+j*80+i) = fgetc(fp);
	  }*/

	{ register int j;
	  for(j=0; j<16; j++)
		{  *ptr++ = fgetc(fp);  *ptr++ = fgetc(fp);
		   ptr += 78;
		}
	}

	getch();
	closegraph();
	fclose(fp);
	exit(0);
 }
