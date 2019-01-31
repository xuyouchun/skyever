#include <stdio.h>
#include <graphics.h>
#include <string.h>
#include <alloc.h>
#include <stdarg.h>
#include <dos.h>
#include <bios.h>
#include <math.h>
#include <dos.h>
#include "extern.h"
#include "hz.h"
#include "mouse.h"
#include "st-tab.h"
#include "key.h"
#include "set.h"
#include "tools.h"

char  INPUT=0, key=0, UP = 1, SOUND=1, showenter=1;
int   PX, PY;
double get_xf(ST_MSG*);
int   exist[5], bsee[5] ;
struct JS  PTR[5];
int   K = 0, jsflag = 0;
float  SD, SB, SX, SR, SF;
float  MB, MX, MR;
char   ERR = TRUE, CLEAR ;
char   fcolor = RED, AT = 0;
int   start[5], inexist, bstart;
int   T = 5;

#define AD(C,str) (C==ch?(CH[0]=str[0],CH[1]=str[1],1):0)
/*在固定位置输入汉字*/
int  far  get_hzs(int x, int y, char *S, int num)
 { char ch, flag=0;  int K, bx, by, maxK, k, color = gethz_color();
   char BS[100], CH[2], *ptr;  strcpy(BS, S);
   maxK = K = strlen(S)/2;  bx = x; by = y;
   sethz_color(RED);
   mouse_off();  show_hzs(x, y, S);  mouse_on();
   x += K*16;
   sethz_file(hzk16fp);  sethz_dot(16); sethz_fx(X_ROUND);
   sethz_zoomxy(1, 1);   sethz_speed(0);
   setfillstyle(1, bkcolor);
   while(getmousebutton()!=NOBUTTON) show_time(0);
BEGIN
   switch( ch=shines(x, y) )
    { case  0: switch(getch())
	{ case  DELETE_KEY:  if(K==maxK)  break;  K++;  x += 16; goto de;
	  case  UP_KEY:  case DOWN_KEY:   key = ch;
	  case  LEFT_KEY:  if(K--==0)  K=0;  else x-=16; break;
	  case  RIGHT_KEY:  if(K++==maxK)  K = maxK; else x+=16; break;
	  case  HOME_KEY: x -= K*16;  K = 0;  break;
	  case  END_KEY:  x += (maxK-K)*16;  K = maxK; break;
	}   break;
       case  ESC_KEY: flag = -1;  goto end;
       case  ENTER_KEY: S[2*maxK]='\0'; goto end;
       case  BACKSPACE_KEY: if(K==0)  break;
		de: mouse_off();
		if(K!=maxK)
		 { for(k=K-1;k<maxK;k++)
		    { S[2*k+1] = S[2*(k+1)+1]; S[2*k] = S[2*(k+1)]; }
		   ptr = (char*)malloc(imagesize(x,y-16,x+16*(maxK-K),y));
		   getimage(x, y-16, x+16*(maxK-K),y,ptr);
		   bar(x,y-16,x+16*(maxK-K),y);
		   putimage(x-16, y-16, ptr,COPY_PUT);
		   free(ptr);
		 }
		else bar(x, y, x-16, y-16);  show_line();
		mouse_on();
		K--; maxK--; x-=16; break;	  
      default:  if(ch>=0)  
		 { if(ch<='9'&&ch>='0') { CH[0]="０"[0]; CH[1]="０"[1]+ch-'0'; goto begin; }
		   if(ch<='j'&&ch>='a') { CH[0]="ａ"[0]+(ch-'a')/10; CH[1]="ａ"[1]+(ch-'a')%10; goto begin; }
		   if(ch<='t'&&ch>='k') { CH[0]="ｋ"[0]+(ch-'k')/10; CH[1]="ｋ"[1]+(ch-'k')%10; goto begin; }
		   if(ch<='z'&&ch>='u') { CH[0]="ｕ"[0]+(ch-'u')/10; CH[1]="ｕ"[1]+(ch-'u')%10; goto begin; }
		   if(ch<='J'&&ch>='A') { CH[0]="Ａ"[0]+(ch-'A')/10; CH[1]="Ａ"[1]+(ch-'A')%10; goto begin; }
		   if(ch<='O'&&ch>='K') { CH[0]="Ｋ"[0]+(ch-'K')/10; CH[1]="Ｋ"[1]+(ch-'K')%10; goto begin; }
		   if(ch<='Z'&&ch>='U') { CH[0]="Ｕ"[0]+(ch-'U')/10; CH[1]="Ｕ"[1]+(ch-'U')%10; goto begin; }
		   if(AD('!',"！")||AD('~',"～")||AD('`',"、")||AD('@',"＠")) goto begin;
		   if(AD('#',"＃")||AD('$',"＄")||AD('%',"％")||AD('^',"＾")) goto begin;
		   if(AD('&',"＆")||AD('*',"＊")||AD('(',"（")||AD(')',"）")) goto begin;
		   if(AD('|',"｜")||AD('\\',"＼")||AD('[',"【")||AD(']',"】")) goto begin;
		   if(AD('-',"－")||AD('_',"＿")||AD('+',"＋")||AD('=',"＝")) goto begin;		   
		   if(AD('{',"〔")||AD('}',"〕")||AD('<',"＜")||AD('>',"＞")) goto begin;
		   if(AD(',',"，")||AD('.',"。")||AD(';',"；")||AD(':',"：")) goto begin;
		   if(AD('?',"？")||AD('/',"／")) goto begin;
		   if(ch=='\'')
		    { static char flag=1;  
		      flag = 1-flag;
		      if(flag==0&&AD('\'',"‘") || flag==1&&AD('\'',"’") ) goto begin;
		    }
		   else if(ch=='\"')
		    { static char flag=1;  
		      flag = 1-flag;
		      if(flag==0&&AD('\"',"“") || flag==1&&AD('\"',"”")) goto begin;
		    }
		   break;
		 }    		
		CH[0] = ch;
		if(!kbhit())  { flag = -1; goto end; }
		CH[1] = getch();
		if(CH[1]>=0)  { flag = -1; goto end; }
	       begin: if(maxK>=num)  break;
		if(K!=maxK)
		 { for(k=maxK; k>K; k--)
		    { S[2*k+1] = S[2*(k-1)+1];  S[2*k] = S[2*(k-1)]; }
		   S[2*K]=CH[0];  S[2*K+1]=CH[1];
		   ptr = (char*)malloc(imagesize(x,y,x+(maxK-K)*16,y-16));
		   mouse_off();
		   getimage(x,y,x+(maxK-K)*16,y-16, ptr);
		   bar(x, y, x+(maxK-K)*16, y-16);
		   putimage(x+16,y-16,ptr,COPY_PUT);
		   mouse_on();
		   free(ptr);
		 }
		else { S[2*K]=CH[0];  S[2*K+1]=CH[1]; }
		mouse_off();
		show_hz(x, y-16, CH);  x += 16;
		mouse_on();
		K++;  maxK++;
		break;
    }
WEND
end: mouse_off();
     bar(x, y, bx, by-16);
     sethz_color(color);
     if(flag==-1 || strcmp(S, BS)==0)  strcpy(S, BS);
     show_hzs(bx, y, S);  show_line();
     mouse_on();
     if(flag==-1 || strcmp(S, BS)==0)  return -1;
     return K;
 }

int  input_boxstr(int x, int y, char *Head, char *S, int num)
 { int x1=x-200, x2=x+200, y1=y-30, y2=y+30;
   char far *ptr, flag=1;
   bkcolor = LIGHTBLUE;  clear_bar();
   ptr = (char far *)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return -1;
   mouse_off();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, LIGHTBLUE);
   bar(x1, y1, x2, y2);
   sethz_color(LIGHTGREEN);
   show_hzs(x1+15, y1+20, Head);
   INPUT = 1;
   sethz_color(YELLOW);
   show_hzs(x1+15, y1+45, S);  setcolor(YELLOW);
   mouse_on();
   settextstyle(2, 0, 6);
   if(Inword(x1+15, y1+30, S, num)==-1)  flag = -1;
   mouse_off();
   putimage(x1, y1, ptr, COPY_PUT);
   mouse_on();
   INPUT = 0;
   farfree(ptr);
   while(getmousebutton()!=NOBUTTON) show_time(0);
   return flag;
 }

