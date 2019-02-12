#include <graphics.h>
#include <stdio.h>
#include <stdarg.h>
#include <dos.h>
#include <alloc.h>
#include "tools.h"
#include "extern.h"
#include "mouse.h"
#include "hz.h"

void  tx_fk(int x, int y, int color)
 { if(x<0||y<0)  return;
   setfillstyle(1, color);
   if(imouse_inbox(x*22+LEFT-25, y*22+TOP-25, (x+1)*22+LEFT, (y+1)*22+TOP ) )
      mouse_off();
   bar(x*22+LEFT+1, y*22+TOP+1, (x+1)*22+LEFT-1, (y+1)*22+TOP-1);
   mouse_on();
 }

void  tx_els(int x, int y, int fk, int round, int color)
 { int i, j;
   for(j=0; j<4; j++)
    for(i=0; i<4; i++)
      if(FK[fk][round] & ((unsigned)0x8000>>(j*4+i)))
	tx_fk(x+i, y+j, color);
  }

int  deal_tx(int dx, int dy, int round)
 { int k;
   if(ideal(dx, dy, round))  return  -1;
   tx_els(X, Y, F, R, BLACK);
   X += dx; Y += dy; R+=round;
   R=R>=4?0:R<0?3:R;
   if(F==7)
     for(k=0; k<10; k++)
       if(BJ[Y-dy]&((unsigned)0x8000>>(3+k)))  tx_fk(k, Y-dy, Color);
       else   tx_fk(k, Y-dy, BLACK);
   tx_els(X, Y, F, R, F==7?Color==RED?GREEN:RED:Color);
   return 0;
 }



int  ideal(int dx, int dy, int round)
 { int  r = R, x=X, y=Y, k;
   unsigned  ZS=0;
   x+=dx; y+=dy; r+=round;
   r=r>=4?0:r<0?3:r;
   if(F!=7)
    { for(k=0; k<4; k++)
       ZS |= ((BJ[y+k]>>(9-x))&0x000f)<<((3-k)*4);
      return  ZS&FK[F][r];
    }
   else
    { for(k=y; k<20; k++)
       if((BJ[k]&((unsigned)0x8000>>(x+4)))==0)  return 0;
      return 1;
    }
 }

void  addinbj()
 { int k;
   for(k=0; k<4; k++)
    { if(Y+k<0)  continue;
      BJ[Y+k] |= ((FK[F][R]>>(3-k)*4)&0x000f)<<(9-X);
    }
 }

void  accept()
 { int k, j, n=0;
   static  faceflag = 0;
   for(k=19; k>0; k--)
    { if(BJ[k]==0xffff)
	{ for(j=k++; j>0; j--)  BJ[j] = BJ[j-1]; n++; }
    }
   if(n)  BJ[0] = 0xe007;
   if(F==7)  tx_fk(X+1, Y, Color);
   settextstyle(1, 0, 6);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW);
   outtextxy(320, 240, n==4?"Great!!!!":n==3?"Best!!!":n==2?"Good!!":
		       n==1?"OK!":"");
   settextstyle(1, 0, 4);
   setcolor(LIGHTBLUE);
   outtextxy(320, 290, n==4?"+1500":n==3?"+800":n==2?"+300":
		       n==1?"+100":"");
   if(n!=0)
     { Sound(0); delay(600); Sound(3); }
   faceflag += n;
   if(n==0)  faceflag = faceflag==0?0:faceflag-1;
   if(n)     face(1);
   else  if(Ispace()==-1)  { faceflag = 0;  face(-1); }
   else  face(faceflag==0?0:1);
   setcolor(BLACK);
   settextstyle(1, 0, 6);
   outtextxy(320, 240, n==4?"Great!!!!":n==3?"Best!!!":n==2?"Good!!":
		       n==1?"OK!":"");
   settextstyle(1, 0, 4);
   outtextxy(320, 290, n==4?"+1500":n==3?"+800":n==2?"+300":
		       n==1?"+100":"");
   renew();
   while(kbhit())   getch();
   FS += n==4?1500L:n==3?800L:n==2?300L:n==1?100L:0L;
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setfillstyle(0, 0);
   mouse_off();
   bar(440, 180, 640, 260);
   settextstyle(1, 0, 4);
   setcolor(RED);       _outtextxy(535, 200, "Test: %ld", FS);
   setcolor(YELLOW);    _outtextxy(537, 202, "Test: %ld", FS);
   settextstyle(1, 0, 3);
   setcolor(LIGHTGRAY);     _outtextxy(535, 240, "MAX: %ld", maxFS);
   if(FS>=FS0)
    { if(++speed>15)  speed = 0;
      FS0+=PASS; write_msg();
      setcolor(LIGHTGREEN);
      show_str(320, 240, 5, "Excellent!!");
      renew();
    }
   mouse_on();
 }

