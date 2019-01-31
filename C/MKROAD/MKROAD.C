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
 { printf("该程序可以将所输入的汉字转化为坦克大战中的阵地,请先设置PATH中\
字库的位置,以便搜索,或者输入全文件名,请按提示输入内容:\n\n\n");

   printf("请输入汉字库文件:(hzk16,hzk16f,hzk24s...)");  scanf("%s", hzkname);
   hzkfp = fopen(searchpath(hzkname), "rb");
   if(hzkfp==NULL)
     { printf("错误:未发现所指定的汉字库!");  return 1; }
   printf("请输入汉字库点阵数:(16,24,48...)"); scanf("%d", &dot);  clear();
   if(dot==16)  offset = 0;   else  offset = 1410;
   printf("请输入汉字库的方向:(0-横向  1-纵向)");
   scanf("%d", &fx);   clear();

   begin: printf("请输入文件名:");  scanf("%s", filename);  clear();
   if(access(filename,0)==0)
     { printf("覆盖掉已存在的文件%s?(Y/N)", filename);
       if((k=getch())!='Y' && k!='y')  goto begin;
       putchar('\n');
     }
   fp = fopen(filename, "wb");
   if(fp==NULL)
     { printf("错误:无法打开文件%s!", filename);  goto begin; }
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  Road[j][i] = 5;
   while(1)
    { long OFFSET; int qh, wh, x, y;  int kind;
      printf("\n\n请按回车键继续,按ESC返回......");
      if(getch()==27)  break;
      printf("\n请输入第%d个汉字:", N+++1);
      scanf("%s", str);
      qh = (unsigned char)str[0]-0xa0;
      wh = (unsigned char)str[1]-0xa0;
      OFFSET = (94*(qh-1)+(wh-1) - offset) * (dot*dot/8L);
      fseek(hzkfp, OFFSET, SEEK_SET);
      printf("请输入横坐标:"); scanf("%d", &x);  clear();
      printf("请输入纵坐标:"); scanf("%d", &y);  clear();
      printf("请输入类型:(0-砖 1-冰 2-铁 3-海 4-林 5-路)");
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


