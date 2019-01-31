#include <graphics.h>
#include <stdio.h>

long  offset;
int   qh, wh;

#define X 242
#define Y 232
#define dx 20
#define dy 5

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

	setfillstyle(1, GREEN);
	for(j=0; j<16; j++)
	 { for(i=0; i<2; i++)
		{ ch = fgetc(fp);
		  for(k=0; k<8; k++)
		   { /*
			 int m, n;
			 if(ch&(0x80>>k))
			   { for(n=0; n<dy; n++)
				   for(m=0; m<dx; m++)
					 putpixel(X+(i*8+k)*dx+m, Y+j*dy+n, GREEN);
			   }
			 */
			 if(ch&(0x80>>k))
				bar(X+(i*8+k)*dx, Y+(j*dy), X+(i*8+k)*dx+dx-1, Y+(j*dy)+dy-1);

		   }
		}
	 }
	getch();
	closegraph();
	fclose(fp);
	exit(0);
 }
