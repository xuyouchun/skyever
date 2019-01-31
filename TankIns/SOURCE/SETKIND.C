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
#include "tools.h"
#include "hz.h"
#include "screen.h"
#include "extern.h"
#include "tank.h"
#include <key.h>
#include <mouse.h>

extern char kindflag;
extern FILE *fp;
void changezd(int code);

#define BARCOLOR  DARKGRAY
void  linebar(int k)
 { int j;
   if(k==-1)  return;
   setwritemode(XOR_PUT);  setlinestyle(0, 0, 3);
   mouse_off();  setcolor(BARCOLOR);
   for(j=0; j<11; j++)
     line(0, 86+k*40+j*3, 639, 86+k*40+j*3);
   mouse_on();
   setlinestyle(0, 0, 1);  setwritemode(COPY_PUT);
 }

#define HZB  2638
#define Pp   (playkind[i])
extern  void  renewbar(int code);  
char  levelflag;
void  far  edkind()
 { void renewkindscreen(int);
   int X, Y, bX, bY, keyflag=0;
   int i, j, k, bkindflag = kindflag;
   char zm[2]={0,0}, ch, ptr=-1;
   unsigned char key;
   KINDDEF  bkind[26];
   for(k=0; k<26; k++)  bkind[k] = playkind[k];
   mouse_off();  cleardevice();  mouse_on();
   renewkindscreen(0);
   getmouse_xy(&X, &Y);  bX = X;  bY = Y;
BEGIN
   getmouse_xy(&X, &Y);
   if(X!=bX || Y!=bY)
     { bX = X; bY = Y;
       if(keyflag) { linebar(ptr);  keyflag = 0; }
     }
   if(keyflag==1)  goto kbh;
   settextstyle(1, 0, 4);  settextjustify(CENTER_TEXT, TOP_TEXT);
   for(k=0; k<26; k++)
    { int flag=0;
      while(imouse_inbox(40+k*22-11, 450, 40+k*22+11, 477))
	{ if(kbhit())  break;
	  if(flag==0)
	   { zm[0] = 'A'+k;  mouse_off();  setcolor(LIGHTRED);
	     outtextxy(40+k*22, 450, zm);  flag = 1;  mouse_on();
	   }
	  if(getmousebutton()==LEFTBUTTON)
	    { while(getmousebutton()!=NOBUTTON);
	      if(imouse_inbox(40+k*22-11, 450, 40+k*22+11, 477))
		{ kindflag = k; renewkindscreen(1); break; }
	    }
	}
      if(flag==1)
	{ zm[0] = 'A'+k;  mouse_off();  setcolor(LIGHTBLUE);
	  outtextxy(40+k*22, 450, zm);  flag = 0;  mouse_on();
	}
    }
   sethz_zoomxy(2, 1);
   for(k=0; k<5; k++)
    { int flag = 0;
      int begin = HZB+89+k*2, end = HZB+89+k*2+1;
      if(k==4&&levelflag)  { begin+=2; end+=2; }
      while(imouse_inbox(210+85*k, 20, 275+85*k, 36))
       { if(kbhit())   break;
	 if(flag==0)
	   { sethz_color(LIGHTRED);  mouse_off();
	     writehzs(210+85*k, 20, begin, end);
	     mouse_on();  flag = 1;
	   }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(imouse_inbox(210+85*k, 20, 275+85*k, 36)) switch(k)
	     { case 0: savekind:
		       fp = fopen("msg\\kindsave", "wb");
		       if(fp==NULL)
			 { show_select(2920, 2933);
			   renewkindscreen(0);  break; 
			 }
		       for(i=0; i<26; i++)
			 { int N = sizeof(KINDDEF), k;
			   unsigned char *ptr = (unsigned char*)&playkind[i];
			   for(k=0; k<N; k++)   fputc(ptr[k], fp);
			 }
		       fclose(fp);  show_select(2909, 2919);
		       renewkindscreen(0);    goto break_;
	       case 1: return;
	       case 2: for(k=0; k<26; k++)  playkind[k] = bkind[k];
		       kindflag = bkindflag;  return;
	       case 3: new_: playkind[kindflag] = bkind[kindflag]; renewkindscreen(1); goto break_;
	       case 4: levelflag = 1-levelflag; renewkindscreen(0); goto break__;
	     }
	  }
       } break_:;
     if(flag==1)
      { sethz_color(YELLOW);  mouse_off();  sethz_zoomxy(2, 1);
	writehzs(210+85*k, 20, begin, end);
	mouse_on();  flag = 0;
      }
    } break__:;
   if(keyflag==1)  goto kbh;
   ptr = -1;
   for(k=0; k<9; k++)
    { int flag = 0;  long i, L=100000L;  
      while(imouse_inbox(0, 85+k*40, 639, 125+k*40))
       { if(kbhit())  break;
	 if(flag==0)  {  flag = 1;  linebar(k);  }
	 ptr = k;
	 if(getmousebutton()==LEFTBUTTON)
	   { leftchange_: if(levelflag==0) switch(k)
	      { case 0: if(P.mynum++==100)  P.mynum = 1; break;
		case 1: if(P.ennum++==100)  P.ennum = 1; break;
		case 2: if(P.myshotkind++==3) P.myshotkind = 0; break;
		case 3: if(P.myshotspeed++==3) P.myshotspeed = 0; break;
		case 4: P.iappear = 1-P.iappear; break;
		case 5: P.isea = 1-P.isea; break;
		case 6: P.tankspeed = 1- P.tankspeed; break;
		case 8: if(P.enshotspeed++==3) P.enshotspeed = 0; break;
		case 7: P.ieneat = 1-P.ieneat; break;
	      } 
	     else  switch(k)
	      { case 0: if(P.shothz++==4)  P.shothz=0; break;
		case 1: if(P.pausetime++==4)  P.pausetime=0; break;
		case 2: if(P.firetime++==4)   P.firetime=0; break;
		case 3: if(P.irontime++==4)  P.irontime=0; break;
		case 4: if(P.addjusttime++==4)  P.addjusttime=0; break;
		case 5: P.inbreak = 1-P.inbreak; break;
		case 6: P.insea = 1-P.insea; break;
		case 8: if(P.fs++==4)  P.fs=0; break;
		case 7: if(P.maxtank++==10)  P.maxtank = 1; break;
	      } renew_:
	     sethz_color(BARCOLOR^YELLOW);  setcolor(BARCOLOR^YELLOW);  setfillstyle(1, BARCOLOR);
	     renewbar(k);
	     for(i=0; i<L; i++) if(getmousebutton()==NOBUTTON) break;  L = 10000L;
	   }
	 else if(getmousebutton()==RIGHTBUTTON)
	   { rightchange_: if(levelflag==0) switch(k)
	      { case 0: if(P.mynum--==1)  P.mynum = 100; break;
		case 1: if(P.ennum--==1)  P.ennum = 100; break;
		case 2: if(P.myshotkind--==0) P.myshotkind = 3; break;
		case 3: if(P.myshotspeed--==0) P.myshotspeed = 3; break;
		case 8: if(P.enshotspeed--==0) P.enshotspeed = 3; break;
		default: goto leftchange_;
	      }   
	     else switch(k)
	      { case 0: if(P.shothz--==0)  P.shothz=4; break;
		case 1: if(P.pausetime--==0)  P.pausetime=4; break;
		case 2: if(P.firetime--==0)   P.firetime=4; break;
		case 3: if(P.irontime--==0)  P.irontime=4; break;
		case 4: if(P.addjusttime--==0)  P.addjusttime=4; break;
		case 8: if(P.fs--==0)  P.fs=3; break;
		case 7: if(P.maxtank--==1)  P.maxtank = 10; break;
		default: goto leftchange_;
	      } goto renew_;
	   }
	else if(getmousebutton()==NOBUTTON)  L = 100000L;
        if(keyflag==1)  goto kbh;
       }
     if(flag==1)  exit_:  linebar(k);
    }
   if(imouse_inbox(50, 60, 50+12*16, 60+16))
    { void  resetkind();
      int flag = 0;  sethz_zoomxy(1, 1);  sethz_dxy(0);
      while(imouse_inbox(50, 60, 50+12*16, 60+16))
       { if(kbhit())  break;
	 if(flag==0)
	   { mouse_off();  sethz_color(LIGHTRED);
	     writehzs(50, 60, HZB+113, HZB+124);
	     flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON && kindflag<=15)
	   { while(getmousebutton()!=NOBUTTON);
	     if(imouse_inbox(50, 60, 50+12*16, 60+16))
	       { if(bioskey(2)&0x03)
		  { int i, kind=kindflag;
		    for(i=0;i<=15;i++) { kindflag=i; resetkind(); }
		    kindflag = kind;  renewkindscreen(1);
		  }
		 else { resetkind(); renewkindscreen(1); }
		 break;
	       }
	   }
       }
      if(flag==1)
	{ mouse_off();  sethz_color(LIGHTBLUE);  sethz_dxy(0);
	  writehzs(50, 60, HZB+113, HZB+124);
	  flag = 0;  mouse_on();
	}
      sethz_dxy(5);
    }
  kbh:
   if(kbhit())  switch(ch=getch())
     { case ENTER_KEY: return;
       case ESC_KEY: for(k=0; k<26; k++)  playkind[k] = bkind[k];
		       kindflag = bkindflag;  return;
       case TAB_KEY: tab:if(kindflag++==25)  kindflag = 0;
		   if(keyflag==1) { linebar(ptr); renewkindscreen(1); linebar(ptr); }
		   else  renewkindscreen(1);  break;
       case 0: switch(ch=getch())
	       { case UP_KEY:  case DOWN_KEY: case HOME_KEY: case END_KEY:
		     if(ptr>=0 && keyflag==1)  linebar(ptr);
		     if(ch==UP_KEY)         { if(--ptr<0)  ptr = 8; }
		     else if(ch==DOWN_KEY)  { if(++ptr>8)  ptr = 0; }
		     else if(ch==HOME_KEY)  ptr = 0;
		     else ptr = 8;
		     linebar(ptr);
		     getmouse_xy(&X, &Y); bX = X; bY = Y;
		     keyflag = 1;    break;
		 case LEFT_KEY: case RIGHT_KEY:
		      if(ptr==-1)  break;
		      k = ptr;
		      if(keyflag==1)  linebar(ptr);  linebar(ptr);
		      keyflag = 1;
		      if(ch==LEFT_KEY)  goto rightchange_;
		      else              goto leftchange_;
		 case PAGEUP_KEY: case PAGEDOWN_KEY:
		     levelflag = 1-levelflag; renewkindscreen(0);
		     if(keyflag==1)  linebar(ptr); goto break__;
		 case F2_KEY: goto savekind;
		 case F3_KEY: goto new_;
	       } break;
       case BACKSPACE_KEY: back:if(kindflag--==0)  kindflag = 25;
		if(keyflag==1) { linebar(ptr); renewkindscreen(1); linebar(ptr); }
		else renewkindscreen(1);  break;
       default: if(ch<='z'&&ch>='a')   kindflag = ch-'a';
		else if(ch<='Z'&&ch>='A')  kindflag = ch-'A';
		else break;
		if(keyflag==1) { linebar(ptr); renewkindscreen(1); linebar(ptr); }
		else renewkindscreen(1);
     }
