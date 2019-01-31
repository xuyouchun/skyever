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
#include <dir.h>
#include <io.h>
#include "tools.h"
#include "hz.h"
#include "screen.h"
#include "extern.h"
#include "tank.h"
#include <key.h>
#include <mouse.h>
void  show_str(int x, int y,  int code, char *str);
void  show_str2(int x, int y, int size, char *str);
int   getdeal(), findzj();
void  setscreen(), sethz24(), sethz16();
int   findfile(char *path, char file[][14], char attrib, int max);
int   Inword(int x, int y, char *S, int Num);
void  waitforclick(), editroad(int code, char *);
void  showico(int x, int y, int code);
void  showtank(int x, int y, int code), show_xy(), renew_screen();
void  runallroads(), show_msg(), show_test(), show_star();
void  show_line(int code, int n), Sound(int code), saveroad(), readroad();
int   show_select(int begin, int end, int code);
void  showfs();  void tank_eat(TANKDEF *Tankp); extern void far edkind();
int   tankfire(int N), findtankfile(int);
int   test_mouseclick(), tankinsea(int), tankintree(int), tanknotinsea(int), tankinice(int);
int   readzdmsg(char*, char*, char*);

char  shotkind, overflag, kindflag, pauseflag, dtank, headdeathflag, fireflag, souflag;
char  irunmyself;
int   shotptr, Gameover, starnum;
long  Test, dTest;
char  mytankX, mytankY;
char  MSG[20], KIND[6];
SHOWFS _showfs[13];
int    _shownum = 0;

char file[89][14], STAGE[15];
long  T1, T0, _;
#define ALLFILES  (FA_RDONLY|FA_HIDDEN|FA_SYSTEM)
#define  EXITGAME  show_select(2883, 2894, 1)
#define  FOR(m,n) for(m=0;m<n;m++)
#define  ADD(x, y, fs)  add_showfs(x, y, 0, 0, fs)
#define  ADDHEAD(str)   ADD(18, 27, str)
#define  SORRYSTR  "Sorry: Can't find any file !!!"

KINDDEF  playkind[26];
char   wave[13], stage[14], N;
char   bkcolor=0, isound, idirectrun, idefaultrun;
char   endcolor = LIGHTGREEN;
void  _sound(int n) { if(isound==0)  sound(n); }
int    fs0, fs1;
FILE   *fp;
int   nowtankappear, hadtankappear, enemytanknum;
#define  RUN          300
#define  WIN          1
#define  LOOSE        -1
int   DTE=10000;
char  result;
int   nowshotappear;
void  interrupt  (*oldkey)();
void  interrupt  newkey();
int   pause, iron, mytanknum, fire, firekind;
int   Tankn[4];

#define  ZDSHOT0  0x80000000L
#define  ZDSHOT1  0x88000000L
#define  ZDSHOT2  0x80a00000L
#define  ZDSHOT3  0x80a40000L
unsigned long ZDkind[4] = { ZDSHOT0, ZDSHOT1, ZDSHOT2, ZDSHOT3 };

int  size;
int  _shotgo, _pausetime, _firetime, _irontime, _addjusttime;
char loadflag;
void  setalltime();
#define  msg_show(str)  strcpy(MSG, str);  size = 1;
#define  _strcmp(str1,str2)   strcmp(strupr(str1), strupr(str2))
struct _KEY  KEY;

char round[4], keyptr, bRound;
void  resetKEY()
 { KEY.left=KEY.right=KEY.up=KEY.down=KEY.akey=KEY.skey=KEY.dkey=
   KEY.fkey=KEY.space=KEY.esc = KEY.insert = 0;
   round[0]=round[1]=round[2]=round[3]=0;
   keyptr = -1;  bRound = -1;
   if(loadflag==0) { Eat.kind = NOEAT;  pause = 0; iron = 0;  fire = 0; }
 }

#define  clear_keyboard()   *((char far*)0x0041aL) = *((char far*)0x0041cL)

int  selectfa()
 { int k, flag;  findzj();
   if(idirectrun||idefaultrun)  goto dir_;
 con_:
   mouse_off();  cleardevice();
   randomize();  k = random(10);
   settextstyle(1, 0, 12);
   setcolor(YELLOW);      show_str(320, 80, random(2), VER);
   delay(200);
   setcolor(DARKGRAY);    settextstyle(2, 0, 5);
   outtextxy(320, 220, "Email: Skyever_youer@sohu.com");
   setcolor(LIGHTGRAY);   show_str2(320, 180, 6, "Skyever");
   delay(600);   settextstyle(1, 0, 12);
   setcolor(YELLOW-8);    outtextxy(320, 80, VER);
   delay(250);   settextstyle(1, 0, 6);
   setcolor(DARKGRAY);    outtextxy(320, 180, "Skyever");
   //***********************************************************
   sethz24();   sethz_speed(1);
   setcolor(DARKGRAY);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(2, 0, 4);
   for(k=0; k<5; k++)
    { char str[5];  sprintf(str, "<%d>", k+1);
      outtextxy(30+k*80, 300+k*40, str);
      writehzs(150+k*80, 300+k*40, 26+k*4, 29+k*4);
    }
  { char str[100];
    settextstyle(1, 0, 1);  setcolor(BLUE);
    sprintf(str, "---- > %s (%d) -> %s", wave, N, stage);
    outtextxy(450, 295, str);
    settextstyle(1, 0, 2);
    outtextxy(490, 300+1*40, "----> Enter Game");
    outtextxy(80,  300+2*40, "---- Set Roads ->");
    outtextxy(150, 300+3*40, "---- Help Me ->");
    outtextxy(220, 300+4*40, "---- Exit Game ->");
    setcolor(LIGHTGREEN);  settextstyle(2, 0, 8);
    sprintf(str, KINDSTR, kindflag+'A');
    show_str(320, 260, flag=random(2), str);
    mouse_on();  mouse_on();
    if(flag==1) for(k=0;k<=320;k++) {setmousexy(k,265);delay(1);}
    else        for(k=640;k>=320;k--) {setmousexy(k,265);delay(1);}
  }
  clear_keyboard();
 dir_: mouse_on();
  if(idefaultrun)
   { char _wave[20], _stage[20];
     idefaultrun = 0;
     if(*STAGE)
      { if(readzdmsg(STAGE, _wave, _stage))
	 { strcpy(wave, _wave);  strcpy(stage, _stage);
	   loadflag = 1;  run();  setscreen();
	 }
	else goto con_;
      }
     else if(readzdmsg(strcpy(STAGE, "#quit"), _wave, _stage)
	 || readzdmsg(strcpy(STAGE, "#auto"), _wave, _stage) )
      { strcpy(wave, _wave);  strcpy(stage, _stage);
	loadflag = 1;  run();  setscreen();
      }
     else goto con_;
   }
  else if(idirectrun)  { run(); setscreen(); }
BEGIN
   switch(getdeal())
    { case 0: re: switch(select(0))
	       { case 10:  select(1); break; case 9: edkind(); break; }
	      break;
      case 1: run();          break;
      case 2: remake();       break;
      case 3: help();         break;
      case 4: if(show_select(2871, 2882, 0)==1)  return 4;  break;
      case 10: edkind();      break;
    }
   setscreen();  while(kbhit())   getch();
WEND
 }

int  getdeal()
 { int k;  char str[20];
   sethz_speed(0);
BEGIN
   if(imouse_inbox(320-130, 260-15, 320+130, 260+15))
    { int flag = 0;
      while(imouse_inbox(320-130, 260-15, 320+130, 260+15))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();setcolor(LIGHTRED);  settextstyle(2, 0, 8);
	    sprintf(str, KINDSTR, 'A'+kindflag);
	    outtextxy(320, 260, str);
	    mouse_on();  flag = 1;
	  }
	 if((k=getmousebutton())!=NOBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(imouse_inbox(320-130, 260-15, 320+130, 260+15)) return 10;
	  }
       }
     if(flag==1)
      { mouse_off();setcolor(LIGHTGREEN);  settextstyle(2, 0, 8);
	sprintf(str, KINDSTR, 'A'+kindflag);
	outtextxy(320, 260, str);  mouse_on();  flag = 0;
      }
    }
   for(k=0; k<5; k++)
     { int flag=0;
       if(getmousebutton()!=NOBUTTON)   continue;
       while(imouse_inbox(150+k*80-115, 300+k*40-15, 150+k*80+115, 300+k*40+15))
	{ if(kbhit())  break;
	  if(flag==0)
	   { sethz_color(LIGHTGREEN);  mouse_off();
	     writehzs(150+k*80, 300+k*40, 26+k*4, 29+k*4);
	     setcolor(RED);
	     line(150+k*80-115, 300+k*40+14, 150+k*80+115, 300+k*40+14);
	     flag = 1;  mouse_on();
	   }
	  if(getmousebutton()==LEFTBUTTON)
	    { while(getmousebutton()!=NOBUTTON);
	      if(imouse_inbox(150+k*80-115, 300+k*40-15, 150+k*80+115, 300+k*40+15))
		return k;
	    }
	}
       if(flag==1)
	{ sethz_color(LIGHTGRAY);  mouse_off();
	  writehzs(150+k*80, 300+k*40, 26+k*4, 29+k*4);
	  setcolor(BLACK);
	  line(150+k*80-115, 300+k*40+14, 150+k*80+115, 300+k*40+14);
	  flag = 0;  mouse_on();
	}
       if(kbhit())  switch(k=getch())
	   { case BACKSPACE_KEY: case '0': case '6': return 10;
	     case SPACE_KEY:  case '1': return 0;
	     case ENTER_KEY:  case '2': return 1;
	     case TAB_KEY:    case '3': return 2;
	     case '4':                  return 3;
	     case ESC_KEY:    case '5': return 4;
	     case 0: switch(getch())
		      { case F2_KEY:  return 2; case F1_KEY: return 3;
			case LEFT_KEY: case UP_KEY: if(kindflag--==0) kindflag=25; goto kindc;
			case RIGHT_KEY: case DOWN_KEY: if(kindflag++==25) kindflag=0; goto kindc;
		      } break;
	     default: if(k<='Z' && k>='A')  k -= 'A';
		      else if(k<='z' && k>='a') k -= 'a';
		      else break;  kindflag = k;  kindc:
		      mouse_off();
		      setfillstyle(0, 0); bar(320-130, 260-15, 320+130, 260+15);
		      setcolor(imouse_inbox(320-130, 260-15, 320+130, 260+15)?LIGHTRED:LIGHTGREEN);
		      settextstyle(2, 0, 8);
		      sprintf(str, KINDSTR, 'A'+kindflag);
		      outtextxy(320, 260, str);     mouse_on();
	   }
     }
WEND
 }

void write_edit()
 { char editname[][10] = { "<  Cut  >", "< Copy >", "<Paste>", "<Delete>", "<Rename>" };
   int  k;
   setcolor(WHITE);  settextstyle(2, 0, 4);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   for(k=0; k<5; k++)  outtextxy(220+70*k, 469, editname[k]);
   setcolor(LIGHTGREEN);   line(151, 465, 639, 465);
 }

void show_editfile(char *wave, char *stage)
 { char str[50];
   setfillstyle(0, 0);   bar(0, 300, 149, 310);
   if(*wave&&*stage)   sprintf(str, "Edit: %s->%s", wave, stage);
   else                strcpy(str, "Edit:  <NONE>");
   setcolor(LIGHTBLUE);
   settextstyle(2, 0, 4);  settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy(1, 300, str);
   setcolor(LIGHTRED);  outtextxy(1, 300, "Edit:");
 }


#define test_savefile(str)\
 (j==13 && (strcmp(str, "#NONAME")==0 || \
	    strcmp(str, "#AUTO")==0 || \
	    strcmp(str, "#QUIT")==0) )