void  round_case()
 { int k;
   if(X<3)
    for(k=1; k<=3; k++)
     { if(ideal(k, 0, 0))  return;
       if(deal_tx(k, 0, 1)==0) return;
     }
   else
    for(k=-1; k>=-3; k--)
     { if(ideal(k, 0, 0))  return;
       if(deal_tx(k, 0, 1)==0) return;
     }
 }

void  end_deal()
 { if(FS > maxFS)  { maxFS = FS;  face(1); }
   else   if(FS == maxFS) face(0);
   else            face(-1);
   setcolor(WHITE);
   show_str(320, 240, 4, "GAME OVER!");
 }

void  show_str(int x, int y, int size, char *MSG)
 { int k;
   setwritemode(XOR_PUT);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   mouse_off();
   outtextxy(x, y, MSG);
   for(k=1; k<size; k++)
    { settextstyle(1, 0, k);
      outtextxy(x, y, MSG);
      settextstyle(1, 0, k+1);
      outtextxy(x, y, MSG);
      delay(70);
    }
   setwritemode(COPY_PUT);
   outtextxy(x, y, MSG);
   mouse_on();
   delay(2000);
   dTime = clock();
   while(!kbhit() && getmousebutton()==NOBUTTON);
   kbhit()&&!getch()?getch():0;
   Time0 += clock()-dTime;
   setcolor(BLACK);
   mouse_off();
   outtextxy(x, y, MSG);
   mouse_on();
 }

int  readx(int F, int R, int code)
 { if(FK[F][R]&((unsigned)0xf000>>(code*4)))  return -1;
   return 0;
 }

int  ready(int F, int R, int code)
 { int k;
   for(k=0; k<4; k++)
    if(FK[F][R]&((unsigned)0x8000>>(k*4+code)))  return -1;
   return 0;
 }

void show_left_els(int F0, int R0)
 { int w, h, k, n=0,m=0, x, y, i, j;
   for(k=0; k<4; k++)  if(readx(F0, R0, k))   m++;
   h = m*22;
   for(k=0; k<4; k++)  if(ready(F0, R0, k))   n++;
   w = n*22;
   x = 40+(100-w)/2;
   y = 40+(100-h)/2;
   for(k=0; k<4; k++)  if(ready(F0, R0, k))   break;
   x -= k*22;
   for(k=0; k<4; k++)  if(readx(F0, R0, k))    break;
   y -= k*22;
   setfillstyle(0, 0);
   mouse_off();
   bar(42, 42, 138, 138);
   setfillstyle(1, Color);
   for(j=0; j<4; j++)
    for(i=0; i<4; i++)
      if(FK[F0][R0] & ((unsigned)0x8000>>(j*4+i)))
	bar(i*22+x+1, j*22+y+1, (i+1)*22+x-1, (j+1)*22+y-1);
   mouse_on();
 }

void  far  _outtextxy(int x, int y, char *format, ...)
 { char str[200];
   va_list  arglist;
   va_start(arglist, format);
   vsprintf(str, format, arglist);
   va_end(arglist);
   mouse_off();
   outtextxy(x, y, str);
   mouse_on();
 }

void  renew()
 { int k, j;
   for(k=0; k<20; k++)
    { for(j=0; j<10; j++)
       if(BJ[k]&((unsigned)0x8000>>(3+j)))  tx_fk(j, k, Color);
       else   tx_fk(j, k, BLACK);
    }
 }

char  *_mktime(long time)
 { int  hour, minute, second;
   static char str[10];
   char Hour[3], Minute[3], Second[3];
   hour = time/3600;
   time %= 3600;
   minute = time/60;
   time %= 60;
   second = time;
   Hour[0]=hour/10+'0'; Hour[1]=hour%10+'0'; Hour[2]='\0';
   Minute[0]=minute/10+'0'; Minute[1]=minute%10+'0'; Minute[2]='\0';
   Second[0]=second/10+'0'; Second[1]=second%10+'0'; Second[2]='\0';
   sprintf(str, "%s:%s:%s", Hour, Minute, Second);
   return  str;
 }

