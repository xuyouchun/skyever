#include <graphics.h>
#include <math.h>
#include <bios.h>
#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include <alloc.h>
#include "set.h"
#include "tools.h"
#include "mouse.h"
#include "hz.h"
#include "screen.h"
#include "extern.h"
#include "tab.h"
#include "key.h"

BALL      ball[16];
LASTBALL  lastball[16];
unsigned  B[16];

#define  NOBALL        0
#define  HAVEGETSPEED  1
#define  HAVEGETWRONG  2

FILE  *hzk24fp, *hzk16fp;
char  ishowscreen = 1;
double a;
char  ZT = 0xbf;
char  PLAY = 0;
char  key, pass=0, meet=0, pass0=0;
#define  INPLAY  (0x40&ZT)
char  PLAYFLAG = 0;
struct play player[2] = { { 1, RED, 7 }, { 0, BLUE, 7 } };
long  Time, T;
char  Ro=2, Rr=2;
char  RR;
char  far *SET;
char  ifsound = 0, ifwhile = 1, whilebegin = 0;
int   Redn, Bluen;

char  far *ptr = (char far*)0xa0000000L;


int main(int argc, char **argv)
 { int k; long L;
   int graphdriver = DETECT, graphmode;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "C:\\TC");
   build(argc, argv);
   setbkcolor(BLUE);
   if(ishowscreen == 1)  { screen();   delay(500); }
   test_sysT();
   for(k=0; k<16; k++)  ball[k].Life = INLIFE;

   SET = (char far *)farmalloc(57600L);
   for(L=0; L<57600L; L++)     SET[L] = 0;

   a  = 0.0015*(Ro+1);
   RR = 10+Rr*2;

   screen2();   get_ball(B);
   resetball();
   setmousexy(320, 400);
   mouse_on();
   randomize();
   write_player();
   write_pass();
   line_msg(">>WELL<<");   speed_msg(0, 0);
BEGIN
   mouse_move();
   if(whilebegin==1)
     { if(!ifwhile && player[0].player+player[1].player==0)
	{ reset();
	  ball[0].Speed = (double)random(400)/100+3;
	  ball[0].Rad   = 0;
	  line_msg("The New . . .");
	  DELAY(2000);
	  run(0);   reset();
	}
     }
   if(kbhit())
    { switch(getch())
       { case SPACE_KEY:
		 ball[0].Speed = (double)random(400)/100+3;
		 ball[0].Rad   = 0;
		 if(player[0].player==0)  run(0);
		 else if(player[1].player==0)  run(1);
		 reset();  break;
	 case ESC_KEY:
		 if(bioskey(2)&0x03)  { Exit(0, 0); break; }
		 else
		  { if(!INPLAY)  break;
		    ZT &= ~0x40; write_button();
		    pass = pass0; meet = 0; write_pass();
		    break;
		  }
	  case 'h': help_msg(); break;
	  case 'A': case 'a': about_msg(); break;
	  case 'S': case 's': set(); break;
	  case 0: switch(getch())
		 { case LEFT_KEY: if(INPLAY) break;
			if(player[0].player++==2)  player[0].player=0;
			 ZT = player[0].player&&player[1].player?0x3f:0xbf;
			 write_button(); write_player(); break;
		   case RIGHT_KEY: if(INPLAY) break;
			 if(player[1].player--==0)  player[1].player=2;
			 ZT = player[0].player&&player[1].player?0x3f:0xbf;
			 write_button(); write_player(); break;
		   case UP_KEY:  if(INPLAY)  break;
			 pass = pass==9?0:pass+1;  pass0 = pass;
			 write_pass(); break;
		   case DOWN_KEY: if(INPLAY)  break;
			 pass = pass==0?9:pass-1;  pass0 = pass;
			 write_pass(); break;
		   case DELETE_KEY: ifsound = 1-ifsound; break;
		 } break;
	 }
    }
   if((k=getmousebutton())==LEFTBUTTON ||
		     (k=getmousebutton())==RIGHTBUTTON&&mouse_click(0)==20)
    { switch(mouse_click(1))
       { case 0: ball[0].Speed = (double)random(400)/100+3;
		 ball[0].Rad   = 0;
		 if(player[0].player==0)  run(0);
		 else if(player[1].player==0)  run(1);
		 reset();  break;
	 case 1: if(!INPLAY)  break;
		 ZT &= ~0x40; write_button();
		 pass = pass0; meet = 0; write_pass();
		 break;
	 case 2: set(); break;
	 case 3: about_msg(); break;
	 case 4: help_msg(); break;
	 case 5: Exit(0, 0); break;
	 case -1:if(player[0].player+player[1].player==0)  break;
		 _rectangle(0,0,0,0);
		 if(player[0].player)  PLAY = 0;
		 else if(player[1].player)  PLAY = 1;
		 if(leave_speed()-1)   continue;
		 ball[0].Speed = _Speed;
		 ball[0].Rad   = _Rad;
		 run(PLAY);
		 reset();   break;
	 case 10:if(INPLAY)  break;
		 if(player[0].player++==2)  player[0].player=0;
		 ZT = player[0].player&&player[1].player?0x3f:0xbf;
		 write_button(); write_player(); break;
	 case 11:if(INPLAY)  break;
		 if(player[1].player--==0)  player[1].player=2;
		 ZT = player[0].player&&player[1].player?0x3f:0xbf;
		 write_button(); write_player(); break;
	 case 20:if(INPLAY)  break;
		 if(k==LEFTBUTTON)  pass = pass==9?0:pass+1;
		 else               pass = pass==0?9:pass-1;
		 pass0 = pass;      write_pass();
		 break;
	 case 30: Redn = Bluen = 0;  write_rb();  break;
       }
    }
