#include <graphics.h>
#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include "tools.h"
#include "screen.h"
#include "hz.h"
#include "mouse.h"
#include "extern.h"
#include "key.h"

#define  BEGIN  while(1){
#define  WEND    }
#define  get_ZT(n)  (ZT&((unsigned char)0x01<<n))
#define  get_KZ(n)  (KZ&((unsigned char)0x01<<n))

FILE  *hzk16fp, *hzk24fp, *txfp;
void  Exit(int code, int flag);
void  screen2(), kz_key(), write_msg();
int   run(), mouse_move();
int   mouse_click();
void  _rectangle(int,int,int,int);
void  time_deal(char *str);
void  show_time(long T);
int   save(), use();
void  set(),  get_set(), help_msg(), Sound(int);
char  left, right, up, down, space;

unsigned  char  KZ = 0xe9;
unsigned  char  ZT = 0xff;
unsigned  FK[16][4] =
 { { 0x0f00, 0x2222, 0x0f00, 0x2222 },  { 0x0622, 0x02e0, 0x4460, 0x0740 },
   { 0x0644, 0x0e20, 0x2260, 0x0470 },  { 0x4620, 0x3600, 0x4620, 0x3600 },
   { 0x2640, 0xc600, 0x2640, 0xc600 },  { 0x4e00, 0x4640, 0x0e40, 0x4c40 },
   { 0x6600, 0x6600, 0x6600, 0x6600 },  { 0x4000, 0x4000, 0x4000, 0x4000 },
   { 0x6400, 0x6200, 0x2600, 0x4600 },  { 0xe440, 0x2e20, 0x44e0, 0x8e80 },
   { 0x2e80, 0xc460, 0x2e80, 0xc460 },  { 0x8e20, 0x64c0, 0x8e20, 0x64c0 },
   { 0xae00, 0x6460, 0x0ea0, 0xc4c0 },  { 0x6000, 0x4400, 0x6000, 0x4400 },
   { 0x4e40, 0x4e40, 0x4e40, 0x4e40 }
 } ;
unsigned  BJ[23];
int  F, F0, R, R0, X, Y;
int  X1, Y1, X2, Y2;
long CLOCK0, CLOCK;
long FS = 0, FS0, maxFS = 0;
int  kind = 7, speed = 0, level = 0, _sound_ = 1, _time_ = 0, speed0;
int  initflag = 0;
long Time, Time0, T, dTime, dT;
int  PASS = 10000;
int  ishowscreen = 1;
int  Color=RED, Bcolor=BLUE, mkind=7, mspeed=0, mlevel=0,
     msound=1, mtime=0, Round=1, step=3;
void far *address;

void interrupt (*oldkey)();
char  down0, busy; unsigned char ch;
void  interrupt  newkey()
 {  char flag = 0;
    if(busy==1)  return;   busy = 1;
    ch = inport(0x60);
    down0 = ch&(~0x7f)?0:1;
    switch(ch&0x7f)
     { case 0x4b: left = down0 ;  break;
       case 0x4d: right = down0 ;  break;
       case 0x48: up = down0 ;  break;
       case 0x50: down = down0;  break;
       case 0x39: space = down0; break;
       default: flag = 1;
     }
    oldkey();
    if(flag==0)  *((char far*)0x0041aL) = *((char far*)0x0041cL);
    busy = 0;
  }



int  main(int argc, char **argv)
 { int  graphdriver = VGA, graphmode=VGAHI, k, flag;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "C:\\TC");
{  char  str[100] = "";
   for(k=1; k<argc; k++)
     strcat(str, argv[k]);
   for(k=0; k<strlen(str); k++)
     if(str[k]=='Q' || str[k]=='q')   ishowscreen = 0;
}
   oldkey = getvect(0x09);
   disable(); setvect(0x09, newkey); enable();
   setbkcolor(BLUE);
   setcolor(YELLOW);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   address = getvect(0x33);
   if(address==NULL || *(unsigned char *)address==0xcf)
    { outtextxy(320, 200, "Mouse driver not installed, please install it!");
      outtextxy(320, 230, "(Run \"mouse.com\")");
      outtextxy(320, 260, "Press any key to continue . . .");
      getch();
      Exit(1, 1);
    }
   get_set();

   hzk24fp = fopen("els.hzk", "rb");
   if(hzk24fp == NULL)
    { outtextxy(320, 220, "Can't open file \"ELS.HZK\" !");
      outtextxy(320, 260, "Press any key to continue . . .");
      getch();  Exit(1, 1);
    }
   hzk16fp = fopen("menu.hzk", "rb");
   if(hzk16fp == NULL)
    { outtextxy(320, 220, "Can't open file \"MENU.HZK\" !");
      outtextxy(320, 260, "Press any key to continue . . .");
      getch();  Exit(1, 1);
    }
   for(k=0; k<20; k++)  BJ[k] = 0xe007;
   for(   ; k<23; k++)  BJ[k] = 0xffff;

   if(ishowscreen==1)   { screen(); delay(100); }
   setbkcolor(Bcolor);
   initmouse();   screen2();
   setmousexy(470, 150);
   randomize();
   mouse_on();