/*在固定位置输入字符串*/
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
   { Ch = shines(getx(), gety()+textheight("i"));
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

/*在固定位置输入数字*/
int  input_num(int x, int y, float *N, int n, int point)
 { char  S[20], C[2]={0,0}, str[20], ch, color = getcolor();
   int   flag=1, K=0, f=1, d=0, k, x0, len=0;
   double  DATA=0;
   float bN = *N; 
   sprintf(S, "%f", *N);  S[n] = '\0';
   settextstyle(2, 0, 6);
   setfillstyle(1, bkcolor);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   if(CLEAR==1)
     { while(S[K]!='.'&&S[K]!='\0')  K++;
       S[K] = '\0';  len = text_wide(S);
       outtextxy(x, y, S);
     }
   else
     { mouse_off();
       bar(x, y, x+text_wide(S), y+15);
       clear_str(S); show_line();
       mouse_on();
     }
   setcolor(fcolor);
   while(getmousebutton()!=NOBUTTON) show_time(0);
   while(flag==1)
    { switch(ch=shines(x+len, y+15))
       { case 0:  if(CLEAR == 1)  break;
		  if((ch=getch())==LEFT_KEY||ch==RIGHT_KEY||ch==UP_KEY||
		      ch==DOWN_KEY)
		    key = ch;  flag = 0;  S[K] = '\0';  break;
	 case 27: flag = -1; break;
	 case 13: flag=0;  S[K] = '\0'; break;
	 case 8:  if(K==0) break;
		  mouse_off();
		  setfillstyle(1, bkcolor);
		  C[0] = S[K-1];
		  bar(x+len, y, x+len-textwidth(C), y+15);
		  len -= textwidth(C);
		  if(C[0]=='.')  f = 1;
		  if(f==0)  d--;
		  show_line(); setcolor(fcolor);
		  mouse_on();
		  K--;  break;
	 default: if(!((ch<='9'&&ch>='0')||(ch=='-'&&K==0)||
		      (ch=='.'&&f==1&&point)) || K>=n || (d>=point&&point)) break;
		  C[0] = ch;
		  mouse_off();  setcolor(fcolor);
		  outtextxy(x+len, y, C);
		  mouse_on();
		  len += textwidth(C);
		  if(f==0)  d++;
		  S[K++] = ch;
		  if(ch=='.')  f = 0;
       }
    }
   setcolor(color);
   if(flag == -1 || strlen(S)==0 || strlen(S)==1&&(S[0]=='-'||S[0]=='.') ) 
     {  mouse_off();       bar(x, y, x+len, y+15);
	show_nums(x, y, bN, n, point);	show_line();
	mouse_on();
	return -1;
     }
   d = -1;   f = -1;
   if(S[0]=='-')  f = 1;
   len = strlen(S);
   if(S[0]=='.')  { K = 1; str[0] = '0'; }
   else  K = 0;
   for(k=0; k<len; k++)
    { if(S[k]=='.')  d = K;
      else if(S[k]>='0'&&S[k]<='9')  str[K++] = S[k];
    }
   str[K] = '\0';
   len = strlen(str);
   for(k=0; k<len; k++)    DATA = DATA*10+(double)(str[k]-'0');
   if(d!=-1)
     { d = len-d;
       for(k=0; k<d; k++)  DATA /= 10;
     }
   if(f==1)  DATA = -DATA;
   *N = (float)DATA;
   mouse_off();
   bar(x, y, x+len, y+15);
   show_nums(x, y, *N, n, point);
   show_line();  mouse_on();
   while(getmousebutton()!=NOBUTTON) show_time(0);
   return  1;
 }

char  shines(int x, int y)
 { long _T; char ch;
   int  k=1, color=getcolor();
   if(key!=0) { ch = key; key = 0; goto end; }
   setwritemode(XOR_PUT);
   setcolor(YELLOW);
   AT = 1;
BEGIN
   for(k=0; k<2; k++)
    { mouse_off();
      line(x, y, x+10, y);
      mouse_on();
      for(_T=0; _T<1400; _T++)
       { setwritemode(COPY_PUT);  show_time(0);
	 setcolor(YELLOW);  setwritemode(XOR_PUT);  setfillstyle(1, bkcolor);
	 if(kbhit()) { ch = getch();  goto end;  }
	 if(getmousebutton()==LEFTBUTTON)  { ch = ENTER_KEY; goto end; }
	 if(getmousebutton()==RIGHTBUTTON) { ch = ESC_KEY;   goto end; }
      }
    }
WEND
   end: if(k==0) { mouse_off(); line(x, y, x+10, y); mouse_on(); }
	setwritemode(COPY_PUT);
	setcolor(color);
	AT = 0;
	return ch;
 }

int  show_nums(int x, int y, double N, int num, int point)
 { char S[20];  int k=0, i;
   sprintf(S, "%f", N);
   S[num] = '\0';
   while(S[k]!='.' && S[k]!='\0')  k++;
   if(point==0)  S[k] = '\0';
   settextstyle(2, 0, 6);  settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy(x, y, S);
   return  textwidth(S);
 }

int  text_wide(char *S)
 { int len, k, lon=0;
   char s[2]={0,0};
   len = strlen(S);
   for(k=0; k<len; k++)
     { s[0] = S[k];
       lon += textwidth(s);
     }
   return lon;
 }

int  msg_box(int x, int y, char *head, char *msg, int code)
 { char str[33];  char far *ptr;
   int len = strlen(msg), flag=0;
   int k, k0=0, x1, y1, x2, y2, n=0, ch;
   if(code==0&&showenter==0)  return 1;
   x1 = x-8*16-20;
   x2 = x+8*16+20;
   y1 = y-(len/30)*8-50;
   y2 = y+(len/30)*8+50;
   ptr = (char far *)farmalloc(imagesize(x1, y1, x2+10, y2+10));
   if(ptr==NULL)  return 0;
   mouse_off();   clear_bar();
   getimage(x1, y1, x2+10, y2+10, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+10, y1+10, x2+10, y2+10);
   setfillstyle(1, LIGHTBLUE);
   bar(x1, y1, x2, y2);
   sethz_color(LIGHTRED);
   show_hzs(x1+10, y1+25, head);
   sethz_color(YELLOW);
   do
    { for(k=0; k<32; k++)
       { str[k] = msg[k0++];
	 if(str[k]=='\0') { flag=1;  break; }
       }
      str[32] = '\0';
      show_hzs(x1+20, y1+(code==0?50:70)+20*n++, str);
    } while(flag==0);
   if(code==1)
     {  mouse_on();
	show_sound();
	while(!kbhit() && getmousebutton()==NOBUTTON)
	  { show_time(0); mouse_on(); }
	goto end;
     }
   mouse_off();
   sethz_color(LIGHTCYAN);
   show_hzs( x-95, y2-10, "是√   否×   取消");
   clear_bar();
   mouse_on();
   show_sound();
   mouse_on();
 BEGIN
	if(test_in(x-110, y2-10-18, x-50, y2-8)==1)  ch = 1;
   else if(test_in(x-30, y2-10-18, x+30, y2-8)==1)   ch = 0;
   else if(test_in(x+50, y2-10-18, x+110, y2-8)==1)  ch = 2;
   else { clear_bar();  ch = -1; }
   show_time(0);
   if(!kbhit()&&getmousebutton()!=LEFTBUTTON)  continue;
   if(kbhit())
      if( (ch=getch())!=ENTER_KEY && ch!=ESC_KEY && ch!=SPACE_KEY)  continue;
   break;
 WEND
 end:
   while(getmousebutton()!=NOBUTTON) show_time(0);
   mouse_off();  clear_bar();
   putimage(x1, y1, ptr, COPY_PUT);
   mouse_on();   farfree(ptr);
   while(kbhit())     getch();
   if(ch==0||ch==32)  return 0;
   if(ch==1||ch==13)  return 1;
   if(ch==2||ch==27)  return 2;
   return 2;
 }

int  input_box(int x, int y, char *Head, char *S, int num)
 { int x1=x-200, x2=x+200, y1=y-30, y2=y+30;
   char far *ptr, flag=1;
   bkcolor = LIGHTBLUE;  clear_bar();
   ptr = (char far *)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return -1;
   mouse_off();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, LIGHTBLUE);
   bar(x1, y1, x2, y2);
   sethz_color(LIGHTGREEN);
   show_hzs(x1+15, y1+20, Head);
   INPUT = 1;
   mouse_on();
   sethz_color(YELLOW);
   show_hzs(x1+15, y1+45, S);
   if(get_hzs(x1+15, y1+45, S, num)==-1)  flag = -1;
   mouse_off();
   putimage(x1, y1, ptr, COPY_PUT);
   mouse_on();
   INPUT = 0;
   farfree(ptr);   while(getmousebutton()!=NOBUTTON) show_time(0);
   return flag;
 }

int  input_boxnum(int x, int y, char *Head, float *N, int n, int point)
 { int x1=x-200, x2=x+200, y1=y-30, y2=y+30;
   char far *ptr, flag=1;
   bkcolor = LIGHTBLUE;  clear_bar();
   ptr = (char far *)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return -1;
   mouse_off();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, LIGHTBLUE);
   bar(x1, y1, x2, y2);
   sethz_color(LIGHTGREEN);
   show_hzs(x1+15, y1+20, Head);
   INPUT = 1;
   mouse_on();
   if(input_num(x1+30, y1+30, N, n, point)==-1)  flag = -1;
   mouse_off();
   putimage(x1, y1, ptr, COPY_PUT);
   mouse_on();
   INPUT = 0;
   farfree(ptr);  while(getmousebutton()!=NOBUTTON) show_time(0);
   return flag;
 }

