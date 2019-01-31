#include <graphics.h>
#include <stdio.h>

long  offset;
int   qh, wh;

#define X 312
#define Y 232

int  i, j, k;
unsigned char  HZ[3] = "苏";
unsigned char  ch;

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

	for(j=0; j<16; j++)
	 { for(i=0; i<2; i++)
		{ ch = fgetc(fp);
		  for(k=0; k<8; k++)
		   { if(ch&(0x80>>k))
			   putpixel(X+i*8+k, Y+j, GREEN);
		   }
		}
	 }
	getch();
	closegraph();
	fclose(fp);
	exit(0);
 }
