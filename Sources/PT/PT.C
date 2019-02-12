#include <graphics.h>
#include <math.h>
#include <bios.h>
#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "tools.h"
#include "move.h"
#include "mouse.h"
#include "hz.h"
#include "screen.h"
#include "extern.h"
#include "pt.h"
#include "key.h"

int main(int argc, char **argv)
 { int graphdriver = DETECT, graphmode;
   int  k, m, click;
   int  buttonflag, key, key2;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "C:\\TC");
   setbkcolor(BLUE);

   init_tx(argc, argv);

   if(ishowscreen == 1)  screen();
   delay(700);
   size = imagesize(0, 0, 120, 120);
   ptr = (char far *)farmalloc(size);
   show_desk(-1);
   passtime(T[door]);   timestr();


BEGIN
   key = 0;
   if(timeflag && passtime(T[door])==1)    timestr();
   if(kbhit())  key = getch();
   if((buttonflag=getmouse_xy(&X, &Y))==NOBUTTON && !key)  continue;
   click = which_active(X, Y);
   if(buttonflag==LEFTBUTTON)
    { int flag=0;
      KEY = 0;
      X0 = X; Y0 = Y;
      !timeflag&&(timeflag=1);   timestr();
      if(click!=-1)    { tx_click(click); flag = 4; }
      while(getmouse_xy(&X, &Y)==LEFTBUTTON)
       { int  dxy;
	 if(timeflag && passtime(T[door]))  timestr();
	 if(bioskey(2)&0x0f)  dxy = 0;
	 else                 dxy = 5;
	 if(fabs(X-X0)<=dxy && fabs(Y-Y0)<=dxy)  continue;
	 if(click==-1)
	  { double radX, radX0, rad;
	    if(active==0)   { select_tx(); flag = 3; }
	    else            { round_tx(0);  flag = 2; }
	  }
	 else  {  move_tx();  flag = 1; }
	 X0 = X;  Y0 = Y;
      }
     if(flag == 3)  continue;
     if(!flag)  { allselect_tx(); continue; }
     if(click==-1)  test_rad();
     else       { test_point(); test_line(); }
    }
/***********************************************************/
   if(key)
    { int dx=0, dy=0, fx=0, order;
      double  drad = 0;      KEY = 1;
      !timeflag&&(timeflag=1);   timestr();
      switch(key)
       { case LEFT_KEY: dx=-5; break;	 case RIGHT_KEY: dx=5; break;
	 case UP_KEY:   dy=-5; break;	 case DOWN_KEY:  dy=5; break;
	 case CTRL_LEFT: dx=-20; break;   case CTRL_RIGHT: dx=20; break;
	 case CTRL_UP:   dy=-20; break;   case CTRL_DOWN:  dy=20; break;
	 case CTRL_TAB:  case TAB_KEY: fx = 1; break;
	 case ESC_KEY: allselect_tx(); continue;
	 case SPACE_KEY: for(k=0; k<4; k++)   if(MSG[k].Active==1)  break;
	      if(k==4)  MSG[0].Active = 1;
	      else { order = k++==3?0:k; for(k=0;k<4;k++) MSG[k].Active=0;
		     MSG[order].Active = 1;
		   }
	      active = 1; break;
	 case ALT_UP:   drad = M_PI/12;  break;
	 case ALT_DOWN: drad = -M_PI/12; break;
	 case ALT_LEFT: drad = M_PI/24;  break;
	 case ALT_RIGHT:drad = -M_PI/24; break;
	 case F1_KEY: case F2_KEY: case F3_KEY: case F4_KEY:
	      for(k=0; k<4; k++)  MSG[k].Active = 0;
	      MSG[key-F1_KEY].Active = 1;  active = 1;
	      set_layer(key-F1_KEY); break;
	 case SHIFT_F1: case SHIFT_F2: case SHIFT_F3: case SHIFT_F4:
	      order = key - SHIFT_F1;
	      MSG[order].Active==1? active--:active++;
	      MSG[order].Active = 1-MSG[order].Active;
	      set_layer(order); break;
	 default : continue;
       }
      cleartx();
      for(k=0; k<4; k++)
       if(MSG[k].Active==1)
	{ MSG[k].XX+=dx; MSG[k].YY+=dy;	}
      getpoints();  writetxs();
      for(k=0; k<4; k++)  if(MSG[k].Active==1)  break;
      if(fx==1)    yround_tx(k);
      if(drad)     round_tx(drad);
      else       { test_point(); test_line(); }
    }
/***********************************************************/
  if(buttonflag==RIGHTBUTTON)
   { KEY = 0;
     if(click!=-1)
      { !timeflag&&(timeflag=1);  yround_tx(click); }
     else     rightbutton_click();
   }
/**********************************************************/
  test_image();
  if(itrue()==1)  true_deal();
  if(timeflag)  timestr();
WEND
 }

