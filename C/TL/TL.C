#include <stdio.h>
#include <graphics.h>
#include <key.h>


#define  X 60
#define  Y 120
void  show_day(int N, int week)
 { int k=1, i=week, j=0, K;
   char  str[10];
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 3);
   setcolor(LIGHTGREEN);
   for(K=1; K<=7; K++)
    { sprintf(str, "%d", K);
      outtextxy(X+(K-1)*80, Y-20, str);
    }
   i %= 7;
   setcolor(GREEN);
   settextstyle(1, 0, 5);
   while(k<=N)
    { sprintf(str, "%d", k++);
      if(i>=7)   { i=0;  j ++; }
      outtextxy(X+i++*80, Y+j*60+20, str);
    }
   setcolor(RED);
   for(k=0; k<8; k++)   line(X+80*k-40, Y-5, X+80*k-40, Y+j*60+55);
   for(k=0; k<j+2; k++)   line(X-40, Y+60*k-5, X+520, Y+60*k-5);
 }
void  show_month(int N)
 { char  str[20];
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(LIGHTGRAY);
   settextstyle(1, 0, 6);
   sprintf(str, "Month : %d", N);
   outtextxy(160, 40, str);
 }

main()
 { int month=1, week=2, day=31;  char ch;
   int graphdriver = DETECT, graphmode;
   initgraph(&graphdriver, &graphmode, "C:\\TC\\BGI");
   show_month(month);  show_day(31, week);
   while( (ch=getch()) != ESC_KEY)
    { cleardevice();
      switch(ch)
       { case PAGEUP_KEY: if(--month==0)    month=12; break;
	 case PAGEDOWN_KEY: if(++month>12) month=1;  break;
	 case UP_KEY: if(--day==27)  day = 28; break;
	 case DOWN_KEY: if(++day==32)  day = 31; break;
	 case LEFT_KEY: if(--week==-1) week = 6; break;
	 case RIGHT_KEY: if(++week==7) week = 1; break;
	 case 0: continue;
       }
      show_month(month);
      show_day(day, week);
    }
   getch();
   return 0;
 }