WEND
 }

void renewkindscreen(int code)
 { int k;  char str[10], zm[2]={0,0};
   int dhz[] = { 6, 6, 7, 7, 10, 9, 7, 12, 7 }, begin=0;
   int Dhz[] = { 9, 5, 5, 5, 5, 9, 13, 12, 13 };
   if(levelflag)  begin = 125;
   mouse_off();  setfillstyle(0, 0);
   if(code==0)   bar(0, 0, 639, 445);
   else { bar(320, 80, 639, 445); bar(0, 0, 200, 55); }
   sprintf(str, "Kind : %c", 'A'+kindflag);
   settextstyle(1, 0, 5);  settextjustify(LEFT_TEXT, TOP_TEXT);
   setcolor(LIGHTGREEN);   outtextxy(10, 10, str);
   sethz_round(X_ROUND);   sethz_text(LEFT_HZ, TOP_HZ);
   sethz_dot(16);          sethz_speed(0);
   sethz_file(hzk16fp);    sethz_color(LIGHTGREEN);
   sethz_fx(X_ROUND);      sethz_dxy(5);
   sethz_zoomxy(1, 2);
   if(levelflag==0)
    for(k=0; k<9; k++)
     { writehzs(50, 85+k*40, begin+HZB, begin+dhz[k]+HZB-1);
       begin += dhz[k];
     }
   else
    for(k=0; k<9; k++)
     { writehzs(50, 85+k*40, begin+HZB, begin+Dhz[k]+HZB-1);
       begin += Dhz[k];
     }
   setcolor(YELLOW);  sethz_color(YELLOW);  setfillstyle(0, 0);
   for(k=0; k<9; k++)   renewbar(k);
   sethz_zoomxy(2, 1);  sethz_color(YELLOW);
   for(k=0; k<5; k++)
    { int begin=HZB+89+k*2, end = HZB+89+k*2+1;
      if(k==4&&levelflag) { begin+=2; end+=2; }
      writehzs(210+85*k, 20, begin, end);
    }
   settextstyle(1, 0, 4);  settextjustify(CENTER_TEXT, TOP_TEXT);
   setcolor(LIGHTBLUE);
   for(k=0; k<26; k++)
    { zm[0] = 'A'+k;  outtextxy(40+k*22, 450, zm); }
   sethz_color(DARKGRAY);   sethz_zoomxy(1, 1);  sethz_dxy(0);
   writehzs(360, 55, HZB+101, HZB+112);
   sethz_color(LIGHTBLUE);  writehzs(50, 60, HZB+113, HZB+124);
   sethz_dxy(5);   mouse_on();
 }

void  renewbar(int code)
 { char str[10];   settextstyle(2, 0, 8);
   mouse_off();
   bar(320, 85+code*40, 639, 117+code*40);
   settextjustify(LEFT_TEXT, TOP_TEXT);  sethz_zoomxy(1, 2);
   if(levelflag==0) switch(code)
    { case 0: sprintf(str, "%d", P.mynum);   outtextxy(400, 85+code*40, str); break;
      case 1: sprintf(str, "%d", P.ennum);   outtextxy(400, 85+code*40, str); break;
      case 2: writehzs(400, 85+code*40, HZB+71+P.myshotkind*2, HZB+71+P.myshotkind*2+1); break;
      case 3: writehzs(400, 85+code*40, HZB+79+P.myshotspeed*2, HZB+79+P.myshotspeed*2+1); break;
      case 4: writehzs(400, 85+code*40, HZB+87+P.iappear, HZB+87+P.iappear); break;
      case 5: writehzs(400, 85+code*40, HZB+87+P.isea, HZB+87+P.isea); break;
      case 6: writehzs(400, 85+code*40, HZB+79+P.tankspeed*2, HZB+79+P.tankspeed*2+1); break;
      case 7: writehzs(400, 85+code*40, HZB+87+P.ieneat, HZB+87+P.ieneat); break;
      case 8: writehzs(400, 85+code*40, HZB+79+P.enshotspeed*2, HZB+79+P.enshotspeed*2+1); break;
    }
   else switch(code)
    { case 0: writehzs(400, 85+code*40, HZB+201+P.shothz*2, HZB+201+P.shothz*2+1); break;
      case 1: writehzs(400, 85+code*40, HZB+211+P.pausetime*2, HZB+211+P.pausetime*2+1); break;
      case 3: writehzs(400, 85+code*40, HZB+211+P.irontime*2, HZB+211+P.irontime*2+1); break;
      case 2: writehzs(400, 85+code*40, HZB+211+P.firetime*2, HZB+211+P.firetime*2+1); break;
      case 4: writehzs(400, 85+code*40, HZB+211+P.addjusttime*2, HZB+211+P.addjusttime*2+1); break;
      case 5: if(!P.inbreak) writehzs(400, 85+code*40, HZB+221, HZB+224);
	      else           writehzs(400, 85+code*40, HZB+225, HZB+232);  break;
      case 6: if(!P.insea)   writehzs(400, 85+code*40, HZB+221, HZB+224);
	      else           writehzs(400, 85+code*40, HZB+225, HZB+232);  break;
      case 8: sprintf(str, "%d", 10000+P.fs*5000); outtextxy(400, 85+code*40, str); break;
      case 7: sprintf(str, "%d", P.maxtank);  outtextxy(400, 85+code*40, str); break;
    }
   mouse_on();
 }

