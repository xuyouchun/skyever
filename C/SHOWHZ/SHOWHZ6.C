#include <graphics.h>
#include <stdio.h>

void show_hz(int x, int y, unsigned char *C);

#define X1 142
#define X2 470
#define Y 132

unsigned char  HZ[300] =
"��������С·�ϣ�ĺ�����ţ����ͬ�飬�������Ϧ�������ţ��ͷ׵��Ʋ�����"
"ϼ�����ѣ��ɰѳ�ͷ�ڼ��ϣ���ͯ�ĸ����ڵ�����ŶŶŶŶ���ǳ�������һ֧�̵�"
"��Լ�ڴ��죬Ц��д�����ϣ���һ�����С������˼����������Ʈ�������į�"
"�궼������Ʈɢ������������С·�ϡ�";

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
	/***   ����ƫ��������   ***/

	qh = C[0]-0xa0;   /* ���� */
	wh = C[1]-0xa0;   /* λ�� */
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