void  init_tx(int argc, char **argv)
 { int  k;
   address = getvect(0x33);
   setcolor(YELLOW);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   if(address==NULL || *(unsigned char *)address==0xcf)
    { outtextxy(320, 200, "Mouse driver not installed, please install it!");
      outtextxy(320, 230, "(Run \"mouse.com\")");
      outtextxy(320, 260, "Press any key to continue . . .");
      getch();
      Exit(1, 1);
    }
   initmouse();
{  char  deal[100] = "";
   for(k=1; k<argc; k++)  strcat(deal, argv[k]);
   for(k=0; k<strlen(deal); k++)
    switch(deal[k])
     { case 'A': case 'a':  txnumflag   = 1;  break;
       case 'Q': case 'q':  ishowscreen = 0;  break;
       case 'E': case 'e':  ireadonly   = 0;  break;
       default : outtextxy(320, 230, "Error on input!");
		 outtextxy(320, 260, "Press any key to continue . . .");
		 getch();
		 Exit(1, 1);
     }
}
   if(ireadonly == 0)
      { txfp = fopen("txset.msg", "at+");
	High = 125;
      }
   else  txfp = fopen("txset.msg", "rt");
   if(txfp == NULL)
    { outtextxy(320, 230, "Can't open file \"TXSET.MSG\" !");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1, 1);
    }
   while((k=fgetc(txfp))!=EOF)
     if(k=='S')  txnum++;
   if(txnumflag==1) txnum0=txnum;
   T = (long*)malloc((txnum+1)*sizeof(long));
   for(k=0; k<txnum+1; k++)  T[k] = 0;
   hzk24fp = fopen("zlpt.hzk", "rb");
   if(hzk24fp == NULL)
    { outtextxy(320, 230, "Can't open file \"zlpt.hzk\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1, 1);
    }
   hzk16fp = fopen("menu.hzk", "rb");
   if(hzk16fp == NULL)
    { outtextxy(320, 230, "Can't open file \"menu.hzk\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1, 1);
    }
 }

int  inrect(int x1, int y1, int x2, int y2, int code,int point)
 { int t, k, x, y;
   if(x1>x2) { t=x1; x1=x2; x2=t; }
   if(y1>y2) { t=y1; y1=y2; y2=t; }
   t = 0;
   for(k=0; k<MSG[code].Points; k++)
    { x = (int)(MSG[code].X[k]+0.5);
      y = (int)(MSG[code].Y[k]+0.5);
      if(x>x1&&x<x2 && y>y1&&y<y2 && ++t==point)
	return IN;
    }
   return OUT;
 }

double  find_nearpoint(int act1, int act2, double *getfarx, double *getfary)
 { double nearx[5], neary[5];
   int  m, p, k;
   double  farx, fary, farxy, farxy0;
   for(m=0; m<MSG[act1].Points; m++)
    { farxy0 = 700;
      for(p=0; p<MSG[act2].Points; p++)
       { farx = MSG[act2].X[p]-MSG[act1].X[m];
	 fary = MSG[act2].Y[p]-MSG[act1].Y[m];
	 farxy = sqrt(farx*farx + fary*fary);
	 if(farxy<farxy0)
	   { nearx[m] = MSG[act2].X[p];
	     neary[m] = MSG[act2].Y[p];
	     farxy0 = farxy;
	   }
       }
     }
   farxy0 = 700; m = -1;
   for(k=0; k<MSG[act1].Points; k++)
    { farx = MSG[act1].X[k]-nearx[k];
      fary = MSG[act1].Y[k]-neary[k];
      farxy = sqrt(farx*farx + fary*fary);
      if(farxy0>farxy)
       { m = k;
	 farxy0 = farxy;
       }
     }
   *getfarx = MSG[act1].X[m]-nearx[m];
   *getfary = MSG[act1].Y[m]-neary[m];
   return farxy0;
 }


double  line_far(double *_X, double *_Y, double *getfarx, double *getfary)
 { double X1, Y1, X2, Y2, X3, Y3, X4, Y4;
   double k, k0, t;
   double P1, P2, Q1, Q2;
   int    i;
   X1=_X[0]; Y1=_Y[0]; X2=_X[1]; Y2=_Y[1];
   X3=_X[2]; Y3=_Y[2]; X4=_X[3]; Y4=_Y[3];

   /******>>>(1)<<<******/
   if(fabs(X1-X2) <=0.00001 )
    { if(fabs(X3-X4)>=0.00001)  return -1;
      if(Y2<Y1)  { t=Y2; Y2=Y1; Y1=t; t=X2; X2=X1; X1=t; }
      if(Y4<Y3)  { t=Y4; Y4=Y3; Y3=t; t=X4; X4=X3; X3=t; }
      if(Y3>Y2 || Y1>Y4)  return  -1;
      *getfarx = X1-X3;
      *getfary = 0;
      return fabs(X1-X3);
    }

   /******>>>(2)<<<******/
   if(fabs(Y2-Y1) <= 0.00001)
    { if(fabs(Y4-Y3)>=0.00001)  return -1;
      if(X2<X1) { t=X2; X2=X1; X1=t; t=Y2; Y2=Y1; Y1=t; }
      if(X4<X3) { t=X4; X4=X3; X3=t; t=Y4; Y4=Y3; Y3=t; }
      if(X3>X2 || X1>X4)  return  -1;
      *getfarx = 0;
      *getfary = Y1-Y3;
      return fabs(Y1-Y3);
    }

   /******>>>(3)<<<******/
   if(fabs(X4-X3)<= 0.00001)   return -1;
   k = (Y2-Y1)/(X2-X1);
   if(fabs(k - (Y4-Y3)/(X4-X3)) >=0.00001)  return  -1;
   if(X2<X1) { t=X2; X2=X1; X1=t; t=Y2; Y2=Y1; Y1=t; }
   if(X4<X3) { t=X4; X4=X3; X3=t; t=Y4; Y4=Y3; Y3=t; }
   k0 = -1/k;
   P1=Y1-k0*X1; P2=k*(X2-X1)+Y1-k0*X2;
   Q1=Y3-k0*X3; Q2=k*(X4-X3)+Y3-k0*X4;
   if(P1>P2) { t=P1; P1=P2; P2=t; }
   if(Q1>Q2) { t=Q1; Q1=Q2; Q2=t; }
   if(P1>Q2 || Q1>P2)  return -1;
   *getfarx = (k*(X1-X3)-(Y1-Y3))/(k-k0);
   *getfary = (k*k0*(X1-X3)-k0*(Y1-Y3))/(k-k0);
   return sqrt(*getfarx**getfarx + *getfary**getfary);
 }

double  find_nearline(int act1, int act2, double *getnearx, double *getneary)
 { double farx, fary, farxy, farxy0=710;
   double X[4], Y[4];
   int k, m, point1, point2;
   for(k=0; k<MSG[act1].Points; k++)
    { point1=k; point2=k+1;
      if(point2==MSG[act1].Points)  point2=0;
      X[0]=MSG[act1].X[point1];  Y[0]=MSG[act1].Y[point1];
      X[1]=MSG[act1].X[point2];  Y[1]=MSG[act1].Y[point2];
      for(m=0; m<MSG[act2].Points; m++)
       { point1=m; point2=m+1;
	 if(point2==MSG[act2].Points)  point2=0;
	 X[2]=MSG[act2].X[point1];  Y[2]=MSG[act2].Y[point1];
	 X[3]=MSG[act2].X[point2];  Y[3]=MSG[act2].Y[point2];
	 farxy = line_far(X, Y, &farx, &fary);
	 if(farxy>0 && farxy<farxy0)
	  { farxy0 = farxy;
	    *getnearx = farx;
	    *getneary = fary;
	  }
       }
     }
   if(farxy0>700)  return -1;
   return  farxy0;
 }


int  pointsnum(double *_pointx, double *_pointy)
 { int k, n, num=0, i, point=0;
   int     rad[16];
   double pointx[16], pointy[16];
   for(k=0; k<16; k++)  rad[k]=0;
   for(k=0; k<4; k++)
    for(n=0; n<MSG[k].Points; n++)
     { for(i=0; i<num; i++)
	{ if(fabs(pointx[i]-MSG[k].X[n])<2 &&
			   fabs(pointy[i]-MSG[k].Y[n])<2)
	    { rad[i] += MSG[k].Rct[n];
	      break;
	    }
	}
       if(i==num)
	{ pointx[num] = MSG[k].X[n];
	  pointy[num] = MSG[k].Y[n];
	  rad[num] = MSG[k].Rct[n];
	  num++;
	}
     }
    for(k=0; k<num; k++)
     if(rad[k]!=8 && rad[k]!=4)
       { _pointx[point] = pointx[k];
	 _pointy[point] = pointy[k];
	 point++;
       }
    return  point;
  }

int  gettx(int n)
 { int k=0, ch;
   rewind(txfp);
   while((ch=fgetc(txfp))!=EOF)
    { if(ch=='S')  k++;
      if(k==n)     break;
    }
   if(ch==EOF)  return  -1;
   fscanf(txfp, "%d ", &TXMSG.N);
   fscanf(txfp, "%d %d ", &TXMSG.Begin, &TXMSG.End);
   if(TXMSG.Begin==0 && TXMSG.End==0)
    { TXMSG.Begin=78; TXMSG.End=79; }
   fscanf(txfp, "%d ", &TXMSG.Points);
   fscanf(txfp, "%lf ", &TXMSG.Dxy);
   for(k=0; k<4; k++)
      fscanf(txfp, "%lf %lf %lf %d ", &TXMSG.R[k],
	      &TXMSG.Rad[k], &TXMSG.Txrad[k], &TXMSG.Round[k]);
   for(k=0; k<TXMSG.Points; k++)
      fscanf(txfp, "%lf ", &TXMSG.Farxy[k]);
 }

void  showtx(int x, int y)
  { int k;
    gettxpoints(x, y);
    for(k=0; k<4; k++)  flood(k);
  }

void  gettxpoints(int x, int y)
 {  int k;
    for(k=0; k<4; k++)
      { MSG[k].XX = r * TXMSG.R[k] * cos(TXMSG.Rad[k]) + x;
	MSG[k].YY = -r * TXMSG.R[k] * sin(TXMSG.Rad[k]) + y;
	MSG[k].Rad = TXMSG.Txrad[k];
	MSG[k].Round = TXMSG.Round[k];
	_getpoints(k);
      }
    setfillcolor(RED);
    setlinkcolor(RED);
 }

void  resettx()
 {
   MSG[0].XX = 180; MSG[0].YY = 160; MSG[0].Round = 0; MSG[0].Rad = 0;
   MSG[1].XX = 420; MSG[1].YY = 180; MSG[1].Round = 0; MSG[1].Rad = 0;
   MSG[2].XX = 170; MSG[2].YY = 325; MSG[2].Round = 0; MSG[2].Rad = 0;
   MSG[3].XX = 420; MSG[3].YY = 340; MSG[3].Round = 0; MSG[3].Rad = 0;
 }

int  itrue()
 {  int k, num, i;
    double  X[16], Y[16];
    double  farx, fary, farxy;
    double  middlex, middley, S;
    num = pointsnum(X, Y);
    if(num != TXMSG.Points)  return -1;
    middlex = MSG[0].XX;
    middley = MSG[0].YY;
    S = MSG[0].S;
    for(k=1; k<4; k++)
     { middlex = (S*middlex+MSG[k].S*MSG[k].XX) / (S+MSG[k].S);
       middley = (S*middley+MSG[k].S*MSG[k].YY) / (S+MSG[k].S);
       S += MSG[k].S;
     }
    for(k=0; k<num; k++)
     { farx = X[k]-middlex;  fary = Y[k]-middley;
       farxy = sqrt(farx*farx + fary*fary);
       for(i=0; i<TXMSG.Points; i++)
	  if(fabs((farxy/r-TXMSG.Farxy[i])) < TXMSG.Dxy)  break;
       if(i == TXMSG.Points)  return -1;
     }
    return 1;
 }

int  show_desk(int A)
 { int n, k=0, i, j;
   char  ST[10];
   int  roundx, roundy, roundx0, roundy0;
   int  dx, dy;
   randomize();
   if(A<0)
    { if(txnum0<=passed)  return -1;
      n = random(txnum0-passed);
      n++;
      for(door=1; door<=txnum0; door++)
       { if(readflag(door)==0) k++;
	 if(k==n)  break;
       }
    }
   else door=A;
   gettx(door);
   for(k=0; k<4; k++)  MSG[k].Active = 0;
   active = 0;

   if(A<0)
    { mouse_off();      cleardevice();
      sprintf(ST, "WAVE %d", passed+1);
      settextstyle(1, 0, 9);
      setcolor(RED);
      randomize();
      for(k=0; k<2; k++)
	{ setcolor(random(14)+2);  show_str(random(8), ST);
	  setcolor(random(14)+2);  show_str(random(8), ST);
	}
      setcolor(YELLOW);  outtextxy(323, 243, ST);
      delay(700);
      clear_str(random(4));
    }
   sethz_file(hzk24fp);   sethz_color(YELLOW);
   sethz_zoomxy(3, 2);    sethz_fx(1);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);   sethz_dxy(20);
   sethz_dot(24);             sethz_mode(XOR_MODEL);
   sethz_speed(5);
   gettxmiddlexy(&roundx, &roundy, 640, 480);
   r = 20;   gettxmiddlexy(&roundx0, &roundy0, 120, 120);
   dx = (int)((float)(roundx-roundx0)/4+0.5);
   dy = (int)((float)(roundy-roundy0)/4+0.5);
   mouse_off();
   for(r=20; r<=100; r+=20)
    { cleardevice();
      showtx(roundx, roundy);
      delay(200);
    }
   writehzs(roundx, roundy, START+TXMSG.Begin, START+TXMSG.End);
   delay(1500);
   writehzs(roundx, roundy, START+TXMSG.Begin, START+TXMSG.End);
   delay(200);
   for(r=80; r>=20; r-=20)
    { cleardevice();
      roundx -= dx;  roundy -= dy;
      showtx(roundx, roundy);
      delay(200);
    }
   sethz_text(LEFT_HZ, TOP_HZ);   sethz_zoomxy(1, 1);
   sethz_speed(5);                sethz_dxy(5);
   sethz_color(LIGHTGREEN);
   writehzs(10, 10, START+TXMSG.Begin, START+TXMSG.End);
   getimage(0, 0, 120, 120, ptr);
   resettx();
   for(r=10; r<=100; r++)
    { for(k=0; k<4; k++) _getpoints(k);
      writetxs();
      delay(8);
    }
   r = 100;
   for(k=0; k<=80; k++)
     { setmousexy(4*k, 3*k);
       mouse_on();
       delay(8);
     }
   return 1;
 }