WEND
 }

void  reset()
 { resetball();   write_button();
   write_pass();  write_ptr(0);
   line_msg(">>WELL<<");
   speed_msg(0, 0);
   mouse_on();
 }

void  Exit(int code, int flag)
 {  while(kbhit())  getch();
    if(flag==0&&show_select("The game will be completed, Are you sure?")==0)  return;
    fcloseall();    closegraph();    farfree(SET);    exit(code);
 }

int  show_select(char *S)
 {  char far *p;   int code;  char ch;  int X, Y;  long L;  int Len;
    settextstyle(1, 0, 3);    Len = 270;
    putch('\a');
    farfree(SET);
    p = (char far*)farmalloc(imagesize(320-Len-5, 240-40-5, 320+Len, 240+40));
    if(p==NULL)  return 1;
    getimage(320-Len-5, 240-40-5, 320+Len, 240+40, p);
    setfillstyle(1, DARKGRAY);
    mouse_off();
    bar(320-Len, 240-40, 320+Len, 240+40);
    setfillstyle(1, LIGHTBLUE);
    bar(320-Len-5, 240-40-5, 320+Len-5, 240+40-5);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(RED);
    outtextxy(320, 217, S);
    setcolor(YELLOW);
    outtextxy(320, 250, "(Yes / No)");
    mouse_on();
    while(1)
      {  while(!kbhit() && getmousebutton()==NOBUTTON);
	 if(kbhit())  ch = getch();  else  ch = 0;
	 if( (ch=='N'||ch=='n'||ch==27) ||
	  getmouse_xy(&X, &Y)==LEFTBUTTON&&imouse_inbox(340, 235, 375, 270)==YES)
	  { code = 0; break; }
	 if( (ch=='Y'||ch=='y'||ch==13) ||
	   getmouse_xy(&X, &Y)==LEFTBUTTON&&imouse_inbox(265, 235, 310, 270)==YES)
	  { code = 1; break; }
      }
    mouse_off();
    putimage(320-Len-5, 240-40-5, p, COPY_PUT);
    mouse_on();
    farfree(p);
    DELAY(2000);
    SET = (char far *)farmalloc(57600L);
    for(L=0; L<57600L; L++)
      SET[L] = 0;
    return code;
  }

void  screen2()
 { int i, j;
   setbkcolor(GREEN);
   setfillstyle(1, BLUE);
   bar(0, 0, 639, 479);
   setcolor(YELLOW);
   sethz_file(hzk24fp);   sethz_dot(24);
   sethz_fx(Y_ROUND);     sethz_zoomxy(2, 1);
   sethz_text(LEFT_HZ, TOP_HZ);  sethz_dxy(0);
   sethz_speed(0);
   sethz_color(RED);     writehzs(20, 10, 8, 11);
   sethz_color(GREEN);   writehzs(21, 11, 8, 11);
   settextstyle(1, 0, 4);
   setcolor(GREEN);
   outtextxy(100, 440, "Skyever");
   outtextxy(540, 440, "Ver : 1.1");
   write_tab();
   write_button();
   write_ballnum();
   write_rou();
   PLAYFLAG = 1;  show_time("00:00:00");  PLAYFLAG = 0;
   write_rb();
 }