void renew_filename(int m, char dirname[][14], char filename[][14], int *i, int j, int code)
 { int k;   char File[50], *path=File;    void show_yl(char *name);
   write_edit();
   settextjustify(LEFT_TEXT, TOP_TEXT);
   settextstyle(1, 0, 1);   setfillstyle(1, BLUE);
   for(k=0; k<m; k++)
     { if(filename[k][13]&FA_HIDDEN)      setcolor(DARKGRAY);
       else if(filename[k][13]&FA_RDONLY) setcolor(GREEN);
       else                               setcolor(LIGHTGRAY);
       if(test_savefile(filename[k]))     setcolor(LIGHTCYAN);
       bar(150+(k/23)*120, 5+(k%23)*20, 150+(k/23)*120+119, 5+(k%23)*20+19);
       outtextxy(170+(k/23)*120, 5+(k%23)*20, filename[k]);
     }
  for(; k<88; k++)  bar(150+(k/23)*120, 5+(k%23)*20, 150+(k/23)*120+119, 5+(k%23)*20+19);
  if(*i!=-1)
   { if(*i>=m)  *i = m-1;
     setcolor(YELLOW);  outtextxy(155+(*i/23)*120, 5+(*i%23)*20, "*");
     if(j==13)  sprintf(File, "msg\\save\\%s", filename[*i]);
     else sprintf(File, "stage\\%s\\%s", dirname[j], filename[*i]);
     show_yl(File);
   }
  setcolor(YELLOW-8);  setfillstyle(0, 0);
  bar(25, 370, 149, 390);  bar(25, 420, 140, 440);
  sprintf(path, "%s (%d)", j==13?"<SAVE>":dirname[j], m);
  outtextxy(25, 370, path);
  outtextxy(25, 420, *i==-1?"None!!":filename[*i]);

  if(code==1&&*i!=-1)
    { setfillstyle(0, 0);   bar(80, 318, 140, 342); bar(80, 448, 149, 472);
      if(filename[*i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
      else                            writehzs(110, 330, 52, 53);
      if(filename[*i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
      else                            writehzs(110, 460, 204, 205);
    }
 }



int  select(int code)
 { int i=0, j=0, k, n, m, k0, bj=0, bi=0, dtime=getdisk()>=2?2:10, *ip=&i;
   static char edit_wave[15], edit_stage[15], pasteflag, cutflag;
   char dirname[12][14];   char  filename[89][14];  char path[50];
   char File[50];  char bwave[15], bstage[15];
   static char static_wave[15], static_stage[15];
   void show_yl(char *name);
   char editname[][10] = { "<  Cut  >", "< Copy >", "<Paste>", "<Delete>", "<Rename>" };

   strcpy(bwave, wave);  strcpy(bstage, stage);
   if(code==1)  { strcpy(wave, static_wave);  strcpy(stage, static_stage); }
   if(code==2)  code = 1;

   n = findfile("stage\\*.*", dirname, FA_DIREC, 12);
   mouse_off();   cleardevice();
   setfillstyle(1, BLUE);  bar(150, 0, 639, 479);
   if(n==0)
     { settextstyle(1, 0, 4);  settextjustify(CENTER_TEXT, CENTER_TEXT);
       setcolor(LIGHTGREEN);   outtextxy(320, 240, SORRYSTR);
       on(); while(!kbhit()&&getmousebutton()==NOBUTTON);  return 0;
     }
   setcolor(GREEN);  settextjustify(LEFT_TEXT, TOP_TEXT);
   settextstyle(1, 0, 1);
   for(k=0; k<n; k++)
     { outtextxy(30, k*20+5, dirname[k]);
       if(_strcmp(dirname[k], wave)==0)  j = k;
     }
   if(strcmp(strupr(wave), "SAVE")==0)  j = 13;
   outtextxy(30, 13*20+5, ">LOAD->");
   setlinestyle(1, 0, 3);  setcolor(LIGHTBLUE);
   line(10, 13*20-3, 130, 13*20-3);  setlinestyle(0, 0, 1);
   setcolor(YELLOW);  outtextxy(0, (bj=j)*20+5, "->");
   sethz24();   sethz_zoomxy(1, 1);   
   if(code==0) 
    { writehzs(30, 460, 46, 47);   writehzs(110, 460, 48, 49);  
      writehzs(30, 330, 208, 209); writehzs(110, 330, 210, 211); 
    }
   else
    { writehzs(30, 460, 216, 217);   writehzs(30, 330, 50, 51);
      writehzs(110, 330, 52, 53);    writehzs(110, 460, 204, 205);
    }
//****************************************************************
   if(j==13)  strcpy(path, "msg\\save\\*.*");
   else  sprintf(path, "stage\\%s\\*.*", wave);
  
   m = findfile(path, filename, code==0?0:ALLFILES, 88);
   setcolor(LIGHTGREEN);    settextstyle(1, 0, 1);
   for(i=0; i<m; i++)  if(_strcmp(filename[i],stage)==0)  break;
   if(i==m)  i = 0;
   renew_filename(m, dirname, filename, &i, j, code);
   if(m==0)
     { i = -1; setcolor(LIGHTGREEN);
       outtextxy(290, 200, SORRYSTR);
     }

   settextstyle(1, 0, 2);   setcolor(BLUE);
   outtextxy(5, 340, "Select :");
   outtextxy(5, 390, "Start with :");
   settextstyle(1, 0, 1);   setcolor(YELLOW-8);
   sprintf(path, "%s (%d)", wave, m);
   outtextxy(25, 370, path);
   outtextxy(25, 420, i==-1?"None!!":stage);   
   if(code==1)
    { setfillstyle(0, 0);   bar(80, 318, 149, 342);  bar(80, 448, 149, 472);
      if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
      else                           writehzs(110, 330, 52, 53);
      if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
      else                           writehzs(110, 460, 204, 205);
    }
   if(pasteflag==1)  show_editfile(edit_wave, edit_stage);
   else              show_editfile("", "");
   mouse_on();
BEGIN
   settextstyle(1, 0, 1);
   for(k=0; k<n+1; k++)
    { int  flag = 0;  char *dirnamep=k==n?">LOAD->":dirname[k];
      int  y = (k==n?13:k)*20;
      while(imouse_inbox(30, y+5, 30+textwidth(dirnamep), y+22))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();    setcolor(YELLOW);
	    outtextxy(30, y+5, dirnamep);
	    setcolor(LIGHTBLUE);  line(30, y+22, 30+textwidth(dirnamep)-3, y+22);
	    flag = 1;    mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(!imouse_inbox(30, y+5, 30+textwidth(dirnamep), y+22))  break;
	    bj = j;  j = k==n?13:k;
	  startchange: bi = 0;
	    mouse_off();
	    if(j==13)  strcpy(path, "msg\\save\\*.*");
	    else       sprintf(path, "stage\\%s\\*.*", dirname[j]);

	    m = findfile(path, filename, code==0?0:ALLFILES, 88);
            i = m?0:-1;
	    renew_filename(m, dirname, filename, &i, j, code);
	    if(m==0)
	     { setcolor(LIGHTGREEN);    settextstyle(1, 0, 1);
	       outtextxy(290, 200, SORRYSTR); i = -1;
	     }
	    else  i = 0;
	    
	    settextstyle(1, 0, 1);
	    setcolor(BLACK);   outtextxy(0, bj*20+5, "->");
	    setcolor(YELLOW);  outtextxy(0, j*20+5, "->");

	    if(code==1)
	     { setfillstyle(0, 0);   bar(80, 318, 140, 342); bar(80, 448, 149, 472);
	       if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
	       else                           writehzs(110, 330, 52, 53);
	       if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
	       else                           writehzs(110, 460, 204, 205); 
	     }
	     mouse_on();
	  }
       }
      
      if(flag==1)
       { mouse_off();
	 setcolor(BLACK); line(30, y+22, 30+textwidth(dirnamep)-3, y+22);
         settextstyle(1, 0, 1);
	 setcolor(GREEN); outtextxy(30, y+5, dirnamep);
	 flag = 0;  mouse_on();
       }
    }
   settextstyle(1, 0, 1);
//*********************************************************************
   for(k=0; k<m; k++)
    { int  flag = 0, ylflag=0;  long T = clock(), clickflag;
      while(imouse_inbox(170+(k/23)*120, 5+(k%23)*20, 170+(k/23)*120+textwidth(filename[k]), 26+(k%23)*20))
       { if(kbhit())  break;
	 if(clock()-T>=(long)dtime && ylflag==0)
	   { if(j==13)  sprintf(File, "msg\\save\\%s", filename[k]);
	     else  sprintf(File, "stage\\%s\\%s", dirname[j], filename[k]);
	     show_yl(File); ylflag = 1;
	   }
	 if(flag==0)
	  { mouse_off();    setcolor(YELLOW);  settextstyle(1, 0, 1);
	    outtextxy(170+(k/23)*120, 5+(k%23)*20, filename[k]);
	    setcolor(LIGHTRED);
	    line(170+(k/23)*120, 21+(k%23)*20, 170+(k/23)*120+textwidth(filename[k])-3, 21+(k%23)*20);
	    flag = 1;    mouse_on();
	  }
	 if( (clickflag=getmousebutton()) != NOBUTTON)
	  { if(clickflag==LEFTBUTTON)
	     { int x, y, bx, by, x0, y0, K, i;  char writeflag[12];
	       if(j==13)  goto select_mark;
	       for(i=0; i<12; i++)  writeflag[i] = 0;
	       getmouse_xy(&bx, &by);  x0 = x = bx;  y0 = y = by;
	       setwritemode(XOR_PUT);  setcolor(YELLOW);
	       settextstyle(1, 0, 1);  settextjustify(LEFT_TEXT, TOP_TEXT);
	       off();  outtextxy(170+(k/23)*120, 5+(k%23)*20, filename[k]);  on();
	       while(getmousebutton()!=NOBUTTON)
		{ getmouse_xy(&x, &y);
		  if(bx==x && by==y)  continue;
		  for(K=0; K<n; K++)
		    if(imouse_inbox(30, K*20+5, 30+textwidth(dirname[K]), K*20+22))
		      { if(writeflag[K]==1)  break;
			mouse_off();
			for(i=0; i<n; i++)
			 { if(i==K)    continue;
			   if(writeflag[i]==1)
			     { outtextxy(30, i*20+5, dirname[i]);
			       line(30, i*20+22, 30+textwidth(dirname[i])-3, i*20+22);
			       writeflag[i] = 0;
			     }
			 }
                        writeflag[K]=1;
			outtextxy(30, K*20+5, dirname[K]);
			line(30, K*20+22, 30+textwidth(dirname[K])-3, K*20+22);
			mouse_on();   break;
		      }
		  if(K==n) for(i=0; i<n; i++)
		    { if(writeflag[i]==0)  continue;
		      off(); outtextxy(30, i*20+5, dirname[i]);
		      line(30, i*20+22, 30+textwidth(dirname[i])-3, i*20+22);
		      writeflag[i] = 0;   on();
		    }
		  mouse_off();
		  outtextxy(170+(k/23)*120+(x-x0), 5+(k%23)*20+(y-y0), filename[k]);
		  outtextxy(170+(k/23)*120+(bx-x0), 5+(k%23)*20+(by-y0), filename[k]);
		  mouse_on();
		  bx = x;  by = y;
		}
	       mouse_off();
	       outtextxy(170+(k/23)*120+(x-x0), 5+(k%23)*20+(y-y0), filename[k]);
	       K = -1;
	       for(i=0; i<n; i++)
		{ if(writeflag[i]==0)  continue;
		  outtextxy(30, i*20+5, dirname[i]);
		  line(30, i*20+22, 30+textwidth(dirname[i])-3, i*20+22);
		  K = i;
		}
	       mouse_on();
	       setwritemode(COPY_PUT);
	       if(K!=-1)
		{ char source[50], object[50];  int filek=0, ch;
		  char *edit_path = source;
		  FILE *fp, *fp0;
		  if(j==K)  goto end_mark;
		  sprintf(source, "stage\\%s\\%s", dirname[j], filename[k]);
		  sprintf(object, "stage\\%s\\%s", dirname[K], filename[k]);
		  while(access(object, 0)==0)
		     sprintf(object, "stage\\%s\\copy-%d", dirname[K], ++filek);
		  fp = fopen(source, "rb");
		  if(fp==NULL)  goto end_mark;
		  fp0 = fopen(object, "wb");
		  if(fp0==NULL)  { fclose(fp);  goto end_mark; }
		  while( (ch=fgetc(fp))!=EOF )  fputc(ch, fp0);
		  fclose(fp);  fclose(fp0);
		  if(!(bioskey(2)&0x04) && unlink(source)==0)
		   { sprintf(edit_path, "stage\\%s\\*.*", dirname[j]);
		     m = findfile(edit_path, filename, code==0?0:ALLFILES, 88);
		     renew_filename(m, dirname, filename, ip, j, code);
		     if(m==0)
		       { setcolor(LIGHTGREEN);    settextstyle(1, 0, 1);
			 outtextxy(290, 200, SORRYSTR);
		       }
		     flag = 0;
		   }
		  end_mark:;
		}
	       else
		{ int _X, _Y;
		  getmouse_xy(&_X, &_Y);
		  if(sqrt( (float)(_X-x0)*(_X-x0) + (float)(_Y-y0)*(_Y-y0) ) < 5)
		     goto select_mark;
		  else
		   { float X0 = 170+(k/23)*120, Y0 = 5+(k%23)*20;
		     float X = _X-(x0-(170+(k/23)*120)), Y = _Y-(y0-(5+(k%23)*20));
		     float dx = (float)(X0-X)/50, dy = (float)(Y0-Y)/50;
		     setwritemode(XOR_PUT);
		     off();   outtextxy(X, Y, filename[k]);   on();
		     for(i=0; i<50; i++)
		      { mouse_off();
			outtextxy(X, Y, filename[k]);
			X += dx;  Y += dy;
			outtextxy(X, Y, filename[k]);
			mouse_on();
		      }
		     off();   outtextxy(X, Y, filename[k]);   on();
		     setwritemode(COPY_PUT);
		   }
		}
	     }
	    else  select_mark:
	     { while(getmousebutton()!=NOBUTTON);
	       if(!imouse_inbox(170+(k/23)*120, 5+(k%23)*20, 170+(k/23)*120+textwidth(filename[k]), 21+(k%23)*20)) break;
	       bi = i; i = k;  setfillstyle(0, 0);  setcolor(YELLOW-8);
	       bar(25, 420, 149, 440);   outtextxy(25, 420, filename[i]);
	       settextstyle(1, 0, 1);  settextjustify(LEFT_TEXT, TOP_TEXT);
	       setcolor(BLUE);    outtextxy(155+(bi/23)*120, 5+(bi%23)*20, "*");
	       setcolor(YELLOW);  outtextxy(155+(i/23)*120, 5+(i%23)*20, "*");
	       if(code==1)
		{ if(clickflag==RIGHTBUTTON)
		   { if(j==13)  sprintf(path, "msg\\save\\%s", filename[i]);
		     else  sprintf(path, "stage\\%s\\%s", dirname[j], filename[i]);
		     if(filename[i][13]&FA_HIDDEN)  filename[i][13]&=~FA_HIDDEN;
		     else                           filename[i][13]|= FA_HIDDEN;
		     _chmod(path, 1, filename[i][13]);
		   }
		  setfillstyle(0, 0); bar(80, 318, 149, 342); bar(80, 448, 149, 472);
		  if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
		  else                           writehzs(110, 330, 52, 53);
		  if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
		  else                           writehzs(110, 460, 204, 205);
		}
	       if(bioskey(2)&0x04)  goto ret_mark;
	     }
	  }
       }
      if(flag==1)
       { mouse_off();
	 setcolor(BLUE);   settextstyle(1, 0, 1);
	 line(170+(k/23)*120, 21+(k%23)*20, 170+(k/23)*120+textwidth(filename[k])-3, 21+(k%23)*20);
	 if(filename[k][13]&FA_HIDDEN)       setcolor(DARKGRAY);
	 else  if(filename[k][13]&FA_RDONLY) setcolor(GREEN);
	 else                                setcolor(LIGHTGRAY);
	 if(test_savefile(filename[k]))      setcolor(LIGHTCYAN);
	 outtextxy(170+(k/23)*120, 5+(k%23)*20, filename[k]);
	 flag = 0;  mouse_on();
       }
      if(ylflag==1)
       { int k;  settextstyle(1, 0, 1);
	 setcolor(GREEN);  settextjustify(LEFT_TEXT, TOP_TEXT);
	 if(j==13)  sprintf(File, "msg\\save\\%s", filename[i]);
	 else sprintf(File, "stage\\%s\\%s", dirname[j], filename[i]);
	 show_yl(File);
       }
    }
//******************************************************************
   for(k=0; k<2-code; k++)
    { int flag = 0;
      while(imouse_inbox(k*80, 448, 60+k*80, 472))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    sethz_color(LIGHTRED);
	    if(code==0)  writehzs(30+k*80, 460, 46+k*2, 47+k*2);
	    else         writehzs(30, 460, 216, 217);   
	    setcolor(LIGHTBLUE);  line(k*80, 472, 60+k*80, 472);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(imouse_inbox(k*80, 448, 60+k*80, 472))
	     if(k==0 && i!=-1)  ret_mark:
	      { if(j!=13)
		 { if(filename[i][13]&FA_HIDDEN)
		     for(i=0; i<m; i++)  if(!(filename[i][13]&FA_HIDDEN))  break;
		   strcpy(wave, dirname[j]); strcpy(stage,filename[i]);
		   loadflag = 0;
		   sprintf(path, "stage\\%s\\*.*", wave);
		   N = findfile(path, filename, 0, 88);
		   return 1;
		 }
		else
		 { char _stage[20], _wave[20];
                   strcpy(STAGE, filename[i]);
		   readzdmsg(filename[i], _wave, _stage);
		   strcpy(stage, _stage);  strcpy(wave, _wave);
		   sprintf(path, "stage\\%s\\*.*", wave);
		   N = findfile(path, filename, 0, 88);
		   loadflag = 1;   return 1;
		 }
	      }
	    else { strcpy(wave, bwave);  strcpy(stage, bstage); return 0; }
	  }
       }
      if(flag==1)
	{ mouse_off();
	  sethz_color(LIGHTGRAY);
	  setcolor(BLACK);  line(k*80, 472, 60+k*80, 472);
	  if(code==0)  writehzs(30+k*80, 460, 46+k*2, 47+k*2);
	  else         writehzs(30, 460, 216, 217);   
	  flag = 0;  mouse_on();
	}
    }
   if(code==1) for(k=0; k<2; k++)
    { int  flag = 0;
      while(imouse_inbox(80*k, 318, 60+80*k, 342))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    sethz_color(LIGHTRED);
	    if(k==0)     writehzs(30+80*k, 330, 50, 51);
	    else
	     { setfillstyle(0, 0); bar(80, 318, 149, 342);
	       if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
	       else                           writehzs(110, 330, 52, 53);
	     }
	    setcolor(LIGHTBLUE);  line(80*k, 342, 60+80*k, 342);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(imouse_inbox(80*k, 318, 60+80*k, 342))
	      if(k==0)
		{ if(i==-1)
		   { if(strcmp(dirname[j], "@MYSELF")!=0)
		      { setfillstyle(0, 0);  bar(0, 0, 148, 299);
			sprintf(path, "stage\\%s", dirname[j]);	rmdir(path);
			n = findfile("stage\\*.*", dirname, FA_DIREC, 13);
			setcolor(GREEN);  settextjustify(LEFT_TEXT, TOP_TEXT);
			settextstyle(1, 0, 1);
			for(k0=0; k0<n; k0++) outtextxy(30, k0*20+5, dirname[k0]);
			outtextxy(30, 13*20+5, ">LOAD->");
			setlinestyle(1, 0, 3);  setcolor(LIGHTBLUE);
			line(10, 13*20-3, 130, 13*20-3);  setlinestyle(0, 0, 1);
			if(j>=n)  j--;   i = 0;
			settextstyle(1, 0, 1);
			setcolor(YELLOW);  outtextxy(0, j*20+5, "->");
		      }
		   }
		 else
		   { if(j==13)  sprintf(path, "msg\\save\\%s", filename[i]);
		     else    sprintf(path, "stage\\%s\\%s", dirname[j], filename[i]);
		     unlink(path);
		   }
		 if(j==13)  strcpy(path, "msg\\save\\*.*");
		 else  sprintf(path, "stage\\%s\\*.*", dirname[j]);
		 m = findfile(path, filename, code==0?0:ALLFILES, 88);
		 setcolor(LIGHTGRAY);    settextstyle(1, 0, 1);

		 renew_filename(m, dirname, filename, &i, j, code);
		 if(m==0)
		  { i=-1; setcolor(LIGHTGREEN);    settextstyle(1, 0, 1);
		    outtextxy(290, 200, SORRYSTR);
		  }
		 else if(i>=m)  i --;

		 settextstyle(1, 0, 1);   setcolor(YELLOW-8);
		 setfillstyle(0, 0);
		 bar(25, 370, 140, 390);  bar(25, 420, 140, 440);
		 sprintf(path, "%s (%d)", j==13?"<SAVE>":dirname[j], m);
		 outtextxy(25, 370, path);
		 outtextxy(25, 420, i==-1?"None!!":filename[i]);
		 setfillstyle(0, 0); bar(80, 318, 149, 342);  sethz_color(LIGHTGRAY);
		 if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
		 else                           writehzs(110, 330, 52, 53);
		 if(i!=-1)
		   { setcolor(YELLOW);  outtextxy(155+(i/23)*120, 5+(i%23)*20, "*"); }
		 while(getmousebutton()!=NOBUTTON);
	       }
	     else  if(i!=-1)
	      { char attrib = filename[i][13];
		if(test_savefile(filename[i]))  break;
		if(j==13)  sprintf(path, "msg\\save\\%s", filename[i]);
		else  sprintf(path, "stage\\%s\\%s", dirname[j], filename[i]);
		if(attrib&FA_RDONLY)  attrib &= ~FA_RDONLY;
		else                  attrib |=  FA_RDONLY;
		if(_chmod(path, 1, attrib)+1)    filename[i][13]=attrib;
		mouse_off();
		setfillstyle(0, 0);   bar(80, 318, 140, 342);
		if(attrib&FA_RDONLY)  writehzs(110, 330, 54, 55);
		else                  writehzs(110, 330, 52, 53);
		if(attrib&FA_HIDDEN)      setcolor(DARKGRAY);
		else if(attrib&FA_RDONLY) setcolor(GREEN);
		else                      setcolor(LIGHTGRAY);
		outtextxy(170+(i/23)*120, 5+(i%23)*20, filename[i]);
		mouse_on();
		while(getmousebutton()!=NOBUTTON);
	      }
	   }
       }
      if(flag==1)
	{ mouse_off();
	  setcolor(BLACK);  line(80*k, 342, 60+80*k, 342);
	  sethz_color(LIGHTGRAY);
	  if(k==0) writehzs(30, 330, 50, 51);
	  else
	   { setfillstyle(0, 0); bar(80, 318, 149, 342);
	     if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
	     else                           writehzs(110, 330, 52, 53);
	   }
	  flag = 0;  mouse_on();
	}
     }
   else for(k=0; k<2; k++)
    { int  flag = 0;
      while(imouse_inbox(80*k, 318, 60+80*k, 342))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    sethz_color(LIGHTRED);
	    writehzs(30+80*k, 330, 208+k*2, 209+k*2);
	    setcolor(LIGHTBLUE);  line(80*k, 342, 60+80*k, 342);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	   { while(getmousebutton()!=NOBUTTON);
	     if(imouse_inbox(80*k, 318, 60+80*k, 342))
	       { if(k==0)
		  { strcpy(static_wave,  j==13?"SAVE":dirname[j]);
		    strcpy(static_stage, filename[i]);
		    return 10;
		  }
		 else { strcpy(wave, bwave); strcpy(stage, bstage); return 9; }
	       }
	  }
       }
      if(flag==1)
	{ mouse_off();
	  setcolor(BLACK);  line(80*k, 342, 60+80*k, 342);
	  sethz_color(LIGHTGRAY);  writehzs(30+80*k, 330, 208+k*2, 209+k*2);
	  flag = 0;  mouse_on();
	}
     }        
   if(code==1)
    { int flag = 0;
      while(imouse_inbox(80, 448, 60+80, 472))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    setfillstyle(0, 0);  bar(80, 448, 149, 472);
	    sethz_color(LIGHTRED);
	    if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
	    else                           writehzs(110, 460, 204, 205);
	    setcolor(LIGHTBLUE);  line(80, 472, 60+80, 472);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON && i!=-1)
	  { char attrib = filename[i][13];
	    if(test_savefile(filename[i]))  break;
	    if(j==13)  sprintf(path, "msg\\save\\%s", filename[i]);
	    else sprintf(path, "stage\\%s\\%s", dirname[j], filename[i]);
	    if(attrib&FA_HIDDEN)  attrib &= ~FA_HIDDEN;
	    else                  attrib |=  FA_HIDDEN;
	    if(_chmod(path, 1, attrib)+1)    filename[i][13]=attrib;
	    mouse_off();
	    setfillstyle(0, 0);   bar(80, 448, 149, 472);
	    if(attrib&FA_HIDDEN)  writehzs(110, 460, 206, 207);
	    else                  writehzs(110, 460, 204, 205);
	    if(attrib&FA_HIDDEN)       setcolor(DARKGRAY);
	    else if(attrib&FA_RDONLY)  setcolor(GREEN);
	    else                       setcolor(LIGHTGRAY);
	    outtextxy(170+(i/23)*120, 5+(i%23)*20, filename[i]);
	    mouse_on();
	    while(getmousebutton()!=NOBUTTON);
	  }
       }
      if(flag==1)
	{ mouse_off();
	  sethz_color(LIGHTGRAY);
	  setcolor(BLACK);  line(80, 472, 60+80, 472);
	  if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
	  else                           writehzs(110, 460, 204, 205);
	  flag = 0;  mouse_on();
	}
    }