void  readkind(FILE *fp)
 { int i;
   for(i=0; i<26; i++)
    { int N = sizeof(KINDDEF), k;
      unsigned char *ptr = (unsigned char *)&playkind[i];
      for(k=0; k<N; k++)  ptr[k] = fgetc(fp);      
    }
 }

void  resetkind()
 { KINDDEF  bkind[] =
    { { 3, 20, 0, 0, 1, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 0, 2, 8 },     
      { 3, 20, 1, 1, 1, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 0, 2, 8 },
      { 3, 20, 2, 2, 1, 0, 1, 0, 0, 1, 2, 2, 2, 2, 1, 0, 2, 8 },      
      { 3, 20, 3, 3, 1, 0, 1, 0, 0, 1, 2, 2, 2, 2, 1, 0, 2, 8 },
      { 3, 20, 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 8 },      
      { 3, 20, 1, 1, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 8 },
      { 3, 20, 2, 2, 1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 8 },      
      { 3, 20, 3, 3, 1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 8 },
      { 3, 20, 0, 0, 1, 0, 0, 1, 0, 1, 2, 2, 2, 2, 0, 0, 2, 10 },      
      { 3, 20, 1, 1, 1, 0, 0, 1, 0, 1, 2, 2, 2, 2, 0, 0, 2, 10 },
      { 3, 20, 2, 2, 1, 0, 1, 2, 0, 1, 2, 2, 2, 2, 0, 0, 2, 10 },      
      { 3, 20, 3, 3, 1, 0, 1, 2, 0, 1, 2, 2, 2, 2, 0, 0, 2, 10 },
      { 3, 20, 0, 0, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2, 0, 1, 2, 10 },      
      { 3, 20, 1, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2, 0, 1, 2, 10 },
      { 3, 20, 2, 2, 1, 0, 1, 2, 1, 0, 2, 2, 2, 2, 0, 1, 2, 10 },      
      { 3, 20, 3, 3, 1, 0, 1, 2, 1, 0, 2, 2, 2, 2, 0, 1, 2, 10 }
    };
   playkind[kindflag] = bkind[kindflag];
 }

void  saveroad()
 { int i, j, k=0;
   if(bioskey(2)&0x04)
    { fputc('S', fp);  fputc('X', fp);
      for(j=0; j<28; j++)  for(i=0; i<36; i++)
	fputc(Road[j][i], fp);
    }
   else if(bioskey(2)&0x08)
    { fputc('S', fp);  fputc('D', fp);
      for(j=0; j<28; j++)  for(i=0; i<18; i++)
       fputc((Road[j][i*2]<<4)|Road[j][i*2+1], fp);
    }
   else if(bioskey(2)&0x03)
    { unsigned char *ptr = (unsigned char*)Road;
      fputc('S', fp);  fputc('N', fp);
      while(k<28*36)
       { unsigned char ch = ptr[k++]&0x0f, K = 0;
	 for(; k<28*36; k++)
	  { if((ptr[k]&0x0f)==ch) K ++;   else  break;
	    if(K==15)   { k++;  break; }
	  }
	 fputc(ch|(K<<4), fp);
       }
    }
   else 
    { unsigned char *ptr = (unsigned char*)Road;
      fputc('S', fp);  fputc('S', fp);
      while(k<28*36)
       { unsigned char ch = ptr[k++]&0x0f, K = 0;
	 for(; k<28*36; k++)
	  { if((ptr[k]&0x0f)==ch) K ++;   else  break;
	    if(K==31)   { k++;  break; }
	  }
	 fputc(ch|(K<<3), fp);
       }
    }
 }

void  readroad()
 { int i, j, k, ch;
   fseek(fp, 1, SEEK_SET);
   if((ch=fgetc(fp))=='X')
    { for(j=0; j<28; j++)   for(i=0; i<36; i++)
	Road[j][i] = fgetc(fp);
    }
   else if(ch=='D')
     for(j=0; j<28; j++)  for(i=0; i<18; i++)
       { Road[j][i*2] = ((ch=fgetc(fp))>>4);
	 Road[j][i*2+1] = ch&0x0f;
       }
   else if(ch=='N')
    { unsigned char *ptr = (unsigned char*)Road;  int K = 0;
      while(K<28*36)
       { unsigned char ch = fgetc(fp);
	 for(k=0; k<=(ch>>4); k++)   ptr[K++] = ch&0x0f;
       }
    }
   else 
    { unsigned char *ptr = (unsigned char*)Road;  int K = 0;
      while(K<28*36)
       { unsigned char ch = fgetc(fp);
	 for(k=0; k<=(ch>>3); k++)   ptr[K++] = ch&0x07;
       }
    }
  for(j=0;j<2;j++)  for(i=0;i<2;i++)  Road[j+26][i+17]=((j*2+i)<<4)|6;
 }

void  add_showfs(int x, int y, int dx, int dy, char *fs)
 { struct textsettingstype  setting;
   int len, left, right, d=-1;
   SHOWFS *Sp = _showfs+_shownum;
   if(_shownum>=12)  return;
   _shownum ++;

   gettextsettings(&setting);
   settextstyle(2, 0, 4);
   len = textwidth(fs);
   settextstyle(setting.font, setting.direction, setting.charsize);
   left = x*16+dx-len/2-1;  right = x*16+dx+len/2+1;
   if(left<0)    d = len/2+2;
   if(right>RIGHT-LEFT)  d = RIGHT-LEFT-len/2-2;

   if(d==-1)
     { Sp->x=x;  Sp->y=y;  Sp->dx=dx; Sp->dy=dy; Sp->dxy=-20; }
   else { Sp->x=d/16;  Sp->y=y; Sp->dx=d%16; Sp->dy=dy;  Sp->dxy=-20; }
   strcpy(Sp->fs, fs);
   return;
 }

void del_showfs(int code)
 { int k;
   if(_shownum<=code)  return;
   _shownum --;
   for(k=code; k<_shownum; k++)  _showfs[k] = _showfs[k+1];
   return;
 }

void appear_showfs(int code)
 { SHOWFS *Sp = _showfs+code;
   setcolor( Sp->dxy>16?DARKGRAY:Sp->dxy>12?LIGHTGRAY:(Sp->dxy&0x01)?YELLOW:WHITE );
   settextstyle(2, 0, 4);
   outtextxy(LEFT+(Sp->x<<4)+(Sp->dx), TOP+(Sp->y<<4)-(Sp->dxy>>1)+(Sp->dy), Sp->fs);
   return;
 }

void add_showfs_fortank(TANKDEF *Tankp, char *str)
 { if(Tankp->round<2) add_showfs(Tankp->X+1, Tankp->Y+1, Tankp->dxy, 0, str);
   else    add_showfs(Tankp->X+1, Tankp->Y+1, 0, Tankp->dxy, str);
 }

