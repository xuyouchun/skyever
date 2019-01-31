#include <graphics.h>
#include <stdio.h>

void show_hz(int x, int y, unsigned char *C);

#define X1 142
#define X2 470
#define Y 132

unsigned char  HZ[300] =
"走在乡间的小路上，暮归的老牛是我同伴，蓝天佩朵夕阳在胸膛，缤纷的云彩是晚"
"霞的衣裳，荷把锄头在肩上，牧童的歌声在荡漾，哦哦哦哦他们唱，还有一支短笛"
"隐约在吹响，笑意写在脸上，哼一曲乡居小唱，任思绪在晚风中飘扬，多少落寞惆"
"怅都随晚风飘散，遗忘在乡间的小路上。";

FILE  *fp;

int main()
 {  int graphdriver=DETECT, graphmode;
	int K = 0, x=X1+32, y=Y;
	fp = fopen("c:\\ucdos\\hzk16", "rb");
	if(fp==NULL)
	 { printf("Error: Can't open hzk file \"hzk16\"!");
	   exit(1);
	 }
	initgraph(&graphdriver, &graphmode, "c:\\tc");
	cleardevice();

	while(HZ[K]!='\0')
	 { show_hz(x, y, HZ+K);
	   K += 2;  x += 16;
	   if(x>X2-20)
		 { x = X1; y += 16; }
	 }

	getch();
	closegraph();
	fclose(fp);
	exit(0);
 }

void show_hz(int x, int y, unsigned char *C)
 {  int i, j, k;
	unsigned char ch;
	int qh, wh;
	long  offset;
	/***   计算偏移量内码   ***/

	qh = C[0]-0xa0;   /* 区码 */
	wh = C[1]-0xa0;   /* 位码 */
	offset = ((qh-1)*94+(wh-1)) * 32L;
	fseek(fp, offset, SEEK_SET);

	for(j=0; j<16; j++)
	 { for(i=0; i<2; i++)
		{ ch = fgetc(fp);
		  for(k=0; k<8; k++)
			 if(ch&(0x80>>k))  	 putpixel(x+i*8+k, y+j, GREEN);
		}
	 }
 }