void  face(int cmd)
 { void _arc(int,int,int,int,int);
   setlinestyle(0, 0, 3);
   setwritemode(COPY_PUT);
   setcolor(YELLOW);
   circle(540, 100, 50);
   setfillstyle(0, 0);
   floodfill(550, 100, YELLOW);
   setcolor(WHITE);
   mouse_off();
   setlinestyle(0, 0, 3);
   switch(cmd)
    { case -1: _arc(515, 75, 220, 320, 15);
	       _arc(565, 75, 220, 320, 15);
	       _arc(540, 140, 40,  140, 20);
	       break;
      case 0:  line(505, 87, 525, 87);
	       line(555, 87, 575, 87);
	       line(527, 125, 553, 125);
	       break;
      case 1:  _arc(515, 100, 40, 140, 15);
	       _arc(565, 100, 40, 140, 15);
	       _arc(540, 110, 220, 320, 20);
	       break;
    }
   mouse_on();
   setlinestyle(0, 0, 1);
 }

void  _arc(int a, int b, int c, int d, int e)
 { setlinestyle(0, 0, 3);
   arc(a, b, c, d, e);
 }


int  Ispace()
 { int k, j, flag=0, m, n;
   for(k=0; k<4; k++)
     if(readx(F, R, k)) { if(flag==0) { m = k; flag = 1; } }
     else  if(flag==1)  break;
   n = k;
   for(k=0; k<4; k++)
    { flag = 0;
      if(ready(F, R, k)==0)  continue;
      for(j=m; j<=n; j++)
       { if(BJ[Y+j]&((unsigned)0x8000>>(3+X+k)))  flag = 1;
	 else if(flag==1)  return -1;
       }
    }
   return 0;
 }

int  show_menu(int num, ...)
 { int X, Y, i, k;
   int X1, Y1, t=0;
   int X0, Y0, n, n0;
   int Wide, High;
   int eHigh = gethz_high(0,0)+2;
   char  *p, *ptr=NULL;
   va_list  arglist;
   sethz_text(LEFT_HZ, TOP_HZ);
   sethz_speed(0);
   getmouse_xy(&X, &Y);
   High = (gethz_high(0,0)+2)*num;
   va_start(arglist, n);
   for(k=0; k<num; k++)
    { int begin = va_arg(arglist, int);
      int end   = va_arg(arglist, int);
      if(end-begin>t)  t = end-begin;
    }
   va_end(arglist);
   Wide = gethz_wide(0, t)+20;

   if(X+Wide+6>639)  X1=X-Wide;
   else            X1=X+Wide;
   if(Y+High+6>479)  Y1=Y-High;
   else            Y1=Y+High;
   if(Y1<0)   { Y1 = (480-High)/2; Y = Y1+High; }
   if(X>X1) { t=X; X=X1; X1=t; }
   if(Y>Y1) { t=Y; Y=Y1; Y1=t; }
   p = (char *)malloc(imagesize(X, Y, X1+6, Y1+6));
   mouse_off();
   getimage(X, Y, X1+6, Y1+6, p);
   t = 0;
   va_start(arglist, n);
   for(i=0; i<num; i++)
    { int  begin = va_arg(arglist, int);
      int  end   = va_arg(arglist, int);
      setfillstyle(1, DARKGRAY);
      bar(X+6, Y+i*eHigh+6, X1+6, Y+(i+1)*eHigh+6);
      setfillstyle(1, GREEN);
      bar(X, Y+i*eHigh, X1, Y+(i+1)*eHigh);
      writehzs(X+10, Y+i*eHigh+1, begin, end);
      delay((100-5*num)<0?0:100-5*num);
    }
   va_end(arglist);
   mouse_on();
   n0 = -1;
   while(getmouse_xy(&X0, &Y0)!=LEFTBUTTON)
    { if(imouse_inbox(X, Y, X1, Y1)==YES)  n = (Y0-Y)/eHigh;
      else        n = -1;
      if(kbhit() && getch()==27)  { n = -1;  break; }
      if(n0!=n)
       { if(ptr!=NULL)
	  {  ptr = (char *)malloc(imagesize(X, Y+n0*eHigh, X1, Y+(n0+1)*eHigh));
	     mouse_off();
	     getimage(X, Y+n0*eHigh, X1, Y+(n0+1)*eHigh, ptr);
	     putimage(X, Y+n0*eHigh, ptr, NOT_PUT);
	     mouse_on();
	     free(ptr);
	  }
	 if(n==-1)  { ptr=NULL; n0 = -1; }
	 else
	  {  ptr = (char *)malloc(imagesize(X, Y+n*eHigh, X1, Y+(n+1)*eHigh));
	     mouse_off();
	     getimage(X, Y+n*eHigh, X1, Y+(n+1)*eHigh, ptr);
	     putimage(X, Y+n*eHigh, ptr, NOT_PUT);
	     mouse_on();
	     free(ptr);
	     n0 = n;
	  }
       }
    }
   while(getmouse_xy(&X0, &Y0)!=NOBUTTON);
   mouse_off();
   putimage(X, Y, p, COPY_PUT);
   mouse_on();
   free(p);
   delay(500);
   return n;
 }