void  gettxmiddlexy(int *x, int *y, int A, int B)
 { int i1, i2, k1, k2;
   int farx=0, fary=0;
   int minx=0, miny=0;
   gettxpoints(0,0);
   for(k2=0; k2<4; k2++)
     for(i2=0; i2<MSG[k2].Points; i2++)
      { if(MSG[k2].X[i2]<(double)minx)    minx=MSG[k2].X[i2];
	if(MSG[k2].Y[i2]<(double)miny)    miny=MSG[k2].Y[i2];
	for(k1=0; k1<4; k1++)
	  for(i1=0; i1<MSG[k1].Points; i1++)
	    { if(fabs(MSG[k2].X[i2]-MSG[k1].X[i1])>farx)
		 farx = (int)fabs(MSG[k2].X[i2]-MSG[k1].X[i1]);
	      if(fabs(MSG[k2].Y[i2]-MSG[k1].Y[i1])>fary)
		 fary = (int)fabs(MSG[k2].Y[i2]-MSG[k1].Y[i1]);
	    }
     }
   *x = (A-farx)/2-minx;
   *y = (B-fary)/2-miny;
 }



void  show_ok()
 { int i=0, k, Tt, fs, n;
   double S;
   int middlex, middley;
   char  test[6];
   Tt = CLOCK/18.2;
   if(Tt<=60) fs = 100;
   else       fs = 100 - (Tt-40)/20;
   if(fs<0)   fs = 10;
   sprintf(test, "%d%%", fs);
   if(readflag(door)==0 && door<=txnum0) Test += fs;
   middlex = MSG[0].XX;
   middley = MSG[0].YY;
   S = MSG[0].S;
   for(k=1; k<4; k++)
    { middlex = (S*middlex+MSG[k].S*MSG[k].XX) / (S+MSG[k].S);
      middley = (S*middley+MSG[k].S*MSG[k].YY) / (S+MSG[k].S);
      S += MSG[k].S;
    }
   setwritemode(XOR_PUT);
   setcolor(LIGHTGREEN);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 1);
   mouse_off();
   outtextxy(middlex, middley, test);
   for(i=2; i<=12; i++)
    { settextstyle(1, 0, i);
      outtextxy(middlex, middley, test);
      settextstyle(1, 0, i-1);
      outtextxy(middlex, middley, test);
      delay(50);
    }
   timestr();
   mouse_off();
   setwritemode(COPY_PUT);
   settextstyle(1, 0, 12);
   setcolor(LIGHTGRAY);
   outtextxy(middlex, middley, test);
   setcolor(LIGHTGREEN);
   outtextxy(middlex+2, middley+2, test);
   settextstyle(1, 0, 7);
   setcolor(YELLOW);
   outtextxy(320, 30, fs==100?"Great!!!!":fs>=90?"Best!!!":fs>=80?
	    "Good!!":fs>=70?"Fine!":fs>=60?"OK!":"Sorry!");
   mouse_on();
 }