//**************************** EDIT ***************************
   for(k=0; k<5; k++)
    { int flag = 0;
      if(imouse_inbox(220+k*70, 469, 220+k*70+50, 479))
	{ while(imouse_inbox(220+k*70, 469, 220+k*70+50, 479))
	   { if(kbhit())  break;
	     if(flag==0)
	      { setcolor(LIGHTGREEN);  settextstyle(2, 0, 4);
		settextjustify(LEFT_TEXT, TOP_TEXT);
		off();  outtextxy(220+k*70, 469, editname[k]);  on();
		flag = 1;
	      }

	    if(getmousebutton()==LEFTBUTTON) edit_mark:
	     { char edit_path[50], edit_path0[50], name[15]="", edit_stage0[13];
	       sprintf(edit_path, "stage\\%s\\%s", edit_wave, edit_stage);
	       while(getmousebutton()!=NOBUTTON);
	       if(!imouse_inbox(220+k*70, 469, 220+k*70+50, 479)) break;
	       switch(k)
		   { case 0: cut_mark:
			      if(m==0||j==13)  break;  cutflag = 1;
		     case 1: copy_mark:
			      if(m==0||j==13)  break;  pasteflag = 1;
			      strcpy(edit_wave, dirname[j]);  strcpy(edit_stage, filename[i]);
			      show_editfile(edit_wave, edit_stage);
			      break;
		     case 2: goto paste_mark_; paste_mark:
			     sprintf(edit_path, "stage\\%s\\%s", edit_wave, edit_stage);
			     paste_mark_:
			      if(j==13)  break;
			      if(pasteflag==1 && m<88)
			       { FILE *fp, *fp0;    int ch, k=1;
				 sprintf(edit_path0, "stage\\%s\\%s", dirname[j], edit_stage);
				 strcpy(edit_stage0, edit_stage);
				 if(access(edit_path0, 0)==0) do
				   { sprintf(edit_path0, "stage\\%s\\copy-%d", dirname[j], k);
				     sprintf(edit_stage0, "copy-%d", k++);
				   }  while(access(edit_path0, 0)==0);
				 strupr(edit_stage0);
				 if( (fp=fopen(edit_path, "rb"))==NULL)  break;
				 if( (fp0=fopen(edit_path0, "wb"))==NULL)  { fclose(fp); break; }
				 while( (ch=fgetc(fp))!=EOF )  fputc(ch, fp0);
				 fclose(fp);  fclose(fp0);
                                 if(cutflag==1&&unlink(edit_path)==0)
				   { cutflag = 0; pasteflag = 0; show_editfile("", ""); }
				 sprintf(edit_path0, "stage\\%s\\*.*", dirname[j]);
				 m = findfile(edit_path0, filename, code==0?0:ALLFILES, 88);
				 for(i=0; i<m; i++) if(strcmp(edit_stage0, filename[i])==0)  break;
				 renew_filename(m, dirname, filename, &i, j, code);
			       }
			      break;
		     case 3: delete_mark:
			      if(m==0)  break;
			      if(j==13)  sprintf(edit_path0, "msg\\save\\%s", filename[i]);
			      else       sprintf(edit_path0, "stage\\%s\\%s", dirname[j], filename[i]);
			      if(unlink(edit_path0)==0)
			       { if(j==13)  strcpy(edit_path, "msg\\save\\*.*");
				 else  sprintf(edit_path, "stage\\%s\\*.*", dirname[j]);
				 m = findfile(edit_path, filename, code==0?0:ALLFILES, 88);
				 renew_filename(m, dirname, filename, &i, j, code);
				 if(m==0)
				   { setcolor(LIGHTGREEN);    settextstyle(1, 0, 1);
				     outtextxy(290, 200, SORRYSTR);
				   }
			       }
			      break;
		     case 4: rename_mark:  flag = 1;  k = 4;
			      if(m==0)  break;
			      mouse_off();
			      setfillstyle(1, bkcolor=BLUE);  bar(220+4*70, 469, 639, 479);
			      mouse_on();
			      setcolor(YELLOW);     settextstyle(2, 0, 4);
			      *name = 0;
			      if(Inword(220+4*70, 469, name, 13)==0)
				{ if(j==13)
				   { sprintf(edit_path0, "msg\\save\\%s", strupr(name));
				     sprintf(edit_path, "msg\\save\\%s", filename[i]);
				   }
				  else
				   { sprintf(edit_path0, "stage\\%s\\%s", dirname[j], strupr(name));
				     sprintf(edit_path, "stage\\%s\\%s", dirname[j], filename[i]);
				   }
				  if(rename(edit_path, edit_path0)==0)
				    { if(j==13)  strcpy(edit_path, "msg\\save\\*.*");
				      else  sprintf(edit_path, "stage\\%s\\*.*", dirname[j]);
				      m = findfile(edit_path, filename, code==0?0:ALLFILES, 88);
				      for(i=0; i<m; i++) if(strcmp(name, filename[i])==0)  break;
				      renew_filename(m, dirname, filename, &i, j, code);
				    }
				}
			      setfillstyle(1, BLUE);
			      off();  bar(220+4*70, 469, 639, 479);  on();
			      while(getmousebutton()!=NOBUTTON);
			      break;
		   }
		 break;
		}
	  }
	 if(flag==1)
	  { setcolor(WHITE);  settextstyle(2, 0, 4);
	    settextjustify(LEFT_TEXT, TOP_TEXT);
	    off();  outtextxy(220+k*70, 469, editname[k]);  on();
	  }
	}
    }

   if(kbhit())
    { unsigned char ch = getch();
      char _stage[20], _wave[20];
      switch(ch)
       { case ENTER_KEY:
	 case SPACE_KEY: if(j==13)
			  { strcpy(STAGE, filename[i]);
			    readzdmsg(filename[i], _wave, _stage);
			    strcpy(stage, _stage);  strcpy(wave, _wave);
			    loadflag = 1;
			    sprintf(path, "stage\\%s\\*.*", wave);
			    N = findfile(path, filename, 0, 88);
			    return 1;
			  }
			 strcpy(wave, dirname[j]); strcpy(stage,filename[i]);
			 loadflag = 0;
			 sprintf(path, "stage\\%s\\*.*", wave);
			 N = findfile(path, filename, 0, 88);
			 return 1;
	 case ESC_KEY:   strcpy(wave, bwave);  strcpy(stage, bstage);  return 0;
	 case UP_KEY:    bi = i;  if(--i<0)  i = m-1;
	 case DOWN_KEY:  if(ch==DOWN_KEY) { bi = i; if(++i>=m)  i = 0; }
	 case LEFT_KEY:  if(ch==LEFT_KEY)
			   { if(i-23>=0)  { bi = i; i -= 23; }
			     else  break;
			   }
	 case RIGHT_KEY: if(ch==RIGHT_KEY)
			   { if(i+23<m) { bi = i;  i += 23; }
			     else    break;
			   }
	 case HOME_KEY:  if(ch==HOME_KEY)  { bi = i; i = 0; }
	 case END_KEY:   if(ch==END_KEY)   { bi = i; i = m-1; }
			 if(i<0 || i>=m)  break;
			 setfillstyle(0, 0);  setcolor(YELLOW-8);
			 bar(25, 420, 149, 440);   outtextxy(25, 420, filename[i]);
			 settextstyle(1, 0, 1);  settextjustify(LEFT_TEXT, TOP_TEXT);
			 setcolor(BLUE);    outtextxy(155+(bi/23)*120, 5+(bi%23)*20, "*");
			 setcolor(YELLOW);  outtextxy(155+(i/23)*120, 5+(i%23)*20, "*");
			 if(j==13)  sprintf(File, "msg\\save\\%s", filename[i]);
			 else sprintf(File, "stage\\%s\\%s", dirname[j], filename[i]);
			 show_yl(File);
			 if(code==1)
			  { setfillstyle(0, 0); bar(80, 318, 149, 342); bar(80, 448, 149, 472);
			    if(filename[i][13]&FA_RDONLY)  writehzs(110, 330, 54, 55);
			    else                           writehzs(110, 330, 52, 53);
			    if(filename[i][13]&FA_HIDDEN)  writehzs(110, 460, 206, 207);
			    else                           writehzs(110, 460, 204, 205);
			  }
			 break;
	 case PAGEUP_KEY:   bj = j; if(--j<0)  j = 13;
			    if(j!=13&&j>=n)  j = n-1;  goto startchange;
	 case PAGEDOWN_KEY: bj = j;
			    if(j==13)  j = 0;   else if(++j>=n) j = 13;
			    goto startchange;
	 case 'X': case 'x': goto cut_mark;
	 case 'C': case 'c': goto copy_mark;
	 case 'V': case 'v': goto paste_mark;
	 case 'R': case 'r': goto rename_mark;
	 case DELETE_KEY: if(bioskey(2)&0x03)  goto delete_mark;
       }
    }
WEND
 }



#define  TT  Tank[10]
int  Stage;
struct ffblk Ffblk;
void  run()
 { void end_sound();   char path[40], flag;   int loadzd(char *);
   KINDDEF  savekind = P;
   Stage = 0;   irunmyself = 0;
   if(strcmp(stage, "NONE")==0 || strcmp(wave, "NONE")==0) 
     { mouse_off();  cleardevice();
       settextstyle(1, 0, 4);  settextjustify(CENTER_TEXT, CENTER_TEXT);
       setcolor(LIGHTGREEN);   outtextxy(320, 240, SORRYSTR);
       on(); while(!kbhit()&&getmousebutton()==NOBUTTON); return;
     }
   shotptr = 0;   resetKEY();  KEY.enter = 0;  keyptr = -1;
   setalltime();    
   oldkey = getvect(0x09);   Test = 0;  dTest = DTE;
   disable();  setvect(0x09, newkey);  enable();
   mytanknum = (int)(P.mynum);
   TT.speed=P.tankspeed+1; shotkind=P.myshotkind;
   TT.shotspeed=P.myshotspeed; TT.iship=TT.ibship=P.isea;
   starnum = (int)TT.shotspeed+shotkind+TT.speed+TT.ibship+1;
   if(starnum<3)  starnum = 3;
BEGIN
   int start_run();  void  show_begin(), show_end();
   int i, j;  char *strp[4] = { "SOUND", "PAUSE", "SAVE", "STOP" };
   char str[100];
   if(bioskey(2)&0x04)  if(EXITGAME)  goto ret;
   if(loadflag==0)   show_begin();
   mouse_off();   write_screen();
   sethz_file(hzk24fp);   sethz_color(DARKGRAY);
   sethz_dot(24);         sethz_fx(Y_ROUND);
   sethz_text(LEFT_HZ, TOP_HZ);
   sethz_round(Y_ROUND);  sethz_speed(0);  sethz_zoomxy(2, 1);
   sethz_dxy(4);
   writehzs(584, 10, 8, 11);
   sethz_color(BLUE);
   writehzs(586, 12, 8, 11);
   setcolor(YELLOW-8);
   settextstyle(1, 0, 2);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(50, 465, "Skyever");  outtextxy(590, 465, "Ver: 3.1");

   showtank(37, 10, 1);    showtank(37, 14, 0);
   settextstyle(2, 0, 4);  setcolor(CYAN);
   for(j=0; j<4; j++)
    { outtextxy(613, 415+12*j, strp[j]);
      rectangle(613-25, 415+12*j-5, 613+25, 415+12*j+5);
    }

   showico(38, 18, 0);  showico(38, 20, 2);  fs0 = fs1 = 0;
   settextstyle(2, 0, 5);  setcolor(WHITE);  settextjustify(RIGHT_TEXT, TOP_TEXT);
   mouse_on();

   if(findtankfile(1)==0)
     { mouse_off();  setcolor(LIGHTGREEN);
       settextjustify(CENTER_TEXT, CENTER_TEXT);
       outtextxy(320, 240, SORRYSTR);
       mouse_on();  waitforclick();  goto ret;
     }
   show_msg();  readroad();  fclose(fp);
   for(j=0; j<2; j++)  for(i=0; i<2; i++)
     { if(Roadattrib[Road[j][17+i]].istoptank==YES)    Road[j][17+i]=5;
       if(Roadattrib[Road[j][i]].istoptank==YES)       Road[j][i]=5;
       if(Roadattrib[Road[j][34+i]].istoptank==YES)    Road[j][34+i]=5;
       if(Roadattrib[Road[j+26][i+20]].istoptank==YES) Road[j+26][i+20]=5;
     }
   for(j=0;j<2;j++)  for(i=0;i<2;i++)  showico(i+17, j+26, ((j*2+i)<<4)|6);
   Stage ++;
   for(i=0; i<4; i++)  Tankn[i] = 0;
   overflag = 0;

   if(loadflag)   loadzd(STAGE);

   mouse_off();
   for(j=0; j<28; j++) for(i=0; i<36; i++)  if(Road[j][i]==0) Road[j][i] = 0xf0;

   settextjustify(LEFT_TEXT, CENTER_TEXT);
   sprintf(str, "%s<%c>%s(%d) -> %s", P.ieneat?"*":"",'A'+kindflag,wave,N,stage);
   bar(100, BOTTOM+2, 529, 479);
   settextstyle(1, 0, 1);
   setcolor(DARKGRAY);  outtextxy(119, 464, str);
   setcolor(LIGHTGRAY); outtextxy(121, 466, str);
   sprintf(KIND, "%s<%c>", P.ieneat?"*":"", 'A'+kindflag);
   
   settextjustify(CENTER_TEXT, CENTER_TEXT);   settextstyle(4, 0, 2);
   sprintf(str, "%d", Stage);
   setcolor(LIGHTRED);    outtextxy(614, 154, str);
   setcolor(LIGHTGREEN);  outtextxy(612, 152, str);
   setcolor(YELLOW);
   settextstyle(2, 0, 4);  outtextxy(612, 140, "STAGE");
   show_test();    showfs();
   mouse_on();

   flag = start_run();    //Start run ----->>>>----

   Sound(-2);   loadflag = 0;
   if(flag==1)  show_end();
   if(flag==0)  { savezd("#quit");  break; }
   if(result==LOOSE)  break;
   findtankfile(0);
WEND
  P = savekind;
  if(flag)
    { char str[30]; int k;
      mouse_off();  cleardevice();      
      sprintf(str, "Test: %ld", Test);      
      _sound(1000);  
      for(k=0; k<100; k++)
       { setviewport(320-2*k, 240-k, 320+2*k, 240+k,1);
         setcolor(LIGHTGREEN);  settextstyle(3, 0, 6);
	 outtextxy(k*2, k-30, "GAME OVER!");
	 setcolor(YELLOW-8);  settextstyle(1, 0, 4);
	 outtextxy(k*2, k+20, str);
       }
      setviewport(0, 0, 639, 479, 0);      
      nosound();  delay(200);  end_sound();
      sleep(2);   hidescreen(random(2)?1:-2);
      mouse_on();
    }
 ret:
  disable(); setvect(0x09, oldkey); enable();
  *((char far*)0x0041aL) = *((char far*)0x0041cL);
  mouse_on();  return;
 }

#define DY  70
void  show_begin()
 { void  show_kz(), start_sound();
   char  str[50];  int i;
   hidescreen(-2);
   mouse_off();   show_kz();
   setcolor(LIGHTRED);
   rectangle(320-250, 240-150-DY, 320+250, 240+150-DY);
   setcolor(LIGHTGREEN);
   rectangle(320-252, 240-152-DY, 320+252, 240+152-DY);
   setfillstyle(6, DARKGRAY);  bar(320-248, 240-148-DY, 320+248, 240+148-DY);
   settextstyle(1, 0, 4);  setcolor(LIGHTGREEN);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(320-250+70, 240-150+20-DY, "Skyever");
   outtextxy(320+250-70, 240+150-20-DY, VER);
   setcolor(CYAN);    settextstyle(2, 0, 5);
   outtextxy(320, 350, "Email: Skyever_youer@sohu.com");
   sprintf(str, KINDSTR, 'A'+kindflag);  settextstyle(1, 0, 5);
   setcolor(LIGHTBLUE);   outtextxy(320, 150-DY, str);
   settextstyle(4, 0, 7);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW);  sprintf(str, "STAGE %d", Stage+1);
   outtextxy(320, 220-DY, str);
   sprintf(str, "%s -> %s", wave, stage);
   settextstyle(1, 0, 4);  setcolor(LIGHTGRAY);
   outtextxy(320, 300-DY, str);
   start_sound();
   hidescreen(1);
   mouse_on();
 }