int  log_box(int x, int y, struct _ZY *D)
 { char far *ptr;
   int _x1, _y1, _x2, _y2;
   int Ch, k;  char ch;
   struct _ZY bD;
   strcpy(bD.Name, D->Name);
   bD.Attrib = D->Attrib;
   bD.Xf = D->Xf;
   _x1=x-100; _x2=x+100; _y1=y-60; _y2=y+60;
   bkcolor = LIGHTBLUE;
   ptr = (char far*)farmalloc(imagesize(_x1, _y1, _x2+5, _y2+5));
   if(ptr==NULL)  return -1;
   mouse_off(); clear_bar();
   getimage(_x1, _y1, _x2+5, _y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(_x1+5, _y1+5, _x2+5, _y2+5);
   setfillstyle(1, LIGHTBLUE);
   bar(_x1, _y1, _x2, _y2);
   INPUT = 1;
   sethz_color(LIGHTGREEN);
   show_hzs(_x1+15, _y1+30, "名称：");
   show_hzs(_x1+15, _y1+50, "属性：");
   show_hzs(_x1+15, _y1+90, "学分：");
   sethz_color(LIGHTRED);
   show_hzs(_x1+35, _y1+70, "必修 限选 任选");
   sethz_color(YELLOW);
   show_hzs(_x1+70, _y1+110, "确 定");
   show_hzs(_x1+70, _y1+30, D->Name);
   show_hzs(_x1+70, _y1+50, D->Attrib==0?"必修":D->Attrib==1?"限选":"任选");
   setcolor(YELLOW);
   show_nums(_x1+70, _y1+75, D->Xf, 5, 1);
   mouse_on();
   sethz_color(YELLOW);
 BEGIN
   key = 0;  show_time(0);
	if(test_in(_x1+15, _y1+11, _x1+185, _y1+31)==1)  Ch = 0;
   else if(test_in(_x1+15, _y1+31, _x1+185, _y1+51)==1)  Ch = 1;
   else if(test_in(_x1+15, _y1+71, _x1+185, _y1+91)==1)  Ch = 2;
   else if(test_in(_x1+50, _y1+91, _x1+145, _y1+111)==1) Ch = 3;
   else
    { for(k=0; k<3; k++)
       if(test_in(_x1+25+k*49, _y1+52, _x1+25+(k+1)*49, _y1+72)==1)
	 { Ch = 10+k;  break; }
     if(k==3)  { Ch = -1; clear_bar(); }
   }
   if(!kbhit() && getmousebutton()!=LEFTBUTTON) continue;
   if(!imouse_inbox(_x1,_y1,_x2,_y2)&&getmousebutton()==LEFTBUTTON)  Ch = 3;
   clear_bar();
   if(getmousebutton()==LEFTBUTTON)  switch(Ch)
    { case 0: get_hzs(_x1+70, _y1+30, D->Name, 4);  break;
      case 1: sethz_color(LIGHTBLUE);  mouse_off();
	      show_hzs(_x1+70, _y1+50, D->Attrib==0?"必修":D->Attrib==1?"限选":"任选");
	      D->Attrib = D->Attrib==2?0:D->Attrib+1;
	      sethz_color(YELLOW);
	      show_hzs(_x1+70, _y1+50, D->Attrib==0?"必修":D->Attrib==1?"限选":"任选");
	      mouse_on();
	      break;
      case 2: input_num(_x1+70, _y1+75, &D->Xf, 5, 3);  break;
      case 3: goto end;
      case 10: case 11: case 12:  mouse_off();
	      sethz_color(LIGHTBLUE);
	      show_hzs(_x1+70, _y1+50, D->Attrib==0?"必修":D->Attrib==1?"限选":"任选");
	      D->Attrib = Ch-10;
	      sethz_color(YELLOW);
	      show_hzs(_x1+70, _y1+50, D->Attrib==0?"必修":D->Attrib==1?"限选":"任选");
	      mouse_on();
	      break;
    }
   while(getmousebutton()!=NOBUTTON) show_time(0);
   if(kbhit())
    { ch = bioskey(1);
      if( (ch>='0'&&ch<='9') || ch=='.' || ch=='-')
	  input_num(_x1+70, _y1+75, &D->Xf, 5, 3);
      else if(ch<0||ch==BACKSPACE_KEY)  get_hzs(_x1+70, _y1+30, D->Name, 4);
      else switch(getch())
       { case ENTER_KEY:  goto end;
	 case ESC_KEY:    strcpy(D->Name, bD.Name);
			  D->Attrib = bD.Attrib;
			  D->Xf = bD.Xf;  goto end;
       }
    }
  WEND
end: clear_bar();
   mouse_off();
   putimage(_x1, _y1, ptr, COPY_PUT);
   mouse_on();
   farfree(ptr);   INPUT = 0;
   if(D->Name[0]=='\0')  return -1;
   if(strcmp(D->Name, bD.Name) || D->Attrib!=bD.Attrib || D->Xf!=bD.Xf)
	  saveflag = 1;
   while(getmousebutton()!=NOBUTTON) show_time(0);
   return 1;
 }

int  show_menu(int n, ...)
  { char str[100];  int  color = getcolor(); 
    char far *ptr;  va_list  arglist;
    int x1, y1, x2, y2, x, y, k, i, len, width=0, heigh, code;
    settextstyle(2, 0, 6);
    va_start(arglist, n);
    for(k=0; k<n; k++)
      { strcpy(str, va_arg(arglist, char*) );
	if(str[0]<0)  len = strlen(str) * 8;
	else          len = textwidth(str);
	if( len > width )	width = len;
      }
    va_end(arglist);
    width = width+10;
    heigh = 20*n + 10;
    getmouse_xy(&x, &y);
    if(x>634)  x = 634;    if(y>474)  y = 474;
    if(x+width>634)  { x1=x-width; x2=x; }
    else             { x1=x; x2=x+width; }
    if(y+heigh>474)
      if(UP==0)      { y1=470-heigh; y2=470; }
      else           { y1=y-heigh; y2=y; }
    else             { y1=y; y2=y+heigh; }
    if(PX)  { x1 = PX; x2 = PX+width; }
    if(PY)  { y1 = PY; y2 = PY+heigh; }
    ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
    if(ptr==NULL)  return -1;
    mouse_off();  clear_bar();  show_date(1);
    getimage(x1, y1, x2+5, y2+5, ptr);
    va_start(arglist, n);
    sethz_color(papercolor==GREEN?YELLOW:RED);
    setcolor(papercolor==GREEN?YELLOW:RED);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    setfillstyle(1, DARKGRAY);
    bar(x1+5, y1+5, x2+5, y1+10);
    setfillstyle(1, papercolor==GREEN?LIGHTBLUE:GREEN);
    bar(x1, y1, x2, y1+5);
    for(k=0; k<n; k++)
      { strcpy(str, va_arg(arglist, char* ) );
	setfillstyle(1, DARKGRAY);
	bar(x2, y1+k*20+10, x2+5, y1+(k+1)*20+10);
	setfillstyle(1, papercolor==GREEN?LIGHTBLUE:GREEN);
	bar(x1, y1+k*20+5, x2, y1+(k+1)*20+5);
	if(str[0]<0) show_hzs(x1+5, y1+2+20*(k+1), str );
	else         outtextxy(x1+5, y1+6+20*k, str );
      }
    setfillstyle(1, DARKGRAY);
    bar(x1+5, y2, x2+5, y2+5);
    setfillstyle(1, papercolor==GREEN?LIGHTBLUE:GREEN);
    bar(x1, y2-5, x2, y2);
    settextstyle(2, 0, 6);
    show_time(1);
 BEGIN
    show_time(0);
    for(k=0; k<n; k++)
       test_in(x1, y1+5+20*k, x2, y1+5+20*(k+1));
    if(!imouse_inbox(x1, y1, x2, y2))  clear_bar();
    mouse_on();
    if(kbhit()&&getch()==ESC_KEY)   { k = -1;  break; }
    if((code=getmousebutton())==NOBUTTON)  continue;
    if(!imouse_inbox(x1, y1, x2, y2))  { k = -1; break; }
    if(code!=LEFTBUTTON)  { k = -1; break; }
    getmouse_xy(&x, &y);
    k = (y-y1-5)/20;
    break;
 WEND
    mouse_off();
    clear_bar();  show_date(1);
    putimage(x1, y1, ptr, COPY_PUT);
    show_time(1);
    farfree(ptr);
    mouse_on();
    while(getmousebutton()!=NOBUTTON) show_time(0);
    setcolor(color);
    return k;
 }

void  line_button(int N)
 { int k;
   mouse_off();  clear_bar();
   setfillstyle(1, deskcolor);
   bar(100, 424, 639, 449);
   sethz_color(deskcolor);
   for(k=0; k<4; k++)  show_hzs(0, 242+k*20, "");
   sethz_color(YELLOW);
   show_hzs(0, 242+N*20, "");
   sethz_color(LIGHTCYAN);
   switch(N)
     { case 0: show_hzs(86, 445, (bioskey(2)&0x08)?
	       "  起始  左一屏  左一列  右一列  右一屏  结尾":
	       "  起始  上一页  上一行  下一行  下一页  结尾"); break;
       case 1: show_hzs(86, 445, "            升序      降序"); break;
       default:show_hzs(86, 445, "           上一页   下一页"); break;
     }
   bar(130, 27, 619, 46);  change_name(0);
   sethz_color(LIGHTGREEN);
   switch(N)
     { case 0: case 1:
	       show_hzs(160, 44, "德 育");
	       show_hzs(340, 44, "智 育");
	       show_hzs(520, 44, "附加分");
	       show_ptr2(msg);
	       break;
       case 3: show_hzs(180, 44, "按学号查询           按姓名查询"); break;
       case 2: show_hzs(180, 44, "以学分浏览           以科目浏览"); break;
     }
    if(N==1)  { write_fl(bcode);  show_ptr(); }
    show_time(1);    mouse_on();
 }

void  show_ptr()
 { if(kind!=1)  return;
   mouse_off();
   clear_bar();
   sethz_color(deskcolor);
   show_hzs(250+125*(1-order), 445, "");
   sethz_color(YELLOW);
   show_hzs(250+125*order, 445, "");
   mouse_on();
 }

void  show_ptr1(int code)
 { static int bcode;
   mouse_off();
   clear_bar();
   sethz_color(GREEN);
   show_hzs(bcode*64+155, 140, "");
   sethz_color(RED);
   show_hzs(code*64+155, 140, "");
   mouse_on();
   bcode = code;
 }

void  show_ptr2(int code)
 { static int bcode;
   if(kind>1)  return;
   clear_bar();
   sethz_color(deskcolor);
   show_hzs(bcode*180+135, 44, "★");
   sethz_color(LIGHTRED);
   show_hzs(code*180+135, 44, "★");
   bcode = code;
   change_name(1);
 }


#define Kd  80
void  show_see(int N)
 { void  show_sl();
   int x1, y1, x2, y2, x, y, K=0, YY=0, A, n;
   float kd;  ST_MSG  *ptr = find_point(N);
   double sum[5], xf[5], zxf=0;
   int k, i, num=0;
   for(k=0; k<5; k++)  { exist[k] = 0; sum[k] = 0; xf[k] = 0; }
   exist[0] = DYnum;
   for(k=0; k<ZYnum; k++)
    { if(ZY[k].Attrib == 0)  exist[1] ++;
      if(ZY[k].Attrib == 1)  exist[2] ++;
      if(ZY[k].Attrib == 2)  exist[3] ++;
    }
   exist[4] = FJnum;
   for(k=0; k<ZYnum; k++)
      { if(ZY[k].Attrib==2)  continue;  zxf += ZY[k].Xf; }
   for(k=0; k<5; k++)  if(exist[k]!=0)  num++;
   x1 = X1+20;  x2 = X2-20;
   y1 = (Y2-Y1-num*60)/2+Y1;  y2 = y1+num*60;
   y1 += 15;  y2 += 15;
   mouse_off();  clear_bar();
   show_paper(X1+10, Y1+10, X2-10, Y2-9);
   if(ptr==NULL)
     { sethz_color(BLACK);
       show_hzs(180, 240, "当前无数据可供查询，请点击“录入”先输入数据！");
       mouse_on(); return;
     }
   setcolor(deskcolor);
   setwritemode(COPY_PUT);
   setlinestyle(0, 0, 1);
   rectangle(x1, y1, x2, y2);
   if(exist[0])  line(x1, y1+1*60, x2, y1+1*60);
   for(k=1; k<num; k++)   line(x1+18, y1+k*60, x2, y1+k*60);
   if(exist[4])  line(x1, y1+(num-1)*60, x2, y1+(num-1)*60);
   line(x1+18, exist[0]?y1+60:y1, x1+18, exist[4]?y2-60:y2);
   line(x1+36, y1, x1+36, y2);
   for(k=0; k<num; k++)   line(x1+36, y1+k*60+40, x2, y1+k*60+40);
   renew_ptrdata();
   sethz_color(BLACK);
   if(exist[0])
    { x = x1; y = y1+(K++)*60; n = exist[0]>8?8:exist[0];
      sethz_color(BLACK);
      show_hzs(x+10, y+28, "德");
      show_hzs(x+10, y+48, "育");
      kd = (float)(x2-x1-36-Kd)/(n+1);
      if(bsee[0]+n>exist[0])  bsee[0] = exist[0]-n;
      for(k=1; k<n+2; k++)
	{ setcolor(k==n?RED:BLUE);
	  line(x+36+(int)(kd*k+0.5), y, x+36+(int)(kd*k+0.5), y+60);
	}
      for(k=0; k<exist[0]; k++)
       {  renew_sdata(ptr); xf[0] += js_xf(PTR[0].str, ptr->Dy[k], -1);
	  if(ERR==FALSE)  { inexist = 0; goto out; }
	  sum[0] += ptr->Dy[k];
	}
      setcolor(BLACK);
      for(k=0; k<n; k++)
	{ xshow_hzs(x+29+kd/2+(int)(kd*k+0.5), y+30, DY[k+bsee[0]].Name);
	  show_nums(x+20+kd/2+(int)(kd*k+0.5), y+43, ptr->Dy[k+bsee[0]], 4, 1);
	}
      sethz_color(RED);
      xshow_hzs(x+29+kd/2+(int)(kd*(k++)+0.5), y+30, "总分");
      xshow_hzs(x+29+Kd/2+(int)(kd*k+0.5),     y+30, "学分");
      setcolor(YELLOW);
      show_nums(x+20+kd/2+(int)(kd*(k-1)+0.5), y+43, sum[0], 4, 1);
      show_nums(x2-73, y+43, xf[0],  7, 4);
    }
   if(exist[1])
    { x = x1; y = y1+(K++)*60;  n = exist[1]>8?8:exist[1];
      if(YY==0)  YY = y;  A = -1;
      sethz_color(BLACK);
      show_hzs(x+20, y+28, "必");
      show_hzs(x+20, y+48, "修");
      kd = (float)(x2-x1-36-Kd)/(n+1);
      if(bsee[1]+n>exist[1])  bsee[1] = exist[1]-n;
      for(k=1; k<n+2; k++)
	{ setcolor(k==n?RED:BLUE);
	  line(x+36+(int)(kd*k+0.5), y, x+36+(int)(kd*k+0.5), y+60);
	}
      for(k=0; k<exist[1]; k++)
       {  while(ZY[++A].Attrib!=0)  if(A>=ZYnum)  break;
	  renew_sdata(ptr);
	  xf[1] += js_xf(PTR[1].str, ptr->Zy[A], ZY[A].Xf);
	  if(ERR == FALSE)  { inexist = 1;  goto out; }
	  sum[1] += ptr->Zy[A];
       }
      A = -1;
      setcolor(BLACK);
      for(k=0; k<bsee[1]; k++)   while(ZY[++A].Attrib!=0);
      for(k=0; k<n; k++)
	{ while(ZY[++A].Attrib!=0)  if(A>=ZYnum)  break;
	  if(k<n)
	   { xshow_hzs(x+29+kd/2+(int)(kd*k+0.5), y+30, ZY[A].Name);
	     show_nums(x+20+kd/2+(int)(kd*k+0.5), y+43, ptr->Zy[A], 4, 1);
	   }
	}
      sethz_color(RED);
      xshow_hzs(x+29+kd/2+(int)(kd*(k++)+0.5), y+30, "总分");
      xshow_hzs(x+29+Kd/2+(int)(kd*k+0.5),     y+30, "学分");
      setcolor(YELLOW);
      show_nums(x+20+kd/2+(int)(kd*(k-1)+0.5), y+43, sum[1], 4, 1);
      show_nums(x2-73, y+43, xf[1],  7, 4);
    }
   if(exist[2])
    { x = x1; y = y1+(K++)*60;   n = exist[2]>8?8:exist[2];
      if(YY==0)  YY = y;
      sethz_color(BLACK);
      show_hzs(x+20, y+28, "限");
      show_hzs(x+20, y+48, "选");
      kd = (float)(x2-x1-36-Kd)/(n+1);
      if(bsee[2]+n>exist[2])  bsee[2] = exist[2]-n;
      for(k=1; k<n+2; k++)
	{ setcolor(k==n?RED:BLUE);
	  line(x+36+(int)(kd*k+0.5), y, x+36+(int)(kd*k+0.5), y+60);
	}
      setcolor(BLACK);
      A = -1;
      for(k=0; k<exist[2]; k++)
	{ while(ZY[++A].Attrib!=1)  if(A>=ZYnum)  break;
	  renew_sdata(ptr);
	  xf[2] += js_xf(PTR[2].str, ptr->Zy[A], ZY[A].Xf);
	  if(ERR == FALSE)  { inexist = 2; goto out; }
	  sum[2] += ptr->Zy[A];
	}
      A = -1;
      for(k=0; k<bsee[2]; k++)   while(ZY[++A].Attrib!=1);
      for(k=0; k<n; k++)
	{ while(ZY[++A].Attrib!=1)  if(A>=ZYnum)  break;
	  xshow_hzs(x+29+kd/2+(int)(kd*k+0.5), y+30, ZY[A].Name);
	  show_nums(x+20+kd/2+(int)(kd*k+0.5), y+43, ptr->Zy[A], 4, 1);
	}
      sethz_color(RED);
      xshow_hzs(x+29+kd/2+(int)(kd*(k++)+0.5), y+30, "总分");
      xshow_hzs(x+29+Kd/2+(int)(kd*k+0.5),     y+30, "学分");
      setcolor(YELLOW);
      show_nums(x+20+kd/2+(int)(kd*(k-1)+0.5), y+43, sum[2], 4, 1);
      show_nums(x2-73, y+43, xf[2],  7, 4);
    }
   if(exist[3])
    { x = x1; y = y1+(K++)*60; n = exist[3]>8?8:exist[3];
      if(YY==0)  YY = y;
      sethz_color(BLACK);
      show_hzs(x+20, y+28, "任");
      show_hzs(x+20, y+48, "选");
      kd = (float)(x2-x1-36-Kd)/(n+1);
      if(bsee[3]+n>exist[3])  bsee[3] = exist[3]-n;
      for(k=1; k<n+2; k++)
	{ setcolor(k==n?RED:BLUE);
	  line(x+36+(int)(kd*k+0.5), y, x+36+(int)(kd*k+0.5), y+60);
	}
      setcolor(BLACK);
      A = -1;
      for(k=0; k<exist[3]; k++)
	{ while(ZY[++A].Attrib!=2)   if(A>=ZYnum)  break;
          xf[3] += js_xf(PTR[3].str, ptr->Zy[A], ZY[A].Xf);
	  if(ERR == FALSE)  { inexist = 3; goto out; }
	  sum[3] += ptr->Zy[A];
	}
      A = -1;
      for(k=0; k<bsee[3]; k++)   while(ZY[++A].Attrib!=2);
      for(k=0; k<n; k++)
	{ while(ZY[++A].Attrib!=2)  if(A>=ZYnum)  break;
	  xshow_hzs(x+29+kd/2+(int)(kd*k+0.5), y+30, ZY[A].Name);
	  show_nums(x+20+kd/2+(int)(kd*k+0.5), y+43, ptr->Zy[A], 4, 1);
	}
      sethz_color(RED);
      xshow_hzs(x+29+kd/2+(int)(kd*(k++)+0.5), y+30, "总分");
      xshow_hzs(x+29+Kd/2+(int)(kd*k+0.5),     y+30, "学分");
      setcolor(YELLOW);
      show_nums(x+20+kd/2+(int)(kd*(k-1)+0.5), y+43, sum[3], 4, 1);
      show_nums(x2-73, y+43, xf[3],  7, 4);
    }
   if(exist[4])
    { x = x1; y = y1+(K++)*60;  n = exist[4]>5?5:exist[4];
      sethz_color(BLACK);
      show_hzs(x+10, y+30, "附");
      show_hzs(x+10, y+50, "加");
      kd = (float)(x2-x1-36-Kd)/(n+1);
      if(bsee[4]+n>exist[4])  bsee[4] = exist[4]-n;
      for(k=1; k<n+2; k++)
	{ setcolor(k==n?RED:BLUE);
	  line(x+36+(int)(kd*k+0.5), y, x+36+(int)(kd*k+0.5), y+60);
	}
      for(k=0; k<exist[4]; k++)
       { xf[4] += js_xf(PTR[4].str, ptr->Fj[k], -1);
	 if(ERR==FALSE)  { inexist = 4; goto out; }
	 sum[4] += ptr->Fj[k];
       }
      setcolor(BLACK);
      for(k=0; k<n; k++)
	 { xshow_hzs(x+29+kd/2+(int)(kd*k+0.5), y+30, FJ[k+bsee[4]].Name);
	   show_nums(x+10+kd/2+(int)(kd*k+0.5), y+43, ptr->Fj[k+bsee[4]], 6, 3);
           renew_sdata(ptr);
	 }
      sethz_color(RED);
      xshow_hzs(x+29+kd/2+(int)(kd*(k++)+0.5), y+30, "总分");
      xshow_hzs(x+29+Kd/2+(int)(kd*k+0.5),     y+30, "学分");
      setcolor(YELLOW);
      show_nums(x+10+kd/2+(int)(kd*(k-1)+0.5), y+43, sum[4], 6, 3);
      show_nums(x2-73, y+43, xf[4],  7, 4);
    }
   num = 0;
   for(k=1; k<4; k++)  if(exist[k])  num++;
   num = num*60/3;
   sethz_color(BLACK);
   if(num)  { show_hzs(x+2, YY+num+8, "智");  show_hzs(x+2, YY+num*2+8, "育"); }
   sethz_color(WHITE);
   show_hzs(x1+10, y1-10, "学号：      姓名：        学分：");
   sethz_color(YELLOW);
   show_nums(x1+70, y1-25, ptr->Xh, 3, 0);
   sethz_color(YELLOW);
   show_hzs(x1+210, y1-10, ptr->Name);
   show_nums(x1+380, y1-25, xf[0]+xf[1]+xf[2]+xf[3]+xf[4], 7, 4);
   out:show_sl();
   mouse_on();
   if(ERR==FALSE)  { start[inexist] = bstart;  input_js(); }
 }

void  show_watch(int N)
 { double sum=0, data, DATA;
   int x1, y1, x2, y2;
   ST_MSG  *ptr = find_point(BeginL);
   int k, i, j, Nx, Ny;
   Ny = (N-2)/1000;  Nx = (N-2)%1000;
   x1 = X1+20; x2 = X2-20;
   y1 = Y1+70; y2 = Y2-20;
   ERR = TRUE;
   mouse_off();   clear_bar();   show_paper(X1+10, Y1+10, X2-10, Y2-9);
   if(ptr==NULL)
     { sethz_color(BLACK);
       show_hzs(180, 240, "当前无数据可供浏览，请点击“录入”先输入数据！");
       mouse_on();  return;
     }

   ptr = Home->Next;
   while(ptr!=NULL)
    {if(N==-2)
	{ sum += get_xf(ptr);
	  if(ERR==FALSE)  goto out;
	}
      else if(Ny==0)  sum += ptr->Dy[Nx];
      else if(Ny==1)  sum += ptr->Zy[Nx];
      else if(Ny==2)  sum += ptr->Fj[Nx];
      ptr = ptr->Next;
    }
   setcolor(YELLOW);
   show_nums(x1+100, y1-30, sum, 7, 4);
   show_nums(x1+355, y1-30, sum/STnum, 7, 4);
   sethz_color(LIGHTCYAN);
   show_hzs(x1+190, y1-35, "浏览－");
   show_hzs(x1+245, y1-35, N==-2?"学分":Ny==0?DY[Nx].Name:Ny==1?
		ZY[Nx].Name:FJ[Nx].Name);
   setcolor(BLACK);  setlinestyle(0, 0, 1);
   rectangle(x1, y1, x2, y2);
   for(k=0; k<14; k++)   line(x1, y1+20*k, x2, y1+20*k);
   for(k=0; k<6;  k++)   line(x1+81*k, y1, x1+81*k, y2);
   ptr = find_point(BeginL);
   sethz_color(BLACK);
   for(j=0; j<14; j++)
     for(i=0; i<3; i++)
       { if(ptr==NULL)  goto out;
	 show_hzs( x1+i*162+15, y1+j*20+18, ptr->Name);
	 if(N==-2)
	   { DATA = get_xf(ptr);
	     if(ERR==FALSE)  goto out;
	     show_nums(x1+i*162+90, y1+j*20+3, DATA, 7, 4);
	   }
	 else if(Ny==0)  show_nums(x1+i*162+90, y1+j*20+3, ptr->Dy[Nx], 4, 1);
	 else if(Ny==1)  show_nums(x1+i*162+90, y1+j*20+3, ptr->Zy[Nx], 4, 1);
	 else if(Ny==2)  show_nums(x1+i*162+90, y1+j*20+3, ptr->Fj[Nx], 6, 2);
	 ptr = ptr->Next;
       }
   out:
   if(ERR==FALSE)
    { start[inexist] = bstart;  input_js(); return; }
   sethz_color(WHITE);
   show_hzs(x1+50, y1-15, "总分：            平均分：");
   mouse_on();
 }

double  get_xf(ST_MSG *ptr)
 { int k;  double xf=0;
   renew_ptrdata();  renew_sdata(ptr);
   for(k=0; k<DYnum; k++)
    { xf += js_xf(PTR[0].str, ptr->Dy[k], 0);
      if(ERR==FALSE)  { inexist = 0; return 1.0; }
    }
   for(k=0; k<ZYnum; k++)
    { if(ZY[k].Attrib!=0)  continue;
      xf += js_xf(PTR[1].str, ptr->Zy[k], ZY[k].Xf);
      if(ERR==FALSE)  { inexist = 1; return 1.0; }
    }
   for(k=0; k<ZYnum; k++)
    { if(ZY[k].Attrib!=1)  continue;
      xf += js_xf(PTR[2].str, ptr->Zy[k], ZY[k].Xf);
      if(ERR==FALSE)  { inexist = 2; return 1.0; }
    }
   for(k=0; k<ZYnum; k++)
    { if(ZY[k].Attrib!=2)  continue;
      xf += js_xf(PTR[3].str, ptr->Zy[k], ZY[k].Xf);
      if(ERR==FALSE)  { inexist = 3; return 1.0; }
    }
   for(k=0; k<FJnum; k++)
     { xf += js_xf(PTR[4].str, ptr->Fj[k], 0);
       if(ERR==FALSE) { inexist = 4; return 1.0; }
     }
   return xf;
 }

void  change_name(int code)
 { int len = strlen(filename);
   while(--len) if(filename[len]=='\\')  break;
   setfillstyle(1, deskcolor);
   if(code==0) bar(50, 32, 150, 45);
   settextstyle(0, 0, 1);
   setcolor(LIGHTGREEN);
   sethz_color(LIGHTRED);
   if(kind<2) show_hzs(msg*180+135, 44, "★");
   settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy(55, 34, filename+len);
   settextstyle(2, 0, 6);
 }

void  format_name(char *S)
 { char sroad[30], smain[30], skz[30];
   int k, i=0, i0, len = strlen(S);
   for(k=len-1; k>=0; k--)
    { if(S[k]=='\\'||S[k]==':')  break;
    }
   if(k < 0) clear_str(sroad);
   else
    { for(; i<=k; i++)  sroad[i] = S[i];
      sroad[i] = '\0';
    }
   i0 = 0;
   for(; S[i]!='\0'; i++)
    { smain[i0] = S[i];
      if(S[i]=='.')   { smain[i0] = '\0'; break; }
      i0 ++;
    }
   if(S[i]=='\0')  { smain[i0] = '\0'; clear_str(skz); }
   else
    { i0 = 0;
      for(; S[i]!='\0'; i++)	skz[i0++] = S[i];
      skz[i0] = '\0';
    }
   smain[8] = '\0';  skz[4] = '\0';
   strcpy(S, sroad);
   strcat(S, smain);
   if(strlen(skz)==0)  strcat(S, ".st");
   else                strcat(S, skz);
 }

void  show_time(int code)
 { static struct time TIME0;
   struct time TIME;
   char str[10];
   if(code==0)  show_date(0);
   gettime(&TIME);
   if(TIME.ti_sec==TIME0.ti_sec&&code==0)  return;
   str[0] = TIME.ti_hour/10+'0';  str[1] = TIME.ti_hour%10+'0';
   str[2] = ':';
   str[3] = TIME.ti_min/10+'0';   str[4] = TIME.ti_min%10+'0';
   str[5] = ':';
   str[6] = TIME.ti_sec/10+'0';   str[7] = TIME.ti_sec%10+'0';
   str[8] = '\0';
   if(imouse_inbox(520, 407, 639, 447)) mouse_off();
   setfillstyle(1, RED);
   bar(540, 427, 639, 447);
   setcolor(WHITE);
   settextstyle(1, 0, 2);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(590, 435, str);
   if(imouse_inbox(520, 407, 639, 447))   mouse_on();
   settextstyle(2, 0, 6);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   TIME0 = TIME;
   if(T>0 && (saveflag==1||jsflag==1) && !AT && strcmp(filename, "NONAME")
	       && clock()-T0>=T*60*18.2)
     { char far *ptr;  int hz_color = gethz_color();
       struct fillsettingstype  fillseting;
       int  x1 = 320-100, x2 = 320+100, y1 = 240-20, y2 = 240+20;
       long  T = clock();
       leftbutton_click(22);   T0 = clock();
       saveflag = 0;
       ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
       if(ptr==NULL)  return ;
       mouse_off(); clear_bar();
       getimage(x1, y1, x2+5, y2+5, ptr);
       getfillsettings(&fillseting);
       setfillstyle(1, DARKGRAY);
       bar(x1+5, y1+5, x2+5, y2+5);
       setfillstyle(1, GREEN);
       bar(x1, y1, x2, y2);
       sethz_color(RED);
       show_sound();
       show_hzs(x1+30, y1+30, "自动保存提示……");
       mouse_on();  clear_bar();
       while(clock()-T<20L)
	 if(kbhit()||getmousebutton()!=NOBUTTON)  break;
       mouse_off();
       putimage(x1, y1, ptr, COPY_PUT);
       mouse_on();
       farfree(ptr);
       sethz_color(hz_color);
       setfillstyle(fillseting.pattern, fillseting.color);
     }
 }

/*滚动条函数*/
int  scroll(int x1,int y1,int x2,int y2, int max, int n)
 { int X, Y, X0, Y0, _x=0, _y=0;   static int BX=-1, BY=-1;
   int code = abs(x1-x2)>abs(y1-y2); float len;
   if(max<=0||n<0)  return 0;
   if(max<=n)    n = max-1;
   if(abs(x1-x2)>abs(y1-y2))  len = (float)abs(x1-x2)/max;
   else                       len = (float)abs(y1-y2)/max;
   getmouse_xy(&X0,&Y0);
   if(BX==-1&&BY==-1)
     { if(code==1)  BX = x1+len*(n);
       else         BY = y1+len*(n);
     }
   do
     { int flag = 0;
       getmouse_xy(&X,&Y);  show_time(0);
       if(X==_x&&Y==_y)  continue;
       setfillstyle(1, MAGENTA);
       if(code==1)
	 { BX+=(X-X0);
	   if(BX<x1)  BX=x1; else if(BX+len>x2) BX=x2-len; else flag = 1;
	 }
       else
	 { BY+=(Y-Y0);
	   if(BY<y1)  BY=y1; else if(BY+len>y2) BY=y2-len; else flag = 1;
	 }
       mouse_off();
       if(code==1)  bar(BX,y1,BX+len,y2);
       else         bar(x1,BY,x2,BY+len);
       setfillstyle(1, LIGHTBLUE);
       if(code==1)  { bar(x1,y1,BX-1,y2);  bar(BX+len+1,y1,x2,y2); }
       else         { bar(x1,y1,x2,BY-1);  bar(x1,BY+len+1,x2,y2); }
       mouse_on();
       if(code==1)  n = (BX-x1+len/2)/len;
       else         n = (BY-y1+len/2)/len;
       if(flag==1) { X0 = X;  Y0 = Y; }
       _x = X;  _y = Y;
     } while(getmousebutton()==LEFTBUTTON);
   show_scroll(x1, y1, x2, y2, max, n);
   BX = BY = -1;
   return n;
 }

void  show_scroll(int x1, int y1, int x2, int y2, int max, int n)
  { float len;
    mouse_off();
    setfillstyle(1, LIGHTBLUE);    bar(x1, y1, x2, y2);
    if(max<=0||n<0)  return;
    if(max<=n)    n = max-1;
    if(abs(x1-x2)>abs(y1-y2))  len = (float)abs(x1-x2)/max;
    else                       len = (float)abs(y1-y2)/max;
    setfillstyle(1, MAGENTA);
    if(abs(x1-x2)>abs(y1-y2))
		 bar(x1+len*(n), y1, x1+len*(n+1), y2);
    else         bar(x1, y1+len*(n), x2, y1+len*(n+1));
    mouse_on();
  }

int  scroll_move()
 { int k, num=0, flag; int x1, y1, x2, y2; int H=-1, _x;
   for(k=0; k<5; k++)  if(exist[k])  num++;
   x1 = X1+20+38;  x2 = X2-21-80;
   y1 = (Y2-Y1-num*60)/2+Y1;  y2 = y1+num*60;
   y1 += 15;  y2 += 15;
   for(k=0; k<5; k++)
     { int M = k==4?5:8;
       if(exist[k]==0)  continue;
       H++;  _x = x2-(k==4?60:40);
       if(imouse_inbox(x1,y1+60*H+25,_x,y1+60*H+45))
	 if(exist[k]>M)
	   { scroll_see(x1,y1+H*60+30,_x,y1+H*60+40,1,exist[k]-M+1,bsee[k]);
	     clear_bar();     return k;
	   }
	 else  { clear_scroll();  return -1; }
     }
   clear_scroll(); flag = -2;
   return flag;
 }

void  scroll_see(int x1, int y1, int x2, int y2, int code, int max, int n)
 { static int bcode, bx1, by1, bx2, by2;
   static char far *ptr=NULL;
   if(code==-1&&bcode==-1)  return;
   mouse_off();
   clear_bar();
   if(code==-1)
     {  if(ptr!=NULL)
	  { putimage(bx1,by1,ptr,COPY_PUT);  farfree(ptr); ptr=NULL; }
	goto end;
     }
   if(x1==bx1&&x2==bx2&&y1==by1&&y2==by2)  return;
   if(ptr!=NULL) { putimage(bx1,by1,ptr,COPY_PUT); farfree(ptr); }
   ptr = (char far*)farmalloc(imagesize(x1,y1,x2+2,y2+2));
   getimage(x1, y1, x2+2, y2+2, ptr);
   setfillstyle(1, DARKGRAY);   bar(x1+2,y1+2,x2+2,y2+2);
   show_scroll(x1, y1, x2, y2, max, n);
   end:   bx1 = x1;  bx2 = x2;  by1 = y1;  by2 = y2; mouse_on();
 }


void  show_sl()
 { int k, num=0, H=-1;
   int x1,y1,x2;
   for(k=0;k<5;k++)  if(exist[k])  num++;
   x1 = X1+20+38, x2 = X2-21-80;
   y1 = (Y2-Y1-num*60)/2+Y1+15;
   sethz_color(LIGHTGREEN);
   settextjustify(CENTER_TEXT,CENTER_TEXT);
   for(k=0;k<5;k++)
    { if(exist[k])  H++;
      if(exist[k]<=(k==4?5:8))  continue;
      if(bsee[k]) show_hzs(x1-10, y1+H*60+40, "←");
      if(bsee[k]+(k==4?5:8)<exist[k])
		  show_hzs(x2-(k!=4?60:80)+12, y1+H*60+40, "→");
    }
 }

double  get_data(char *S, float data, float xf)
 { double DATA = 0, flo;  int flag = 0;
   if(ERR == FALSE)  return (float)1.0;
   if(S[K]=='+'||S[K]=='*'||S[K]=='/'||S[K]=='%')
     { msg_box(320, 240, "错误：",
	     "计算过程发生意外错误－－运算符前没有数字！"
	     "请仔细检查计算公式……", 1);
       ERR = FALSE;  return 1.0;
     }
   if(S[K]=='-')
     { K++; return -get_data(S, data, xf); }
   while(S[K]!='\0'&&S[K]!='[')
    { switch(S[K])
       { case '+': K++; DATA += get_DATA(S, data, xf); break;
	 case '-': K++; DATA -= get_DATA(S, data, xf); break;
	 case '*': K++; DATA *= get_DATA(S, data, xf); break;
	 case '/': K++; flo = get_DATA(S, data, xf);
			if(flo==0)
			  { msg_box(320, 240, "错误：",
			      "计算过程发生意外错误－－除数为零！"
			      "请仔细检查计算公式……", 1);
			    ERR = FALSE;  return 1.0;
			  }
			DATA /= flo; break;
	 case '(': if(flag)
		       { msg_box(320, 240, "错误：",
			      "计算过程发生意外错误－－缺少运算符！"
			      "请仔细检查计算公式……", 1);
			 ERR = FALSE;  return  1.0;
		       }
		   K++; DATA =  get_data(S, data, xf); break;
	 case ')': K++; return DATA;
	 default : if(flag)
		       { msg_box(320, 240, "错误：",
			      "计算过程发生意外错误－－缺少运算符！"
			      "请仔细检查计算公式……", 1);
			 ERR = FALSE;  return  1.0;
		       }
		    DATA = get_DATA(S, data, xf);  break;
       }
      flag = 1;
      if(ERR==FALSE)  return 1.0;
    }
   return DATA;
 }

double  get_DATA(char *S, float data, float xf)
 { int d = 0, f = 0, i;
   double DATA = 0;
   if(ERR == FALSE)  return 1.0;
   switch(S[K])
    { case 'S': case 's': K++;
	switch(S[K++])
	  { case 'D': case 'd': return SD;  case 'B': case 'b': return SB;
	    case 'X': case 'x': return SX;  case 'R': case 'r': return SR;
	    case 'F': case 'f': return SF;
	    default: msg_box(320, 240, "错误：",
		      "计算公式有误，发现未允许的总学分变量Ｓ…！"
		      "请仔细检查计算公式……", 1);
		     ERR = FALSE;  return 1.0;
	  }
      case 'N': case 'n': K++;
	switch(S[K++])
	  { case 'D': case 'd': return (float)exist[0];
	    case 'B': case 'b': return (float)exist[1];
	    case 'X': case 'x': return (float)exist[2];
	    case 'R': case 'r': return (float)exist[3];
	    case 'F': case 'f': return (float)exist[4];
	    default: msg_box(320, 240, "错误：",
		      "计算公式有误，发现未允许的科目总数变量Ｎ…！"
		      "请仔细检查计算公式……", 1);
		     ERR = FALSE;  return 1.0;
	  }
      case 'M': case 'm': K++;
	switch(S[K++])
	   { case 'B': case 'b': return MB;  case 'X': case 'x': return MX;
	     case 'R': case 'r': return MR;
	     default: msg_box(320, 240, "错误：",
		      "计算公式有误，发现未允许的学分满分总分变量Ｍ…！"
		      "请仔细检查计算公式……", 1);
		      ERR = FALSE;  return 1.0;
	   }
      case 'x':  case 'X': K++; return data;
      case 'f':  case 'F': K++;
		 if(xf<0)
		   { msg_box(320, 240, "错误：",
			      "计算数据出现问题：错误引用学分变量ｆ！"
			      "请仔细检查计算公式……", 1);
		     ERR = FALSE;  return 1.0;
		   }
		 return xf;
      case '(':  K++; return get_data(S, data, xf);
      default: if(!(S[K]>='0'&&S[K]<='9'||S[K]=='-'))
		   { msg_box(320, 240, "错误：",
			  "计算公式有误，发现未允许的数字形式！"
			  "请仔细检查计算公式……", 1);
		     ERR = FALSE;  return 1.0;
		   }
	       if(S[K]=='-')
		 { f = 1; K++;
		   if(!(S[K]>='0'&&S[K]<='9'))  return -get_DATA(S, K, xf);
		 }
	       while(S[K]>='0'&&S[K]<='9'||S[K]=='.'||S[K]=='%')
		 { if(S[K]=='.')
		      { if(d==0)   d = 1;
			else { msg_box(320, 240, "错误：",
			       "计算过程发生意外错误：小数点错误！"
			       "请仔细检查计算公式……", 1);
				  ERR = FALSE;  return 1.0;
			     }
		       }
		   else if(S[K]=='%')  { DATA /= 100;  K++; break; }
		   else
		     { DATA = DATA*10+(float)(S[K]-'0');
		       if(d)  d++;
		     }
		   K ++;
		 }
	       if(f==1)  DATA = -DATA;
	       if(d==0)   return DATA;
	       for(i=0; i<d-1; i++)   DATA /= 10;
	       return DATA;
   }
 }

void renew_ptrdata()
 { int i; MB = MX = MR = 0;
   for(i=0; i<ZYnum; i++)
    {      if(ZY[i].Attrib==0)  MB += ZY[i].Xf;
      else if(ZY[i].Attrib==1)  MX += ZY[i].Xf;
      else                      MR += ZY[i].Xf;
    }
   exist[0] = DYnum;
   exist[1]=exist[2]=exist[3]=0;
   for(i=0; i<ZYnum; i++)
    {       if(ZY[i].Attrib==0)  exist[1]++;
      else  if(ZY[i].Attrib==1)  exist[2]++;
      else                       exist[3]++;
    }
   exist[4] = FJnum;
 }

void  renew_sdata(ST_MSG *ptr)
 { int i;
   SD = 0;
   for(i=0; i<DYnum; i++)  SD += ptr->Dy[i];
   SB = SX = SR = 0;
   for(i=0; i<ZYnum; i++)
    {      if(ZY[i].Attrib==0)  SB += ptr->Zy[i];
      else if(ZY[i].Attrib==1)  SX += ptr->Zy[i];
      else                      SR += ptr->Zy[i];
    }
   SF = 0;
   for(i=0; i<FJnum; i++)  SF += ptr->Fj[i];

 }

#define min(x,y)  (x<y?x:y)
#define max(x,y)  (x>y?x:y)
double js_xf(char *S, float data, float xf)
 { float A, B, _MIN, _MAX;  int flag1, flag2;  int k , n = 0;
   K = 0;  ERR = TRUE;  bstart = 0;
   while(S[K]!='\0')
    { A = B = 0;  flag1 = flag2 = 0;
      while(S[K]!='[')  K++;
      K++;
      if(S[K]==',')  flag1 = 1;
      else           A = get_DATA(S, data, xf);
      if(ERR==FALSE)  return  (float)1.0;
      K++;
      if(S[K]==']')  flag2 = 1;
      else           B = get_DATA(S, data, xf);
      if(ERR==FALSE)  return  (float)1.0;
      K++;
      _MIN = min(A,B);  _MAX = max(A,B);
      if((data>=_MIN||flag1==1) && (data<=_MAX||flag2==1))
	{ k = K;
	  while(S[k]!='['&&S[k]!='\0')
	    { if(S[k] == '(')   n ++;
	      if(S[k] == ')')   n --;
	      k ++;
	    }
	  if(n == 0)
	    return get_data(S, data, xf);
          msg_box(320, 240, "错误：",
			  "计算公式有误，括号不匹配！", 1);
		     ERR = FALSE;  return (float)1.0;
	 }
      else  bstart ++;
    }
   return 0.0;
 }

#define  Y  120
void  input_js()
 { int x1, y1, x2, y2;  int flag = 0, tab = 0;
   char far *ptr;  int code, k, j, begin, end;
   char  *p[5];  int num[5], bstart[5];
   char str[100], left[20], right[20];
   x1 = 320-200; x2 = 320+200;
   y1 = 240-120; y2 = 240+120;
   ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return;
   for(k=0; k<5; k++)
     {  p[k] = (char*)malloc(PTR[k].num);
	strcpy(p[k], PTR[k].str);
	num[k] = PTR[k].num;
	bstart[k] = start[k];
     }
   mouse_off();  clear_bar();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, GREEN);
   bar(x1, y1, x2, y2);
   sethz_color(LIGHTCYAN);
   show_hzs(x1+55, y1+20, "德育  必修  限选  任选  附加");
   sethz_color(BLUE);
   show_hzs(x1+10, y1+53, "条件：      （正负无穷请保留空缺状态）");
   show_hzs(x1+10, y1+72, "当ｘ值介于      时和      之间时：");
   show_hzs(x1+10, y1+94, "应用计算公式");
   setfillstyle(1, DARKGRAY);
   bar(x1+110, y1+75, x2-10, y1+95);
   bar(x1+95, y1+53, x1+180, y1+73);
   bar(x1+222, y1+53, x1+312, y1+73);
   sethz_color(RED);
   show_hzs(x1+10, y1+Y*1+16, "原始总分Ｓ");
   show_hzs(x1+10, y1+Y+2*16, "科目总数Ｎ");
   show_hzs(x1+10, y1+Y+3*16, "学分满分Ｍ");
   show_hzs(x1+90, y1+Y, "德育Ｄ 必修Ｂ 限选Ｘ 任选Ｒ 附加Ｆ");
   sethz_color(YELLOW);
   show_hzs(x1+100, y1+Y+1*16, "ＳＤ  ＳＢ  ＳＸ  ＳＲ  ＳＦ");
   show_hzs(x1+100, y1+Y+2*16, "ＮＤ  ＮＢ  ＮＸ  ＮＲ  ＮＦ");
   show_hzs(x1+100, y1+Y+3*16, "－－  ＭＢ  ＭＸ  ＭＲ  －－" );
   setcolor(papercolor);   setlinestyle(2, 0, 1);
   sethz_color(RED);
   show_hzs(x1+50, y1+Y+4*16+5, "＋  －  ＊  ／  （  ）  ％");
   show_hzs(x1+16, y1+Y+5*16+5, "ｘ代表要计算的原始数据  ｆ代表该科的规定学分");
   sethz_color(BLUE);
   show_hzs(x1+25, y1+Y+6*16+20, "确定  取消  新添  删除");
   show_hzs(x1+275, y1+Y+6*16+20, "上一项 下一项");
   line(x1+10, y1+Y+53, x2-10, y1+Y+53);
   line(x1+10, y1+Y-20, x2-10, y1+Y-20);
   line(x1+10, y2-30, x2-10, y2-30);
   line(x1+10, y1+24, x2-10, y1+24);
   setlinestyle(0, 0, 1);
   if(start[inexist]>chnum(PTR[inexist].str))   start[inexist]=chnum(PTR[inexist].str);
   inline_str(start[inexist],PTR[inexist].str,str,left,right,&begin,&end);
   show_inline(start[inexist], chnum(PTR[inexist].str), str, left, right);
   bkcolor = DARKGRAY;
   show_ptr1(inexist);
   mouse_on();
 BEGIN
	if(test_in(x1+110, y1+75, x2-10, y1+95)==1)  code = 10;
   else if(test_in(x1+95, y1+53, x1+180, y1+73)==1)  code = 11;
   else if(test_in(x1+222, y1+53, x1+312, y1+73)==1) code = 12;
   else
     { for(k=0; k<5; k++)
	if(test_in(x1+40+k*64, y1+3, x1+40+(k+1)*64, y1+20)==1)
	  { code = k; break; }
	if(k==5)
	  for(k=0; k<6; k++)
	   if(test_in(x1+17+64*k, y1+218, x1+17+64*k+50, y1+238)==1)
	     { code = 20+k; break; }
	   if(k==6)  { code = -1;  clear_bar(); }
     }

   show_time(0);
   if(!kbhit()&&getmousebutton()!=LEFTBUTTON)  continue;
   clear_bar();
   if(getmousebutton()!=LEFTBUTTON)  code = -1;
   if(kbhit())
    switch((unsigned char)getch())
     { case ENTER_KEY: goto out;
       case ESC_KEY:   flag = -1; goto out;
       case TAB_KEY:   tab++; if(tab==3)  tab = 0;
		       code = 10 + tab;   goto click;
       case LEFT_KEY:  code = 11; goto click;
       case RIGHT_KEY: code = 12; goto click;
       case UP_KEY: case DOWN_KEY: code = 10; goto click;
       case PAGEUP_KEY: case CTRL_PAGEUP: code = 24; goto click;
       case PAGEDOWN_KEY: case CTRL_PAGEDOWN: code = 25; goto click;
     }
   if(code == -1)  continue;
   if(getmousebutton()==LEFTBUTTON)
    { click:
      if(code==10||code==11||code==12||code==23)
	{ int k = end, i = begin;
	  if(chnum(PTR[inexist].str)==1&&code==23)  continue;
	  while(PTR[inexist].str[k]!='\0')
	     PTR[inexist].str[i++] = PTR[inexist].str[k++];
	  PTR[inexist].num -= end-begin;
	  PTR[inexist].str = (char *)realloc(PTR[inexist].str, PTR[inexist].num);
	  PTR[inexist].str[PTR[inexist].num-1] = '\0';
	  PTR[inexist].num = strlen(PTR[inexist].str)+1;
	}
      while(getmousebutton()!=NOBUTTON) show_time(0);
	switch(code)
	 { case 20:  goto out;
	   case 21:  flag = -1; goto out;
	   case 22:  if(!(bioskey(2)&0x04))
		       { while(PTR[inexist].str[++begin]!='['&&
			   PTR[inexist].str[begin]!='\0') ;
			 start[inexist]++;
		       }
		     clear_str(str); clear_str(left); clear_str(right); break;
	   case 24:  start[inexist] -= (bioskey(2)&0x04)? 5 : 1;
		     if(start[inexist]<0)  start[inexist] = 0; break;
	   case 25:  start[inexist] += (bioskey(2)&0x04)? 5 : 1;
		     if(start[inexist]>chnum(PTR[inexist].str)-1)
			  start[inexist] = chnum(PTR[inexist].str)-1;  break;
	   case 10:  mouse_off();  clear_bar();  mouse_on();  tab = 0;
		     Inword(x1+115, y1+75, str, 27);  break;
	   case 11:  mouse_off();  clear_bar();  mouse_on();  tab = 1;
		     Inword(x1+100, y1+53, left, 7);  break;
	   case 12:  mouse_off();  clear_bar();  mouse_on();  tab = 2;
		     Inword(x1+227, y1+53, right, 7);  break;
	   case 0: case 1: case 2: case 3: case 4: inexist = code;
		     if(start[inexist]>chnum(PTR[inexist].str))
				      start[inexist]=chnum(PTR[inexist].str);
		     show_ptr1(inexist);
		     break;
	}
      if(code==10||code==11||code==12||code==22)
	{ char bstr[100]; int k, len;
	  sprintf(bstr, "[%s,%s]%s", left, right, str);
	  len = strlen(bstr);
	  PTR[inexist].num += len;
	  PTR[inexist].str = (char *)realloc(PTR[inexist].str, PTR[inexist].num);
	  for(k=0; k<PTR[inexist].num-begin; k++)
	    PTR[inexist].str[PTR[inexist].num-k-1] =
			PTR[inexist].str[PTR[inexist].num-len-k-1];
	  for(k=0; k<len; k++)
	    PTR[inexist].str[begin+k] = bstr[k];
	  PTR[inexist].str[PTR[inexist].num-1] = '\0';
	  PTR[inexist].num = strlen(PTR[inexist].str)+1;
	}
     }
   if(start[inexist]>=chnum(PTR[inexist].str)-1)
	  start[inexist] = chnum(PTR[inexist].str)-1;
   inline_str(start[inexist],PTR[inexist].str,str,left,right,&begin,&end);
   show_inline(start[inexist], chnum(PTR[inexist].str), str, left, right);

   while(kbhit())  getch();
   while(getmousebutton()!=NOBUTTON) show_time(0);
 WEND
 out:
   mouse_off();  clear_bar();
   putimage(x1, y1, ptr, COPY_PUT);
   farfree(ptr);
   if(flag==-1)
     for(k=0; k<5; k++)
       { strcpy(PTR[k].str, p[k]);
	 PTR[k].num = num[k];
	 start[k] = bstart[k];
       }
   else
     { for(k=0; k<5; k++)
	 if(strcmp(PTR[k].str, p[k])!=0)  break;
       if(k!=5)  jsflag = 1;
     }
   for(k=0; k<5; k++)   free(p[k]);
   if(flag==0)
     if(kind==3 || kind==2&&watch==-2)  deal_kind(kind);
   return;
 }