SOUNDDEF  SOUND[] =
 { { 2, { 50 , 100 }, { 1, 1 } },
   { 2, { 250, 200 }, { 1, 1 } },
   { 2, { 150, 100 }, { 1, 1 } },
   { 4, { 100, 150, 200, 150 }, { 3, 3, 3, 3 } },
   { 3, { 200, 250, 200 }, { 3, 3, 3 } },
   { 2, { 250, 150 }, { 4, 4 } },
   { 5, { 250, 200, 150, 100, 250 }, { 3, 3, 3, 3, 3 } },
   { 2, { 200, 300 }, { 2, 1 } },
   { 2, { 250, 150 }, { 2, 2 } },
   { 3, { 15, 10, 6 }, { 2, 2, 2 } },
   { 4, { 100, 150, 200, 250 }, { 2, 2, 2, 2 } },
   { 5, { 100, 50, 150, 100, 200 }, { 5, 5, 5, 5, 5 } },
   { 3, { 50, 100, 50 }, { 1, 1, 1 } },
   { 3, { 10, 15, 20 }, { 1, 1, 1 } },
   { 2, { 45, 25 }, { 1, 1 } },
   { 5, { 100, 150, 200, 250, 100 }, { 3, 3, 3, 3, 3 } }
 };

extern char isound;
struct { char code, ptr, time, time0, life; } So[2];
static char soundptr;
#define  S  So[soundptr]
void  Sound(int code)
 { void  put_sound();
   if(isound)  { nosound();  return; }
   if(code==-2)  { nosound(); So[0].life=So[1].life=0; return; }
   if(code==-1 && So[0].life==0 && So[1].life==0)  return; 
   if(code==-1)  put_sound(); 
   else 
    { soundptr = 1-soundptr;  S.code = code;
      S.ptr = 0;  S.time0 = SOUND[code].Ti[0]; sound(SOUND[code].Hz[0]*10);
      S.life = 1; S.time = 0;  put_sound(soundptr); 
    }      
 }
#define T So[k]
void  put_sound()
 { int k;
   for(k=0; k<2; k++)
    { if(T.life==0)  continue;
      if(T.time++>=T.time0)
	{ if(++(T.ptr)>=SOUND[T.code].num) 
	   { if(soundptr==k) 
	       { nosound(); soundptr=1-soundptr;   T.life = 0;
		 if(S.life==1)  sound(SOUND[S.code].Hz[S.ptr]*10); 
	       }
	   }
	  else
	   { T.time=0; T.time0=SOUND[T.code].Ti[T.ptr];
	     if(soundptr==k)  { sound(SOUND[T.code].Hz[T.ptr]*10); }
	   }
	}
    }
 }
#undef  S
#undef  T

void  start_sound()
 { int k;
   int sou[] = { 100, 150, 200, 101, 151, 201,
		   150, 200, 250, 151, 201, 251,
		   200, 250, 300, 201, 251, 301,
		   0, 0, 300, 300, 300, 0
	       } ;
   for(k=0; k<24; k++)
     { _sound(sou[k]*5-20);   delay(5);
       _sound(sou[k]*5);      delay(60);
       _sound(sou[k]*5+20);   delay(5);
	nosound();            delay((k+1)%3==0?30:60);
     }
   nosound();
 }

void  end_sound()
 { int k;
   for(k=0; k<20; k++)
    { _sound(1600);  delay(40);
      _sound(1500);  delay(40);
      _sound(1400);  delay(40);
    }
   for(k=0; k<4; k++)
    { _sound(1000+500*k-100); delay(40);
      _sound(1000+500*k);     delay(40);
      _sound(1000+500*k+100); delay(40);
      nosound();              delay(40);
    }
   nosound();
 }

#define X 540
#define Y 400
#define BO 5
void show_yl(char *name)
 { int i, j, color;
   fp = fopen(name, "rb");   
   if(fp==NULL)  return;   readroad();  fclose(fp);
   mouse_off();
   setfillstyle(11, GREEN);  bar(X-BO, Y-BO, X+2*36+BO, Y+2*28+BO);
   setcolor(LIGHTGRAY);   rectangle(X-BO-1, Y-BO-1, X+2*36+BO+1, Y+2*28+BO+1);
   for(j=0; j<28; j++)  for(i=0; i<36; i++)
    { switch(Road[j][i]&0x0f)
       { case 0: color = BROWN; break; case 1: color = LIGHTGRAY; break;
	 case 2: color = WHITE; break; case 3: color = LIGHTBLUE; break;
	 case 4: color = GREEN; break; case 5: color = BLACK; break;
	 default: color = BLACK; 
       }
      setfillstyle(1, color);   bar(X+2*i, Y+2*j, X+2*i+1, Y+2*j+1);
    }
   setcolor(YELLOW);  
   line((X+X+2*36-1)/2, Y+2*28-1, (X+X+2*36-1)/2, Y+2*28-2);
   line((X+X+2*36-1)/2+1, Y+2*28-1, (X+X+2*36-1)/2+1, Y+2*28-2);
   mouse_on();
 }
#undef X
#undef Y

void  show_kz()
 { void show_key(int x, int y, char *str);
   settextstyle(1, 0, 3);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW);  outtextxy(50, 380, "Use:");
   show_key(1, 0, "A");  show_key(2, 0, "S");
   show_key(3, 0, "D");  show_key(4, 0, "F");
   show_key(2, 2, "Space");
   show_key(12, 1, "Up");    show_key(11, 2, "Left");
   show_key(12, 2, "Down");  show_key(13, 2, "Right");
   show_key(0, 2, "Ctrl");   show_key(1, 2, "Alt");
   show_key(8, 2, "Alt");    show_key(9, 2, "Ctrl");
   show_key(7, 0, "Enter");  show_key(11, 0, "Del");
   show_key(11, -1, "Ins");
 }

#define _X  80
#define _Y  370

void show_key(int x, int y, char *str)
 { int x1 = _X+x*35, y1 = _Y+y*30, x2 = x1+25, y2 = y1+25;
   int color = random(4);
   if(strcmp(str, "Space")==0)   { x2 = x1+200; settextstyle(1, 0, 2); }
   else if(strcmp(str, "Enter")==0)  { x2 = x1+70; settextstyle(1, 0, 2); }
   else if(strlen(str)>1)   settextstyle(2, 0, 4);
   else                     settextstyle(1, 0, 3);
   setcolor(LIGHTGRAY);   line(x1, y1, x2, y1);  line(x1, y1, x1, y2);
   setcolor(DARKGRAY);    line(x2, y2, x1, y2);  line(x2, y2, x2, y1);
   setfillstyle(1, color==0?BLUE:color==1?CYAN:color==2?MAGENTA:BROWN);
   bar(x1+1, y1+1, x2-1, y2-1);
   setcolor(color==0?LIGHTGREEN:color==1?RED:color==2?WHITE:BLACK);
   outtextxy((x1+x2)/2, (y1+y2)/2-1, str);
 }



void settankza(int K)
 { int X=Tank[K].X, Y=Tank[K].Y;
   if(Tank[K].life)
     { Roadtank[Y][X] = K;  Roadtank[Y][X+1] = K;
       Roadtank[Y+1][X] = K; Roadtank[Y+1][X+1] = K;
     }
   else
     { Roadtank[Y][X] = NOTANK;  Roadtank[Y][X+1] = NOTANK;
       Roadtank[Y+1][X] = NOTANK; Roadtank[Y+1][X+1] = NOTANK;
     }
 }


void arrow(x1, y1, x2, y2)
int x1, y1, x2, y2;
 { setcolor(DARKGRAY);
   setfillstyle(6, getcolor());
   bar(x1, y1, x2, y2);
   setlinestyle(0, 0, 1);
   line(x2, y1-5, x2+40, (y2+y1)/2);
   line(x2, y2+5, x2+40, (y2+y1)/2);
   line(x2, y1-5, x2, y2+5);
   floodfill(x2+20, (y1+y2)/2, getcolor());
   setfillstyle(0, 0);
 }