void  show_end()
 { char  str[50];  int k, i;  int fs = 0;
   mouse_off();  renew_screen();  hidescreen(1);   cleardevice();
   settextstyle(1, 0, 5);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW);  sprintf(str, "STAGE %d", Stage);
   outtextxy(320, 50, str);
   sprintf(str, "%s -> %s", wave, stage);
   settextstyle(1, 0, 2);  setcolor(LIGHTGRAY);
   outtextxy(320, 100, str);
   setlinestyle(0, 0, 3);   
   setcolor(LIGHTBLUE);  line(0, 130, 640, 130);
   setfillstyle(0, 0);
   for(k=0; k<4; k++)
     { showtank(8, 10+k*4, k+1);
       setcolor(LIGHTGREEN);  arrow(180, 170+65*k, 390, 190+65*k);
       setcolor(YELLOW);  settextstyle(1, 0, 4);
       settextjustify(LEFT_TEXT, CENTER_TEXT);
       for(i=0; i<=Tankn[k]; i++)
	{ bar(440, 180+65*k-20, 500, 180+65*k+20);
	  sprintf(str, "%d", i);
	  outtextxy(450, 180+65*k, str);
	  _sound(1000); delay(75); _sound(800); delay(25); nosound(); delay(100);
	  if(KEY.enter==1)
	   { int i; settextstyle(2, 0, 6); 
	     while(KEY.enter==1)
	      for(i=0; i<64; i++)
	       { setcolor(i/4); outtextxy(200, 20, "Press Enter to continue . . ."); }
	     setcolor(YELLOW);   bar(200, 10, 640, 30);  settextstyle(1, 0, 4);
	   }
	}
       fs += Tankn[k]*(k+1)*100;
       sprintf(str, "%d", Tankn[k]*(k+1)*100);
       setcolor(LIGHTBLUE); settextstyle(1, 0, 2);
       settextjustify(CENTER_TEXT, CENTER_TEXT);
       outtextxy(300, 170+65*k, str);
       _sound(1500);  delay(100);   _sound(1200);  delay(50);
       nosound();     delay(200);
     }
   setcolor(LIGHTGREEN);  arrow(180, 180+65*4, 390, 200+65*4);
   setcolor(YELLOW);  settextstyle(1, 0, 4);
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   setlinestyle(0, 0, 3);
   setcolor(LIGHTBLUE);   line(0, 420, 640, 420);
   setlinestyle(0, 0, 1);
   sprintf(str, "%d", Tankn[0]+Tankn[1]+Tankn[2]+Tankn[3]);
   setcolor(YELLOW);      outtextxy(450, 190+65*4, str);
   _sound(750);delay(100);_sound(800);delay(100);_sound(850);delay(100);nosound();
   sprintf(str, "%d", fs);
   setcolor(LIGHTBLUE); settextstyle(1, 0, 2);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   show_str(300, 180+65*k, 0, str);
   setcolor(LIGHTGREEN); sprintf(str, "%ld", Test);
   show_str2(120, 450, 6, str);
   sleep(3);
   mouse_on();
 }

void demorectangle()
 { int X1=LEFT, Y1=TOP, X2=RIGHT, Y2=BOTTOM;
   int x1=613-25, y1=415+12*2-5, x2=613+25, y2=415+12*2+5;
   float dx1=(float)(x1-X1)/200, dx2=(float)(x2-X2)/200,
	 dy1=(float)(y1-Y1)/200, dy2=(float)(y2-Y2)/200;
   int k;
   float _x1=X1, _y1=Y1, _x2=X2, _y2=Y2;
   setwritemode(XOR_PUT);   setlinestyle(0, 0, 1);
   setcolor(random(2)?WHITE:LIGHTGREEN);
   mouse_off();
   rectangle(X1, Y1, X2, Y2);
   for(k=0; k<200; k++)
    { rectangle(_x1, _y1, _x2, _y2);
      _x1 += dx1;  _y1 += dy1;  _x2 += dx2;  _y2 += dy2;
      rectangle(_x1, _y1, _x2, _y2);
    }
   rectangle(_x1, _y1, _x2, _y2);
   mouse_on();
   setwritemode(COPY_PUT);
 }

#define Fputc(x)  _Fputc(x, fp, sizeof(x))
void _Fputc(long x, FILE *fp, int n)
 { int k;
   for(k=0; k<n; k++)   fputc( (char)(x>>(k*8)), fp );
 }
int savezd(char *name)
 { int i, j, k;  unsigned char *ptr;  char n, *pstr[3] = { wave, stage, KIND };
   char str[50];  FILE *fp;
   sprintf(str, "msg\\save\\%s", name);
   fp = fopen(str, "wb");
   if(fp==NULL)  return 0;
   fputc('S', fp);  fputc('X', fp);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  Fputc(Road[j][i]);
   for(k=0; k<3; k++)
    { n = strlen(pstr[k]);  Fputc(n);
      for(i=0; i<n; i++)    Fputc(pstr[k][i]);
    }
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  Fputc(Roadtank[j][i]);
   Fputc(Stage);
   Fputc(shotkind);  Fputc(overflag);  Fputc(kindflag);
   Fputc(pauseflag); Fputc(dtank);     Fputc(headdeathflag);
   Fputc(fireflag);  Fputc(souflag);   Fputc(irunmyself);
   Fputc(Gameover);  Fputc(starnum);
   Fputc(Test);      Fputc(dTest);
   Fputc(mytankX);   Fputc(mytankY);
   Fputc(T1);        Fputc(T0);        Fputc(_);
   Fputc(N);
   Fputc(bkcolor);  Fputc(isound);  Fputc(idirectrun);   Fputc(endcolor);
   Fputc(fs0);      Fputc(fs1);
   Fputc(nowtankappear);  Fputc(hadtankappear);   Fputc(enemytanknum);
   Fputc(DTE);            Fputc(result);
   Fputc(nowshotappear);
   Fputc(pause);  Fputc(iron);  Fputc(mytanknum);  Fputc(fire);  Fputc(firekind);
   for(i=0; i<4; i++)  Fputc(Tankn[i]);
   Fputc(size);  Fputc(_shownum);
   Fputc(_shotgo);  Fputc(_pausetime);  Fputc(_firetime);  Fputc(_irontime);  Fputc(_addjusttime);
   for(k=0; k<12; k++)
    { ptr = (unsigned char *)&Tank[k];
      for(i=0; i<sizeof(TANKDEF); i++)   Fputc(ptr[i]);
    }
   for(k=0; k<MAXSHOTNUM; k++)
    { ptr = (unsigned char *)&Shot[k];
      for(i=0; i<sizeof(SHOTDEF); i++)   Fputc(ptr[i]);
    }
   ptr = (unsigned char *)&Eat;
   for(i=0; i<sizeof(struct _EAT); i++)  Fputc(ptr[i]);
   ptr = (unsigned char *)&P;
   for(i=0; i<sizeof(KINDDEF); i++)      Fputc(ptr[i]);
   for(k=0; k<13; k++)
    { ptr = (unsigned char *)&_showfs[k];
      for(i=0; i<sizeof(SHOWFS); i++)    Fputc(ptr[i]);
    }
   fclose(fp);
   if(strcmp(name,"#auto") && strcmp(name,"#quit"))  demorectangle();
   return 1;
 }

int readzdmsg(char *str, char *wave, char *stage)
 { FILE *fp;  char *pstr[2];
   char filename[50];  int k, i, n;
   pstr[0] = wave;  pstr[1] = stage;
   sprintf(filename, "msg\\save\\%s", str);
   fp = fopen(filename, "rb");
   if(fp==NULL)  return 0;
   fseek(fp, 28*36+2, SEEK_SET);
   for(k=0; k<2; k++)
    { n = fgetc(fp);
      for(i=0; i<n; i++)    pstr[k][i] = fgetc(fp);
      pstr[k][n] = '\0';
    }
   fclose(fp);   return 1;
 }

#define Fgetc(x)  x=_Fgetc(fp, sizeof(x))
long _Fgetc(FILE *fp, int n)
 { int i;  long result = 0;
   for(i=0; i<n; i++)  result |= ((long)fgetc(fp)<<(8*i));
   return result;
 }

int loadzd(char *name)
 { int i, j, k;
   FILE *fp;  unsigned char *ptr; char n, *pstr[3] = { wave, stage, KIND };
   char filename[50];  sprintf(filename, "msg\\save\\%s", name);
   fp = fopen(filename, "rb");
   if(fp==NULL)  return 0;

   fgetc(fp);  fgetc(fp);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)    Fgetc(Road[j][i]);
   for(k=0; k<3; k++)
    { Fgetc(n);
      for(i=0; i<n; i++)    Fgetc(pstr[k][i]);
      pstr[k][n] = '\0';
    }
   for(j=0; j<28; j++)  for(i=0; i<36; i++)    Fgetc(Roadtank[j][i]);
   Fgetc(Stage);
   Fgetc(shotkind);   Fgetc(overflag);  Fgetc(kindflag);
   Fgetc(pauseflag);  Fgetc(dtank);     Fgetc(headdeathflag);
   Fgetc(fireflag);   Fgetc(souflag);   Fgetc(irunmyself);
   Fgetc(Gameover);   Fgetc(starnum);
   Fgetc(Test);       Fgetc(dTest);
   Fgetc(mytankX);    Fgetc(mytankY);
   Fgetc(T1);         Fgetc(T0);        Fgetc(_);
   Fgetc(N);
   Fgetc(bkcolor);    Fgetc(isound);    Fgetc(idirectrun);
   Fgetc(endcolor);
   Fgetc(fs0);        Fgetc(fs1);
   Fgetc(nowtankappear);  Fgetc(hadtankappear);   Fgetc(enemytanknum);
   Fgetc(DTE);        Fgetc(result);
   Fgetc(nowshotappear);
   Fgetc(pause);  Fgetc(iron);  Fgetc(mytanknum);  Fgetc(fire);  Fgetc(firekind);
   for(i=0; i<4; i++)   Fgetc(Tankn[i]);
   Fgetc(size);   Fgetc(_shownum);
   Fgetc(_shotgo);  Fgetc(_pausetime);  Fgetc(_firetime);  Fgetc(_irontime);  Fgetc(_addjusttime);
   for(k=0; k<12; k++)
    { ptr = (unsigned char *)&Tank[k];
      for(i=0; i<sizeof(TANKDEF); i++)   Fgetc(ptr[i]);
    }
   for(k=0; k<MAXSHOTNUM; k++)
    { ptr = (unsigned char *)&Shot[k];
      for(i=0; i<sizeof(SHOTDEF); i++)   Fgetc(ptr[i]);
    }
   ptr = (unsigned char *)&Eat;
   for(i=0; i<sizeof(struct _EAT); i++)  Fgetc(ptr[i]);
   ptr = (unsigned char *)&P;
   for(i=0; i<sizeof(KINDDEF); i++)      Fgetc(ptr[i]);
   for(k=0; k<13; k++)
    { ptr = (unsigned char *)&_showfs[k];
      for(i=0; i<sizeof(SHOWFS); i++)    Fgetc(ptr[i]);
    }
   strcpy(MSG, Gameover==RUN?"":result==WIN?"THE END!":"GAME OVER!");
   fclose(fp);  return 1;
 }


void  mytankappear(int);
int  start_run()
 { int   time=0;  int i, j, k;  char flag=0, color=0;
   void  redflag();
   long  T = clock(), dT = T1-T0;
   void  suptankround(int), supshotgo(int), supshotth(int), setmytankround();
   void  tank_appear(), time_set();  TANKDEF  *Tankp=Tank+10;  SHOTDEF  *Shotp;

   T1 = T;  _ = T0 = T-18L;
   if(loadflag)  { T0-=dT; goto directrun_; }

   nowtankappear=0; nowshotappear=0; hadtankappear=0; enemytanknum=dtank=(int)P.ennum;
   for(j=0; j<28; j++)  for(i=0; i<36; i++)  Roadtank[j][i] = NOTANK;
   for(i=0; i<MAXSHOTNUM; i++)  Shot[i].life = 0;
   for(i=0; i<3; i++)
    { if((i==0||i==2)&&(P.ennum==1||P.maxtank==1)) { Tank[i].life=0; continue; }
      if(i==1 && (P.ennum==2||P.maxtank==2))  { Tank[i].life=0; continue; }
      Tank[i].blife=Tank[i].life = random(4)+1; Tank[i].round=3; Tank[i].Y=0;
      Tank[i].dxy = 0; Tank[i].ienemy = 1;  Tank[i].X = i*17;
      Tank[i].state = 7;   settankza(i);   Tank[i].shotspeed = random(P.enshotspeed+1);
      if(!random(3))  Tank[i].eats = random(Tank[i].life+1);
      Tank[i].addjust = 30;  Tank[i].speed = random(3)?1:2;   Tank[i].ipause = 0;
      nowtankappear++;    hadtankappear++;
    }
   Gameover = RUN;   result = WIN;  
   mytankappear(1);   
   for(i=3; i<12; i++)
     { if(i==10)  continue;
       Tank[i].life = 0;  Tank[i].X = Tank[i].Y = 0;
       Tank[i].dxy = 0; Tank[i].ienemy = 1;  Tank[i].round = 3;
       Tank[i].state = 7;  Tank[i].statetime = 0;
     }
   for(i=0; i<12; i++)
     { if(i==10)  continue; Tank[i].istop=0; Tank[i].stop=0; Tank[i].ibstop=0; }
   for(i=0; i<MAXSHOTNUM; i++)  Shot[i].life = 0;
   endcolor = random(4);
   endcolor = endcolor==0?LIGHTGREEN:endcolor==1?LIGHTBLUE:endcolor==2?WHITE:endcolor==3?YELLOW:LIGHTRED;
   headdeathflag = 0;  msg_show("");  _shownum = 0;
 directrun_:
   resetKEY();  show_star();    show_msg();
   show_line(0, pause/_pausetime); show_line(1, iron/_irontime);
   show_line(2, Tankp->addjust/_addjusttime); show_line(3, fire/_firetime);
   redflag();      Sound(-2);
   if(getdisk()>=2)   savezd("#auto");
BEGIN
   int returnflag = test_mouseclick();
   if(returnflag==-1)  if(EXITGAME)  return 0; else {off();renew_screen();on();}
   else if(returnflag==20 || KEY.insert==1)
    { KEY.insert = 0;
      if(bioskey(2)&0x03)
	{ int k = 0;  char filename[20] = "#noname", path[50]="msg\\save\\#noname";
	  while(access(path, 0)==0)
	   { sprintf(filename, "save-%d", ++k);
	     sprintf(path, "msg\\save\\%s", filename);
	   }
	  savezd(filename);
	}
      else       savezd("#noname");
    }
   else if(KEY.enter==1 || returnflag==10)
     { int color = 0;  setcolor(LIGHTGREEN);
       KEY.enter = 1;
       if(Gameover!=RUN&&Gameover>20)  { Sound(10); Gameover = 20; KEY.enter = 0; }
       else
	{ long  bT = clock();
	  show_str2(320, 240, 3, ">>> PAUSE <<<");
	  Sound(10);  redflag();  souflag = 0;
	  mouse_off();  renew_screen();  mouse_on();
	  while(KEY.enter==1)
	   { int k;
	     setcolor(color=++color==16?1:color);
	     settextjustify(CENTER_TEXT, CENTER_TEXT); settextstyle(1, 0, 3);
	     outtextxy(320, 240, ">>> PAUSE <<<");
	     settextjustify(LEFT_TEXT, CENTER_TEXT);   settextstyle(1, 0, 1);
	     setcolor(color|0x08);  outtextxy(121, 466, KIND);
	     for(k=0; k<1200; k++)
	      { returnflag = test_mouseclick();
		if(returnflag==-1) if(EXITGAME) return 0; else {off();renew_screen();on();}
		if(returnflag==10)  { KEY.enter=0; goto _exit; }
	      }  Sound(-1);
	    }
	  _exit: T0 += clock()-bT;
	 }
     }
   if(souflag==1) { redflag(); souflag=0; }
   if(++time == 150)  { time = 0; tank_appear(); }
   else if((time&0x0007)==0)
    { for(i=0; i<=10; i++)
	 if(Tank[i].life&&Tank[i].state==3)   settankza(i);
    }
//*********************************************************
   Shotp = Shot;
   for(k=0; k<MAXSHOTNUM; k++)
     { if(Shotp->life==0)   { Shotp ++;  continue; }
       if(Shotp->state==0)
	 { if(Shotp->round==1)  i = 1;  else i = 2;
	   if(++(Shotp->statetime) >= i)
	     { Shotp->life=0; nowshotappear--; Shotp++; continue; }
	 }
       if(Shotp->state)     supshotgo(k);
       if(Shotp->state==0)  Shotp->speed = 0;
       else if(Shotp->state>=2)  Shotp->state --;
       else if(Shotp->speed) Shotp->dxy = 0;
     if(Shotp->istep==1)
       if(Shotp->speed==0)  switch(Shotp->round)
	{ case 0: Shotp->dxy -= 8;
		  if(Shotp->dxy<-8) { (Shotp->X)--; Shotp->dxy=0; } break;
	  case 1: Shotp->dxy += 8;
		  if(Shotp->dxy>8)  { (Shotp->X)++; Shotp->dxy=0; } break;
	  case 2: Shotp->dxy -= 8;
		  if(Shotp->dxy<-8) { (Shotp->Y)--; Shotp->dxy=0; } break;
	  case 3: Shotp->dxy += 8;
		  if(Shotp->dxy>8)  { (Shotp->Y)++; Shotp->dxy=0; } break;
	}
       else   switch(Shotp->round)
	{ case 0: Shotp->X--; break;	  case 1: Shotp->X++; break;
	  case 2: Shotp->Y--; break;	  case 3: Shotp->Y++; break;
	}
       Shotp ++;
     }
//*************************************************************
   Tankp = Tank;
   for(k=0; k<12; k++)
     { char speed = Tankp->speed;
       if(k==10 || Tankp->life==0)   { Tankp ++;  continue; }
       if(Tankp->state!=3)
	 { if(Tankp->state>3)
	     { if(++Tankp->statetime==10)
		 { Tankp->statetime=0;  Tankp->state--; }
	     }
	   else
	     { if(++Tankp->statetime==15)
		 { Tankp->statetime=0; Tankp->state--; }
	       if(Tankp->state==0)
		 { Tankp->life = 0;  nowtankappear--; }
	     }  Tankp ++;  continue;
	 }
       if(++(Tankp->flashflag)==6)   Tankp->flashflag = 0;
       if(Tankp->addjust)        Tankp->addjust --;
       if(pause&&pauseflag==0&&Tankp->dxy==0)  
	 { if(Tankp->ipause==0)
	    { int i, j, x=Tankp->X, y=Tankp->Y;
	      for(j=-1; j<3; j++)  for(i=-1; i<3; i++)  
		if(Roadtank[y+j][x+i]==k)  Roadtank[y+j][x+i] = NOTANK;
	      for(j=0; j<2; j++)  for(i=0; i<2; i++)  Roadtank[y+j][x+i] = k;
	      Tankp->ipause = 1;
	    }
	   Tankp++;  continue;
	 }
       if(Tankp->state==3)
	 if(Tankp->dxy==0)
	   { suptankround(k); supshotth(k); supshotth(k); supshotth(k); }
	 else                 supshotth(k);
       if(P.ieneat && Tankp->dxy==0)  tank_eat(Tankp);
       if(Tankp->istop==1)  { Tankp ++;  continue; }
       switch(Tankp->round)
	{ case 0: Tankp->dxy-=speed;
		  if((Tankp->dxy)<=-8) { (Tankp->X)--; Tankp->dxy = 8;}  break;
	  case 1: Tankp->dxy+=speed;
		  if((Tankp->dxy)>=8)  { (Tankp->X)++; Tankp->dxy = -8;} break;
	  case 2: Tankp->dxy-=speed;
		  if((Tankp->dxy)<=-8) { (Tankp->Y)--; Tankp->dxy = 8;}  break;
	  case 3: Tankp->dxy+=speed;
		  if((Tankp->dxy)>=8)  { (Tankp->Y)++; Tankp->dxy = -8;} break;
	}  Tankp ++;
     }
   if(fire && --fire%_firetime==0)    show_line(3, fire/_firetime);
   if(++firekind==8)  firekind = 0;
   if(pause && --pause%_pausetime==0)  show_line(0, pause/_pausetime);
   if(iron)
    { int i, j;
      if(iron<201 && (iron-10)%20==0)
	{ for(j=0; j<3; j++)   for(i=0; i<4; i++)
		    if((Road[25+j][16+i]&0x0f)!=6)  Road[25+j][16+i]=2;
	}
      else if(iron<=201 && (iron-1)%20==0 )
	{ for(j=0; j<3; j++)   for(i=0; i<4; i++)
	       if((Road[25+j][16+i]&0x0f)!=6)  Road[25+j][16+i]=0xf0;
	}
      if(--iron % _irontime==0)  show_line(1, iron/_irontime);
     }
   setmytankround();   Sound(-1);
//*************************************************************
   renew_screen();   time_set();
   
   if(++color==32)  color = 0;
   if((color&0x03)==0)
     { setcolor((color>>2)|0x08);
       settextjustify(LEFT_TEXT, CENTER_TEXT);   settextstyle(1, 0, 1);
       outtextxy(121, 466, KIND);
     }
   if(Gameover!=RUN)  if(--Gameover<=0)    return 1;
   if(KEY.esc==1 && (bioskey(2)&0x03) )    return 0;
   if(++flag==2)  { while(clock()-T<1);  T = clock();  flag = 0; }
WEND
 }