BEGIN
   mouse_move();
   if(_time_==1)  show_time(0);
   if((flag=getmousebutton())!=NOBUTTON)
     switch(mouse_click())
      { case 0:  kind = kind==15?7:15;  write_msg(); break;
	case 1:  flag==LEFTBUTTON? level++: level--;
		 level = level>15?0:level<0?level=15:level;
		 write_msg();  break;
	case 2:  flag==LEFTBUTTON? speed++: speed--;
		 speed = speed>15?0:speed<0?speed=15:speed;
		 write_msg();  break;
	case 13:
	case 3:  _sound_ = 1-_sound_; write_msg();  break;
	case 4:  _time_ = 1-_time_; show_time(0); break;
	case 10: KZ = 0x1e; kz_key(); ZT = 0x78; run(); break;
	case 15:
	      {  int  _speed_ = speed, _kind_ = kind, _level_ = level,
		      _PASS_ = PASS ;
		 if(use()==0)
		   { initflag = 1; KZ = 0x1e; kz_key(); ZT = 0xf8;
		     write_msg();  accept(); run();
		     speed = _speed_; kind = _kind_; level = _level_;
		     PASS = _PASS_;  write_msg();
		   }  break;
	      }
	case 16: Exit(0, 0); break;
	case 21: set();      break;
	case 20: help_msg(); break;
      }
   if(space) { KZ = 0x1e; kz_key(); ZT = 0xf8; run(); }
   if(kbhit())
     switch((unsigned char)getch())
       { case SPACE: KZ = 0x1e; kz_key(); ZT = 0xf8; run(); break;
	 case 'D': case 'd':
		 _sound_ = 1-_sound_; write_msg(); break;
	 case 'U': case 'u':
	      {  int  _speed_ = speed, _kind_ = kind, _level_ = level,
		      _PASS_ = PASS ;
		 if(use()==0)
		   { initflag = 1; KZ = 0x1e; kz_key(); ZT = 0xf8;
		     write_msg();  accept(); run();
		     speed = _speed_; kind = _kind_; level = _level_;
		     PASS = _PASS_;  write_msg();
		   }  break;
	      }

       }
WEND
 }


void  Exit(int code, int flag)
 {  char ch;  int X, Y;
    char far *p;
    if(flag == 1)
     { fcloseall();       closegraph();
       exit(code);
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
	    delay(500);
	    return;
	  }
	 if(bioskey(1)&&((ch==bioskey(0))=='Y'||ch=='y'||ch==13) ||
	   getmouse_xy(&X, &Y)==LEFTBUTTON&&imouse_inbox(530, 220, 560, 260)==YES)
	   break;
      }
    disable(); setvect(0x09, oldkey); enable();
    fcloseall();    closegraph();    exit(code);
 }