int  inline_str(int n, char *S, char *str, char *left,
				char *right, int *begin, int *end)
 { int K=0, n0=-1, k=0;
   while(S[K]!='\0')
    { if(S[K]=='[') n0++;
      if(n0==n)  break;
      K++;
    }
   if(S[K]=='\0')   return -1;
   *begin = K;
   if(S[K+1]==']')
      { clear_str(left);  clear_str(right); K+=2; }
   else
    {  K++; k = 0;
       while(S[K]!=',')   left[k++] = S[K++];
       left[k] = '\0';
       K++; k = 0;
       while(S[K]!=']')   right[k++] = S[K++];
       right[k] = '\0';
       K++;
    }
   k = 0;
   while(S[K]!='\0'&&S[K]!='[')   str[k++] = S[K++];
   str[k] = '\0';
   *end = K;
   return 1;
 }

int  chnum(char *S)
 { int K=-1, num=0;
   while(S[++K])   if(S[K]=='[')  num++;
   return num;
 }

void  show_inline(int now, int max, char *str, char *left, char *right)
 { int x1, y1, x2;  char s[20];
   x1 = 320-200; x2 = 320+200;
   y1 = 240-120;
   setcolor(WHITE);
   settextstyle(2, 0, 6);
   setfillstyle(1, DARKGRAY);
   mouse_off();
   bar(x1+110, y1+75, x2-10, y1+95);
   bar(x1+95, y1+53, x1+180, y1+73);
   bar(x1+222, y1+53, x1+312, y1+73);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy(x1+115, y1+75, str);
   outtextxy(x1+100, y1+53, left);
   outtextxy(x1+227, y1+53, right);
   setfillstyle(1, GREEN);
   bar(x1+55, y1+31, x1+140, y1+51);
   sprintf(s, "%d/%d", now+1, max);
   outtextxy(x1+60, y1+34, s);
   mouse_on();
 }