void mytankappear(int code)
 { Tank[10].life = 1;
   if(Gameover==RUN)  Tank[10].round = keyptr==-1?2:round[keyptr];
   else               Tank[10].round = 2;
   Tank[10].dxy = 0;
   mytankY = Tank[10].Y = 26; mytankX = Tank[10].X = 20;
   Tank[10].ienemy = 0;  settankza(10);
   Tank[10].istop = Tank[10].ibstop = 0;   Tank[10].state = 7;
   Tank[10].addjust = _addjusttime*4+1;
   if(code==0)
     { TT.speed=P.tankspeed+1; shotkind=P.myshotkind;
       TT.shotspeed=P.myshotspeed; TT.iship=TT.ibship=P.isea;
     }
   show_star();  show_xy();
   if(pauseflag==1&&pause)  pause = 0;
 }

#define  RoadT(y,x)   if(Roadtank[y][x]==10) Roadtank[y][x]=NOTANK
void  showshot(int Num, int flag);
#define  Tests(k,n0,n1,m)\
	 if(Roadattrib[k&0x0f].istopshot==NO) sflag[m]=0;\
	 else if((k&0x0f)==0&&!(k&((unsigned char)0x80>>n0))&&!(k&((unsigned char)0x80>>n1))) sflag[m]=0;\
	 else sflag[m] = 1;

void  setmytankround()
 { TANKDEF  *Tankp = Tank + 10;  unsigned long _shotkind = ZDkind[shotkind];
   char key=keyptr, _round=round[key], flag=0, flag0=0,speed=Tankp->speed, f=0;
   int X = Tankp->X, Y = Tankp->Y, sflag[2], itankflag=0, itankshot=0;
   static char justround;  int testtank(int Y, int X, int round);
   if(tankfire(10))  return;
   if(Tankp->state!=3)
     { if(Tankp->state>3)
	{ if(++Tankp->statetime==10)
	   { Tankp->statetime=0;  Tankp->state--; }
	}
       else
	{ if(++Tankp->statetime==18)
	    { Tankp->statetime=0; Tankp->state--; }
	  if(Tankp->state==0)
	    { Tankp->life=0; if(mytanknum==0) return; mytankappear(0); }
	}  return;
     }
   if(++(Tankp->flashflag)==6)   Tankp->flashflag = 0;
   if(Tankp->addjust)
     { if((--Tankp->addjust)%_addjusttime==0)  show_line(2, Tankp->addjust/_addjusttime); }
   if(Tankp->iship!=Tankp->ibship)
     if(tanknotinsea(10)) { Tankp->iship = 0; show_star(); }
   if(result==LOOSE)   return;
   if(key==-1)
    { if(Tankp->ibstop==0)
       { bRound=-1; Tankp->istop=1; Tankp->ibstop=1; }
    }
   else if(bRound!=_round)
    { Tankp->round = bRound = _round;
      Tankp->istop = 0;  Tankp->ibstop = 0;
      if(justround!=_round)  { Tankp->dxy = 0; justround=_round; }
    }
//*****************************************************************
   if(Tankp->dxy==0)
    { int k, i, j;  long bTest = Test;  char str[15];
      if(X!=34) { RoadT(Y,X+2); RoadT(Y+1,X+2); }
      if(X!=0)  { RoadT(Y,X-1); RoadT(Y+1,X-1); }
      if(Y!=26) { RoadT(Y+2,X); RoadT(Y+2,X+1); }
      if(Y!=0)  { RoadT(Y-1,X); RoadT(Y-1,X+1); }

      if(Eat.kind!=NOEAT && X>=Eat.X-2 && X<=Eat.X+3 && Y>=Eat.Y-2 && Y<=Eat.Y+2)
       { int num = 0;  Sound(4);
	 switch(Eat.kind)
	  { case 0: for(k=0; k<10; k++)
		      if(Tank[k].life && Tank[k].state==3 && Tank[k].addjust==0)
			{ TANKDEF *Tankp = Tank+k;
			  Tank[k].state = 2;  num ++;
			  add_showfs_fortank(Tankp, "Bomb!");
			  for(j=0; j<28; j++)  for(i=0; i<36; i++)
			    if(Roadtank[j][i]==k)   Roadtank[j][i] = NOTANK;
			}
		    enemytanknum -= num;   show_msg();
		    if(enemytanknum<=0)  { Gameover--; if(result==WIN) msg_show("THE END!");}
		    if(num==0)  { Test += 100;   show_test(); }
		    break;
	    case 1: if(enemytanknum==0)  { Test += 100;  show_test(); }
		    else
		     { pause = _pausetime*10+1;  pauseflag = 0;
		       for(k=0; k<10; k++)
			 if(Tank[k].life && Tank[k].state>=3)
			   add_showfs_fortank(Tank+k, "Pause - 100%");
		     }
		    break;
	    case 2: iron  = _irontime*10+1;
		    for(j=0; j<3; j++)   for(i=0; i<4; i++)
		    if((Road[25+j][16+i]&0x0f)!=6)  Road[25+j][16+i]=2;
		    ADDHEAD("Protected, OK!");
		    break;
	    case 3: mytanknum ++; show_msg();
		    sprintf(str, "%d tanks", mytanknum);
		    add_showfs_fortank(Tank+10, str);
	            break;
	    case 4: Tankp->addjust = _addjusttime*10+1;
		    add_showfs_fortank(Tank+10, "Protected!");
		    break;
	    case 5: if(Tankp->shotspeed!=3)
		      { Tankp->shotspeed++;
			add_showfs_fortank(Tank+10, "Shotspeed+");
			show_star();
		      }
		    else { Test += 100;   show_test(); }
		    break;
	    case 6: if(Tankp->speed!=2)
		     { Tankp->speed++; show_star();
		       add_showfs_fortank(Tank+10, "Tankspeed+");
		     }
		    else if(shotkind<3)
		     { shotkind ++; show_star();
		       add_showfs_fortank(Tank+10, "Shotkind+");
		     }
		    else { Test += 100;   show_test(); }
		    break;
	    case 7: if(Tankp->ibship!=1)
		      { Tankp->iship = Tankp->ibship = 1;  show_star();
			add_showfs_fortank(Tank+10, "Ship+");
		      }
		    else { Test += 100;  show_test(); }
		    break;
	    case 8: for(j=0; j<28; j++)  for(i=0; i<36; i++)
		     if(Road[j][i]==4)  goto _break; _break:
		    if(j==28&&i==36)  { Test += 100;  show_test(); }
		    else
		     { fire = _firetime*10+1;  fireflag = 0;
		       add_showfs_fortank(Tank+10, "Fire!");
		       for(k=0; k<10; k++)
			 if(Tank[k].life&&Tank[k].state==3) tankfire(k);
		     }
		    break;
	  }
	 Eat.kind = NOEAT;    Test += 100;  show_test();
	 sprintf(str, "+%ld", Test-bTest);
	 add_showfs(Eat.X+1, Eat.Y+1, 8, 0, str);
       }
      if(key==-1)  goto end;
      if(Tankp->iship==1)   Roadattrib[3].istoptank = NO;
      switch(Tankp->round)
       { case 0: if(X<=0)  { flag = 1; break; }
		 if(Roadattrib[Road[Y][X-1]&0x0f].istoptank==YES  ||
		   Roadattrib[Road[Y+1][X-1]&0x0f].istoptank==YES)  flag = 1;
		 if(Roadtank[Y][X-1]!=NOTANK || Roadtank[Y+1][X-1]!=NOTANK)
		   { itankflag = 1; if(testtank(Y,X,0)) itankshot = 1; }
		 if(Road[Y][X-1]==3 || Road[Y+1][X-1]==3)  f = 1;
		 Tests(Road[Y][X-1],2,3,0); Tests(Road[Y+1][X-1],0,1,1); break;
	 case 1: if(X>=34) { flag = 1; break; }
		 if(Roadattrib[Road[Y][X+2]&0x0f].istoptank==YES  ||
		   Roadattrib[Road[Y+1][X+2]&0x0f].istoptank==YES)  flag = 1;
		 if(Roadtank[Y][X+2]!=NOTANK || Roadtank[Y+1][X+2]!=NOTANK)
		   { itankflag = 1;  if(testtank(Y,X,1)) itankshot = 1; }
		 if(Road[Y][X+2]==3 || Road[Y+1][X+2]==3)  f = 1;
		 Tests(Road[Y][X+2],2,3,0); Tests(Road[Y+1][X+2],0,1,1); break;
	 case 2: if(Y<=0) { flag = 1; break; }
		 if(Roadattrib[Road[Y-1][X]&0x0f].istoptank==YES  ||
		  Roadattrib[Road[Y-1][X+1]&0x0f].istoptank==YES)  flag = 1;
		 if(Roadtank[Y-1][X]!=NOTANK || Roadtank[Y-1][X+1]!=NOTANK )
		   { itankflag = 1; if(testtank(Y,X,2)) itankshot = 1; }
		 if(Road[Y-1][X]==3 || Road[Y-1][X+1]==3)  f = 1;
		 Tests(Road[Y-1][X],1,3,0); Tests(Road[Y-1][X+1],0,2,1); break;
	 case 3: if(Y>=26)  { flag = 1; break; }
		 if(Roadattrib[Road[Y+2][X]&0x0f].istoptank==YES  ||
		  Roadattrib[Road[Y+2][X+1]&0x0f].istoptank==YES)  flag = 1;
		 if(Roadtank[Y+2][X]!=NOTANK || Roadtank[Y+2][X+1]!=NOTANK)
		   { itankflag=1; if(testtank(Y,X,3)) itankshot = 1; }
		 if(Road[Y+2][X]==3 || Road[Y+2][X+1]==3)  f = 1;
		 Tests(Road[Y+2][X],1,3,0); Tests(Road[Y+2][X+1],0,2,1); break;
       }
      if(Tankp->iship==1)   Roadattrib[3].istoptank = YES;
      if(itankflag) { if(itankshot) _shotkind=0x88888888L; Tankp->istop=1;  goto end; }
      if(flag==1)
	 { if(!(f==1||(sflag[0]==0&&sflag[1]==0)))  _shotkind=0x88888888L;
	   Tankp->istop = 1;  goto end;
	 }
      switch(Tankp->round)
       { case 1: Roadtank[Y][X+2]=10; Roadtank[Y+1][X+2]=10; break;
	 case 0: Roadtank[Y][X-1]=10; Roadtank[Y+1][X-1]=10; break;
	 case 2: Roadtank[Y-1][X]=10; Roadtank[Y-1][X+1]=10; break;
	 case 3: Roadtank[Y+2][X]=10; Roadtank[Y+2][X+1]=10; break;
       }
    }
   if(Road[Y][X]==1||Road[Y][X+1]==1||Road[Y+1][X]==1||Road[Y+1][X+1]==1)
      flag0 = 1;
   if((key!=-1 || flag0==1) && !(pause&&pauseflag==1))
     { static char flag;
       if(speed==1)  if(flag)  speed = 2;
       if(tankinice(10))  speed <<= 1;
       switch(Tankp->round)
	{ case 0: Tankp->dxy -= speed;
		  if((Tankp->dxy)<=-8) { (Tankp->X)--; Tankp->dxy = 8;}  break;
	  case 1: Tankp->dxy += speed;
		  if((Tankp->dxy)>=8)  { (Tankp->X)++; Tankp->dxy = -8;} break;
	  case 2: Tankp->dxy -= speed;
		  if((Tankp->dxy)<=-8) { (Tankp->Y)--; Tankp->dxy = 8;}  break;
	  case 3: Tankp->dxy += speed;
		  if((Tankp->dxy)>=8)  { (Tankp->Y)++; Tankp->dxy = -8;} break;
	}
       if(abs(Tankp->dxy)<=((speed>>2)+1))  Tankp->dxy = 0;
       flag = 1-flag;
       mytankX = Tankp->X;  mytankY = Tankp->Y;   show_xy();
       if(flag0==1&&key==-1&&Tankp->dxy==0)  Sound(9);
     }
//*****************************************************************
end: if(_shotkind&((unsigned long)0x80000000>>shotptr))
     if(KEY.space)  showshot(10, 0);
     else if(KEY.akey) showshot(10, 'a');
     else if(KEY.skey) showshot(10, 's');
     else if(KEY.dkey) showshot(10, 'd');
     else if(KEY.fkey) showshot(10, 'f');
     else if(shotptr==0)  { shotptr = 0; return; }
   if(++shotptr>=_shotgo)  shotptr = 0;
 }

int testtank(int Y, int X, int round)
 { int x[2], y[2], k;
   int shotx, shoty, tankx1, tanky1, tankx2, tanky2;
   switch(round)
    { case 0: x[0]=X-1; y[0]=Y; x[1]=X-1; y[1]=Y+1;  break;
      case 1: x[0]=X+2; y[0]=Y; x[1]=X+2; y[1]=Y+1;  break;
      case 2: x[0]=X; y[0]=Y-1; x[1]=X+1; y[1]=Y-1;  break;
      case 3: x[0]=X; y[0]=Y+2; x[1]=X+1; y[1]=Y+2;  break;
    }
   if(round<2)
     for(k=0; k<2; k++)
      { int K=Roadtank[y[k]][x[k]];  if(K==NOTANK)  continue;
	if(Tank[K].state!=3)  continue; 
	if(Tank[K].round<2)  return 1;
	shoty = (int)(Y+1)<<4;
	tanky1 = ((int)(Tank[K].Y)<<4)+Tank[K].dxy;  tanky2 = tanky1+32;
	if(shoty<=tanky2+1 && shoty>=tanky1-1)  return 1;
      }
   else
     for(k=0; k<2; k++)
       { int K=Roadtank[y[k]][x[k]];  if(K==NOTANK)  continue;
	 if(Tank[K].state!=3)  continue;
	 if(Tank[K].round>1)  return 1;
	 shotx = (int)(X+1)<<4;
	 tankx1 = ((int)(Tank[K].X)<<4)+Tank[K].dxy;  tankx2 = tankx1+32;
	 if(shotx<=tankx2+1 && shotx>=tankx1-1)  return 1;
       }
   return 0;
 }

int   shot0, shot1;
void  supshotth(int Num)
 { TANKDEF  *Tankp = Tank+Num;
   int X, Y;
   if(++(Tankp->shottime)!=60)  return;
   Tankp->shottime = 0;  X = Tankp->X;  Y = Tankp->Y;
   shot0 = 100;  shot1 = 100;
   if(Y>=24)
     if(Tankp->round==1 && X<20)  shot0 += 2000;
     else if(Tankp->round==0 && X>20)  shot0 += 2000;
   if(X>=19&&X<=21&&Tankp->round==3)  shot0 += 2000;
   if(abs(mytankX-X)<2)
     if(Tankp->round==2 && mytankY<Y)  shot0 += 2000;
     else if(Tankp->round==3 && mytankY>Y)  shot0 += 2000;
   if(abs(mytankY-Y)<2)
     if(Tankp->round==0 && mytankX<X)  shot0 += 2000;
     else if(Tankp->round==1 && mytankX>X)  shot0 += 2000;
   if(random(shot0+shot1)<shot0)  showshot(Num, 0);
 }