void  screen2()
 { int i, j;
   setcolor(YELLOW);
   rectangle(LEFT-3, TOP-3, RIGHT+3, BUTTOM+3);
   rectangle(LEFT-7, TOP-7, RIGHT+7, BUTTOM+7);
   setlinestyle(0, 0, 3);   setcolor(GREEN);
   rectangle(LEFT-5, TOP-5, RIGHT+5, BUTTOM+5);
   sethz_file(hzk24fp);   sethz_dot(24);
   sethz_fx(Y_ROUND);     sethz_zoomxy(1, 1);
   sethz_text(LEFT_HZ, TOP_HZ);
   sethz_speed(0);
   sethz_color(RED);     writehzs(20, 10, 8, 12);
   sethz_color(GREEN);   writehzs(21, 11, 8, 12);
   rectangle(40, 40, 140, 140);
   sethz_dxy(10);
   for(j=0; j<5; j++)
     { sethz_color(DARKGRAY);  writehzs(20, 170+j*35, 28+j*2, 28+j*2+1);
       sethz_color(LIGHTGRAY); writehzs(21, 172+j*35, 28+j*2, 28+j*2+1);
     }
   settextstyle(1, 0, 4);
   setcolor(GREEN);
   outtextxy(100, 430, "Skyever");
   outtextxy(540, 450, "Ver : 1.1");
   setcolor(RED);      outtextxy(535, 200, "Test: 0");
   setcolor(YELLOW);   outtextxy(537, 202, "Test: 0");
   settextstyle(1, 0, 3);
   setcolor(LIGHTGRAY);    outtextxy(535, 240, "MAX: 0");
   setlinestyle(0, 0, 1);  setcolor(LIGHTBLUE);
   for(j=0; j<3; j++)  for(i=0; i<2; i++)
    rectangle(455+i*80, 295+j*30, 525+i*80, 320+j*30);
   rectangle(465, 395, 600, 422);
   rectangle(15, 375, 95, 402);
   rectangle(105, 375, 185, 402);
   kz_key();
   write_msg();
   show_time(0);
   face(0);
 }

void  kz_key()
 { int  i, j;
   setlinestyle(0, 0, 1);   setcolor(YELLOW);
   sethz_file(hzk16fp);     sethz_dot(16);
   sethz_dxy(0);sethz_fx(X_ROUND);  sethz_zoomxy(2, 1);
   for(j=0; j<3; j++)
    for(i=0; i<2; i++)
     { if(get_KZ(j*2+i))  sethz_color(LIGHTGREEN);
       else                    sethz_color(LIGHTGRAY);
       writehzs(460+i*80, 300+j*30, (j*2+i)*2, (j*2+i)*2+1);
     }
   if(get_KZ(6))   sethz_color(LIGHTGREEN);
   else               sethz_color(LIGHTGRAY);
   writehzs(470, 400, 12, 15);
   sethz_color(LIGHTGREEN);
   writehzs(25, 380, 16, 17);
   writehzs(115, 380, 18, 19);
 }

void Delay(int n)
 { long T=clock();  long k=0;
   while(clock()-T<n)
     if(++k==100000L) { k=0; Sound(-1); }
 }