void  Exit(int code, int flag)
 {  char ch;
    char far *p;
    if(flag==1)
     { fcloseall();       closegraph();   free(T);
       farfree(ptr);      exit(code);
     }
    putch('\a');
    p = (char far*)farmalloc(imagesize(320-290-5, 240-20-5, 320+290, 240+20));
    mouse_off();
    getimage(320-290-5, 240-20-5, 320+290, 240+20, p);
    setfillstyle(1, DARKGRAY);
    bar(320-290, 240-20, 320+290, 240+20);
    setfillstyle(1, LIGHTBLUE);
    bar(320-290-5, 240-20-5, 320+290-5, 240+20-5);
    settextstyle(1, 0, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(RED);
    outtextxy(320, 237, "The game will be completed, Are you sure?(Y/N)");
    mouse_on();
    while(1)
      { if(bioskey(1)&&((ch=bioskey(1))=='N'||ch=='n'||ch==27) ||
	  getmouse_xy(&X, &Y)==LEFTBUTTON&&imouse_inbox(570, 220, 600, 260)==YES)
	  { mouse_off();
	    putimage(320-290-5, 240-20-5, p, COPY_PUT);
	    mouse_on();
	    farfree(p);
	    delay(300);
	    return;
	  }
	 if(bioskey(1)&&((ch==bioskey(0))=='Y'||ch=='y'||ch==13) ||
	   getmouse_xy(&X, &Y)==LEFTBUTTON&&imouse_inbox(530, 220, 560, 260)==YES)
	   break;
       if(timeflag && passtime(T[door])==1)  timestr();
      }
    fcloseall();
    closegraph();
    farfree(ptr);
    farfree(p);
    free(T);
    exit(code);
 }

int  show_menu()
 { int X, Y, i, X1, Y1, t;
   int X0, Y0, n, n0, flag;
   char  *p, *ptr=NULL, *Bp;
   int  num[5] = { 2, 1, 1, 1, 2 };
   MENU = 1;
   sethz_file(hzk16fp);   sethz_color(YELLOW);   sethz_zoomxy(2, 1);
   sethz_fx(0);    sethz_text(LEFT_HZ, TOP_HZ);   sethz_dxy(5);
   sethz_dot(16);        sethz_mode(COPY_MODEL);   sethz_speed(0);
   getmouse_xy(&X, &Y);
   if(X+Wide+6>639)  X1=X-Wide;
   else            X1=X+Wide;
   if(Y+High+6>479)  Y1=Y-High;
   else            Y1=Y+High;
   if(X>X1) { t=X; X=X1; X1=t; }
   if(Y>Y1) { t=Y; Y=Y1; Y1=t; }
   p = (char *)malloc(imagesize(X, Y, X1+6, Y1+6));
   Bp = (char *)malloc(imagesize(X, Y, X1+6, Y1+6));
   mouse_off();
   getimage(X, Y, X1+6, Y1+6, p);
   t = 0;
   for(i=0; i<High; i+=25)
    { setfillstyle(1, DARKGRAY);
      bar(X+6, Y+i+6, X1+6, Y+i+31);
      setfillstyle(1, GREEN);
      bar(X, Y+i, X1, Y+i+25);
      writehzs(X+10, Y+i+4, t, t+num[i/25]);
      t += num[i/25]+1;
      delay(40);
    }
   getimage(X, Y, X1+6, Y1+6, Bp);
   mouse_on();
   n0 = -1;
   while(getmouse_xy(&X0, &Y0)!=LEFTBUTTON)
    { if(imouse_inbox(X, Y, X1, Y1)==YES)  n = (Y0-Y)/25;
      else        n = -1;
      if(kbhit() && getch()==27)  { n = -1;  break; }
      if(timeflag && passtime(T[door])==1 )
	{ timestr();
          for(flag=0; flag<4; flag++)
	    if(inrect(500, -95, 800, 40, flag, 1)==IN)  break;
	  if(X1+6>=500&&Y<=40 || flag!=4)
	   { mouse_off();
	     putimage(X, Y, Bp, COPY_PUT);
	     mouse_on();
	   }
	}
      if(n0!=n)
       { if(ptr!=NULL)
	  {  ptr = (char *)malloc(imagesize(X, Y+n0*25, X1, Y+(n0+1)*25));
	     mouse_off();
	     getimage(X, Y+n0*25, X1, Y+(n0+1)*25, ptr);
	     putimage(X, Y+n0*25, ptr, NOT_PUT);
	     getimage(X, Y, X1+6, Y1+6, Bp);
	     mouse_on();
	     free(ptr);
	  }
	 if(n==-1)  { ptr=NULL; n0 = -1; }
	 else
	  {  ptr = (char *)malloc(imagesize(X, Y+n*25, X1, Y+(n+1)*25));
	     mouse_off();
	     getimage(X, Y+n*25, X1, Y+(n+1)*25, ptr);
	     putimage(X, Y+n*25, ptr, NOT_PUT);
             getimage(X, Y, X1+6, Y1+6, Bp);
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
   free(p);  free(Bp);
   delay(300);
   return n;
 }

int  rightbutton_click()
 { int n;
   switch(n=show_menu())
      { case 0: timeflag = 0; T[door] = CLOCK; show_desk(-1);
                if(T[door]==0)  timeflag = 0;
		else            timeflag = 1;
		MENU = 0;
		passtime(T[door]); timestr(); break;
	case 1: MENU = 0; if(show_select()==-1)  n = -1; break;
	case 2: show_help();  break;
	case 3: Exit(0, 0);   break;
	case 4: MENU = 0; save();
      }
   MENU = 0;
   return n;
 }

int  show_select()
 { char *p=NULL;
   int begin, end, c, L = 0, I=0, J=0, I0, J0, k, n, K, Bdoor;
   sethz_file(hzk24fp);
   sethz_zoomxy(1, 1); 	    sethz_fx(1);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);	    sethz_dxy(5);
   sethz_dot(24);             	    sethz_mode(XOR_MODEL);
   sethz_speed(0);
   for(k=0; k<2; k++)
    { rewind(txfp);
      L = 0;
      mouse_off();
      while((c=fgetc(txfp))!=EOF)
       { if(c=='S')
	 { fscanf(txfp, "%d %d %d", &begin, &begin, &end);
	   if(readflag(L+1)==0)
	     { if(T[L+1]==0)     sethz_color(YELLOW);
	       else              sethz_color(LIGHTCYAN);
	     }
	   else                  sethz_color(LIGHTRED);
	   writehzs(90+(L%5)*110, 100+(L/5)*40, START+begin, START+end);
	   if(L++>35)  break;
	 }
       }
      sethz_color(YELLOW);
      writehzs(90+(L%5)*110, 100+(L/5)*40, START+78, START+79);
      mouse_on();
      if(k==1)
       { if(I==-1 || J==-1)  K = -1;
	 else                K = J*5+I+1;
	 break;
       }
      I0=J0=-1;
      while(getmouse_xy(&X, &Y)!=LEFTBUTTON)
       { I = (X-40)/110;  J = (Y-81)/40;
	if(X<37||X>587) I=-1;
	if(Y<81||Y>401) J=-1;
	if(J*5+I>37)    I = J = -1;
	if(kbhit() && getch()==27)  { I = J = -1;  break; }
	if(timeflag && passtime(T[door])==1)    timestr();
	if(I!=I0 || J!=J0)
	 { if(p!=NULL)
	    { p = (char *)malloc(imagesize(37+110*I0, 81+J0*40,
		   37+(I0+1)*110, 81+(J0+1)*40));
	      mouse_off();
	      getimage(37+110*I0, 81+J0*40,
				   37+(I0+1)*110, 81+(J0+1)*40, p);
	      putimage(37+110*I0, 81+J0*40, p, NOT_PUT);
	      mouse_on();
	      free(p);
	    }
	   if(J==-1||I==-1)  { p=NULL; I0 = J0 = -1; }
	   else
	    {  p = (char *)malloc(imagesize(37+110*I, 81+J*40,
				   37+(I+1)*110, 81+(J+1)*40));
	       mouse_off();
	       getimage(37+110*I, 81+J*40,
				   37+(I+1)*110, 81+(J+1)*40, p);
	       putimage(37+110*I, 81+J*40, p, NOT_PUT);
	       mouse_on();
	       free(p);
	       I0 = I;  J0 = J;
	     }
	  }
       }
      if(p!=NULL)
       { p = (char *)malloc(imagesize(37+110*I0, 81+J0*40,
				   37+(I0+1)*110, 81+(J0+1)*40));
	 mouse_off();
	 getimage(37+110*I0, 81+J0*40,
				   37+(I0+1)*110, 81+(J0+1)*40, p);
	 putimage(37+110*I0, 81+J0*40, p, NOT_PUT);
	 mouse_on();
	 free(p);
       }
    }
   if(K==-1)  return -1;
   Bdoor = K;
   if(Bdoor==38)
    { Bdoor = inputbox();
      if(Bdoor==-1 || (37+Bdoor)>txnum)  return -1;
      Bdoor += 37;
    }
   if(readflag(door)==0)  T[door] = CLOCK;
   else                   T[door] = 0;
   door = Bdoor;
   if(T[door]==0)  timeflag = 0;
   else            timeflag = 1;
   show_desk(door);
   passtime(T[door]);  timestr();
   return 1;
  }

void  show_help()
 { int k, l, flag = 0, num;
   int begin[4] = {12,341,961,1476}, end[4] = {340,960,1475,1717},
       dx[4] = {18,18,18,18}, dy[4] = {20,17,20,25}, _y[4] = {100,70,80,130};
   sethz_file(hzk16fp);   sethz_color(YELLOW);
   sethz_zoomxy(1, 1);    sethz_fx(0);
   sethz_text(LEFT_HZ, TOP_HZ);   sethz_dxy(0);
   sethz_dot(16);   sethz_mode(XOR_MODEL);
   sethz_speed(0);

   for(num=0; num<4; num++)
    { for(l=0; l<2; l++)
       { for(k=begin[num]; k<=end[num]; k++)
	   writehz( 55+(((k-begin[num]+2)%30)*dx[num]),
		    _y[num]+(((k-begin[num]+2)/30)*dy[num]), k);
	 if(l==0)  while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON)
	 if(timeflag && passtime(T[door])==1)  timestr();
	 if(kbhit()&&getch()==27)  flag = 1;
       }
      if(flag == 1) break;
    }
 }

void  save()
 {  int k , num, rad;
    double  X[16], Y[16], middlex, middley, S;
    double  RX0, RY0, RX, RY, RXY, RAD, farx, fary, farxy;
    middlex = MSG[0].XX;
    middley = MSG[0].YY;
    S = MSG[0].S;
    for(k=1; k<4; k++)
     { middlex = (S*middlex+MSG[k].S*MSG[k].XX) / (S+MSG[k].S);
       middley = (S*middley+MSG[k].S*MSG[k].YY) / (S+MSG[k].S);
       S += MSG[k].S;
     }
    fseek(txfp, 0, SEEK_END);
    fprintf(txfp, "/*ÎÞÌâ%d*/ S%d 0 0 ", txnum-36, txnum+1);
    num = pointsnum(X, Y);
    fprintf(txfp, "%d ", num);
    fprintf(txfp, "0.000010 ");
    RX0 = middlex;
    RY0 = middley;
    for(k=0; k<4; k++)
     { RX = MSG[k].XX;
       RY = MSG[k].YY;
       RXY = sqrt((RX-RX0)*(RX-RX0) + (RY-RY0)*(RY-RY0));
       if(RXY<1) RAD = 0;
       else      RAD = middle_rad(RX0, RY0, RX, RY);
       RXY /= r;
       fprintf(txfp, "%lf %lf %lf %d ", RXY, RAD, MSG[k].Rad, MSG[k].Round);
     }
    for(k=0; k<num; k++)
     { farx = X[k]-middlex; fary = Y[k]-middley;
       farxy = sqrt(farx*farx + fary*fary)/r;
       fprintf(txfp, "%lf ", farxy);
     }
    putc('\n', txfp);
    txnum ++;
    if(txnumflag==1)  txnum0++;
    T = (long*)realloc(T, (txnum+1)*sizeof(long));
    T[txnum] = 0;
    setfillstyle(0, 0);
    bar(320-100, 450-30, 320+100, 480);
    writetxs();  timestr();
  }

int  inputbox()
 { long  get10x(int n);
   char  shines(int x, int y);
   char *p,  ch, flag=1;
   char  S[20], C[2]={0,0}, str[20];
   int  x=320-90, y=240-20+2, K=0, N=0, i, len;
   p = (char far*)farmalloc(imagesize(320-100-5, 240-20-5, 320+100, 240+20));
   getimage(320-100-5, 240-20-5, 320+100, 240+20, p);
   setfillstyle(1, DARKGRAY);
   mouse_off();
   bar(320-100, 240-20, 320+100, 240+20);
   setfillstyle(1, LIGHTBLUE);
   bar(320-100-5, 240-20-5, 320+100-5, 240+20-5);
   settextstyle(1, 0, 3);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   setcolor(RED);
   sprintf(str, txnum==37?"NO.(NONE!)":"NO.(1-%d) :", txnum-37);
   outtextxy(320-100+10, 240-20+2, str);
   len = textwidth(str);
   setcolor(LIGHTGREEN);
   mouse_on();
   while(flag==1)
    { switch(ch=shines(x+K*15+len, y+30))
       { case 0:  getch(); break;
	 case 13: flag=0;  break;
	 case 27: flag=-1; break;
	 case 8:  if(K==0) break;
		  mouse_off();
		  bar(x+(K-1)*15+len, y, x+K*15+len, y+30);
		  mouse_on();
		  K--;  break;
	 default: if(!(ch<='9'&&ch>='0') || K>=3) break;
		  C[0] = ch;
		  settextstyle(1, 0, 3);
		  mouse_off();
		  outtextxy(x+K*15+len, y, C);
		  mouse_on();
		  S[K++] = ch;
       }
    }
   mouse_off();
   putimage(320-100-5, 240-20-5, p, COPY_PUT);
   free(p);
   mouse_on();
   if(flag==-1 || K==0)  return -1;
   for(i=0; i<K; i++)
     N += get10x(K-i-1)*(S[i]-'0');
   if(N==0)  return -1;
   return N;
 }

long  get10x(n)
 { int i;
   long N=1;
   for(i=0; i<n; i++)
     N *= 10;
   return  N;
 }

char  shines(int x, int y)
 { long _T;
   int  k, color=getcolor();
   setwritemode(XOR_PUT);
   setcolor(YELLOW);
BEGIN
   for(k=0; k<2; k++)
    { mouse_off();
      line(x, y, x+10, y);
      mouse_on();
      for(_T=0; _T<3000; _T++)
       if(kbhit())
	{ if(k==0)
	    { mouse_off(); line(x, y, x+10, y); mouse_on(); }
	  setwritemode(COPY_PUT);
	  setcolor(color); setwritemode(COPY_PUT);
	  return getch();
	}
    }
   if(timeflag && passtime(T[door])==1)    timestr();
WEND
 }

void  tx_click(int n)
 { int k;
   if(MSG[n].Active==1)
    { if(((bioskey(2)&0x02) || (bioskey(2)&0x01)))
	{ MSG[n].Active = 0;
	  active--;
	}
    }
   else
    { if((!(bioskey(2)&0x02) && !(bioskey(2)&0x01)))
       { for(k=0; k<4; k++)  MSG[k].Active = 0;
	 active = 0;
       }
      active++;  MSG[n].Active = 1;
    }
    writetxs();
 }

void  test_rad()
 { double _Rad, _Rad0 = 20, rad, act;
   int  N; int fg, k;
   if(bioskey(2)&0x08)      return;
   else if(bioskey(2)&0x03) fg = 24;
   else if(bioskey(2)&0x04) fg = 4;
   else                     fg = 12;
   for(act=0; act<4; act++)
    { if(MSG[act].Active==0) continue;
      while(MSG[act].Rad>=2*M_PI)   MSG[act].Rad-=2*M_PI;
      while(MSG[act].Rad<0)         MSG[act].Rad+=2*M_PI;
    }
   for(act=0; act<4; act++)
     if(MSG[act].Active==1) break;
   for(k=0; k<=fg*2; k++)
    { _Rad = fabs(MSG[act].Rad-k*M_PI/fg);
      if(_Rad0>_Rad) { _Rad0 = _Rad; N = k; }
    }
   rad = N*M_PI/fg - MSG[act].Rad;
   if(MSG[act].Round==1) rad=-rad;
   cleartx();
   for(k=0; k<4; k++)
    { if(MSG[k].Active==0) continue;
      if(MSG[k].Round==1)   MSG[k].Rad -= rad;
      else                  MSG[k].Rad += rad;
      middle_round(&MSG[k].XX, &MSG[k].YY, rad);
    }
   getpoints();
   writetxs();
 }

void  test_point()
{  int i, j, k;
   double  farx0, fary0, farx, fary, farxy, farxy0=700;
   if(active!=4 && !(bioskey(2)&0x08))
    { for(j=0; j<4; j++)
       { if(MSG[j].Active==0) continue;
	 for(i=0; i<4; i++)
	  { if(MSG[i].Active==1) continue;
	    farxy = find_nearpoint(i, j, &farx, &fary);
	    if(farxy<farxy0)
	     { farx0 = farx;
	       fary0 = fary;
	       farxy0 = farxy;
	     }
	  }
	}
      if(farxy0<=15)
       { for(k=0; k<4; k++)
	  { if(MSG[k].Active == 0)  continue;
	    MSG[k].XX += farx0;
	    MSG[k].YY += fary0;
	  }
	 cleartx();   getpoints();     writetxs();
       }
    }
  }

void  test_line()
 { int i, j, k, num=0;
   double  farx, fary, farxy;
   if(active!=4 && !(bioskey(2)&0x04))
    { for(j=0; j<4; j++)
       { if(MSG[j].Active==0) continue;
	 if(num>10) break;
	 for(i=0; i<4; i++)
	  { if(MSG[i].Active==1) continue;
	    farxy = find_nearline(i, j, &farx, &fary);
	    if(farxy>0.001 && farxy<=10)
	     { for(k=0; k<4; k++)
		 { if(MSG[k].Active == 0)  continue;
		   MSG[k].XX += farx;
		   MSG[k].YY += fary;
		 }
	       cleartx();     getpoints();    writetxs();
	       num++;
	       j = 0; break;
	     }
	  }
       }
    }
 }

void  select_tx()
 { int Xb=X0, Yb=Y0, k;
   setwritemode(XOR_PUT);
   setcolor(CYAN);
   setlinestyle(1, 0, 1);
   mouse_off();
   rectangle(Xb, Yb, X, Y);
   mouse_on();
   X0 = X; Y0 = Y;
   while(getmouse_xy(&X, &Y)==LEFTBUTTON)
    { if(timeflag && passtime(T[door]))  timestr();
      if(fabs(X-X0)<5 && fabs(Y-Y0)<5) continue;
      setwritemode(XOR_PUT);
      setcolor(CYAN);
      mouse_off();
      rectangle(Xb, Yb, X0, Y0);
      rectangle(Xb, Yb, X, Y);
      mouse_on();
      X0 = X; Y0 = Y;
    }
   setwritemode(XOR_PUT);
   setcolor(CYAN);
   mouse_off();
   rectangle(Xb, Yb, X0, Y0);
   mouse_on();
   setwritemode(COPY_PUT);
   setlinestyle(0, 0, 1);
   for(k=0; k<4; k++)  MSG[k].Active = 0;
   active = 0;
   for(k=0; k<4; k++)
    { if(inrect(Xb, Yb, X0, Y0, k, 2) == IN)
       { MSG[k].Active = 1; active++; }
    }
   getmiddle(); cirs(middlex, middley, BLACK);  writetxs();
 }

void  round_tx(double _rad)
{ double radX, radX0, rad, k;
  if(_rad==0)
    { radX  = middle_rad(middlex, middley, X, Y);
      radX0 = middle_rad(middlex, middley, X0, Y0);
      rad = radX - radX0;
    }
  else  rad = _rad;
  cleartx();
  getmiddle();
  for(k=0; k<4; k++)
   { if(MSG[k].Active==0)   continue;
     if(MSG[k].Round==1)    MSG[k].Rad -= rad;
     else                   MSG[k].Rad += rad;
     middle_round(&MSG[k].XX, &MSG[k].YY, rad);
   }
  getpoints();
  writetxs();
 }

void  move_tx()
 { int k;
   cleartx();
   for(k=0; k<4; k++)
    if(MSG[k].Active==1)
     { MSG[k].XX+=X-X0; MSG[k].YY+=Y-Y0; }
   getpoints();
   writetxs();
 }

void  yround_tx(int n)
{ int k;
 if(MSG[n].Active == 0)
   { active = 1;
     for(k=0; k<4; k++)  MSG[k].Active = 0;
     MSG[n].Active = 1;
   }
  getmiddle();
  for(k=0; k<4; k++)
  if(MSG[k].Active==1)
   { MSG[k].Round = 1-MSG[k].Round;
     if(bioskey(2)&0x04)
      { MSG[k].Rad += M_PI; MSG[k].YY += 2*(middley - MSG[k].YY); }
     else  MSG[k].XX  +=  2*(middlex - MSG[k].XX);
   }
  cleartx();   getpoints();   writetxs();
  while(getmousebutton()!=NOBUTTON);
}

void  true_deal()
 { int F=1, k;
   for(k=0; k<4; k++)   MSG[k].Active = 0;
   active = 0;
   setfillstyle(0, 0);
   bar(170, 430, 470, 475);
   writetxs();   show_ok();
   timeflag = 0;
   T[door] = 0;  putch('\a');
   if(readflag(door)==0 && door<=txnum0) { writeflag(door); passed++; }
   while(getmousebutton()!=LEFTBUTTON && !kbhit())
    { if(getmousebutton()==RIGHTBUTTON)
	if( rightbutton_click() >= 0) { F = 0; break; }
    }
   kbhit()&&getch()==0?getch():0;
   if(F==0) return;
   if(show_desk(-1)==-1)
    { char str[20];
      int  wave = Test/txnum0;
      sprintf(str, "Test: %d", Test);
      settextstyle(1, 0, 9);
      settextjustify(CENTER_TEXT, CENTER_TEXT);
      mouse_off();
      cleardevice();
      setcolor(RED);     outtextxy(320, 100, "The End");
      setcolor(YELLOW);  outtextxy(323, 103, "The End");
      settextstyle(1, 0, 7);
      setcolor(YELLOW);  outtextxy(320, 240, str);
      setcolor(RED);     outtextxy(323, 243, str);
      settextstyle(1, 0, 9);
      strcpy(str, wave>=95?"Great!!!!":wave>=90?"Best!!!":wave>=80?
		  "Good!!":wave>=70?"Fine!":wave>=60?"OK!":"Sorry!");
      setcolor(RED);     outtextxy(320, 400, str);
      setcolor(GREEN);   outtextxy(323, 403, str);
      for(k=0; k<20; k++)  txmsg[k] = 0;
      passed = 0; Test = 0;
      getch();
      show_desk(-1);
    }
  if(T[door]==0)  timeflag = 0;
  else            timeflag = 1;
  passtime(T[door]);    timestr();
 }

void  allselect_tx()
 { int k;
   getmiddle();   cirs(middlex, middley, BLACK);
   if(active==0)
    { for(k=0; k<4; k++)  MSG[k].Active = 1;  active = 4; }
   else
    { for(k=0; k<4; k++)  MSG[k].Active = 0;  active = 0; }
   writetxs();
 }

void  show_str(int code, char *str)
 { int k;
   int beginx[8]={0,320,640,640,640,320,0,0}, beginy[8]={0,0,0,240,480,480,480,240},
       dx[8]={32,0,-32,-32,-32,0,32,32}, dy[8]={24,24,24,0,-24,-24,-24,0};
   setwritemode(XOR_PUT);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(beginx[code], beginy[code], str);
   for(k=0; k<10; k++)
    { delay(15);
      outtextxy(beginx[code]+k*dx[code], beginy[code]+k*dy[code], str);
      outtextxy(beginx[code]+(k+1)*dx[code], beginy[code]+(k+1)*dy[code], str);
    }
   setwritemode(COPY_PUT);
   outtextxy(beginx[code]+k*dx[code], beginy[code]+k*dy[code], str);
 }

void  test_image()
 { int k;
   for(k=0; k<4; k++)
     if(inrect(-300, -100, 120, 120, k, 1)==IN||idotin(k, 120, 120))
	{ putimage(0, 0, ptr, COPY_PUT);
	  writetxs(k); break;
	}
  }

void  clear_str(int code)
 { int k;
   setcolor(BLACK);
   switch(code)
    { case 0: for(k=0; k<100; k++)
		{ line(0, 240-k, 639, 240-k);
		  line(0, 240+k, 639, 240+k);   delay(6);
		}  break;
      case 1: for(k=0; k<100; k++)
		{ line(0, 140+k, 639, 140+k);
		  line(0, 340-k, 639, 340-k);   delay(6);
		}  break;
      case 2: for(k=0; k<200; k++)
		{ line(320-k, 0, 320-k, 479);
		  line(320+k, 0, 320+k, 479);   delay(3);
		}  break;
      case 3: for(k=0; k<200; k++)
		{ line(520-k, 0, 520-k, 479);
		  line(120+k, 0, 120+k, 479);   delay(3);
		}  break;
    }
 }