int  tankinsea(int N)
 { int i, j;
   int x = Tank[N].X, y = Tank[N].Y;
   for(j=0; j<2; j++)  for(i=0; i<2; i++)
     if(Road[y+j][x+i]!=3)  return 0;
   return 1;
 }
int  tanknotinsea(int N)
 { int i, j;
   int x = Tank[N].X, y = Tank[N].Y;
   for(j=0; j<2; j++)  for(i=0; i<2; i++)
     if(Road[y+j][x+i]==3)  return 0;
   return 1;
 }
int  tankintree(int N)
 { int i, j, flag=0;
   int x = Tank[N].X, y = Tank[N].Y;
   for(j=0; j<2; j++)  for(i=0; i<2; i++)
     if(Road[y+j][x+i]==4)  { flag = 1; break; }
   return  flag;
 }
int  tankinice(int N)
 { int i, j;
   int x = Tank[N].X, y = Tank[N].Y;
   for(j=0; j<2; j++)  for(i=0; i<2; i++)
     if(Road[y+j][x+i]==1)  return 1;
   return 0;
 }

void  showeats()
 { int i, j, flag;
   Eat.kind = random(P.iappear==0?7:9);
   do
    { int X=Eat.X=random(33), Y=random(25);
      if(P.ieneat&&(Eat.kind<=1||Eat.kind==8))  Y = random(20)+5;
      else Y = random(25);    flag = 0;     Eat.Y = Y;
      for(j=-3; j<4; j++)
       { if(Y+j<0||Y+j>27)  continue;
	 for(i=-3; i<4; i++)
	   { if(X+i<0||X+i>35)  continue;
	     if(Roadtank[Y+j][X+i]!=NOTANK)  { flag = 1;  goto conbreak; }
	   }
       } conbreak:;
    } while(flag==1);
   Sound(3);
 }


extern char bkcolor;
char  shines(int x, int y)
 { char ch;  int len = textwidth("*");
   int  k=1, color=getcolor();
   setcolor(YELLOW);   setwritemode(XOR_PUT);
BEGIN
   for(k=0; k<2; k++)
    { long T = clock();
      off();   line(x, y, x+len, y);    on();
      while(clock()-T<5L)
       { if(kbhit()) { ch = getch();  goto end;  }
	 if(getmousebutton()==LEFTBUTTON)  { ch = ENTER_KEY; goto end; }
	 if(getmousebutton()==RIGHTBUTTON) { ch = ESC_KEY;   goto end; }
      }
    }
WEND
   end: if(k==0) { off(); line(x, y, x+len, y); on(); }
	setcolor(color);  setwritemode(COPY_PUT);
	return ch;
 }

int Inword(int x, int y, char *S, int Num)
 {int  i = 0, K = strlen(S), maxK=strlen(S), color = getcolor();
  char Ch;  int x0;
  char C[2] = {0, 0};
  char WH[2] = {0, 0};
  char BackS[50];
  strcpy(BackS, S);
  settextjustify(LEFT_TEXT, TOP_TEXT);
  setfillstyle(1,bkcolor);
  moveto(x, y);
  while(S[i])  {  C[0] = S[i++];  outtext(C);  }
  while(1)
   { Ch = shines(getx(), gety()+textheight("i")+1);
    switch(Ch)
     { case TAB_KEY:
       case 13: S[maxK]='\0';    return 0;
       case 27:mouse_off();
	      bar(x, y, getx(), gety()+textheight("i")*3/2);
	      moveto(x, y);
	      mouse_off(); outtext(BackS); mouse_on();
	      strcpy(S, BackS);
	      mouse_on();
	      return -1;
       case 8:if(K==0) break;
	      K--;   maxK--;
	      mouse_off();
	      WH[0] = S[K];
	      moverel(- textwidth(WH), 0);
	      x0 = getx();
	      setcolor(bkcolor);  outtext(S+K);
	      moveto(x0, y);
	      for(i=K; i<=maxK; i++)  S[i] = S[i+1];
	      setcolor(color);
	      outtext(S+K);
	      mouse_on();
	      S[maxK] = '\0';
	      moveto(x0, y);
	      break;
       case 0: switch(getch())
		{  case LEFT_KEY: if(K==0)  break;
			K--;
			WH[0] = S[K];
			moverel(-textwidth(WH), 0);
			break;
		   case RIGHT_KEY: if(K==maxK)  break;
			WH[0] = S[K];
			moverel(textwidth(WH), 0);
			K++;	break;
		   case END_KEY:  if(K==maxK)  break;
			moverel(text_wide(S+K), 0);
			K = maxK;   break;
		   case HOME_KEY:  if(K==0)  break;
			Ch = S[K];  S[K] = '\0';
			moverel(-text_wide(S), 0);
			S[K] = Ch;  K = 0;
			break;
                   case DELETE_KEY: if(K==maxK)  break;
			WH[0] = S[K];  moverel(textwidth(WH), 0);
			maxK--;
			mouse_off();
			WH[0] = S[K];	 moverel(- textwidth(WH), 0);
			x0 = getx(); 	 setcolor(bkcolor);  outtext(S+K);
			moveto(x0, y); 	 for(i=K; i<=maxK; i++)  S[i] = S[i+1];
			setcolor(color); outtext(S+K);
			mouse_on();
			S[maxK] = '\0';  moveto(x0, y);
			break;
		}  break;
       default:if(maxK >= Num) break;
	       x0 = getx();
	       setcolor(bkcolor);
	       mouse_off();  outtext(S+K);  mouse_on();
	       moveto(x0, y);
	       for(i=maxK+1; i>K; i--)   S[i] = S[i-1];
	       S[K] = Ch;     C[0] = Ch;
	       setcolor(color);
	       mouse_off(); outtext(C);  x0 = getx();
	       outtext(S+K+1);
	       mouse_on();  moveto(x0, y);
	       K++;   maxK++;
	       S[maxK] = '\0';
	       break;
     }
   }
 }

extern char wave[13], stage[14], irunmyself;

#define Showflag()\
   setcolor(((color=color==56?0:color+1)/4)+1);\
   outtextxy(10, 70+sel*30, "->");