int  run()
 { void  _return();
   int  X0, k, FLAG=0, End=0;
   char leftflag=0, rightflag=0, upflag=0, downflag=0, spaceflag=0;
   if(initflag==0)
    { FS0 = PASS;
      speed0 = speed;
      for(k=0; k<level; k++) do
	{ BJ[19-k] = random(0x7fff);
	  BJ[19-k] |= 0xe007;
	} while(BJ[19-k] == 0xffff);
     F0 = random(kind);
     R0 = random(4);
   }
   renew();
   Time0 = clock();
   if(initflag==1)  Time0 -= dT;
BEGIN
   if(initflag==0)
    { F = F0;  R = R0;
      F0 = random(kind);  R0 = random(4);
      show_left_els(F0, R0);
      X = 3;    X0 = X;
      if(readx(F, R, 0)==0)  Y = -1;
      else             Y = 0;
    }
   deal_tx(0, 0, 0);
   CLOCK0 = clock();
   if(initflag==1)  initflag = 0;
   if(KZ==0x0e)  { KZ = 0x1e;  kz_key(); }
 BEGIN
     mouse_move();
     if(getmousebutton()==LEFTBUTTON)
       switch(mouse_click())
	{ case 13:
	  case 3:  _sound_ = 1-_sound_;  write_msg();   break;
	  case 4:  _time_  = 1-_time_;   show_time(T);  break;
	  case 11: FLAG = 1-FLAG;
		   if(FLAG==1) dTime = clock();
		   else        Time0 += clock()-dTime;
		   break;
	  case 12: _return();   return 0;
	  case 14: if(KZ==0x1e&&!save())  { KZ = 0x0e; kz_key();}  break;
	  case 21: dTime = clock(); set(); Time0 += clock()-dTime; break;
	  case 20: dTime = clock(); help_msg(); Time0 += clock()-dTime; break;
	}
     if(kbhit() && FLAG==1)
       switch((unsigned char)getch())
	{ case ENTER: if(End==0) { FLAG = 1-FLAG;  Time0 += clock()-dTime; }
		      break;
	  case ESC:   _return();  return 0;
	  case 'S': case 's':
		   if(KZ==0x1e&&!save()) { KZ = 0x0e; kz_key();} break;
	}
     if(FLAG==1) continue;
     Time = CLOCK = clock();
     if(CLOCK-CLOCK0>=16-speed)
       { if(deal_tx(0,1,0)==-1)
	  { Sound(2);
	    if((readx(F, R, 0)==0&&Y==-1)||(readx(F, R, 0)==-1&&Y==0) && X==X0)
	      { end_deal(); FLAG = 1; End = 1; }
	    addinbj(); accept(); break;
	  }
	 CLOCK0 = CLOCK;
       }
     if((int)((Time-Time0)/18.2) != T)
       { T=(Time-Time0)/18.2;
	 show_time(T);
       }
     if(left)
       { deal_tx(-1, 0, 0);  Sound(1);
	 Delay(leftflag==0?3:1);
	 leftflag = 1;
       }
     else  leftflag = 0;
     if(right)
       { deal_tx(1, 0, 0);  Sound(1);
	 Delay(rightflag==0?3:1);
	 rightflag = 1;
       }
     else  rightflag = 0;
     if(down)
       { deal_tx(0, 1, 0);  Sound(1);
	 Delay(downflag==0?3:1);
	 downflag = 1;
       }
     else downflag = 0;
     if(up)
       { for(k=0; k<step; k++) deal_tx(0, 1, 0);
	 Sound(1);
	 Delay(upflag==0?3:1);
	 upflag = 1;
       }
     else upflag = 0;
     if(space)  round_deal:
       { if(deal_tx(0, 0, Round))  round_case();
	 Sound(1);
	 Delay(spaceflag==0?3:1);
	 spaceflag = 1;
       }
     else spaceflag = 0;
     if(kbhit())
       switch((unsigned char)getch())
	 { case  0: switch((unsigned char)getch())
		     { /*
		       case  115: case 155:
		       case  LEFT_KEY:  deal_tx(-1, 0, 0); Sound(1); break;
		       case  116: case 157:
		       case  RIGHT_KEY: deal_tx(1, 0, 0);  Sound(1); break;
		       case  145: case 160:
		       case  DOWN_KEY:  deal_tx(0, 1, 0);  Sound(1); break;
		       case  141: case 152:
		       case  UP_KEY:    for(k=0; k<step; k++) deal_tx(0, 1, 0);
					       Sound(1); break;
		       */
		       case  148:  goto round_deal;
		     } break;
	   case  TAB:  case '0':
	/*   case  SPACE: round_deal:
		       Sound(1);
			    if(!deal_tx(0, 0, Round)) break;
			    round_case();     break;*/
	   case  ENTER: FLAG = 1-FLAG;
                       if(FLAG==1) dTime = clock();
		       else        Time0 += clock()-dTime;
		       break;
	   case  ESC: if(bioskey(2)&0x03) { _return();  return 0; } break;
	   case 'D':   case 'd':
		      _sound_ = 1-_sound_; write_msg(); break;
	   case 'S':  case 's':
	      if(KZ==0x1e&&!save()) { KZ = 0x0e; kz_key();} break;
	 }
     Sound(-1);
 WEND
WEND
 }

void  _return()
 { int k;
   for(k=0; k<20; k++)  BJ[k] = 0xe007; renew();
   KZ = 0xe9;  ZT = 0xff; kz_key(); speed = speed0;
   FS = 0; accept();
   write_msg();
   show_time(0);
   Sound(0);
 }

void  write_msg()
 { setfillstyle(0, 0);      settextstyle(1, 0, 3);
   setcolor(LIGHTGREEN);    setlinestyle(2, 0, 1);
   setwritemode(XOR_PUT);
   _rectangle(0,0,0,0);
   setcolor(LIGHTGRAY);     setlinestyle(0, 0, 1);
   setwritemode(COPY_PUT);
   mouse_off();
   settextjustify(CENTER_TEXT, TOP_TEXT);
   bar(90, 170+0*35, 180, 195+0*35);
   outtextxy(120, 170+0*35, kind==7?"A":"B");
   bar(90, 170+1*35, 180, 195+1*35);
   _outtextxy(120, 170+1*35, "%d", level);
   bar(90, 170+2*35, 180, 195+2*35);
   _outtextxy(120, 170+2*35, "%d", speed);
   bar(90, 170+3*35, 180, 195+3*35);
   outtextxy(120, 170+3*35, _sound_==1?"ON":"OFF");
   mouse_on();
 }