void  showshot(int Num, int flag)
 { int k; TANKDEF *Tankp = Tank + Num;  SHOTDEF *Shotp = Shot;
   if(nowshotappear>=MAXSHOTNUM)  return;
   for(k=0; k<MAXSHOTNUM; k++)  { if(Shotp->life==0)   break;  Shotp++; }
   if(k==MAXSHOTNUM)  return;  Shotp->istep = 1;
   Shotp->life = 1;   Shotp->ienemy = Tankp->ienemy;  Tankp->spendflag = 1;
   Shotp->round = Tankp->round;   Shotp->state = 3;   Shotp->statetime = 0;
   Shotp->X=Tankp->X+1;  Shotp->Y=Tankp->Y+1;  Shotp->dxy=Tankp->dxy;
   switch(Tankp->round)
    { case 0: Shotp->X--; break;  case 1: Shotp->X++; break;
      case 2: Shotp->Y--; break;  case 3: Shotp->Y++; break;
    }
   Shotp->treebreak=0;
   if(flag == 0)
     { Shotp->speed=Tankp->shotspeed; if(shotptr==0) Shotp->treebreak = 1; }
   else switch(flag)
    { case 'a': Shotp->speed = 0;  break;
      case 's': if(Tankp->shotspeed>=1)  Shotp->speed = 1;
		else                     Shotp->speed = 0; break;
      case 'd': if(Tankp->shotspeed>=2)  Shotp->speed = 2;
		else       Shotp->speed = Tankp->shotspeed; break;
      case 'f': Shotp->speed = Tankp->shotspeed; if(shotptr==0) Shotp->treebreak=1; break;
    }
   Shotp->which = Num;  nowshotappear ++;
 }

#define  Bit(n)  ((Road[_y][_x])&((unsigned char)0x80>>n))
#define  Cls(n)  (Road[_y][_x])&=(~((unsigned char)0x80>>n))

void  supshotgo(const int K)
 { SHOTDEF  *Shotp = Shot + K;  TANKDEF  *Tankp;
   int X = Shotp->X, Y = Shotp->Y, dxy, k0;
   int x[2], y[2], sflag[2], k, _x, _y, tankx1, tanky1, tankx2, tanky2, shotx, shoty;
   switch(Shotp->round)
     { case 0: if(X<=0) { Shotp->state = 0; if(Shotp->ienemy==0) Sound(14); return; }
	       else { x[0]=X-1; y[0]=Y-1; x[1]=X-1; y[1]=Y; }
	       Tests(Road[y[0]][x[0]],2,3,0); Tests(Road[y[1]][x[1]],0,1,1); break;
       case 1: if(X>=36) { Shotp->state = 0; if(Shotp->ienemy==0) Sound(14); return; }
	       else { x[0]=X; y[0]=Y-1; x[1]=X; y[1]=Y; }
	       Tests(Road[y[0]][x[0]],2,3,0); Tests(Road[y[1]][x[1]],0,1,1); break;
       case 2: if(Y<=0)  { Shotp->state = 0; if(Shotp->ienemy==0) Sound(14); return; }
	       else { x[0]=X-1; y[0]=Y-1; x[1]=X; y[1]=Y-1; }
	       Tests(Road[y[0]][x[0]],1,3,0); Tests(Road[y[1]][x[1]],0,2,1); break;
       case 3: if(Y>=28) { Shotp->state = 0; if(Shotp->ienemy==0) Sound(14); return; }
	       else { x[0]=X-1; y[0]=Y; x[1]=X; y[1]=Y; }
	       Tests(Road[y[0]][x[0]],1,3,0); Tests(Road[y[1]][x[1]],0,2,1); break;
     }
headbreak:
   if( Roadattrib[Road[y[0]][x[0]]&0x0f].iend == YES ||
	    Roadattrib[Road[y[1]][x[1]]&0x0f].iend == YES )
     { int i, j;  Shotp->state = 0;
       if(headdeathflag==0)
	{ Gameover--;   result = LOOSE;   msg_show("GAME OVER!");
	  Shotp->state = 0;
	  Tank[11].life = 1;  Tank[11].state = 2;  Tank[11].X=17; Tank[11].Y=26;
	  overflag = 1;  Sound(11); headdeathflag = 1;
	  ADD(Tank[11].X+1, Tank[11].Y+1, "GAME OVER!");
	} return;
     }

//************************** Test road ************************
roadbreak:
   if(Shotp->speed==1)
     for(k=0; k<2; k++)
	{ if(sflag[k]==0)  continue;
	  Shotp->state = 0;
	  if(Roadattrib[Road[y[k]][x[k]]&0x0f].ihard==YES)
	     { if(Shotp->which==10) Sound(1);   continue; }
	  Road[y[k]][x[k]] = 5;
	  if(Shotp->ienemy==0)
	    { Test += 2;  show_test(); fs0 += 2; showfs(); Sound(2); }
	}
   else if(Shotp->speed==0)
     for(k=0; k<2; k++)
       { if(sflag[k]==0)  continue;
	 _x = x[k];  _y = y[k];
	 Shotp->state = 0;
	 if(Roadattrib[Road[_y][_x]&0x0f].ihard==YES)
	    { if(Shotp->which==10)  Sound(1);  continue; }
	 switch(Shotp->round)
	   { case 0: if(Bit(1)||Bit(3))  { Cls(1); Cls(3); }
		    else  { Cls(0); Cls(2); }   break;
	     case 1: if(Bit(0)||Bit(2))  { Cls(0); Cls(2); }
		    else  { Cls(1); Cls(3); }   break;
	     case 2: if(Bit(2)||Bit(3))  { Cls(2); Cls(3); }
		    else  { Cls(0); Cls(1); }   break;
	     case 3: if(Bit(0)||Bit(1))  { Cls(0); Cls(1); }
		    else  { Cls(2); Cls(3); }   break;
	   }
	 if(Road[_y][_x]==0)    Road[_y][_x] = 5;
	 if(Shotp->ienemy==0)
	   { Test ++;  show_test(); fs0++; showfs(); }
	 if(Shotp->which==10)  Sound(2);
       }
    else if(Shotp->speed==2)  s2:
      for(k=0; k<2; k++)
	{ char flag=0;
	  if(sflag[k]==0)  continue;
	  if(Roadattrib[Road[y[k]][x[k]]&0x0f].ihard==YES)  flag = 1;
	  Road[y[k]][x[k]] = 5;    Shotp->state = 0;
	  if(Shotp->ienemy==0)
	    if(flag==1)
	      { Test += 5;  show_test(); fs1 += 5; showfs(); Sound(1); }
	    else { Test += 2; show_test(); fs0 += 2;  showfs(); Sound(2); }
	}
    else if(Shotp->speed==3)
      { char flag=0;
	for(k=0; k<2; k++)
	  if(Roadattrib[Road[y[k]][x[k]]&0x0f].icoverd==YES)
	     { Road[y[k]][x[k]] = 5;  flag = 1;
	       if(Shotp->ienemy==NO) Sound(13);
	     }
	if(flag==1)
	  if(Shotp->treebreak==0)  Shotp->speed = 2;
	  else   Shotp->treebreak = 0;
	goto s2;
      }                   
tankbreak:
   //************************** Test tank ************************
  dxy = Shotp->dxy;
     switch(Shotp->round)
      { case 0: shotx = (X<<4)+dxy-4;  shoty = (Y<<4);  break;
	case 1: shotx = (X<<4)+dxy+4;  shoty = (Y<<4);  break;
	case 2: shotx = (X<<4);  shoty = (Y<<4)+dxy-4;  break;
	case 3: shotx = (X<<4);  shoty = (Y<<4)+dxy+4;  break;
      }
  for(k=0; k<2; k++)
   { if((k0=Roadtank[y[k]][x[k]])==NOTANK)  continue;
     Tankp = Tank+k0;  if(Tankp->life==0 || Tankp->state!=3)  continue;
     if(Shotp->which==k0)  continue;
     if(Tankp->ienemy == Shotp->ienemy)  continue;
     if(Tankp->addjust!=0)  { Shotp->state = 0; Sound(12); continue; }
     X = Tankp->X;  Y = Tankp->Y;   dxy = Tankp->dxy;
     switch(Tankp->round)
      {	case 0: case 1: tankx1 = (X<<4) + dxy; tanky1 = (Y<<4);  break;
	case 2: case 3: tankx1 = (X<<4);  tanky1 = (Y<<4)+dxy;   break;
      }
     tankx2 = tankx1+32;  tanky2 = tanky1+32;
     if(shotx<=tankx2+1 && shotx>=tankx1-1 && shoty<=tanky2+1 && shoty>=tanky1-1)
       { int i, j;  void  showeats();
	 Shotp->state = 0;  Sound(7);
	 if(Tankp->eats)  { Tankp->eats --;  showeats(); }
	 if(Tankp->life!=1)
	   { Tankp->life--; if(k0==Roadtank[y[1]][x[1]])break;continue;}
	 Tankp->state=2; Tankp->statetime=0; Tankp->life=1;
	 if(Tankp->ienemy)
	  { char str[10]; sprintf(str, "+%d", Tankp->blife*100);
	    Tankn[Tankp->blife-1]++; Test += Tankp->blife*100; show_test();
	    add_showfs_fortank(Tankp, str);
	  }
	 if(tankinsea(k0)&&!P.insea) { Tank[k0].state = 1;  Sound(15); }
	 else  if(k0!=10) { Sound(5); if(--enemytanknum<=0) { Gameover--;  if(result==WIN) msg_show("THE END!"); } show_msg(); }
	 else  if((int)TT.shotspeed+shotkind+TT.speed+TT.ibship>=starnum)
	   { if(Tank[10].shotspeed) Tank[10].shotspeed--;    
	     if(!tankinsea(10))   Tank[10].iship = Tank[10].ibship = 0;
	     else  Tank[10].ibship = 0;
	     if(shotkind)  shotkind--; if(Tank[10].speed!=1)  Tank[10].speed--;
	     Sound(5); show_star();  Tankp->state=3;  Tankp->life=1;
	     add_showfs_fortank(Tank+10, "Level-");
	   }
	 else
	   { char str[30];
	     if(mytanknum==1)  strcpy(str, "Sorry - No tanks!");
	     else sprintf(str, "Sorry - %d tank%s rest", mytanknum-1, mytanknum-1==1?"":"s");
	     Sound(6);  ADD(Tankp->X+1, Tankp->Y+1, str);
	     if(--mytanknum<=0) { Gameover--; result=LOOSE; msg_show("GAME OVER!"); }
             show_msg(); 
	   }
	 if(Tankp->state!=3)
	  { for(j=0; j<28; j++)  for(i=0; i<36; i++)
	      if(Roadtank[j][i]==k0)  Roadtank[j][i] = NOTANK;
	    for(i=0; i<MAXSHOTNUM; i++)  
	      if(Shot[i].life==1 && Shot[i].which==k0)
		{ if(Shotp->round + Shot[i].round == 1)
		    { if(Shotp->Y==Shot[i].Y && abs(X-Shot[i].X-1)<4)  Shot[i].state = 0; }
		  else if(Shotp->round + Shot[i].round == 5)
		    { if(Shotp->X==Shot[i].X && abs(Y-Shot[i].Y-1)<4)  Shot[i].state = 0; }
		}	    
	  }
	 i = Roadtank[y[1]][x[1]];
	 if(i==NOTANK || k0==i)  break;
       }
    }

 //*************************** Test Shot **************************
  if(Shotp->round<2)
     for(k=0; k<MAXSHOTNUM; k++)
      { int flag = 0;
	if(k==K || Shot[k].life==0 || Shot[k].state==0)  continue;
	if(Shot[k].which==Shotp->which)  continue;
	if(Shot[k].round<2)
	  { if(Shot[k].round!=Shotp->round)
	     { int d;
	       if(Shot[k].speed + Shotp->speed != 2)  d = 1;
	       else                                   d = 2; 
	       if(Y==Shot[k].Y && abs(X-Shot[k].X)<d)  flag = 1; 
	     }
	  }
	else if(X==Shot[k].X && Y==Shot[k].Y)       flag = 1;
	if(flag==1)
	  { Shotp->state=0; Shot[k].state=0; Shotp->istep=0; Shot[k].istep=0;
	    Shot[k].X = Shotp->X = (int)(((float)Shot[k].X+Shotp->X)/2+0.5);
	    Shot[k].Y = Shotp->Y = (int)(((float)Shot[k].Y+Shotp->Y)/2+0.5);
	    if(Shotp->ienemy != Shot[k].ienemy)  Sound(8); 
	  }
      }
   else
     for(k=0; k<MAXSHOTNUM; k++)
      { int flag = 0;
	if(k==K || Shot[k].life==0 || Shot[k].state==0)  continue;
	if(Shot[k].which==Shotp->which)  continue;
	if(Shot[k].round>1)
	  { if(Shot[k].round!=Shotp->round)
	     { int d;
	       if(Shot[k].speed + Shotp->speed != 2)  d = 1;
	       else                                   d = 2; 
	       if(X==Shot[k].X && abs(Y-Shot[k].Y)<d)  flag = 1; 
	     }
	  }
	else if(X==Shot[k].X && Y==Shot[k].Y)     flag = 1;
	if(flag==1)
	  { Shotp->state=0; Shot[k].state=0; Shotp->istep=0; Shot[k].istep=0;
	    Shot[k].X = Shotp->X = (int)(((float)Shot[k].X+Shotp->X)/2+0.5);
	    Shot[k].Y = Shotp->Y = (int)(((float)Shot[k].Y+Shotp->Y)/2+0.5);
	    if(Shotp->ienemy != Shot[k].ienemy)  Sound(8); 
	  }
      }
   return;
 }


int  Round[4], Sround[4];
#define  Roadt(y,x)   if(Roadtank[y][x]==Num) Roadtank[y][x]=NOTANK
void  suptankround(int Num)
 { TANKDEF  *Tankp = Tank+Num;
   int  round=Tankp->round, X=Tankp->X, Y=Tankp->Y, Rand, k;
   int  flag = 0, hide = 100;
   if(tankfire(Num))  return;
   if(Tankp->istop==1)
     if(++(Tankp->stop)!=10)   return;
     else  { Tankp->istop=0; Tankp->stop=0; }
   for(k=0; k<4; k++)   Round[k] = 50;
   if(Tankp->ibstop==0)  Round[round] = 650;
   Round[3] += 300;
   if(Y>14)
     { if(X<20)  Round[1] += 70;
       else if(X>20)  Round[0] += 70;
       else      Round[3] += 150;
     }
   else
     { if(X<20)  Round[1] += 25;
       else if(X>20)  Round[0] += 25;
       else      Round[3] += 75;
     }
   if(Y==26)
     if(X<20)  Round[1] += 300;
     else      Round[0] += 300;
   if(Road[Y][X]==4)     hide -= 25;
   if(Road[Y+1][X]==4)   hide -= 25;
   if(Road[Y][X+1]==4)   hide -= 25;
   if(Road[Y+1][X+1]==4) hide -= 25;
   if(hide)
    { if(abs(mytankX-X)<2)
	if(mytankX==X)
	 { if(mytankY>Y) Round[3] += hide*3;
	   else          Round[2] += hide*3;
	 }
	else 
	 { if(mytankY>Y) Round[3] += hide;
	   else          Round[2] += hide;
	 }
      if(abs(mytankY-Y)<2)
	if(mytankY==Y)
	 { if(mytankX<X) Round[0] += hide*3;
	   else          Round[1] += hide*3;
	 }
	else 
	 { if(mytankX<X) Round[0] += hide;
	   else          Round[1] += hide;
	 }
    }
   if(Tankp->istop==0) switch(round)
    { case 0: if(X!=34) { Roadt(Y,X+2); Roadt(Y+1,X+2); } break;
      case 1: if(X!=0)  { Roadt(Y,X-1); Roadt(Y+1,X-1); } break;
      case 2: if(Y!=26) { Roadt(Y+2,X); Roadt(Y+2,X+1); } break;
      case 3: if(Y!=0)  { Roadt(Y-1,X); Roadt(Y-1,X+1); } break;
    }

   if(Tankp->spendflag==1)
     { for(k=0; k<4; k++)  Round[k] = 0;
       Tankp->spendflag = 0;  Round[round] = 100;
     }
   if(X==0)  Round[0] = 0;  else   if(X==34)  Round[1] = 0;
   if(Y==0)  Round[2] = 0;  else   if(Y==26)  Round[3] = 0;
   
   Sround[0] = Round[0];  Sround[1] = Sround[0]+Round[1];
   Sround[2] = Sround[1]+Round[2];  Sround[3] = Sround[2]+Round[3];
   if(Sround[3]==0)  { Tankp->istop=1; Tankp->ibstop=1; return; }
   Rand = random(Sround[3]);
   if(Rand<Sround[0])   Tankp->round = 0;
   else if(Rand<Sround[1])   Tankp->round = 1;
   else if(Rand<Sround[2])   Tankp->round = 2;
   else                      Tankp->round = 3;

   if(Tankp->iship==1)   Roadattrib[3].istoptank = NO;
   switch(Tankp->round)
    { case 0: if(Roadattrib[Road[Y][X-1]&0x0f].istoptank==YES  ||
		Roadattrib[Road[Y+1][X-1]&0x0f].istoptank==YES  ||
		  Roadtank[Y][X-1]!=NOTANK || Roadtank[Y+1][X-1]!=NOTANK )
		    flag = 1;  break;
      case 1: if(Roadattrib[Road[Y][X+2]&0x0f].istoptank==YES  ||
		Roadattrib[Road[Y+1][X+2]&0x0f].istoptank==YES  ||
		  Roadtank[Y][X+2]!=NOTANK || Roadtank[Y+1][X+2]!=NOTANK )
		    flag = 1;  break;
      case 2: if(Roadattrib[Road[Y-1][X]&0x0f].istoptank==YES  ||
		Roadattrib[Road[Y-1][X+1]&0x0f].istoptank==YES  ||
		  Roadtank[Y-1][X]!=NOTANK || Roadtank[Y-1][X+1]!=NOTANK )
		    flag = 1;  break;
      case 3: if(Roadattrib[Road[Y+2][X]&0x0f].istoptank==YES  ||
		Roadattrib[Road[Y+2][X+1]&0x0f].istoptank==YES  ||
		  Roadtank[Y+2][X]!=NOTANK || Roadtank[Y+2][X+1]!=NOTANK )
		    flag = 1;  break;
    }

   if(Tankp->iship==1)   Roadattrib[3].istoptank = YES;
   if(flag==1)
     { Tankp->istop = 1;
       if(random(4)==0) Tankp->round = random(4);
       if(Tankp->ibstop==0)  switch(round)
	{ case 0: if(X!=34) { Roadt(Y,X+2); Roadt(Y+1,X+2); } break;
	  case 1: if(X!=0)  { Roadt(Y,X-1); Roadt(Y+1,X-1); } break;
	  case 2: if(Y!=26) { Roadt(Y+2,X); Roadt(Y+2,X+1); } break;
	  case 3: if(Y!=0)  { Roadt(Y-1,X); Roadt(Y-1,X+1); } break;
	}  Tankp->ibstop = 1;   return;
     }
   switch(Tankp->round)
    { case 1: Roadtank[Y][X+2]=Num; Roadtank[Y+1][X+2]=Num; break;
      case 0: Roadtank[Y][X-1]=Num; Roadtank[Y+1][X-1]=Num; break;
      case 2: Roadtank[Y-1][X]=Num; Roadtank[Y-1][X+1]=Num; break;
      case 3: Roadtank[Y+2][X]=Num; Roadtank[Y+2][X+1]=Num; break;
    }
   Tankp->istop = 0;  Tankp->stop = 0;   Tankp->ibstop = 0;  Tankp->ipause = 0;
 }