void addnewzd()
 { int n, k, sel, color=0;
   char  bpath[15];
   char  filename[15] = "", dirname[13][14], path[50];

   strcpy(bpath, wave);
   if(irunmyself==0)
    { strcpy(wave, "@MYSELF");
      if(access("stage\\@myself", 0)==-1)  mkdir("stage\\@myself");
      irunmyself = 1;
    }
   n = findfile("stage\\*.", dirname, FA_DIREC, 13);
   mouse_off();  cleardevice();
   sethz24();   sethz_color(BLUE);  sethz_dxy(0);  sethz_text(LEFT_HZ,TOP_HZ);
   writehzs(180, 100, 109, 114);
   writehzs(220, 250, 115, 119);
   writehzs(300, 400, 46, 47);
   writehzs(450, 400, 48, 49);
   setcolor(GREEN);   settextstyle(1, 0, 3);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy(500, 100, wave);
   for(k=0; k<n; k++)
     { outtextxy(50, 70+k*30, dirname[k]);
       if(strcmp(dirname[k], wave)==0) 
	 { outtextxy(10, 70+k*30, "->");  sel = k; }
     }
   setfillstyle(1, bkcolor=DARKGRAY);
   bar(250, 280, 500, 315);
   mouse_on();
BEGIN
   for(k=0; k<n; k++)
     { int flag = 0;
       int x1=50, x2=50+textwidth(dirname[k]), y1=70+k*30, y2=y1+22;
       while(imouse_inbox(x1,y1,x2,y2))
	{ if(kbhit())  break;
	  if(flag==0)
	   { setcolor(YELLOW);  mouse_off();
	     outtextxy(50, 70+k*30, dirname[k]);
	     setcolor(LIGHTGREEN);  line(x1, y2, x2, y2);
	     flag = 1;          mouse_on();
	   }
	  if(getmousebutton()==LEFTBUTTON)
	   { while(getmousebutton()!=NOBUTTON)
	       if(!imouse_inbox(x1,y1,x2,y2))  goto __exit;
	     strcpy(wave, dirname[k]);
	     setfillstyle(0, 0);   bar(500, 100, 640, 130);
	     setcolor(GREEN);   outtextxy(500, 100, wave);
	     setcolor(BLACK);   outtextxy(10, 70+sel*30, "->");  sel = k;
	   }
	  Showflag();
	}
       __exit:
       if(flag==1)
	{ mouse_off();
	  setcolor(BLACK);  line(x1, y2, x2, y2);
	  setcolor(GREEN);  outtextxy(50, 70+k*30, dirname[k]);
	  flag = 0;  mouse_on();
	}
      }
    if(imouse_inbox(250, 280, 500, 315))
      { char flag=0;
	if(kbhit())  break;
	while(imouse_inbox(250,280,500,315))
	  { if(kbhit())  break;
	    if(flag==0)
	      { mouse_off();  setcolor(LIGHTGREEN);
		line(250, 316, 500, 316);  flag = 1;
		mouse_on();
	      }
	    if(getmousebutton()==LEFTBUTTON)
	     { while(getmousebutton()!=NOBUTTON);
	       setcolor(WHITE);
	       Inword(260, 283, filename, 13);
	     }
	    Showflag();
	  }
	mouse_off();  setcolor(BLACK);
	line(250, 316, 500, 316); mouse_on();
      }
   for(k=0; k<2; k++)
    { int flag = 0;
      while(imouse_inbox(k*150+300, 400, 420+k*150, 424))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    sethz_color(LIGHTRED);
	    writehzs(300+k*150, 400, 46+k*2, 47+k*2);
	    setcolor(LIGHTBLUE);  line(300+k*150, 424, 396+k*150, 424);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(!(imouse_inbox(k*150+300, 400, 420+k*150, 424)))  goto __break;
	    if(k==1) { strcpy(wave, bpath); return; }
	    else
	     { enterin:
	       sprintf(path, "stage\\%s\\%s", wave, filename);
	       if(access(path, 0)==0)
		 { strcpy(stage, filename);  changezd(1); return; }
	       fp = fopen(path, "wb");
	       if(fp==NULL)
		 { sethz_color(YELLOW-8); mouse_off();
		   writehzs(230, 330, 120, 125); mouse_on();
		   waitforclick();  mouse_off();
		   setfillstyle(0, 0);  bar(230, 330, 640, 360); mouse_on();
		 }
	       else
		 { int i, j;  char bstage[15];  strcpy(bstage, stage);
		   strcpy(stage, filename);
		   for(j=0; j<28; j++)  for(i=0;i<36;i++)  Road[j][i]=5;
		   for(j=0; j<3; j++)
		     { Road[j+25][16] = 0; Road[j+25][19] = 0; }
		   for(j=0;j<2;j++)  for(i=0;i<2;i++)
			  Road[j+26][i+17] = ((j*2+i)<<4)|6;
		   for(i=0; i<2; i++)   Road[25][17+i] = 0;
		   for(j=0; j<2; j++) for(i=0; i<2; i++)
		       Road[j+14][i+17] = 2;
		   editroad(0, path);
		   if(access(path, 0)==-1)
		     { strcpy(wave, bpath);  strcpy(stage, bstage); }
		   return;
		 }
	     }
	  }  __break:;
	 Showflag();
       }
      if(flag==1)
	{ mouse_off();
	  sethz_color(BLUE);
	  setcolor(BLACK);  line(300+k*150, 424, 396+k*150, 424);
	  writehzs(k*150+300, 400, 46+k*2, 47+k*2);
	  flag = 0;  mouse_on();
	}
    }
    if(kbhit())
     { unsigned char ch = getch();
       if(ch==ESC_KEY) { strcpy(wave, bpath);  return; }
       if(ch==ENTER_KEY)  goto enterin;
     }                
    Showflag();
WEND
 }


void mknewzc()
 { char  filename[15] = "", path[50];
   char dirname[12][14];   int  k, n;
   n = findfile("stage\\*.*", dirname, FA_DIREC, 12);
   sethz24();   sethz_color(BLUE);  sethz_dxy(0);
   sethz_text(LEFT_HZ, TOP_HZ);
   mouse_off();  cleardevice();
   writehzs(100, 40, 88, 96);
   setfillstyle(1, bkcolor=BLUE);
   bar(220, 200, 500, 230);
   settextstyle(1, 0, 3);   setcolor(LIGHTGRAY);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   sethz24();   sethz_color(BLUE);  sethz_dxy(0);  sethz_text(LEFT_HZ,TOP_HZ);
   writehzs(300, 400, 212, 213);
   writehzs(450, 400, 214, 215);
   setcolor(LIGHTGRAY);  settextjustify(LEFT_TEXT, TOP_TEXT);
   settextstyle(1, 0, 1);
   for(k=0; k<n; k++) outtextxy(10, k*20+100, dirname[k]);
   setcolor(WHITE);    mouse_on();
   Inword(230, 200, filename, 13);
BEGIN
   int flag = 0;
   while(imouse_inbox(220, 200, 500, 230))
    { if(kbhit())  break;
      if(flag==0)
	{ setcolor(LIGHTGREEN);
	  mouse_off();  line(220, 230, 500, 230);  mouse_on();
	  flag = 1;
	}
      if(getmousebutton()==LEFTBUTTON)  
       { *filename = 0;
	 while(getmousebutton()!=NOBUTTON);
	 mouse_off();
	 setfillstyle(0, 0);     bar(150, 280, 500, 340);
	 setfillstyle(1, BLUE);  bar(220, 200, 500, 230);
	 mouse_on();
	 setcolor(WHITE);        Inword(230, 200, filename, 13);
       }
    }
   if(flag==1) { setcolor(BLACK); off(); line(220, 230, 500, 230); on(); }
   for(k=0; k<2; k++)
    { int flag = 0;
      while(imouse_inbox(k*150+300, 400, 420+k*150, 424))
       { if(kbhit())  break;
	 if(flag==0)
	  { mouse_off();
	    sethz_color(LIGHTRED);
	    writehzs(300+k*150, 400, 212+k*2, 213+k*2);
	    setcolor(LIGHTBLUE);  line(300+k*150, 424, 396+k*150, 424);
	    flag = 1;  mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON);
	    if(!(imouse_inbox(k*150+300, 400, 420+k*150, 424)))  goto __break;
	    if(k==1)  return;
	    sprintf(path, "stage\\%s", filename);
	    mouse_off();  sethz_color(GREEN);
	    setfillstyle(0, 0);  bar(150, 280, 500, 340);
	    if(mkdir(path)==0)
	     { writehzs(150, 300, 97, 102);
	       n = findfile("stage\\*.*", dirname, FA_DIREC, 12);
	       setfillstyle(0, 0);  bar(10, 100, 120, 400);
	       setcolor(LIGHTGRAY);  settextjustify(LEFT_TEXT, TOP_TEXT);
	       settextstyle(1, 0, 1);
	       for(k=0; k<n; k++) outtextxy(10, k*20+100, dirname[k]);
	     }
	    else  writehzs(150, 300, 103, 108);
	    mouse_on();
	  }  __break:;
       }
      if(flag==1)
	{ mouse_off();	  sethz_color(BLUE);
	  setcolor(BLACK);  line(300+k*150, 424, 396+k*150, 424);
	  writehzs(k*150+300, 400, 212+k*2, 213+k*2);
	  flag = 0;  mouse_on();
	}
    }
   if(kbhit())  if(getch()==ESC_KEY)  return;
WEND
 }