void  show_sound()
 { if(SOUND == 0)  return;
   sound(600);  delay(50);
   sound(700);  delay(50);
   sound(500);  delay(50);
   nosound();
 }

void  set_time()
 { int x1 = 320-150,       x2 = 320+150,
       y1 = 240-70,        y2 = 240+70;
   int code, k, d, changeflag = 0;
   int _N,_Y,_R,_S,_F,_M;
   struct date _date;
   struct time _time;
   char far *ptr;
   char str[][3] = { "年","月","日","时","分","秒"};
   getdate(&_date);
   gettime(&_time);
   _N = _date.da_year;  _Y = _date.da_mon;  _R = _date.da_day;
   _S = _time.ti_hour;  _F = _time.ti_min;  _M = _time.ti_sec;
   ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return;
   mouse_off();  clear_bar();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, GREEN);
   bar(x1, y1, x2, y2);
   setcolor(WHITE);
   rectangle(x1+3, y1+3, x2-3, y2-3);
   setfillstyle(1, RED);
   bar(x1+30-4, y1+40, x1+70, y1+55);
   for(k=0; k<5; k++)  bar(x1+90+k*40-4, y1+40, x1+110+k*40, y1+55);
   show_nums(x1+30,   y1+40, _N, 4, 0);
   show_nums(x1+90,   y1+40, _Y, 2, 0);
   show_nums(x1+130,  y1+40, _R, 2, 0);
   show_nums(x1+170,  y1+40, _S, 2, 0);
   show_nums(x1+210,  y1+40, _F, 2, 0);
   show_nums(x1+250,  y1+40, _M, 2, 0);
   sethz_color(BLUE);
   show_hzs(x1+40, y1+30, "年");
   for(k=0; k<5; k++)  show_hzs(x1+90+40*k, y1+30, str[k+1]);
   show_hzs(x1+40, y1+80,  "↑");
   for(k=1; k<6; k++)  show_hzs(x1+50+40*k, y1+80,  "↑");
   show_hzs(x1+40, y1+100, "↓");
   for(k=1; k<6; k++)  show_hzs(x1+50+40*k, y1+100, "↓");
   show_hzs(x1+75, y1+130, "确定  取消  应用");
   mouse_on();