int  mouse_move()
 { int  mouse_in(int,int,int,int);
   setlinestyle(2, 0, 1);
   setcolor(LIGHTGREEN);
   setwritemode(XOR_PUT);

  if(get_ZT(0) && mouse_in(20, 170+0*35, 180, 195+0*35))
	  _rectangle(20, 170+0*35, 180, 195+0*35);
  else if(get_ZT(1) && mouse_in(20, 170+1*35, 180, 195+1*35))
	  _rectangle(20, 170+1*35, 180, 195+1*35);
  else if(get_ZT(2) && mouse_in(20, 170+2*35, 180, 195+2*35))
	   _rectangle(20, 170+2*35, 180, 195+2*35);
  else if(get_ZT(3) && mouse_in(20, 170+3*35, 180, 195+3*35))
	   _rectangle(20, 170+3*35, 180, 195+3*35);
  else if(get_ZT(4) && mouse_in(20, 170+4*35, 180, 195+4*35))
	   _rectangle(20, 170+4*35, 180, 195+4*35);
  else if(get_KZ(0) && mouse_in(455+0*80, 295+0*30, 525+0*80, 320+0*30))
	   _rectangle(455+0*80, 295+0*30, 525+0*80, 320+0*30);
  else if(get_KZ(1) && mouse_in(455+1*80, 295+0*30, 525+1*80, 320+0*30))
	   _rectangle(455+1*80, 295+0*30, 525+1*80, 320+0*30);
  else if(get_KZ(2) && mouse_in(455+0*80, 295+1*30, 525+0*80, 320+1*30))
	   _rectangle(455+0*80, 295+1*30, 525+0*80, 320+1*30);
  else if(get_KZ(3) && mouse_in(455+1*80, 295+1*30, 525+1*80, 320+1*30))
	   _rectangle(455+1*80, 295+1*30, 525+1*80, 320+1*30);
  else if(get_KZ(4) && mouse_in(455+0*80, 295+2*30, 525+0*80, 320+2*30))
	   _rectangle(455+0*80, 295+2*30, 525+0*80, 320+2*30);
  else if(get_KZ(5) && mouse_in(455+1*80, 295+2*30, 525+1*80, 320+2*30))
	   _rectangle(455+1*80, 295+2*30, 525+1*80, 320+2*30);
  else if(get_KZ(6) && mouse_in(465, 395, 600, 422))
	  _rectangle(465, 395, 600, 422);
  else if(mouse_in(15, 375, 95, 402))    _rectangle(15, 375, 95, 402);
  else if(mouse_in(105, 375, 185, 402))  _rectangle(105, 375, 185, 402);
  setwritemode(COPY_PUT);
  setlinestyle(0, 0, 1);
  return 0;
 }

int  mouse_in(int x1, int y1, int x2, int y2)
 {  return imouse_inbox(x1, y1, x2, y2) && (X1-x1||X2-x2||Y1-y1||Y2-y2);
 }

void  _rectangle(int x1, int y1, int x2, int y2)
 {  mouse_off();
    rectangle(X1, Y1, X2, Y2);  X1=x1; Y1=y1; X2=x2; Y2=y2;
    rectangle(X1, Y1, X2, Y2);  mouse_on();
 }

int  mouse_click()
 { while(getmousebutton()!=NOBUTTON);
   while(kbhit())  getch();
       if(imouse_inbox(20, 170+0*35, 180, 195+0*35))  return 0;
  else if(imouse_inbox(20, 170+1*35, 180, 195+1*35))  return 1;
  else if(imouse_inbox(20, 170+2*35, 180, 195+2*35))  return 2;
  else if(imouse_inbox(20, 170+3*35, 180, 195+3*35))  return 3;
  else if(imouse_inbox(20, 170+4*35, 180, 195+4*35))  return 4;
  else if(imouse_inbox(455+0*80, 295+0*30, 525+0*80, 320+0*30))  return 10;
  else if(imouse_inbox(455+1*80, 295+0*30, 525+1*80, 320+0*30))  return 11;
  else if(imouse_inbox(455+0*80, 295+1*30, 525+0*80, 320+1*30))  return 12;
  else if(imouse_inbox(455+1*80, 295+1*30, 525+1*80, 320+1*30))  return 13;
  else if(imouse_inbox(455+0*80, 295+2*30, 525+0*80, 320+2*30))  return 14;
  else if(imouse_inbox(455+1*80, 295+2*30, 525+1*80, 320+2*30))  return 15;
  else if(imouse_inbox(465, 395, 600, 422))                      return 16;
  else if(imouse_inbox(15, 375, 95, 402))    return 20;
  else if(imouse_inbox(105, 375, 185, 402))  return 21;
  else return -1;
 }