void  help_screen()
 { int k, lenth[] = { 8, 7, 8, 8, 8, 8, 5 }, begin = 152, end;
   mouse_off();  cleardevice();
   sethz24();  sethz_color(YELLOW);
   writehzs(120, 100, 148, 151);
   settextjustify(CENTER_TEXT, CENTER_TEXT); setcolor(YELLOW);
   settextstyle(1, 0, 4);  outtextxy(250, 95, ":");
   sethz_color(GREEN);   sethz_text(LEFT_HZ, TOP_HZ);  sethz_dxy(0);
   settextstyle(2, 0, 4);
   for(k=0; k<7; k++)
    { char str[5];
      setcolor(YELLOW);   circle(60+k*20, 142+k*40, 8);
      setcolor(GREEN);    sprintf(str, "%d", k+1);
      outtextxy(60+k*20, 142+k*40, str);
      writehzs(80+k*20, 130+k*40, begin, begin+lenth[k]-1);
      begin += lenth[k];
    }
   mouse_on();
 }

int  help()
 { void  showhelp(int begin, int end, int line, int dxy);
   int k, lenth[] = { 8, 7, 8, 8, 8, 8, 5 }, begin = 152, end;
   unsigned ch;
   help_screen();
BEGIN
   begin = 152;
   for(k=0; k<7; k++)
    { int  flag = 0, x1, y1, x2, y2;
      end = begin+lenth[k]-1;
      x1 = 80+k*20;       y1 = 130+k*40;
      x2 = x1+lenth[k]*48;  y2 = y1+24;
      while(imouse_inbox(x1, y1, x2, y2))
       { if(kbhit())   break;
	 if(flag==0)
	  { sethz_color(LIGHTRED);    mouse_off();
	    writehzs(x1, y1, begin, end);  flag = 1;
	    setcolor(LIGHTBLUE);  line(x1, y2, x2, y2);
	    mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON)
	      if(!imouse_inbox(x1, y1, x2, y2))  goto  __exit;
	  select_k:
	    switch(k)
	     { case 0:  showhelp(0, 330, 30, 20); break;
	       case 1:  showhelp(331, 871, 32, 19); break;
	       case 2:  showhelp(872, 1389, 33, 18); break;
	       case 3:  showhelp(1390, 1958, 32, 19); break;
	       case 4:  showhelp(1959, 2406, 32, 19); break;
	       case 5:  showhelp(2407, 2637, 27, 22); break;
	       case 6:  return 0;
	     }
	    help_screen();   flag = 0;
	  }
       }
      __exit:
      if(flag==1)
       { sethz_color(GREEN);   mouse_off();
	 setcolor(BLACK);  line(x1, y2, x2, y2);
	 writehzs(x1, y1, begin, end);
	 flag = 0;  mouse_on();
       }
      begin += lenth[k];
      if(kbhit())  switch(ch=getch())
       { case ESC_KEY: return 0;
	 case '1': case '2': case '3': case '4': case '5': case '6': case '7':
		       k = ch-'1';  goto select_k;
       }
    }
WEND
 }

void  showhelp(int begin, int end, int line, int dxy)
 { int k;   mouse_off();  cleardevice();
   setlinestyle(0, 0, 3);
   setcolor(YELLOW);      rectangle(5, 5, 635, 475);
   setcolor(LIGHTGREEN);  rectangle(10, 10, 630, 470);
   setlinestyle(0, 0, 1);
   settextstyle(1, 0, 5); settextjustify(CENTER_TEXT, CENTER_TEXT);
   setcolor(YELLOW);      outtextxy(100, 30, "Skyever");
   settextstyle(1, 0, 4); outtextxy(520, 440, "2002.2.20");
   sethz_dot(16);         sethz_speed(0);
   sethz_file(hzk16fp);   sethz_color(GREEN);
   sethz_fx(X_ROUND);     sethz_dxy(0);
   sethz_zoomxy(1, 1);
   for(k=begin; k<=end; k++)
     writehz(20+((k-begin+2)%line)*dxy, 80+((k-begin+2)/line)*dxy, k);
   mouse_on();
   waitforclick();
 }

void  show_str(int x, int y, int code, char *str)
 { int k, begin, d, n;
   randomize();
   if(code==0)
	{ n = x/10+4; begin = x-n*10; d = 10; }
   else { n = (639-x)/10+4; begin = x+n*10; d = -10; }
   setwritemode(XOR_PUT);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(begin, y, str);
   for(k=0; k<n; k++)
    { delay(10);
      outtextxy(begin+k*d, y, str);
      outtextxy(begin+(k+1)*d, y, str);
    }
   setwritemode(COPY_PUT);
   outtextxy(x, y, str);
 }

void  show_str2(int x, int y, int size, char *str)
 { int k;
   setwritemode(XOR_PUT);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(x, y, str);
   for(k=1; k<size; k++)
    { settextstyle(1, 0, k);      outtextxy(x, y, str);
      settextstyle(1, 0, k+1);    outtextxy(x, y, str);
      delay(50);
    }
   setwritemode(COPY_PUT);   outtextxy(x, y, str);
 }

extern char N;


void remake_screen()
 { int k, lenth[5] = { 7, 5, 5, 8, 5 }, begin = 58, end;
   mouse_off();  cleardevice();
   sethz24();  sethz_color(YELLOW);
   writehzs(120, 100, 56, 57);
   settextjustify(CENTER_TEXT, CENTER_TEXT); setcolor(YELLOW);
   settextstyle(1, 0, 4);  outtextxy(190, 95, "?");
   sethz_color(GREEN);   sethz_text(LEFT_HZ, TOP_HZ);  sethz_dxy(0);
   settextstyle(2, 0, 4);
   for(k=0; k<5; k++)
    { char str[5];
      setcolor(YELLOW);    circle(60+k*30, 162+k*40, 8);
      setcolor(GREEN);     sprintf(str, "%d", k+1);
      outtextxy(60+k*30, 162+k*40, str);
      writehzs(80+k*30, 150+k*40, begin, begin+lenth[k]-1);
      begin += lenth[k];
    }
   mouse_on();
 }

extern char file[89][14];
int  remake()
 { int k, lenth[5] = { 7, 5, 5, 8, 5 }, begin = 58, end;
   unsigned ch;
   remake_screen();
BEGIN
   begin = 58;
   for(k=0; k<5; k++)
    { int  flag = 0, x1, y1, x2, y2;
      end = begin+lenth[k]-1;
      x1 = 80+k*30;       y1 = 150+k*40;
      x2 = x1+lenth[k]*48;  y2 = y1+24;
      while(imouse_inbox(x1, y1, x2, y2))
       { if(kbhit())   break;
	 if(flag==0)
	  { sethz_color(LIGHTRED);    mouse_off();
	    writehzs(x1, y1, begin, end);  flag = 1;
	    setcolor(LIGHTBLUE);  line(x1, y2, x2, y2);
	    mouse_on();
	  }
	 if(getmousebutton()==LEFTBUTTON)
	  { while(getmousebutton()!=NOBUTTON)
	      if(!imouse_inbox(x1, y1, x2, y2))  goto  __exit;
	   select_k:
	    switch(k)
	     { case 0: select(2); break;
	       case 1: mknewzc(); break;
	       case 2: addnewzd(); break;
	       case 3: changezd(0); break;
	       case 4: goto ret;
	     }
	    remake_screen();   flag = 0;
	  }
       }
      __exit:
      if(flag==1)
       { sethz_color(GREEN);   mouse_off();
	 setcolor(BLACK);  line(x1, y2, x2, y2);
	 writehzs(x1, y1, begin, end);
	 flag = 0;  mouse_on();
       }
      begin += lenth[k];
      if(kbhit())
       { char str[50];
	 switch(ch=getch())
	  { case '5': case ESC_KEY:  ret:
		      sprintf(str, "stage\\%s\\*.*", wave);
		      N = findfile(str, file, 0, 88);  return 0;
	    case '1': case '2': case '3': case '4':
		      k = ch-'1';  goto select_k;
	  }
       }
    }
WEND
 }