BEGIN
   show_time(0);
   if(test_in(x1+35, y1+60, x1+40+25, y1+80)==1)  code = 0;
   else
    { for(k=1; k<6; k++)
       if(test_in(x1+45+40*k, y1+60, x1+50+40*k+25, y1+80)==1)  break;
      if(k!=6)   code = k;
      else if(test_in(x1+35, y1+80, x1+40+25, y1+100)==1)  code = 10;
      else
       { for(k=1; k<6; k++)
	  if(test_in(x1+45+40*k, y1+80, x1+50+40*k+25, y1+100)==1) break;
	 if(k!=6)  code = k+10;
	 else
	  { for(k=0; k<3; k++)
	     if(test_in(x1+75-10+k*64, y1+110, x1+75+40+k*64, y1+135)==1) break;
	    if(k!=3)  code = 20+k;
	    else { code = -1; clear_bar(); }
	  }
       }
    }
   if(bioskey(2)&0x04)  d = 10;
   else                 d = 1;
   if(!kbhit() && getmousebutton()!=LEFTBUTTON)  continue;
   if(code<20)  changeflag = 1;
   switch(code)
     { case 0:  _N += d; if(_N>=9999)  _N = 9999;
       case 10: if(code==10) { _N -= d; if(_N<=1980)  _N = 1980; }
		bar(x1+30-4, y1+40, x1+70, y1+55);
		show_nums(x1+30, y1+40, _N, 4, 0);
		break;
       case 1:  _Y += d; if(_Y>=12)   _Y = 12;
       case 11: if(code==11) { _Y -= d; if(_Y<=1)  _Y = 1; }
		bar(x1+90-4, y1+40, x1+110, y1+55);
		show_nums(x1+90, y1+40, _Y, 2, 0);
		break;
       case 2:  _R += d; if(_R>=31)   _R = 31;
       case 12: if(code==12) { _R -= d; if(_R<=1)  _R = 1; }
		bar(x1+130-4, y1+40, x1+150, y1+55);
		show_nums(x1+130, y1+40, _R, 2, 0);
		break;
       case 3:  _S += d; if(_S>=23)   _S = 23;
       case 13: if(code==13) { _S -= d; if(_S<=0)  _S = 0; }
		bar(x1+170-4, y1+40, x1+190, y1+55);
		show_nums(x1+170, y1+40, _S, 2, 0);
		break;
       case 4:  _F += d; if(_F>=59)   _F = 59;
       case 14: if(code==14) { _F -= d; if(_F<=0)  _F = 0; }
		bar(x1+210-4, y1+40, x1+230, y1+55);
		show_nums(x1+210, y1+40, _F, 2, 0);
		break;
       case 5:  _M += d; if(_M>=59)   _M = 59;
       case 15: if(code==15) { _M -= d; if(_M<=0)  _M = 0; }
		bar(x1+250-4, y1+40, x1+270, y1+55);
		show_nums(x1+250, y1+40, _M, 2, 0);
		break;
       case 20:
       case 22: _date.da_year = _N; _date.da_mon = _Y; _date.da_day = _R;
		_time.ti_hour = _S; _time.ti_min = _F; _time.ti_sec = _M;
		if(changeflag||code==22)
		  { setdate(&_date);  settime(&_time); changeflag = 0; }
		if(code == 22)  break;
       case 21: goto out;
     }
   while(getmousebutton()!=NOBUTTON);
   while(kbhit())  getch();