void  tank_appear()
 { int  k, x, Rand;  static char  flag;
   if(hadtankappear>=dtank)  return;
   if(nowtankappear>4 && random(nowtankappear)!=0)   return;
   if(nowtankappear>=P.maxtank)   return;
   x = random(3)*17 ;
   if(Roadtank[1][x]!=NOTANK || Roadtank[1][x+1]!=NOTANK)
    { for(k=0; k<2; k++)
       { if(++flag==3)  flag = 0;    x = flag*17;
	 if(Roadtank[1][x]==NOTANK && Roadtank[1][x+1]==NOTANK)  break;
       }
      if(k==2)  return;
    }
   for(k=0; k<10; k++)  if(Tank[k].life==0)  break;
   if(k==10)  return;
   Tank[k].X = x;  Tank[k].Y = 0; Tank[k].ienemy = 1;
   Tank[k].state = 7;  Tank[k].statetime = 0;  Tank[k].spendflag = 0;
   Rand = random(6);
   if(Rand<2)  Tank[k].life = 1;
   else        Tank[k].life = Rand-1;
   Tank[k].blife = Tank[k].life;  Tank[k].iship = Tank[k].ibship = 0;
   Tank[k].dxy = 0;  Tank[k].round = 3;   Tank[k].ibstop = 0;
   Tank[k].ipause = 0;
   nowtankappear++;   hadtankappear++;   settankza(k);
   if(!random(3))  Tank[k].eats = random(Tank[k].life+1);
   Tank[k].addjust = 30;  Tank[k].speed = random(3)?1:2;
   Tank[k].shotspeed = random(P.enshotspeed+1);
   if(pause&&pauseflag==0)
    { char str[20];  sprintf(str, "Pause - %d/100", pause*10/_pausetime);
      add_showfs_fortank(Tank+k, str);
    }
 }


int  findtankfile(int flag0)
 { char path[50], filename[50];
   int k, _flag, f=0;  static int flag;
   sprintf(path, "stage\\%s\\*.*", wave);
   if(flag0==1)
     { N = findfile(path, file, 0, 88);
       for(k=0; k<N; k++)
	 if(_strcmp(stage, file[k])==0)
	   { sprintf(filename, "stage\\%s\\%s", wave, stage);
	     if(chestfile(filename))  { flag = ++k;  return 1; }
	     else break;
	   }
       if(k==N)  return 0;
     }
   do
    { if(flag>=N)  { flag = 0; if(++f==2)  return 0; }
      sprintf(filename, "stage\\%s\\%s", path, file[_flag=flag++]);
    } while(chestfile(filename)==1);
   strcpy(stage, file[_flag]);
   return 1;
 }


int  findfile(char *path, char file[][14], char attrib, int max)
 { int k=0, j, i;  struct ffblk Ffblk;  char str[20], ch;
   if(max==0||findfirst(path, &Ffblk, attrib)==-1)  return 0;
   do
    { if(Ffblk.ff_name[0]=='.')  continue;
      if(attrib&FA_DIREC)  if((Ffblk.ff_attrib&FA_DIREC)==0)  continue;      
      strcpy(file[k], Ffblk.ff_name);   file[k++][13] = Ffblk.ff_attrib;
      if(k==max)  break;
    } while(findnext(&Ffblk)!=-1);
   for(j=0; j<k; j++)  for(i=j+1; i<k; i++)
      if(_strcmp(file[i], file[j])<0)
	{ strcpy(str,file[i]); strcpy(file[i],file[j]);strcpy(file[j],str);
	  ch=file[i][13]; file[i][13]=file[j][13]; file[j][13]=ch;
	}
   return k;
 }

void sethz24()
 { sethz_round(X_ROUND);   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_dot(24);          sethz_speed(0);
   sethz_file(hzk24fp);    sethz_color(LIGHTGRAY);
   sethz_fx(Y_ROUND);      sethz_dxy(10);
   sethz_zoomxy(2, 1);
 }

void sethz16()
 { sethz_round(X_ROUND);   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_dot(16);          sethz_speed(0);
   sethz_file(hzk16fp);    sethz_color(LIGHTGRAY);
   sethz_fx(X_ROUND);      sethz_dxy(0);
   sethz_zoomxy(2, 1);
 }

int  findzj()
 { int k, m, n;
   char dirname[12][14];   char  filename[89][14];  char path[50];
   n = findfile("stage\\*.*", dirname, FA_DIREC, 12);
   if(n==0) { strcpy(wave, "NONE");  strcpy(stage, "NONE"); N=0; return 0; }
   if(*wave)
    { sprintf(path, "stage\\%s\\*.*", wave);
      m = findfile(path, filename, 0, 88);
      if(m==0)  goto find_;
      if(*stage)
       { sprintf(path, "stage\\%s\\%s", wave, stage);
	 if(_chmod(path, 0)==-1)  strcpy(stage, filename[0]);
       }
      else strcpy(stage, filename[0]);
      N = m; return 1;
    }
   else find_:
    { for(k=0; k<n; k++)
       { sprintf(path, "stage\\%s\\*.*", dirname[k]);
	 m = findfile(path, filename, 0, 88);
	 if(m!=0)  break;
       }
      if(k==n) { strcpy(wave, "NONE");  strcpy(stage, "NONE"); N=0; return 0; }
      strcpy(wave, dirname[k]); strcpy(stage, filename[0]); N = m; return 1;
    }
 }

//*******************************************************************

int  text_wide(char *S)
 { int len, k, lon=0;
   char str[2]={0,0};
   len = strlen(S);
   for(k=0; k<len; k++)
     { str[0] = S[k];
       lon += textwidth(str);
     }
   return lon;
 }


void waitforclick()
 { while(getmousebutton()!=NOBUTTON);
   while(!kbhit()&&getmousebutton()==NOBUTTON);
   while(kbhit())  getch();   while(getmousebutton()!=NOBUTTON);
 }

void editroad(int code, char *filename)
 { int   roadkind = 0; char path[30], saveflag=0;
   int   i, j, k, dx, dy;  char bRoad[28][36], Rou[][6]={"LEFT","RIGHT","UP","DOWN"};
   mouse_off();   write_screen();
   sethz24();  sethz_round(Y_ROUND);  sethz_color(YELLOW);  sethz_dxy(7);
   writehzs(610, 80, 126, 129);
   sethz_round(X_ROUND);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)
     {  showico(i,j,Road[j][i]);  bRoad[j][i] = Road[j][i]; }
   setcolor(YELLOW);  setlinestyle(0,0,3);
   for(i=0; i<6; i++)
     { showico(37, 10+i*2, i); rectangle(590, 163+i*32, 625, 189+i*32); }
   setcolor(WHITE);  setlinestyle(1, 0, 3);
   showico(37, 10+7*2, 0); rectangle(590, 163+7*32, 625, 189+7*32);
   setlinestyle(0,0,1);  sethz_color(YELLOW);
   sethz_zoomxy(1, 1);   sethz_dxy(0);
   for(i=0; i<4+code; i++)    writehzs(100+i*70, 468, 130+i*2, 131+i*2);
   settextstyle(1, 0, 1);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW); sprintf(path, "%s->%s", wave, stage);
   outtextxy(510, 465, path);  setcolor(RED); outtextxy(512, 467, path);
   settextstyle(2, 0, 4);  setcolor(YELLOW);
   outtextxy(600, 420, "Use:");  setcolor(WHITE);
   for(j=0; j<4; j++)   outtextxy(615, 430+j*10, Rou[j]);
   setfillstyle(1, YELLOW);
   for(j=0; j<35; j+=17)   bar(9+j*16, 2, 9+(j+2)*16-3, 5);
   for(j=0;j<2;j++)  for(i=0;i<2;i++)
      { Road[j+26][i+17] = 5;  showico(i+17, j+26, ((j*2+i)<<4)|6); }
   mouse_on();
BEGIN
   setwritemode(XOR_PUT);  setlinestyle(1, 0, 1);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)
     { int flag = 0;
       while(imouse_inbox(LEFT+(i<<4), TOP+(j<<4), LEFT+(i<<4)+16, TOP+(j<<4)+16))
	 { int x1=LEFT+16*i, y1=TOP+j*16, x2=LEFT+16*i+16, y2=TOP+j*16+16;
	   if(kbhit())  break;
	   if(flag==0)
	    { setcolor(YELLOW);   mouse_off();
	      rectangle(x1,y1,x2,y2);  flag=1; mouse_on();
	    }
	   if(getmousebutton()==RIGHTBUTTON)
	    { if((Road[j][i]&0x0f)!=6)
	       { roadkind = Road[j][i];
		 showico(37, 10+7*2, roadkind);
	       }
	      while(getmousebutton()!=NOBUTTON);
	    }
	   if(getmousebutton()==LEFTBUTTON)
	    { int X=i, Y=j, x0, y0, _x1=x1, _y1=y1;
	      int X1, Y1, X2, Y2, X0=X, Y0=Y, flag0=0;
	      while(getmousebutton()==LEFTBUTTON)
	       { if(kbhit()&&getch()==ESC_KEY)  { flag0=1; break; }
		 getmouse_xy(&x0, &y0);
		 X0=(x0-LEFT)/16; Y0=(y0-TOP)/16;
		 if(X0>35)  X0 = 35;  if(Y0>27)  Y0 = 27;
		 if(X0!=X||Y0!=Y)
		  { X1=x1+8; Y1=y1+8; X2=_x1+8; Y2=_y1+8;
		    mouse_off();
		    if(X1<X2){X1-=8;X2+=8;} else{X2-=8;X1+=8;}
		    if(Y1<Y2){Y1-=8;Y2+=8;} else{Y2-=8;Y1+=8;}
		    rectangle(X1, Y1, X2, Y2);
		    _x1=LEFT+(X0<<4); _y1=TOP+(Y0<<4);
		    X1=x1+8; Y1=y1+8; X2=_x1+8; Y2=_y1+8;
		    if(X1<X2){X1-=8;X2+=8;} else{X2-=8;X1+=8;}
		    if(Y1<Y2){Y1-=8;Y2+=8;} else{Y2-=8;Y1+=8;}
		    rectangle(X1, Y1, X2, Y2);
		    X = X0;  Y = Y0;
		    mouse_on();
		  }
	       }
	     X1=x1+8; Y1=y1+8; X2=_x1+8; Y2=_y1+8;
	     if(X1<X2){X1-=8;X2+=8;} else{X2-=8;X1+=8;}
	     if(Y1<Y2){Y1-=8;Y2+=8;} else{Y2-=8;Y1+=8;}
	     if(i<X)  { int t; t=i; i=X; X=t; }
	     if(j<Y)  { int t; t=j; j=Y; Y=t; }
	     mouse_off(); rectangle(X1, Y1, X2, Y2);
	     if(flag0==0)
	      { for(y0=0;y0<28;y0++) for(x0=0;x0<36;x0++)
		  bRoad[y0][x0] = Road[y0][x0];
		for(y0=Y;y0<=j;y0++)  for(x0=X;x0<=i;x0++)
		   { Road[y0][x0] = (bioskey(2)&0x08)?5:roadkind;
		     showico(x0, y0, Road[y0][x0]);
		   }
		for(j=0;j<2;j++)  for(i=0;i<2;i++)
		   showico(i+17, j+26, ((j*2+i)<<4)|6);
		saveflag ++;
	       }
	     flag = 0;  mouse_on();
	     while(getmousebutton()!=NOBUTTON);
	    }
	 }
	if(flag==1)
	 { setcolor(YELLOW);   mouse_off();
	   rectangle(LEFT+(i<<4), TOP+(j<<4), LEFT+(i<<4)+16, TOP+(j<<4)+16);
	   mouse_on();  flag=0;
	 }
     }
   setwritemode(COPY_PUT);  setlinestyle(0, 0, 1);

   for(k=0; k<4; k++)
    { int flag = 0, jep = 0;
      while(imouse_inbox(615-20, 430+10*k-5, 615+20, 430+10*k+5))
       { if(kbhit())  break;
	 if(flag==0)
	   { settextstyle(2, 0, 4);  setcolor(LIGHTRED);
	     settextjustify(CENTER_TEXT, CENTER_TEXT);
	     off(); outtextxy(615, 430+10*k, Rou[k]); on();
	     flag = 1;
	   }
	 if(getmousebutton()==LEFTBUTTON)
	   { while(getmousebutton()!=NOBUTTON);
	     if(!imouse_inbox(615-20, 430+10*k-5, 615+20, 430+10*k+5))  continue;
	     dx = dy = 0;
	     switch(k) 
	      { case 0: dx = 1; break;    case 1: dx = -1; break;
		case 2: dy = 1; break;    case 3: dy = -1; break;
	      }
	     jep = 1;  break;
	   }
       }
      if(flag==1)
	{ setcolor(WHITE);  
	  off(); outtextxy(615, 430+10*k, Rou[k]); on(); 
	  flag = 0;
	}
      if(jep==1)  goto move;
    }
   
   for(k=0; k<6; k++)
     { int flag = 0;
       while(imouse_inbox(590, 163+k*32, 625, 189+k*32))
	 { if(kbhit())  break;
	   if(flag == 0)
	     { setlinestyle(0,0,3);  setcolor(BLUE);   mouse_off();
	       rectangle(590, 163+k*32, 625, 189+k*32);
	       flag = 1;  mouse_on();  setlinestyle(0,0,1);
	     }
	   if(getmousebutton()!=NOBUTTON)
	    { while(getmousebutton()!=NOBUTTON);
	      if(bioskey(2)&0x04)
	       { for(j=0; j<28; j++)  for(i=0; i<36; i++)
		  { bRoad[j][i] = Road[j][i];
		    if(Road[j][i]==roadkind) 
		      { Road[j][i] = k;  showico(i, j, k); }
		  }	
		 for(j=0;j<2;j++)  for(i=0;i<2;i++)
		   showico(i+17, j+26, ((j*2+i)<<4)|6);
		 saveflag ++;
	       }
	      else { roadkind = k;  showico(37, 10+7*2, roadkind); }
	    }
	 }
       if(flag==1)
	{ setlinestyle(0,0,3);  setcolor(YELLOW);   mouse_off();
	  rectangle(590, 163+k*32, 625, 189+k*32);
	  flag = 0;  mouse_on();  setlinestyle(0,0,1);
	}
     }
   for(k=0; k<4+code; k++)
    { int flag=0;
      while(imouse_inbox(76+k*70, 456, 124+k*70, 480))
       { if(kbhit())  break;
	 if(flag==0)
	  { sethz_color(GREEN);  mouse_off();
	    writehzs(100+k*70, 468, 130+k*2, 131+k*2);
	    mouse_on();  flag=1;
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(imouse_inbox(76+k*70, 456, 124+k*70, 480))
	    switch(k)
	     { case 0: if(code==1){fclose(fp);fp=fopen(filename,"wb");}
		       rewind(fp);  saveroad();
		       fclose(fp);  return;
	       case 1: if(saveflag&&!show_select(2895, 2908, 0))
			  { off();FOR(j,28)FOR(i,36)showico(i,j,Road[j][i]);on();break; }
		       fclose(fp);
		       if(code==0)
			{ char path[50];
			  sprintf(path, "stage\\%s\\%s", wave, stage);
			  unlink(path);
			}			
			return;
	       case 2: redo:  mouse_off();
		       for(j=0; j<28; j++)  for(i=0; i<36; i++)
			 { char t;
			   t = Road[j][i]; Road[j][i]=bRoad[j][i];
			   bRoad[j][i]=t; showico(i,j,Road[j][i]);
			 }
                       for(j=0;j<2;j++)  for(i=0;i<2;i++)
			  showico(i+17, j+26, ((j*2+i)<<4)|6);
		       if(saveflag==1) saveflag=0;  else saveflag ++;
		       mouse_on();  break;
	       case 3: mouse_off();
		       for(j=0; j<28; j++)  for(i=0; i<36; i++)
			 if((Road[j][i]&0x0f)!=6)
			   { bRoad[j][i] = Road[j][i];
			     Road[j][i] = 5;  showico(i,j,5);
			   }
		       for(j=0;j<2;j++)  for(i=0;i<2;i++)
			  showico(i+17, j+26, ((j*2+i)<<4)|6);
		       saveflag++;  mouse_on();  break;
	       case 4: mouse_off();
		       fseek(fp,2,SEEK_SET);
		       for(j=0; j<28; j++) for(i=0; i<36; i++) bRoad[j][i] = Road[j][i];
		       readroad();
		       for(j=0; j<28; j++) for(i=0; i<36; i++) showico(i,j,Road[j][i]);
		       for(j=0;j<2;j++)  for(i=0;i<2;i++)  showico(i+17, j+26, ((j*2+i)<<4)|6);
		       if(saveflag)saveflag++; mouse_on();  break;
	     }
	    if(k==1)  
	      { sethz24(); sethz_zoomxy(1, 1); sethz_dxy(0); 
		for(j=0;j<2;j++)  for(i=0;i<2;i++)
		   showico(i+17, j+26, ((j*2+i)<<4)|6);
	      }
	   }
       }
      if(flag==1)
       { sethz_color(YELLOW);  mouse_off();
	 writehzs(100+k*70, 468, 130+k*2, 131+k*2);
	 mouse_on();  flag=1;
       }
    }
   dx = dy = 0;
   if(kbhit()) switch(k=getch())
     { case SPACE_KEY:  goto redo;
       case LEFT_KEY:   dx = 1;
       case RIGHT_KEY: if(k==RIGHT_KEY)  dx = -1;
       case UP_KEY:    if(k==UP_KEY)     dy = 1;
       case DOWN_KEY:  if(k==DOWN_KEY)   dy = -1;
    move: 
	{ unsigned char xRoad[28][36];
	  for(j=0; j<28; j++)  for(i=0; i<36; i++)
	   { int i0=i+dx, j0=j+dy;
	     if(i0>=36)  i0 = 0;  else if(i0<=-1)  i0 = 35;
	     if(j0>=28)  j0 = 0;  else if(j0<=-1)  j0 = 27;
	     xRoad[j][i] = Road[j0][i0];
	   }
	  mouse_off();
	  for(j=0; j<28; j++)  for(i=0; i<36; i++)
	   { bRoad[j][i] = Road[j][i]; Road[j][i] = xRoad[j][i];
	     showico(i,j,Road[j][i]);
	   }
	  for(j=0;j<2;j++)  for(i=0;i<2;i++)
	     showico(i+17, j+26, ((j*2+i)<<4)|6);
	  mouse_on();
	}
     }