void changezd(int code)
 { char path[50], ch;  int i, j;
   if(code==0) re: switch(select(0))
     { case 9: edkind(); goto re; case 10: select(1); break; case 0: return; }
   sprintf(path, "stage\\%s\\%s", wave, stage);
   fp = fopen(path, "rb+");
   if(fgetc(fp)!='S' || (ch=fgetc(fp))!='X'&&ch!='N'&&ch!='D'&&ch!='S')
    { sethz24();  setcolor(LIGHTGRAY);
      mouse_off();  cleardevice();
      writehzs(320, 200, 140, 143);  writehzs(320, 240, 144, 147);
      mouse_on();   waitforclick();  fclose(fp);  return;
    }
   readroad();    editroad(1, path);
 }


void setscreen()
 { int k;  char str[100];
   mouse_off();  cleardevice();
   setcolor(DARKGRAY);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 12);
   setcolor(YELLOW-8);      outtextxy(320, 80, VER);
   settextstyle(1, 0, 6);
   setcolor(DARKGRAY);   outtextxy(320, 180, "Skyever");
   setcolor(DARKGRAY);   settextstyle(2, 0, 5);
   outtextxy(320, 220, "Email: Skyever_youer@sohu.com");
   //***********************************************************
   sethz24();
   setcolor(DARKGRAY);  settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(2, 0, 4);
   for(k=0; k<5; k++)
    { char str[5];  sprintf(str, "<%d>", k+1);
      outtextxy(30+k*80, 300+k*40, str);
      writehzs(150+k*80, 300+k*40, 26+k*4, 29+k*4);
    }
   settextstyle(1, 0, 1);  setcolor(BLUE);
   sprintf(str, "---- %s (%d) -> %s", wave, N, stage);
   outtextxy(450, 295, str);
   settextstyle(1, 0, 2);
   outtextxy(490, 300+1*40, "----> Enter Game");
   outtextxy(80,  300+2*40, "---- Set Roads ->");
   outtextxy(150, 300+3*40, "---- Help Me ->");
   outtextxy(220, 300+4*40, "---- Exit Game ->");
   setcolor(LIGHTGREEN);  settextstyle(2, 0, 8);
   sprintf(str, KINDSTR, 'A'+kindflag);
   outtextxy(320, 260, str);   mouse_on();
 }


extern long T1, T0, _;
extern struct _KEY  KEY;
void  time_set()
 { char str[15] = "00:00:00";  int S, F, M, flag=0;
   T1 = clock();  if(T1-_<18)  return;  _ = T1;
   S = (T1-T0)/3600/18;  F = (((T1-T0)/18)%3600)/60;  M = (((T1-T0)/18)%3600)%60;
   str[0] = S/10 + '0';  str[1] = S%10 + '0';
   str[3] = F/10 + '0';  str[4] = F%10 + '0';
   str[6] = M/10 + '0';  str[7] = M%10 + '0';
   if(imouse_inbox(586, 125-16, 640, 135))  {  flag = 1; mouse_off(); }
   setfillstyle(1, GREEN);   bar(586, 125, 640, 135);
   settextstyle(2, 0, 4);     setcolor(RED);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy((587+640)/2, (122+137)/2, str);
   if(flag==1)  mouse_on();
 }


void  redflag()
 { int k, flag=0;
   if(!imouse_inbox(LEFT,TOP,RIGHT,BOTTOM)) { mouse_off(); flag = 1; }
   for(k=0; k<2; k++)
    { int color = -1;
      if(imouse_inbox(585, 410+k*12, 640, 425+k*12)) color = YELLOW;
      else color = k==0?isound?LIGHTRED:CYAN:KEY.enter?LIGHTRED:CYAN;
      if(color==-1)  continue;
      settextstyle(2, 0, 4); settextjustify(CENTER_TEXT, CENTER_TEXT);
      setcolor(color); 
      outtextxy(613, 415+k*12, k==0?"SOUND":"PAUSE");
      if(flag==1)  mouse_on();
    }
 }

extern void interrupt  (*oldkey)();
extern void interrupt  newkey();

int  show_select(int begin, int end, int code)
 {  int k, ch;
    if(code==1)  { disable();  setvect(0x09, oldkey);  enable(); }
    setfillstyle(1, DARKGRAY);
    mouse_off();
    bar(320-250, 240-40, 320+250, 240+40);
    setfillstyle(1, LIGHTBLUE);
    bar(320-250-5, 240-40-5, 320+250-5, 240+40-5);
    setcolor(RED);
    rectangle(320-250-5, 240-40-5, 320+250-5, 240+40-5);
    sethz16();  
    sethz_color(RED);    writehzs(320, 220, begin, end);
    sethz_color(WHITE);  writehzs(321, 221, begin, end);
    settextstyle(1, 0, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(LIGHTGREEN);  outtextxy(320, 250, "(Yes / No)");
    mouse_on();   nosound();
    while(1)
      { for(k=0; k<2; k++)
	  { int flag = 0; 
	    int h1=textheight("Yes")+4, h2=textheight("No")+4;
	    int w1=textwidth("Yes"), w2=textwidth("No");
	    while(imouse_inbox(265+k*75, 235, 310+k*75, 270))
	     { if(kbhit())  break;
	       if(flag == 0)
		{ mouse_off(); 	  
		  if(k==0) 
		    { setcolor(YELLOW);  outtextxy(286, 250, "Yes");
		      setcolor(LIGHTRED); line(286-w1/2, 250+h1/2, 286+w1/2, 250+h1/2);
		    }
		  else  
		    { setcolor(YELLOW);  outtextxy(357, 250, "No");
		      setcolor(LIGHTRED); line(357-w2/2, 250+h2/2, 357+w2/2, 250+h2/2);
		    }
		  flag = 1; mouse_on();
		}
	       if(getmousebutton()==LEFTBUTTON)
		{ while(getmousebutton()!=NOBUTTON);
		  if(imouse_inbox(265+k*75, 235, 310+k*75, 270))  return 1-k;		    
		}
	     }
	    if(flag==1)
	     { mouse_off();  
	       if(k==0)  
		 { setcolor(LIGHTBLUE); line(286-w1/2, 250+h1/2, 286+w1/2, 250+h1/2);
		   setcolor(LIGHTGREEN);  outtextxy(286, 250, "Yes");
		 } 
	       else      
		 { setcolor(LIGHTBLUE); line(357-w2/2, 250+h2/2, 357+w2/2, 250+h2/2);
		   setcolor(LIGHTGREEN); outtextxy(357, 250, "No");
		 }
	       flag = 0; mouse_on();
	     }
	  }
	 if(kbhit()) switch(getch())
	  { case 'Y': case 'y': case ENTER_KEY:  ch = 1;  goto ret;
	    case 'N': case 'n': case ESC_KEY:    ch = 0;  goto ret;
	  }
      }
   ret: if(code==1)  { disable();  setvect(0x09, newkey);  enable(); }
   return ch;
  }

int  chestfile(char *filename)
 { char ch;
   fp = fopen(filename, "rb");  if(fp==NULL)  return 0;
   if(fgetc(fp)!='S' || (ch=fgetc(fp))-'X'&&ch-'N'&&ch-'D'&&ch-'S')
     { fclose(fp); return 0; }
   return 1;
 }


void hidescreen(int code)
 { int k, color = getcolor();   mouse_off();
   for(k=(code==1?0:2400); code==1?k<=2400:k>=0; code==1?k++:k--)
    { if(k%10==0)
	{ setcolor(WHITE);
	  line(0, k/10, 640, k/10);  line(0, 479-k/10, 640, 479-k/10);
	}
      else
	{ setcolor(BLACK);
	  line(0, k/10-code, 640, k/10-code);
	  line(0, 479-k/10+code, 640, 479-k/10+code);
	}
    }
   setcolor(color);  mouse_on();
 }