#define  X3  ((X2+X1)/2)
void  write_rou()
 { setlinestyle(0, 0, 3);
   setcolor(DARKGRAY);
   setwritemode(COPY_PUT);
   mouse_off();
   line(X1, Y1, X1+RR, Y1);  line(X1, Y1, X1, Y1+RR);
   line(X1, Y2, X1+RR, Y2);  line(X1, Y2, X1, Y2-RR);
   line(X2, Y1, X2-RR, Y1);  line(X2, Y1, X2, Y1+RR);
   line(X2, Y2, X2-RR, Y2);  line(X2, Y2, X2, Y2-RR);
   line(X3-RR, Y1, X3+RR, Y1);
   line(X3-RR, Y2, X3+RR, Y2);
   mouse_on();
 }

void  write_tab()
 { int k;
   setcolor(YELLOW);
   rectangle(X1-1, Y1-1, X2+1, Y2+1);
   setfillstyle(0, 0);
   bar(X1, Y1, X2, Y2);
   setlinestyle(0, 0, 3);
   setcolor(LIGHTCYAN);   rectangle(X1-4, Y1-4, X2+4, Y2+4);
   setcolor(CYAN);        rectangle(X1-7, Y1-7, X2+7, Y2+7);
   setcolor(LIGHTGRAY);   rectangle(X1-10, Y1-10, X2+10, Y2+10);
			  rectangle(X1-13, Y1-13, X2+13, Y2+13);
   setcolor(DARKGRAY);    setlinestyle(2, 0, 1);
			  rectangle(X1-11, Y1-11, X2+11, Y2+11);
   setlinestyle(0, 0, 1);
   setfillstyle(2, LIGHTBLUE);
   bar(20, 45, 200, 85);
   setcolor(RED);          circle(45, 65, 15);
   setfillstyle(1, RED);   floodfill(45, 65, RED);
   setcolor(GREEN);        circle(175, 65, 15);
   setfillstyle(1, BLUE);  floodfill(175, 65, GREEN);
   setcolor(BLUE);         circle(175, 65, 15);
   write_button();
   setlinestyle(0, 0, 1);
   setcolor(LIGHTBLUE);
   for(k=0; k<3; k++)   rectangle(35+k*75, 380, 105+k*75, 405);
   for(k=0; k<3; k++)   rectangle(385+k*75, 380, 455+k*75, 405);
 }

void  write_button()
 { sethz_file(hzk16fp);   sethz_dot(16);
   sethz_fx(X_ROUND);     sethz_zoomxy(2, 1);
   sethz_text(LEFT_HZ, TOP_HZ);  sethz_dxy(0);
   sethz_speed(0);
   sethz_color( (!player[1].player || !player[0].player)
	   &&!PLAYFLAG?GREEN:LIGHTGRAY);    writehzs(40, 385, 0, 1);
   sethz_color(ZT&0x40?GREEN:LIGHTGRAY);    writehzs(115, 385, 2, 3);
   sethz_color(ZT&0x20?GREEN:LIGHTGRAY);    writehzs(190, 385, 4, 5);
   sethz_color(ZT&0x10?GREEN:LIGHTGRAY);    writehzs(390, 385, 6, 7);
   sethz_color(ZT&0x08?GREEN:LIGHTGRAY);    writehzs(465, 385, 8, 9);
   sethz_color(ZT&0x04?GREEN:LIGHTGRAY);    writehzs(540, 385, 10, 11);
 }

int  Xx1=0, Yy1=0, Xx2=0, Yy2=0;
void  mouse_move()
 { int  k;
   int  mouse_in(int,int,int,int);
   void _rectangle(int, int, int, int);
   setlinestyle(2, 0, 1);
   setcolor(LIGHTGREEN);
   setwritemode(XOR_PUT);
   for(k=0; k<3; k++)
     if( (ZT&(0x80>>k)) && mouse_in(35+k*75, 380, 105+k*75, 405))
	  _rectangle(35+k*75, 380, 105+k*75, 405);
   for(k=0; k<3; k++)
     if( (ZT&(0x80>>k+3)) && mouse_in(385+k*75, 380, 455+k*75, 405))
	  _rectangle(385+k*75, 380, 455+k*75, 405);
  if( !INPLAY && mouse_in(26, 48, 63, 83) )
	  _rectangle(26, 48, 63, 83);
  if( !INPLAY && mouse_in(156, 48, 193, 83) )
	  _rectangle(156, 48, 193, 83);
  if( !INPLAY && mouse_in(235, 60, 405, 95) )
	  _rectangle(235, 60, 405, 95);
  if( !INPLAY && mouse_in(240, 428, 400, 457))
	  _rectangle(240, 428, 400, 457);
  setwritemode(COPY_PUT);
  setlinestyle(0, 0, 1);
 }