void time_deal(char *str)
 { setfillstyle(0, 0);      settextstyle(1, 0, 3);
   setcolor(LIGHTGREEN);    setlinestyle(2, 0, 1);
   setwritemode(XOR_PUT);
   _rectangle(0,0,0,0);
   setcolor(LIGHTGRAY);     setlinestyle(0, 0, 1);
   setwritemode(COPY_PUT);
   mouse_off();
   settextjustify(CENTER_TEXT, TOP_TEXT);
   bar(80, 170+4*35, 180, 195+4*35);
   outtextxy(130, 170+4*35, str);
   mouse_on();
 }

void  show_time(long T)
 { static struct time TI;
   static long T0;
   if(_time_==0)  time_deal(_mktime(T));
   else
    { gettime(&TI);
      if(T0 != TI.ti_hour*3600L + TI.ti_min*60L + TI.ti_sec)
       { T0 = TI.ti_hour*3600L + TI.ti_min*60L + TI.ti_sec;
	 time_deal(_mktime(T0));
       }
    }
 }

int  save()
 { FILE  *fp;
   int  k;
   fp = fopen("els.sav", "wt");
   if(fp == NULL)  return -1;
   for(k=0; k<20; k++)    fprintf(fp, "[%u]" , BJ[k]);
   fputc('\n', fp);
   fprintf(fp, "%d,%d,%d,%d\n",  kind, level, speed, speed0);
   fprintf(fp, "%d,%d,%d,%d,%d,%d\n", X, Y, F, R, F0, R0);
   fprintf(fp, "%d,%ld,%ld,%ld,%ld\n", PASS, FS, FS0, maxFS, Time-Time0);
   fclose(fp);
   return 0;
 }

int  use()
 { FILE  *fp;
   int  k;
   fp = fopen("els.sav", "rt");
   if(fp == NULL)  return -1;
   for(k=0; k<20; k++)     fscanf(fp, "[%u]" , &BJ[k]);
   fgetc(fp);
   fscanf(fp, "%d,%d,%d,%d\n",  &kind, &level, &speed, &speed0);
   fscanf(fp, "%d,%d,%d,%d,%d,%d\n", &X, &Y, &F, &R, &F0, &R0);
   fscanf(fp, "%d,%ld,%ld,%ld,%ld\n", &PASS, &FS, &FS0, &maxFS, &dT);
   fclose(fp);
   return 0;
 }

#define _X  60
#define _Y  100