WEND
out:
   mouse_off();  clear_bar();
   putimage(x1, y1, ptr, COPY_PUT);
   farfree(ptr);
   mouse_on();
 }

void  show_date(int code)
 { struct date DATE;
   char str[11]; int color = getcolor();
   static char flag ;
   static char far *ptr;
   if(imouse_inbox(540, 427, 639, 447) && code==0)
    { if(flag == 1)  goto out;
      getdate(&DATE);
      sprintf(str, "%d", DATE.da_year);
      str[4] = '.';
      str[5] = DATE.da_mon/10+'0';   str[6] = DATE.da_mon%10+'0';
      str[7] = '.';
      str[8] = DATE.da_day/10+'0';   str[9] = DATE.da_day%10+'0';
      str[10] = '\0';
      ptr = (char far*)farmalloc(imagesize(510, 397, 609, 417));
      mouse_off();
      clear_bar();
      getimage(510, 397, 609, 417, ptr);
      setfillstyle(1, GREEN);
      bar(510, 397, 609, 417);
      setcolor(RED);
      settextstyle(1, 0, 2);
      settextjustify(CENTER_TEXT, CENTER_TEXT);
      outtextxy(560, 405, str);
      mouse_on();
      flag = 1;
    }
   else
    { if(flag == 0)  goto out;
      mouse_off();
      clear_bar();
      putimage(510, 397, ptr, COPY_PUT);
      mouse_on();
      farfree(ptr);
      flag = 0;
    }
   out:
   setcolor(color);
   settextstyle(2, 0, 6);
   settextjustify(LEFT_TEXT, TOP_TEXT);
 }