int  mouse_in(int x1, int y1, int x2, int y2)
 {  return imouse_inbox(x1, y1, x2, y2) && (Xx1-x1||Xx2-x2||Yy1-y1||Yy2-y2);
 }

void  _rectangle(int x1, int y1, int x2, int y2)
 {  setlinestyle(2, 0, 1);  setcolor(LIGHTGREEN);
    setwritemode(XOR_PUT);
    mouse_off();
    rectangle(Xx1, Yy1, Xx2, Yy2);  Xx1=x1; Yy1=y1; Xx2=x2; Yy2=y2;
    rectangle(Xx1, Yy1, Xx2, Yy2);  mouse_on();
 }

int  mouse_click(int code0)
 { int k, code=-1;
   for(k=0; k<3; k++)
     if( (ZT&0x80>>k) && imouse_inbox(35+k*75, 380, 105+k*75, 405) )  code = k;
   for(k=0; k<3; k++)
     if( (ZT&0x80>>k+3) && imouse_inbox(385+k*75, 380, 455+k*75, 405) ) code = k+3;
   if( (ZT&0x02) && imouse_inbox(26, 48, 63, 83) )       code = 10;
   if( (ZT&0x01) && imouse_inbox(156, 48, 193, 83) )     code = 11;
   if( (ZT&0x01) && imouse_inbox(235, 60, 405, 95) )     code = 20;
   if(imouse_inbox(210, 425, 430, 455) )                 code = 30;
   if(code0&&code!=-1)  while(getmousebutton()!=NOBUTTON);
   return code;
 }


