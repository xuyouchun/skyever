#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "move.h"
#include "tools.h"
#include "extern.h"
#include "mouse.h"


void  writetx(int code)
 { setfillcolor(RED);
   if(MSG[code].Active==0) setlinkcolor(DARKGRAY);
   else                    setlinkcolor(GREEN);
   flood(code);
 }


void  writetxs()
 { int k, m, layer[4], min, flag;
   char  str[4];
   static  long  num=0;
   int color = getcolor();
   getmiddle();
   if(active)  cirs(middlex0, middley0, BLACK);
   for(k=0; k<4; k++)  layer[k] = MSG[k].Layer;
   if(num++%5==0)  putimage(0, 0, ptr, COPY_PUT);
   for(k=0; k<4; k++)
    { min = 10;
      for(m=0; m<4; m++)
       if(min>layer[m])
	 {  min = layer[m];  flag = m; }
      writetx(flag);
      layer[flag] = 20;
    }
   if(active == 0)  return;
   cirs(middlex, middley, YELLOW);
   if(KEY==1)
    { setcolor(LIGHTGREEN);
      settextstyle(1, 0, 2);
      settextjustify(CENTER_TEXT, CENTER_TEXT);
      for(k=0; k<4; k++)
	{ sprintf(str, "F%d", k+1);
	  outtextxy(MSG[k].XX, MSG[k].YY, str);
	}
    }
   setcolor(color);
 }

void  cirs(double mx, double my, int _color)
 { int color=getcolor();
   mouse_off();
   setcolor(_color);
   line(mx-14, my, mx+14, my);
   line(mx, my-14, mx, my+14);
   circle(mx, my, 8);
   mouse_on();
   setcolor(color);
 }

void  cleartx()
  { int k;
    setfillcolor(BLACK);
    setlinkcolor(BLACK);
    cirs(middlex0, middley0, BLACK);
    for(k=0; k<4; k++)
     if(MSG[k].Active==1)
       { flood(k);  link(k); }
  }

void  getmiddle()
 { int k;  double S;
   middlex0 = middlex; middley0 = middley;
   for(k=0; k<4; k++)
    { if(MSG[k].Active==1)
       { middlex = MSG[k].XX;
	 middley = MSG[k].YY;
	 S = MSG[k].S;
	 break;
       }
    }
   for(k++; k<4; k++)
    { if(MSG[k].Active == 0)  continue;
      middlex = (S*middlex+MSG[k].S*MSG[k].XX) / (S+MSG[k].S);
      middley = (S*middley+MSG[k].S*MSG[k].YY) / (S+MSG[k].S);
      S += MSG[k].S;
    }
 }

int  passtime(long t)
 { long  time;
   if(timeflag==0)
     { _mktime(S, t/18.2);
       return  1;
     }
   if(timeflag==1)
    { CLOCK0 = clock()-t;
      BKC = 0;
      strcpy(S, "00:00:00");
      timeflag = 2;
      return  1;
    }
   CLOCK = clock()-CLOCK0;
   time = CLOCK/18.2;
   if(time == BKC)  return 0;
   BKC = time;
   _mktime(S, time);
   return  1;
 }

void  timestr()
 { int k, X, Y, flag=0;
   static char str[20];
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 4);
   setfillstyle(0, 0);
   setwritemode(COPY_PUT);
   getmouse_xy(&X, &Y);
   if(Y<35 && X>480) mouse_off();
   bar(500, 5, 640, 40);
   for(k=0; k<4; k++)
     if(inrect(500, -95, 800, 40, k, 1)==IN||idotin(k, 500, 40))
       {  flag = 1;  writetxs(k);  break; }
   if(active&&flag==1) cirs(middlex, middley, YELLOW);
   setcolor(DARKGRAY);
   outtextxy(559, 19, S);
   setcolor(LIGHTGREEN);
   outtextxy(562, 22, S);
   if(MENU==1)  { mouse_on(); return; }
   setcolor(GREEN);
   sprintf(str, "%d/%d - %ld", passed+1, txnum0, Test);
   outtextxy(100, 450, "Skyever");
   outtextxy(540, 450, "Ver : 1.1");
   setcolor(LIGHTGRAY);
   outtextxy(320, 450, str);
   mouse_on();
 }


void  _mktime(char *S, long time)
 { int  hour, minute, second;
   char Hour[3], Minute[3], Second[3];
   hour = time/3600;
   time %= 3600;
   minute = time/60;
   time %= 60;
   second = time;
   Hour[0]=hour/10+'0'; Hour[1]=hour%10+'0'; Hour[2]='\0';
   Minute[0]=minute/10+'0'; Minute[1]=minute%10+'0'; Minute[2]='\0';
   Second[0]=second/10+'0'; Second[1]=second%10+'0'; Second[2]='\0';
   sprintf(S, "%s:%s:%s", Hour, Minute, Second);
 }