void  print_scr(x1, y1, x2, y2)
 int x1, y1, x2, y2;
 { long scr;
   int i, j, k;
   int x, y, flag;
   char byte;
   outportb(0x37A, inportb(0x37A)&(~0x04));
   byte = biosprint(2, 0, 0);
   if(byte&0x01)
     { msg_box(320, 240, "打印机出错：", "设备超时，请检查打印机是否已经连接正确！", 1);
       return;
     }
   if(byte&0x20)
     { msg_box(320, 240, "打印机出错：", "打印机无纸，请放好纸后再重新开始打印！", 1);
       return;
     }
   if(msg_box(320, 240, "打印确认：", "请按回车键开始打印，按ＥＳＣ键取消打印", 0)!=1)
     return;
   mouse_off();  clear_bar();  show_date(1);
   for(k=0; k<=(y2-y1)/24; k++)
    { fprintf(stdprn, "%c%c%c%c%c", 27, 42, 39, (x2-x1+1)%256, (x2-x1+1)/256);
      for(i=x1; i<=x2; i++)
       { scr = 0;
	 for(j=0; j<24; j++)
	  { x = i; y = y1+k*24+j;
	    if(y>y2)  flag = 0;
	    else
	     { if(getpixel(x, y)==papercolor)  flag = 0;
	       else  flag = 1;
	     }
	    if(flag==1)  scr |= 0x80000000L>>j;
	  }
	 for(j=0; j<3; j++)
	   biosprint(0, (char)(scr>>8*(3-j)), 0);
      }
      fprintf(stdprn, "%c%c%c", 27, 51, 24);
      fprintf(stdprn, "%c%c", 13, 10);
    }
  mouse_on(); show_date(0);
  msg_box(320, 240, "", "打印完毕！", 0);
 }


int select_str(int n, int size, char *head, char *str, int *begin)
 { int x1, x2, y1, y2;
   int k, select=0, code;
   int wide, high, exist, d=0, flag = 1;
   char far *ptr;
   float len;
   exist = n>10?10:n;
   if(n>10)  d = n-10;
   if(*begin>d)  *begin = d;
   wide = (strlen(head)>size-1?strlen(head):size-1)*8+40;
   high = exist*16+46+25;
   x1 = 320-wide/2;  x2 = 320+wide/2;
   y1 = 240-high/2;  y2 = 240+high/2;
   len = (float)(y2-y1-10)/(d+1);
   ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return NULL;
   mouse_off();  clear_bar();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, GREEN);
   bar(x1, y1, x2, y2);
   setcolor(WHITE);
   rectangle(x1+3, y1+3, x2-3, y2-3);
   sethz_color(RED);
   for(k=0; k<exist; k++)
     show_hzs(x1+10, y1+10+(k+1)*16+25, str+(*begin+k)*size+1);
   setcolor(LIGHTCYAN);
   line(x1+10, y2-30, x2-27, y2-30);
   line(x1+10, y1+30, x2-27, y1+30);
   sethz_color(YELLOW);
   show_hzs(x1+10, y1+23, head);
   sethz_color(WHITE);
   show_hzs(x1+(wide-48-20)/2, y2-10, "取 消");
   show_scroll(x2-18, y1+5, x2-8, y2-5, d+1, *begin);
   mouse_on(); mouse_on();
BEGIN
   for(k=0; k<exist; k++)
     if(test_in(x1+10, y1+10+k*16+25, x2-32, y1+10+k*16+15+25)==1)
	{ code = k; break; }
   if(k==exist)
    { if(test_in(x1+15, y2-26, x2-32, y2-10)==1)   code = 20;
      else if(test_in(x2-18, y1+5+(int)(len*(*begin)+0.5)-1, x2-8,
	       y1+5+(int)(len*(*begin)+0.5)+len+1)==1)  code = 30;
      else if(imouse_inbox(x2-18, y1, x2-8, y1+5+(int)(len*(*begin)+0.5))) code = 31;
      else if(imouse_inbox(x2-18, y1+5+(int)(len*(*begin)+0.5)+len, x2-8, y2)) code = 32;
      else { code = -1;  clear_bar(); }
    }
   show_time(0);
   if(!kbhit() && getmousebutton()!=LEFTBUTTON)  continue;
   if(kbhit())
     if(getch()==ESC_KEY)  { flag = -1;  break; }
     else  continue;
   if(code == -1)  continue;
   if(code<10) { select = code+(*begin); break; }
   switch(code)
    { case 20: flag = -1; goto out;
      case 30: clear_bar();
	       *begin = scroll(x2-18, y1+5, x2-8, y2-5, d+1, *begin);
      case 31: if(code==31) { *begin -= 3; if(*begin<0)   *begin = 0; }
      case 32: if(code==32) { *begin += 3; if(*begin>d)   *begin = d; }
	       setfillstyle(1, GREEN);
	       bar(x1+10, y1+10+25, x1+10+(size-1)*8, y1+10+exist*16+25);
	       sethz_color(RED);
               for(k=0; k<exist; k++)
		 show_hzs(x1+10, y1+10+(k+1)*16+25, str+(*begin+k)*size+1);
	       clear_bar();
	       show_scroll(x2-18, y1+5, x2-8, y2-5, d+1, *begin);
	       break;
    }
   while(getmousebutton()!=NOBUTTON)  show_time(0);
WEND
   out:
   mouse_off(); clear_bar();
   putimage(x1, y1, ptr, COPY_PUT);
   mouse_on();
   farfree(ptr);
   while(getmousebutton()!=NOBUTTON)  show_time(0);
   if(flag==-1)  return -1;
   return select;
 }

int select_name(char *S)
 { char *ptr;
   int  select;
   int  k, size, num = DYnum+ZYnum+FJnum, k0=0;
   static int begin;
   if(num==0)
    { msg_box(320, 240, "错误：", "无科目可供选择！", 1);
      return 0;
    }
   ptr = (char *)malloc(size = num*9+5);
   if(ptr==NULL)  return 0;
   for(k=0; k<size-1; k++)   ptr[k] = 0;
   k = 0;
   for(k0=0; k0<DYnum; k0++)    strcpy(ptr+(k++)*9+1, DY[k0].Name);
   for(k0=0; k0<ZYnum; k0++)    strcpy(ptr+(k++)*9+1, ZY[k0].Name);
   for(k0=0; k0<FJnum; k0++)    strcpy(ptr+(k++)*9+1, FJ[k0].Name);
   select = select_str(num, 9, "请选择科目名称：", ptr, &begin) ;
   if(select!=-1)  strcpy(S, ptr+select*9+1);
   free(ptr);
   return select==-1?0:1;
 }

int select_stu(char *S)
 { char *ptr;
   int  select;
   static int begin;
   ST_MSG  *pb = Home->Next;
   int k, size, num = STnum;
   if(num==0)
    { msg_box(320, 240, "错误：", "无学生姓名可供选择！", 1);
      return 0;
    }
   ptr = (char *)malloc(size = num*7+5);
   if(ptr==NULL)  return 0;
   for(k=0; k<size-1; k++)  ptr[k] = 0;
   k = 0;
   do
    { strcpy(ptr+7*(k++)+1, pb->Name);
      pb = pb->Next;
    } while(pb!=NULL);
   select = select_str(num, 7, "请选择学生姓名：", ptr, &begin);
   if(select!=-1) strcpy(S, ptr+select*7+1);
   free(ptr);
   return select==-1?0:1;
 }