void  build(int argc, char **argv)
 { int k;  char str[200] = "";
   void far *address;
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
   for(k=1; k<argc; k++)  strcat(str, argv[k]);
   get_msg();
   for(k=0; k<strlen(str); k++)
     switch(str[k])
	{  case 'Q': case 'q':  ishowscreen = 0;  break;
	   case 'G': player[0].player = 1; break;
	   case 'g': player[1].player = 1; break;
	   case 'B': player[0].player = 2; break;
	   case 'b': player[1].player = 2; break;
	   case 'C': player[0].player = 0; break;
	   case 'c': player[1].player = 0; break;
	   case 'R': case 'r': whilebegin = 1; ifwhile = 0; break;
	   case 'S': case 's': ifsound = 1; break;
	   default: if(str[k]<='9' && str[k]>='1')
		       pass = pass0 = str[k]-'0'-1;
		     else if(str[k]=='0')
		       pass = pass0 = 9;  break;
       }
   hzk24fp = fopen("tab.hzk", "rb");
   if(hzk24fp == NULL)
    { outtextxy(320, 230, "Can't open file \"TAB.HZK\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1, 1);
    }
   hzk16fp = fopen("menu.hzk", "rb");
   if(hzk16fp == NULL)
    { outtextxy(320, 230, "Can't open file \"MENU.HZK\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1, 1);
    }
 }


int  play(int code)
 { int k, color = test_life(player[code].color)?player[code].color:BLACK;
   int begin = color==RED?1:color==BLUE? 9:8,
       end   = color==RED?7:color==BLUE?15:8;
   line_msg(player[code].color==RED?"Red please . . .":"Blue please . . .");
   DELAY(2000);
   if(player[code].player==0)
     { if(!get_roads( color, (double)random(300)/100+0.5 )
	   && !get_roads( color, 0) )
	 { for(k=begin; k<=end; k++)  if(ball[k].Life==INLIFE)  break;
	   ball[0].Speed = (double)random(400)/100+3;
	   ball[0].Rad   = _get_rad(ball[0].X, ball[0].Y, ball[k].X, ball[k].Y);
	   return  HAVEGETWRONG;
	 }
       ball[0].Speed = FA.Speed;   ball[0].Rad = FA.Rad;
       return HAVEGETSPEED;
     }
   else
    { for(k=begin; k<=end; k++)  if(ball[k].Life == INLIFE)  break;
      if(k==end+1)  return  NOBALL;
    BEGIN
      mouse_move();
      if(kbhit()&&getch()==27)   return -1;
      show_time( _mktime(clock()-Time) );
      if(getmousebutton()!=LEFTBUTTON)  continue;
      if( (k=click_deal())==-1)  return -1;
      if( k==1 )  continue;
      if(leave_speed()==0)
	{ line_msg(player[code].color==RED?"Red please . . .":"Blue please . . .");
	  continue;
	}
      ball[0].Speed = _Speed;
      ball[0].Rad   = _Rad;
      return HAVEGETSPEED;
    WEND
    }
 }

int  run(int code)
 { int  flag, N=0;
   ZT = 0x78;  write_button();
   randomize();   PLAY = code;
   write_ptr(1);
   _rectangle(0, 0, 0, 0);
   PLAYFLAG = 1;
   setwritemode(COPY_PUT);
   write_button();
   write_ballnum();
   whilebegin = 0;
   if(!player[0].player&&player[1].player || player[0].player&&!player[1].player)
    { char str[20];
      sprintf(str, "WAVE %d", pass+1);
      setcolor(LIGHTCYAN);    show_str(320, 160, 6, str);
      sprintf(str, "The %s Chance", meet==0?"First":meet==1?"Second":"Last");
      setcolor(LIGHTBLUE); show_str(320, 300, 4, str);
      Sound(5);
      T = clock();
      while(clock()-T<15)  if(kbhit()||getmousebutton()!=NOBUTTON)  break;
    }
   ZT |= 0x40;
   Time = clock();
BEGIN
   do
    { reset_ballin();
      line_msg("Wait . . .");
      while(test_speed())
	{ if(N++==8)
	   { N = 0;
	     mouse_move();
	     show_time( _mktime(clock()-Time) );
	     if(click_deal()==-1)
	      { pass = pass0; meet = 0;
		ZT = 0xbf;    PLAYFLAG = 0; return -1;
	      }
	    }
	   move_ball();
	 }
      line_msg("");
      mouse_off(); show_ball(); mouse_on();
      reset_speedflag();
      if((flag=test_deal())==-1)
		{ line_msg("The End !");
		  setcolor(player[PLAY].color==BLUE?LIGHTBLUE:LIGHTRED);
		  show_str(320, player[PLAY].player?160:230, 7,
			 player[PLAY].color==BLUE?"Blue Win!":"Red Win!");
		  if(player[PLAY].player)
		   { setcolor(player[PLAY].player==1?LIGHTRED:LIGHTBLUE);
			 show_str(320, 300, 5, player[PLAY].player==1?
			  "Charming Girl!":"Handsome Boy!");
		   }
		  while(!kbhit()&&getmousebutton()==NOBUTTON)
			if(!ifwhile && player[0].player+player[1].player==0)
			  { long Time = clock();
				while(clock()-Time<50)
				  if(kbhit()||getmousebutton()!=NOBUTTON)  break;
				whilebegin = 1;
				break;
			  }
		  while(getmousebutton()!=NOBUTTON);
		  while(kbhit())  getch();
		  mouse_off();
		  setfillstyle(0, 0);  bar(X1, Y1, X2, Y2);
		  mouse_on();
		  if(player[0].player==0&&player[1].player || player[1].player==0&&player[0].player)
			if(player[PLAY].player)
			  { meet = 0;   pass++;
			    if(pass==10)
				 { setcolor(player[PLAY].player==1?LIGHTRED:LIGHTBLUE);
				   show_str(320, 240, 5, "Great! You win!");
				   while(getmousebutton()==NOBUTTON);
				   while(getmousebutton()!=NOBUTTON);
				   pass = 0;
				 }
			   ZT = 0xbf;
			   PLAYFLAG = 0;   return 0;
			 }
	      else { if(++meet<3)
		       { ZT = 0xff;  PLAYFLAG = 0;  return 1; }
		     else  { meet = 0;     ZT = 0xbf;
			     pass = pass0;  PLAYFLAG = 0;
			     setcolor(LIGHTRED);  settextstyle(1, 0, 5);
			     settextjustify(CENTER_TEXT, CENTER_TEXT);
			     mouse_off();
			     outtextxy(320, 240, "GAME OVER !");
			     setcolor(LIGHTGREEN);
			     outtextxy(323, 243, "GAME OVER !");
			     mouse_on();  delay(5000);
			     return -1;
			   }
		   }
	    else
	      { ZT = 0xbf;
		meet = 0;  PLAYFLAG = 0;  return -1;
	      }
	}
      write_ballnum();
      if(flag==1)  if(play(PLAY)==-1)  goto end;
    } while(flag==1);
   PLAY = 1-PLAY ;
   write_ptr(1);
   if(play(PLAY)==-1)  goto end;
WEND
end:
   ZT = player[0].player&&player[1].player?0x3f:0xbf;
   PLAYFLAG = 0;
   return 0;
 }

int  click_deal()
 { if(getmousebutton()==LEFTBUTTON)
     switch(mouse_click(1))
      { case 1: if(show_select("The game will reset, Are you sure?")==1)
				return -1;  return 1;
	case 2: set(); return 1;
	case 3: about_msg(); return 1;
	case 4: help_msg(); return 1;
      }
   if(kbhit())
     switch(getch())
      { case ESC_KEY: if(show_select("The game will reset, Are you sure?")==1)
			return -1;  return 1;
		case 'H': case 'h': help_msg(); return 1;
		case 'A': case 'a': about_msg(); return 1;
		case 'S': case 's': set(); return 1;
        case 0: if(kbhit()&&getch()==DELETE_KEY)  ifsound = 1-ifsound;
      }
   return 0;
 }

int  test_deal()
 { if(ballin.WHITE)  reset_inball(0);
   if(ballin.BLACK)  if(reset_black(player[PLAY].color)==HAVEEND)  return -1;
   if(player[PLAY].color==RED && ballin.RED &&
       !(ballin.BLUE+ballin.BLACK+ballin.WHITE) )  return 1;
   if(player[PLAY].color==BLUE && ballin.BLUE &&
       !(ballin.RED+ballin.BLACK+ballin.WHITE) )   return 1;
   return 0;
 }

#define pl(x)  player[x].player
void  write_player()
 { setcolor(LIGHTGREEN);
   settextstyle(0, 0, 1);
   settextjustify(CENTER_TEXT, TOP_TEXT);
   setfillstyle(1, BLUE);
   mouse_off();
   bar(10, 87, 210, 98);
   outtextxy(45, 89, pl(0)==0?"Computer":pl(0)==1?"Girl":"Boy");
   outtextxy(175, 89, pl(1)==0?"Computer":pl(1)==1?"Girl":"Boy");
   setcolor(LIGHTMAGENTA);
   outtextxy(110, 89, "VS");
   mouse_on();
 }

void  write_ballnum()
 { int i;
   setwritemode(XOR_PUT);
   setcolor(LIGHTGREEN);
   _rectangle(0, 0, 0, 0);
   setwritemode(COPY_PUT);
   settextstyle(1, 0, 4);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   player[0].leaves = player[1].leaves = 0;
   for(i=0; i<16; i++)
     if(ball[i].Life==INLIFE)
       if(i>=9)            player[1].leaves++;
       else if(i>=1&&i<8)  player[0].leaves++;
   setfillstyle(1, BLUE);       bar(70, 45, 150, 85);
   setfillstyle(2, LIGHTBLUE);  bar(70, 45, 150, 85);
   setcolor(DARKGRAY);
   _outtextxy(106, 61, "%d : %d", 7-player[0].leaves, 7-player[1].leaves);
   setcolor(YELLOW);
   _outtextxy(109, 64, "%d : %d", 7-player[0].leaves, 7-player[1].leaves);
 }

void  write_pass()
 { mouse_off();
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setwritemode(XOR_PUT);
   setcolor(LIGHTGREEN);
   _rectangle(0, 0, 0, 0);
   setwritemode(COPY_PUT);
   setfillstyle(1, BLUE);
   bar(220, 60, 420, 95);
   settextstyle(1, 0, 4);
   setcolor(YELLOW);
   _outtextxy(320, 75, "WAVE : %d", pass+1);
   setcolor(RED);
   _outtextxy(323, 78, "WAVE : %d", pass+1);
   bar(260, 375, 380, 405);
   setcolor(LIGHTGRAY);
   settextstyle(1, 0, 3);
   _outtextxy(320, 390, "%d%s of 3", meet+1, meet==0?"st":meet==1?"nd":"th");
   mouse_on();
 }

void  show_time(char *S)
 { if(PLAYFLAG == 0)  return;
   if(S[0] == 'N')  return;
   setfillstyle(1, BLUE);
   if(imouse_inbox(500,5,640,40)) mouse_off();
   bar(500, 5, 640, 40);
   settextstyle(1, 0, 4);
   setwritemode(COPY_PUT);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(DARKGRAY); outtextxy(556, 16, S);
   setcolor(GREEN);    outtextxy(559, 19, S);
   if(imouse_inbox(500,5,640,40)) mouse_on();
 }


#define _X  150
#define _Y  240
void  set()
 { int k; long L;
   int  mouse_remove();
   void set_msg();
   void  far *ptr;
   farfree(SET);
   sethz_file(hzk16fp);  sethz_dot(16);  sethz_fx(0);
   sethz_color(RED);   sethz_dxy(2);  sethz_zoomxy(1, 1);
   ptr = (void far *)farmalloc(imagesize(_X, _Y, _X+150+10, _Y+20*5+40));
   if(ptr==NULL)  return ;
   getimage(_X, _Y, _X+150+10, _Y+20*5+40, ptr);
   setfillstyle(1, DARKGRAY);
   mouse_off();
   bar(_X+10, _Y+10, _X+150+10, _Y+20*5+40);
   setfillstyle(1, LIGHTBLUE);
   bar(_X, _Y, _X+150, _Y+20*5+30);
   setlinestyle(0, 0, 3);  setcolor(YELLOW);
   rectangle(_X+5, _Y+5, _X+145, _Y+20*5+23);
   for(k=0; k<2; k++)
	 writehzs(_X+10, _Y+20*k+17, 12+k*4, 12+k*4+3);
   writehzs(_X+10, _Y+20*4+17, 12+2*4, 12+2*4+3);
   writehzs(_X+10, _Y+20*2+17, 21+2*4, 21+2*4+3);
   writehzs(_X+10, _Y+20*3+17, 25+2*4, 25+2*4+3);
   mouse_on();
 BEGIN
   set_msg();
   switch(k=mouse_remove())
    { case 10: Ro = Ro==0?4:Ro-1; break;
      case 0:  Ro = Ro==4?0:Ro+1; break;
      case 11: Rr = Rr==0?4:Rr-1; break;
	  case 1:  Rr = Rr==4?0:Rr+1; break;
	  case 12: case 2: ifsound = 1-ifsound; break;
	  case 13: case 3: ifwhile = 1-ifwhile; break;
	  case 4:  save();  break;
      case -1: goto out;
    }
 WEND
   out:
   a  = 0.002*(Ro+1);
   RR = 10+Rr*2;
   mouse_off();
   putimage(_X, _Y, ptr, COPY_PUT);
   mouse_on();
   setlinestyle(0, 0, 1);
   setcolor(YELLOW);  rectangle(X1-1, Y1-1, X2+1, Y2+1);
   write_rou();
   farfree(ptr);
   SET = (char far *)farmalloc(57600L);
   for(L=0; L<57600L; L++)
     SET[L] = 0;
   show_ball();
 }

void  set_msg()
 { int  F;
   setfillstyle(1, LIGHTBLUE);
   mouse_off();
   bar(_X+100, _Y+10, _X+140, _Y+20*4+20);
   sethz_color(YELLOW);
   writehz(_X+120, _Y+20*0+17, 24+Ro);
   writehz(_X+120, _Y+20*1+17, 24+Rr);
   writehz(_X+120, _Y+20*2+17, 31+ifsound);
   writehz(_X+120, _Y+20*3+17, 31+ifwhile);
   mouse_on();
 }

int  mouse_remove()
 { int k=-1, k0=-1;
   int X, Y, button;
   setlinestyle(2, 0, 1);
   setcolor(CYAN);
   setwritemode(XOR_PUT);
   do
    { getmouse_xy(&X, &Y);
      if(X>_X && X<_X+145)
       { k0 = (Y-_Y-17)/20;
	 if(k0>4||k0<0)  k0 = -1;
       }
      else  k0 = -1;
      if(k==k0)  continue;
      mouse_off();
      if(k0!=-1)  rectangle(_X+10, _Y+k0*20+15, _X+137, _Y+(k0+1)*20+15);
      if(k !=-1)  rectangle(_X+10, _Y+k*20+15,  _X+137, _Y+(k+1)*20+15);
      mouse_on();
      k = k0;
    } while( ((button=getmousebutton())==NOBUTTON&&!kbhit()) );
   mouse_off();
   if(k!=-1)  rectangle(_X+10, _Y+k*20+15, _X+137, _Y+(k+1)*20+15);
   mouse_on();
   setwritemode(COPY_PUT);
   while( getmousebutton()!=NOBUTTON );
   if(kbhit()) { getch();  return -1; }
   if(!imouse_inbox(_X+10, _Y+15, _X+190, _Y+11*20+15))  return -1;
   if(button==LEFTBUTTON) return k;
   else                   return k+10;
 }

void  save()
 { FILE *fp;
   fp = fopen("set.msg", "wt");
   if(fp==NULL)  return;
   fprintf(fp, "/******** TABLE  BALL  SET ********/\n");
   fprintf(fp, "Ro = %d;\n", Ro);
   fprintf(fp, "Rr = %d;\n", Rr);
   fprintf(fp, "Sound = %d;\n", ifsound);
   fprintf(fp, "While = %d;\n", ifwhile);
   fprintf(fp, "/******** TABLE  BALL  SET ********/\n");
   fclose(fp);
 }

void  get_msg()
 { FILE *fp;
   fp = fopen("set.msg", "rt");
   if(fp==NULL)  return;
   fscanf(fp, "/******** TABLE  BALL  SET ********/\n");
   fscanf(fp, "Ro = %d;\n", &Ro);
   fscanf(fp, "Rr = %d;\n", &Rr);
   fscanf(fp, "Sound = %d;\n", &ifsound);
   fscanf(fp, "While = %d;\n", &ifwhile);
   fscanf(fp, "/******** TABLE  BALL  SET ********/\n");
   fclose(fp);
 }

void  write_ptr(int code)
 { settextjustify(CENTER_TEXT, CENTER_TEXT);
   setwritemode(COPY_PUT);
   settextstyle(1, 0, 3);
   mouse_off();
   setcolor(RED);    outtextxy(46,  65, "P");
   setcolor(BLUE);   outtextxy(176, 65, "P");
   setcolor(YELLOW);
   if(code)  outtextxy(PLAY?176:46, 65, "P");
   mouse_on();
 }

#define  B  8
void  help_msg()
 { int l, k, X, Y, flag=0;
   sethz_file(hzk16fp);   sethz_color(BLUE);
   sethz_zoomxy(1, 1);    sethz_fx(0);
   sethz_text(LEFT_HZ, TOP_HZ);   sethz_dxy(0);
   sethz_dot(16);         sethz_mode(XOR_MODEL);
   sethz_speed(0);
   for(l=0; l<2; l++)
	{ for(k=94+B; k<=237+B; k++)
	writehz( 85+(((k-92-B)%26)*18), 150+(((k-92-B)/26)*30), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON)
	if(INPLAY)  show_time(_mktime(clock()-Time) );
      while(kbhit()) if(getch()==27)  flag = 1;
    }
   if(flag==1)  return;
   for(l=0; l<2; l++)
	{ for(k=238+B; k<=521+B; k++)
	writehz( 85+(((k-236-B)%26)*18), 130+(((k-236-B)/26)*20), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON)
	if(INPLAY)  show_time(_mktime(clock()-Time) );
      while(kbhit()) if(getch()==27)  flag = 1;
    }
   if(flag==1)  return;
   for(l=0; l<2; l++)
	{ for(k=522+B; k<=739+B; k++)
	writehz( 85+(((k-520-B)%26)*18), 150+(((k-520-B)/26)*20), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON)
	if(INPLAY)  show_time(_mktime(clock()-Time) );
      while(kbhit())  getch();
    }
   sethz_mode(COPY_MODEL);
 }

void  about_msg()
 { int l, k, X, Y;
   sethz_file(hzk16fp);   sethz_color(RED);
   sethz_zoomxy(1, 1);    sethz_fx(0);
   sethz_text(LEFT_HZ, TOP_HZ);   sethz_dxy(0);
   sethz_dot(16);         sethz_mode(XOR_MODEL);
   sethz_speed(0);
   for(l=0; l<2; l++)
	{ for(k=33+B; k<=92+B; k++)
	writehz( 125+(((k-31-B)%21)*18), 165+(((k-31-B)/21)*30), k);
      if(l==0)
      while(!kbhit() && getmouse_xy(&X, &Y)==NOBUTTON)
	if(INPLAY)  show_time(_mktime(clock()-Time) );
      kbhit()&&(getch()==0?getch():0);
    }
   sethz_mode(COPY_MODEL);
   while( getmousebutton()!=NOBUTTON );
 }

void  write_rb()
 { mouse_off();
   _rectangle(0,0,0,0);
   setwritemode(COPY_PUT);
   setfillstyle(1, BLUE);
   bar(210, 425, 430, 455);
   setcolor(CYAN);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   _outtextxy(320, 440, "%d : %d", Redn, Bluen);
   mouse_on();
 }