void  set()
 { int k;
   int  mouse_remove();
   void set_msg();
   char  far *ptr;
   sethz_file(hzk16fp);  sethz_dot(16);  sethz_fx(0);
   sethz_color(RED);   sethz_dxy(2);  sethz_zoomxy(1, 1);
   ptr = (char far*)farmalloc(imagesize(_X, _Y, _X+200+10, _Y+20*11+40));
   getimage(_X, _Y, _X+200+10, _Y+20*11+40, ptr);
   setfillstyle(1, DARKGRAY);
   mouse_off();
   bar(_X+10, _Y+10, _X+200+10, _Y+20*11+40);
   setfillstyle(1, LIGHTBLUE);
   bar(_X, _Y, _X+200, _Y+20*11+30);
   setlinestyle(0, 0, 3);  setcolor(YELLOW);
   rectangle(_X+5, _Y+5, _X+195, _Y+20*11+23);
   for(k=0; k<11; k++)
     writehzs(_X+20, _Y+20*k+17, 20+k*4, 20+k*4+3);
   mouse_on();
BEGIN
   set_msg();
   switch(mouse_remove())
    { case -1: goto out;
      case  0: if((k=
	show_menu(16, 64,65, 66,67, 68,69, 70,71, 72,73, 74,75, 76,77, 78,79,
		      80,81, 82,83, 84,85, 86,87, 88,89, 90,91, 92,93, 94,95))
		   != -1)    setbkcolor(Bcolor=k);  break;
      case  1: if((k=
	show_menu(16, 64,65, 66,67, 68,69, 70,71, 72,73, 74,75, 76,77, 78,79,
		      80,81, 82,83, 84,85, 86,87, 88,89, 90,91, 92,93, 94,95))
		   != -1)    Color = k;  break;
      case  2: Round = -Round;  break;
      case  3: if((k=show_menu(4, 104,104, 105,105, 106,106, 107,107))!=-1)
		   step = k+2;  break;
      case  4: mkind = mkind==7?15:7; break;
      case  5: if((k=
	show_menu(16, 102,102, 103,103, 104,104, 105,105, 106,106, 107,107,
		      108,108, 109,109, 110,110, 111,111, 112,113, 114,115,
		      116,117, 118,119, 120,121, 122,123, 124,125))
		   != -1)     mlevel = k;  break;
      case  6: if((k=
	show_menu(16, 102,102, 103,103, 104,104, 105,105, 106,106, 107,107,
		      108,108, 109,109, 110,110, 111,111, 112,113, 114,115,
		      116,117, 118,119, 120,121, 122,123, 124,125))
		   != -1)     mspeed = k;  break;
      case  7: msound = 1-msound;  break;
      case  8: mtime  = 1-mtime;   break;
      case  9: if((k=show_menu(4, 140,144, 145,149, 150,154, 155,159))!=-1)
		 PASS = k==0?20000:k==1?10000:k==2?5000:k==3?2500:0;
      case 10:{ FILE  *fp;
		fp = fopen("set.msg", "wt");
		if(fp==NULL)  break;
		fprintf(fp, "Bcolor:%d\n", Bcolor);
		fprintf(fp, "Color:%d\n",  Color);
		fprintf(fp, "Round:%d\n",  Round);
		fprintf(fp, "Step:%d\n",   step);
		fprintf(fp, "Kind:%d\n",   mkind);
		fprintf(fp, "Level:%d\n",  mlevel);
		fprintf(fp, "Speed:%d\n",  mspeed);
		fprintf(fp, "Sound:%d\n",  msound);
		fprintf(fp, "Time:%d\n",   mtime);
		fprintf(fp, "Pass:%d\n",   PASS);
		fclose(fp);  break;
	      }
    }
WEND
   out:
   mouse_off();
   putimage(_X, _Y, ptr, COPY_PUT);
   if(get_ZT(7)==0) { renew();  deal_tx(0, 0, 0);  show_left_els(F, R); }
   mouse_on();
   farfree(ptr);
 }

void  set_msg()
 { int  F;
   setfillstyle(1, LIGHTBLUE);
   mouse_off();
   bar(_X+100, _Y+10, _X+190, _Y+20*11+20);
   sethz_color(YELLOW);
   writehzs(_X+120, _Y+20*0+17, 64+Bcolor*2, 64+Bcolor*2+1);
   writehzs(_X+120, _Y+20*1+17, 64+Color*2,  64+Color*2+1 );
   writehzs(_X+120, _Y+20*2+17, 96+(Round+1)/2*3, 96+(Round+1)/2*3+2);
   writehz (_X+120, _Y+20*3+17, 102+step);
   writehz (_X+120, _Y+20*4+17, 124+(mkind==15));
   if(mlevel<10)  writehz(_X+120, _Y+20*5+17, 102+mlevel);
   else  writehzs(_X+120, _Y+20*5+17, 112+(mlevel-10)*2, 112+(mlevel-10)*2+1);
   if(mspeed<10)  writehz(_X+120, _Y+20*6+17, 102+mspeed);
   else  writehzs(_X+120, _Y+20*6+17, 112+(mspeed-10)*2, 112+(mspeed-10)*2+1);
   writehzs(_X+120, _Y+20*7+17, 126+(1-msound)*3, 126+(1-msound)*3+2);
   writehzs(_X+120, _Y+20*8+17, 132+mtime*4, 132+mtime*4+3);
   F = PASS==20000?0:PASS==10000?1:PASS==5000?2:PASS==2500?3:0;
   writehzs(_X+100, _Y+20*9+17, 140+F*5, 140+F*5+4);
   mouse_on();
 }

