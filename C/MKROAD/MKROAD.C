#include <stdio.h>
#include <dir.h>
#include <io.h>

char filename[50], hzkname[50];
FILE  *fp, *hzkfp;
int i, j, k, N;
int dot, offset, fx;
unsigned char Road[28][36], str[10];
void clear()  { while(kbhit())  getch(); }

int main()
 { printf("�ó�����Խ�������ĺ���ת��Ϊ̹�˴�ս�е����,��������PATH��\
�ֿ��λ��,�Ա�����,��������ȫ�ļ���,�밴��ʾ��������:\n\n\n");

   printf("�����뺺�ֿ��ļ�:(hzk16,hzk16f,hzk24s...)");  scanf("%s", hzkname);
   hzkfp = fopen(searchpath(hzkname), "rb");
   if(hzkfp==NULL)
     { printf("����:δ������ָ���ĺ��ֿ�!");  return 1; }
   printf("�����뺺�ֿ������:(16,24,48...)"); scanf("%d", &dot);  clear();
   if(dot==16)  offset = 0;   else  offset = 1410;
   printf("�����뺺�ֿ�ķ���:(0-����  1-����)");
   scanf("%d", &fx);   clear();

   begin: printf("�������ļ���:");  scanf("%s", filename);  clear();
   if(access(filename,0)==0)
     { printf("���ǵ��Ѵ��ڵ��ļ�%s?(Y/N)", filename);
       if((k=getch())!='Y' && k!='y')  goto begin;
       putchar('\n');
     }
   fp = fopen(filename, "wb");
   if(fp==NULL)
     { printf("����:�޷����ļ�%s!", filename);  goto begin; }
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  Road[j][i] = 5;
   while(1)
    { long OFFSET; int qh, wh, x, y;  int kind;
      printf("\n\n�밴�س�������,��ESC����......");
      if(getch()==27)  break;
      printf("\n�������%d������:", N+++1);
      scanf("%s", str);
      qh = (unsigned char)str[0]-0xa0;
      wh = (unsigned char)str[1]-0xa0;
      OFFSET = (94*(qh-1)+(wh-1) - offset) * (dot*dot/8L);
      fseek(hzkfp, OFFSET, SEEK_SET);
      printf("�����������:"); scanf("%d", &x);  clear();
      printf("������������:"); scanf("%d", &y);  clear();
      printf("����������:(0-ש 1-�� 2-�� 3-�� 4-�� 5-·)");
      scanf("%d", &kind);  clear();
      for(j=0; j<dot; j++)
       for(k=0; k<dot/8; k++)
	{ unsigned char ch = fgetc(hzkfp);
	  for(i=0; i<8; i++)
	    if(ch&(0x80>>i))
	      if(fx==0)  Road[y+j][x+k*8+i] = kind;
	      else       Road[y+k*8+i][x+j] = kind;
	}
    }
   fputc('S', fp);  fputc('X', fp);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  fputc(Road[j][i], fp);
   fcloseall();   return 0;
 }

