#include <graphics.h>
#include <stdio.h>

long  offset;
int   qh, wh;

#define X 312
#define Y 232

int  i, j, k;
unsigned char  HZ[3] = "苏";
unsigned linestyle;

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

	setcolor(GREEN);
	for(j=0; j<16; j++)
	 { linestyle = (unsigned)fgetc(fp)<<8 | fgetc(fp);
	   setlinestyle(4, linestyle, 1);
	   line(X+15, Y+j, X, Y+j);
	 }
	getch();
	closegraph();
	fclose(fp);
	exit(0);
 }