int  mouse_remove()
 { int k=-1, k0=-1, flag = 0;
   int X, Y;
   setlinestyle(2, 0, 1);
   setcolor(CYAN);
   setwritemode(XOR_PUT);
   do
    { getmouse_xy(&X, &Y);
      if(X>_X && X<_X+200)
       { k0 = (Y-_Y-17)/20;
	 flag = 0;
	 if(k0>10||k0<0)  k0 = -1;
	 if(k0==9&&get_KZ(7)==0)  { k0 = -1; flag=1; }
	 if(k==k0)  continue;
	 mouse_off();
	 if(k0!=-1)  rectangle(_X+10, _Y+k0*20+15, _X+190, _Y+(k0+1)*20+15);
	 if(k !=-1)  rectangle(_X+10, _Y+k*20+15,  _X+190, _Y+(k+1)*20+15);
	 mouse_on();
	 k = k0;
       }
    } while( (getmousebutton()==NOBUTTON&&!kbhit()) ||
	      flag&&imouse_inbox(_X+10, _Y+15, _X+190, _Y+11*20+15));
   mouse_off();
   if(k!=-1)  rectangle(_X+10, _Y+k*20+15, _X+190, _Y+(k+1)*20+15);
   mouse_on();
   setwritemode(COPY_PUT);
   delay(500);
   if(kbhit()) { getch();  return -1; }
   if(!imouse_inbox(_X+10, _Y+15, _X+190, _Y+11*20+15))  return -1;
   return k;
 }

void  get_set()
 { FILE  *fp;
   fp = fopen("set.msg", "rt");
   if(fp==NULL)  return;
   fscanf(fp, "Bcolor:%d\n", &Bcolor);
   fscanf(fp, "Color:%d\n",  &Color);
   fscanf(fp, "Round:%d\n",  &Round);
   fscanf(fp, "Step:%d\n",   &step);
   fscanf(fp, "Kind:%d\n",   &mkind);
   fscanf(fp, "Level:%d\n",  &mlevel);
   fscanf(fp, "Speed:%d\n",  &mspeed);
   fscanf(fp, "Sound:%d\n",  &msound);
   fscanf(fp, "Time:%d\n",   &mtime);
   fscanf(fp, "Pass:%d\n",   &PASS);
   kind = mkind;    level = mlevel;
   speed = mspeed;  _sound_ = msound;
   _time_ = mtime;
   fclose(fp);
 }

void  help_msg()
 { int l, k;
   sethz_file(hzk16fp);   sethz_color(WHITE);
   sethz_zoomxy(1, 1);    sethz_fx(0);
   sethz_text(LEFT_HZ, TOP_HZ);   sethz_dxy(0);
   sethz_dot(16);         sethz_mode(XOR_MODEL);
   sethz_speed(0);
   for(l=0; l<2; l++)
    { for(k=161; k<=280; k++)
	writehz( 55+(((k-159)%30)*18), 165+(((k-159)/30)*30), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON);
      kbhit()&&(getch()==0?getch():0);
    }
   for(l=0; l<2; l++)
    { for(k=281; k<=553; k++)
	writehz( 55+(((k-279)%30)*18), 130+(((k-279)/30)*25), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON);
      kbhit()&&(getch()==0?getch():0);
    }
   sethz_mode(COPY_MODEL);
 }

void  Sound(int code)
 { static int _delay[4], _hz[4], bcode, num, K;
   if(_sound_==0)  { nosound(); return; }
   if(code!=-1)
    { switch(code)
       { case 0: break;
	 case 1: _delay[0]=0;   _delay[1] = 100;  _delay[2] = 100;  _delay[3] = 200;
		    _hz[0]=0;   _hz[1] = 500;     _hz[2] = 1000;    _hz[3] = 1500;
		 break;
	 case 2: _delay[0]=0;   _delay[1] = 150;  _delay[2] = 150;  _delay[3] = 150;
		    _hz[0]=0;   _hz[1] = 600;     _hz[2] = 600;     _hz[3] =  600;
		 break;
	 case 3: _delay[0]=0;   _delay[1] = 400;  _delay[2] = 400;  _delay[3] = 400;
		    _hz[0]=0;   _hz[1] = 800;     _hz[2] =   500;   _hz[3] =  0;
		 break;
       }
     num = 0;   K = 0;   if(_hz[0])  sound(_hz[0]);  else nosound();
    }
   if(code==-1&&bcode==0 || code==0)  { nosound(); return; }
   else  bcode = code;
   num++;
   if(num>=_delay[K])
     { if(K++==3)  { nosound();  bcode = 0; }
       else        { sound(_hz[K]); num = 0; }
     }
 }