WEND
 }


void  showico(int x, int y, int code)
 { char far *pp = (char far*)(0xa0000000L+(8L+((long)y<<4))*80+1+(x<<1));
   char *p;  int k, j;
   unsigned char kindlow = code&0x0f, kindhigh = (unsigned char)code>>4;
   if(kindlow<=4)
     { char far *p0 = pp;
       p = Roadptr[code][0];
       for(k=0; k<4; k++)
	{ outportb(0x3c4, 2);
	  outportb(0x3c5, 0x01<<k);
	  for(j=0; j<16; j++)
	    { *p0++ = *p++;  *p0++ = *p++;  p0 += 78; }
	  p0 = pp;
	}
       outportb(0x3c5, 0x0f);
     }
   else if(kindlow==5)
     { char far *p0 = pp;
       for(k=0; k<4; k++)
	{ outportb(0x3c4, 2);
	  outportb(0x3c5, 0x01<<k);
	  for(j=0; j<16; j++)
	    { *p0++ = 0;  *p0++ = 0;  p0 += 78; }
	  p0 = pp;
	}
       outportb(0x3c5, 0x0f);
     }
   else if(kindlow==6)
    { char far *p0 = pp, *_p;
      _p = p = Headptr[0] + ((kindhigh/2)<<6) + ((kindhigh%2)<<1);
      for(k=0; k<4; k++)
       { outportb(0x3c4, 2);
	 outportb(0x3c5, 0x01<<k);
	 p = _p+k*(32*4);
	 for(j=0; j<16; j++)
	   { *p0++ = *p++;  *p0++ = *p++;  p0 += 78; p += 2; }
	 p0 = pp;
       }
      outportb(0x3c5, 0x0f);
    }
 }

void  showtank(int x, int y, int code)
 { char far *pp = (char far*)(0xa0000000L+(8L+((long)y<<4))*80+1+(x<<1));
   char *p=Tankptr[code][2][0];  int k, j;
   for(k=0; k<4; k++)
    { char *p0 = pp, *bitp = Tankbit[code][2];
      outportb(0x3c4, 2);      outportb(0x3c5, 0x01<<k);
      outportb(0x3ce, 4);      outportb(0x3cf, k);
      for(j=0; j<32; j++)
	{ *p0&=*bitp++; *p0++|=*p++;	  *p0&=*bitp++; *p0++|=*p++;
	  *p0&=*bitp++; *p0++|=*p++;	  *p0&=*bitp++; *p0++|=*p++;
	  p0 += 76;
	}
      p0 = pp;
    }
   outportb(0x3c5, 0x0f);
 }

char  down, busy; unsigned char ch;
void  setkey(int key, char down);
void  interrupt  newkey()
 {  if(busy==1)  return;   busy = 1;
    ch = inport(0x60);
    down = ch&(~0x7f)?0:1;
    switch(ch&0x7f)
     { case 0x4b: if(KEY.left!=down)   setkey(0, KEY.left=down); break;
       case 0x4d: if(KEY.right!=down)  setkey(1, KEY.right=down); break;
       case 0x48: if(KEY.up!=down)     setkey(2, KEY.up=down); break;
       case 0x50: if(KEY.down!=down)   setkey(3, KEY.down=down); break;
       case 0x39: KEY.space = down; break;
       case 0x1d: KEY.skey = down; break;
       case 0x38: KEY.akey = down; break;
       case 0x1e: KEY.akey = down; break;
       case 0x1f: KEY.skey = down; break;
       case 0x20: KEY.dkey = down; break;
       case 0x21: KEY.fkey = down; break;
       case 0x01: KEY.esc = down;  break;
       case 0x1c: if(down==1) KEY.enter = 1-KEY.enter; souflag=1; break;
       case 0x53: if((bioskey(2)&0x03) && down==1) isound = 1-isound; souflag=1; break;
       case 0x52: if(down==1)  KEY.insert = 1; break;
     }
    oldkey();   *((char far*)0x0041aL) = *((char far*)0x0041cL);
    busy = 0;
  }
void setkey(int key, char down)
 { int k;
   if(down==1)  { round[++keyptr] = key;  return; }
   for(k=0; k<=keyptr; k++)  if(round[k]==key)  break;
   if(k==keyptr+1)  return;
   for(; k<keyptr; k++)   round[k] = round[k+1];
   keyptr --;
 }

void  show_msg()
 { char str[30];
   int flag = 0;
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   setfillstyle(11, GREEN);
   bar(585, 200, 640, 230);   bar(585, 265, 640, 295);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(0, 0, 2);
   setcolor(enemytanknum?LIGHTRED:YELLOW);
   if(enemytanknum<0)  enemytanknum = 0;
   sprintf(str, "%d", enemytanknum);
   outtextxy(613, 215, str);
   setcolor(mytanknum?LIGHTRED:YELLOW);
   sprintf(str, "%d", mytanknum);
   outtextxy(613, 280, str);
   if(flag==1)  mouse_on();
 }

void  show_test()
 { int flag = 0;   char str[30];
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   settextjustify(RIGHT_TEXT, CENTER_TEXT);
   setfillstyle(11, GREEN);
   sprintf(str, "Test: %ld", Test);
   bar(430, BOTTOM+2, 549, 479);
   settextstyle(1, 0, 1);
   setcolor(YELLOW); outtextxy(550, 465, str);
   if(flag==1)  mouse_on();
   if(Test>=dTest)
    { dTest+=DTE; mytanknum++; Sound(10);
      setcolor(WHITE); outtextxy(550, 465, str);  show_msg();
      sprintf(str, "%d tanks", mytanknum);
      add_showfs_fortank(Tank+10, str);
    }
 }

void  show_star()
 { int i, n = 0, dx;
   int flag = 0;
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   switch((int)TT.iship+TT.speed+shotkind+TT.shotspeed)
    { case 9:   dx = 5; break;  case 8:  dx = 6; break;
      default:  dx = 7; break;
    }
   setfillstyle(11, BLUE);       bar(585, 395, 639, 405);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(WHITE);
   for(i=0; i<TT.iship; i++)      outtextxy(590+n++*dx, 400, "*");
   setcolor(LIGHTGREEN);
   for(i=0; i<TT.speed; i++)      outtextxy(590+n++*dx, 400, "*");
   setcolor(YELLOW);
   for(i=0; i<shotkind; i++)      outtextxy(590+n++*dx, 400, "*");
   setcolor(LIGHTRED);
   for(i=0; i<TT.shotspeed; i++)  outtextxy(590+n++*dx, 400, "*");
   if(flag==1)  mouse_on();
 }

void  show_xy()
 { static int bx, by;
   int flag = 0;
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   setfillstyle(11, GREEN);
   bar((bx<<4)+8,0,(bx<<4)+40,6);  bar(0, (by<<4)+8, 6, (by<<4)+40);
   setfillstyle(5, GREEN);  bx = mytankX;  by = mytankY;
   bar((bx<<4)+8,0,(bx<<4)+40,6);  bar(0, (by<<4)+8, 6, (by<<4)+40);
   if(flag==1)  mouse_on();
 }

char STR[20];
void  renew_screen()
 { int k, j, y;
   
   if(*MSG)
    { static int bGameover;
      if(bGameover!=Gameover/25)
	{ sprintf(STR, "%s - %d", MSG, Gameover/25);
	  bGameover = Gameover/25;
	}
    }

   for(j=0; j<_shownum; j++)
     if(++_showfs[j].dxy==20)  del_showfs(j);

   for(k=0; k<14; k++)
     { setroad(k);   
       if(*MSG)
	{ settextjustify(CENTER_TEXT, CENTER_TEXT);
	  if(k==6)
	   { settextstyle(4, 0, 4);  setcolor(endcolor);
	     outtextxy(320, 208+TOP, STR);
	   }
	  else if(k==7)
	   { settextstyle(2, 0, 6);  setcolor(CYAN);
	     outtextxy(320, 240+TOP, "--->>> Skyever <<<---");
	     setcolor(YELLOW);
	     outtextxy(322, 242+TOP, "--->>> Skyever <<<---");
	   }
	}
       for(j=0; j<_shownum; j++)
	{ y = _showfs[j].y;  settextjustify(CENTER_TEXT, CENTER_TEXT);
	  if((k<<1)==y-1 || (k<<1)==y)  appear_showfs(j);
	}
     }
   return;
 }

void  show_line(int code, int n)
 { int flag = 0;   
   settextstyle(2, 0, 4);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   if(code==0)
    { if(n>10)  n = 10;
      if(n) { setfillstyle(1, YELLOW);    bar(587,370,587+n*6,375); }
      setfillstyle(1, GREEN);             bar(587+n*6, 370, 637, 375);
      setcolor(LIGHTCYAN); outtextxy( (587+637)/2, (370+375)/2, "PAUSE");
    }
   else if(code==1)
    { if(n>10)  n = 10;
      if(n) { setfillstyle(1, YELLOW);    bar(587,360,587+n*6,365); }
      setfillstyle(1, RED);               bar(587+n*6, 360, 637, 365);
      setcolor(WHITE); outtextxy( (587+637)/2, (360+365)/2, "COVERD");
    }
   else if(code==2)
    { if(n>10)  n = 10;
      if(n) { setfillstyle(1, YELLOW);    bar(587,350,587+n*6,355); }
      setfillstyle(1, BLUE);              bar(587+n*6, 350, 637, 355);
      setcolor(LIGHTGREEN); outtextxy( (587+637)/2, (350+355)/2, "PROTECTED");
    }
   else
    { if(n>10)  n = 10;
      if(n) { setfillstyle(1, YELLOW);    bar(587,380,587+n*6,385); }
      setfillstyle(1, BLUE);              bar(587+n*6, 380, 637, 385);
      setcolor(LIGHTGREEN); outtextxy( (587+637)/2, (380+385)/2, "FIRE");
    }
   if(flag==1)  mouse_on();
 }


int   test_mouseclick()
 { int k;
   static char flag[4];  char *str[] = { "SOUND", "PAUSE", "SAVE", "STOP" };
   for(k=0; k<4; k++)
    { int color = -1;
      if(getmousebutton()==LEFTBUTTON && imouse_inbox(585, 410+k*12, 640, 425+k*12-2))
	{ nosound();  while(getmousebutton()!=NOBUTTON);
	  if(imouse_inbox(585, 410+k*12, 640, 425+k*12-2))
	    if(k==0)       { isound = 1-isound; nosound(); }     //SOUND
	    else if(k==1)    return 10; 			 //PAUSE
	    else if(k==2)    return 20; 			 //SAVE
	    else             return -1;  			 //STOP
	}
      if(flag[k]==0 && imouse_inbox(585, 410+k*12, 640, 425+k*12-2)) color = YELLOW;
      else if(flag[k]==1 && !imouse_inbox(585, 410+k*12, 640, 425+k*12-2))
	color = k==0?isound?LIGHTRED:CYAN:k==1?KEY.enter?LIGHTRED:CYAN:CYAN;
      if(color==-1)  continue;
      settextstyle(2, 0, 4); settextjustify(CENTER_TEXT, CENTER_TEXT);
      setcolor(color);  mouse_off();
      outtextxy(613, 415+k*12, str[k]);
      rectangle(613-25, 415+12*k-5, 613+25, 415+12*k+5);
      mouse_on();
      flag[k] = color==YELLOW?1:0;
    }
   return 0;
 }


void  showfs()
 { char  str[10];
   setfillstyle(11, GREEN);   bar(587, 290, 615, 342);
   settextstyle(2, 0, 5);  setcolor(WHITE);
   settextjustify(RIGHT_TEXT, TOP_TEXT);
   sprintf(str, "%d", fs0);
   outtextxy(615, 298, str);
   sprintf(str, "%d", fs1);
   outtextxy(615, 330, str);
 }


void tank_eat(TANKDEF *Tankp)
  { int X=Tankp->X, Y=Tankp->Y, i, j, k;
    if(Eat.kind!=NOEAT && X>=Eat.X-2 && X<=Eat.X+3 && Y>=Eat.Y-2 && Y<=Eat.Y+2)
       { Sound(4);
	 switch(Eat.kind)
	  { case 0: if((int)TT.shotspeed+shotkind+TT.speed+TT.ibship>=4 && !(tankinsea(10)&&!P.insea) && P.inbreak )
		      { if(Tank[10].shotspeed) Tank[10].shotspeed--;   
			if(!tankinsea(10))     Tank[10].iship = Tank[10].ibship = 0;
			else                   Tank[10].ibship = 0;
			if(shotkind)  shotkind--; if(Tank[10].speed!=1)  Tank[10].speed--;
			Sound(5); show_star();  Tankp->state=3;  Tankp->life=1;
		      }
		    else
		      { Tank[10].state = 2;
			for(j=0; j<28; j++)  for(i=0; i<36; i++)
			  if(Roadtank[j][i]==10)   Roadtank[j][i] = NOTANK;
			Sound(6); if(--mytanknum<=0) { Gameover--; result=LOOSE; msg_show("GAME OVER!"); } show_msg(); 
		      }
		    add_showfs_fortank(Tank+10, "Bomb!");
		    break;
	    case 1: if(Tank[10].life && Tank[10].state==3 && Tank[10].addjust==0)
		      { pause = _pausetime*4+1; pauseflag=1; }
		    add_showfs_fortank(Tank+10, "Pause...");
		    break;
	    case 2: for(j=0; j<3; j++)   for(i=0; i<4; i++)
		      if((Road[25+j][16+i]&0x0f)!=6)  Road[25+j][16+i]=5;
		    iron = 0;  show_line(1, 0);
		    ADDHEAD("SOS!!");
		    break;
	    case 3: k = -1; while(++k<10 && Tank[k].life);
		    if(k==10)  break;
		    nowtankappear++; hadtankappear++; dtank++; enemytanknum++;
		    Tank[k].blife=Tank[k].life=4; Tank[k].speed=2; Tank[k].shotspeed=2;
		    Tank[k].state=7; Tank[k].addjust=30; Tank[k].statetime = 0;
		    j = random(3);
		    for(i=0; i<3; i++)
		      { int x;  if(++j==3) j=0;  x=j*17;
			if(Roadtank[1][x]==NOTANK && Roadtank[1][x+1]==NOTANK)  break;
		      }
		    if(i==3)  j = 1;  Tank[k].X = 17*j; Tank[k].Y = 0;
		    add_showfs_fortank(Tank+k, "Appear!!");
		    show_msg();  break;
	    case 4: Tankp->addjust = _addjusttime*4+1;
		    add_showfs_fortank(Tankp, "Protected!");
		    break;
	    case 5: Tankp->shotspeed=2;  Tankp->speed=2;
		    add_showfs_fortank(Tankp, "Speed+");
		    break;
	    case 6: if(Tankp->shotspeed!=2) Tankp->shotspeed=1;
		    Tankp->speed=2;  add_showfs_fortank(Tankp, "Speed+");
		    break;
	    case 7: for(k=0; k<10; k++)
		     { if((Tank+k)->life==0 || Tank[k].state<3)  continue;
		       (Tank+k)->iship = (Tank+k)->ibship = 1;
		       (Tank+k)->dxy=0;
		       add_showfs_fortank(Tank+k, "Ship+");
		     } break;
	    case 8: fire = _firetime*4+1;  fireflag=1;
		    if(Tank[10].life && Tank[10].state==3)  tankfire(10);
		    add_showfs_fortank(Tankp, "Fire!");
		    break;
	  }
	 Eat.kind = NOEAT;
      }
 }


void  setalltime()
 { _shotgo = (4-P.shothz) * 4 + 16;
   _pausetime = 30+25*P.pausetime;
   _firetime  = 50+25*P.firetime;
   _irontime  = 90+30*P.irontime;
   _addjusttime = 30+20*P.addjusttime;
   DTE = 10000 + P.fs*5000;
 }

int  tankfire(int N)
 { TANKDEF *Tankp = Tank+N;  int i, j;
   if(Tankp->addjust)  return 0;
   if(Tankp->life==0 || Tankp->state!=3)  return 0;
   if(fire==0)  return 0;
   if(fireflag==0)
     { if(Tankp->ienemy==0)  return 0;
       if(!tankintree(N))    return 0;
       Tankp->state = 2;  enemytanknum --;   show_msg();
       if(enemytanknum<=0)  { Gameover--; if(result==WIN) msg_show("THE END!");}
       for(j=0; j<28; j++)  for(i=0; i<36; i++)
	 if(Roadtank[j][i]==N)  Roadtank[j][i] = NOTANK;
       add_showfs_fortank(Tankp, "Fire!");
       Sound(5);  return 1;
     }
   else
     { if(Tankp->ienemy==1)  return 0;
       if(!tankintree(N))    return 0;
       if(--mytanknum<=0)
	  { Gameover--; result=LOOSE; msg_show("GAME OVER!"); }
       Tankp->state = 2;  show_msg();  Sound(15);
       for(j=0; j<28; j++)  for(i=0; i<36; i++)
	 if(Roadtank[j][i]==N)  Roadtank[j][i] = NOTANK;
       add_showfs_fortank(Tankp, "Fire!");
       return 1;
     }
 }

