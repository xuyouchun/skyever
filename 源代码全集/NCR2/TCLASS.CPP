#include <stdio.h>
#include <bios.h>
#include <graphics.h>
#include <dos.h>
#include <dir.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <mouse.h>
#include <key.h>
#include <ctype.h>
#include "tclass.h"

T_ShowStr  _pn("font\\Font", "font\\Asc");
BOOL Sight = true;
BOOL isound = true;
extern T_Time  _time;


void _delay(int n)
 { long T = clock();
   while(clock()-T<n/30);
 }

void Sound(int code=0)
 { if(isound==false)  return;
   switch(code)
    { case 0: sound(600);   _delay(30);  sound(900);  _delay(30);
	      sound(300);   _delay(30);  break;   //按钮按下
      case 1: sound(1200);  _delay(30);  sound(2000);  _delay(30);
	      sound(1600);  _delay(30);  break;   //菜单弹出
      case 2: sound(800);   _delay(30);  sound(1200);  _delay(30);
	      sound(400);  _delay(30);  break;    //菜单选中
      case 3: sound(2000); delay(5); sound(2500);  delay(5);
	      break;                             //菜单光条移动
      case 4: sound(1000);  _delay(30);  sound(500);  _delay(30);
	      break; 			         //滚动条滑动
    }
   nosound();
 }

void  bar3ds(int x1, int y1, int x2, int y2, BOOL flag=down, int color=-1)
 { if(color!=-1)
    { setfillstyle(1, color);
      bar(x1, y1, x2, y2);
    }
   if(flag==def)  return;
   setcolor(flag==down?DARKGRAY:LIGHTGRAY);
   line(x1, y1, x2, y1);  line(x1, y1, x1, y2);
   setcolor(flag==down?LIGHTGRAY:DARKGRAY);
   line(x2, y2, x1, y2);  line(x2, y2, x2, y1);
 }


//汉字

void T_ShowHz::zoom(int _zoomx, int _zoomy)  //设置横向和纵向缩放比例
 { if(_zoomx!=CURSET)   zoomx = _zoomx;
   if(_zoomy!=CURSET)   zoomy = _zoomy;
 }

void T_ShowStr::qshowstr(int x, int y, char *str, int color, char begin, char end)
 { if(Sight==false)  return;
   int K = -1; int X = x, Y = y;  int ch;
   FILE *ascfp = Asc.fp,  *hzfp = Hz.fp;
   if(begin==NOSEL && end!=NOSEL)  begin = 0;
   outportb(0x3ce, 4);
   outportb(0x3c5, color);
   while((ch=str[++K])!=0)
     { if(ch>=32&&ch<127)
	{ unsigned char *ptr0 = Asc.asck+((ch-32)<<4);
	  unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*Y+(X>>3)+1;
	  if(K<=end&&K>=begin)
	    { register j;
	      for(j=0; j<16; j++)    { *ptr = ~*ptr0++;  ptr += 80; }
	    }
	  else
	    { register j;
	      for(j=0; j<16; j++)    { *ptr = *ptr0++;  ptr += 80; }
	    }
	  X += 8;
	}
       else if(ch&0x80)
	{ if((str[K+1]&0x80)==0)  goto show__;
	  long order = (long)((unsigned char)ch-161)*94+((unsigned char)str[++K]-161);
	  fseek(hzfp, order*32L, SEEK_SET);
	  unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*Y+(X>>3)+1;
	  if(K<=end&&K>=begin)
	    { register j;
	      for(j=0; j<16; j++)
	       { *ptr++ = ~fgetc(hzfp);
		 *ptr++ = ~fgetc(hzfp);
		  ptr += 78;
	       }
	    }
	  else
	    { register j;
	      for(j=0; j<16; j++)
	       { *ptr++ = fgetc(hzfp);
		 *ptr++ = fgetc(hzfp);
		  ptr += 78;
	       }
	    }
	  X += 16;
	}
       else show__:
	{ fseek(ascfp, (unsigned)ch<<4, SEEK_SET);
	  unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*Y+(X>>3)+1;
	  register j;
	  for(j=0; j<16; j++)   { *ptr = fgetc(ascfp);  ptr += 80;  }
	  X += 8;
	}
     }
   outportb(0x3c5, 0x0f);
 }

void T_ShowHz::showhz(int x, int y, long order, int _color)  //在指定位置显示汉字
 { fseek(fp, order*size, SEEK_SET);
   int color=_color==-1?getcolor():_color;
   if(x<0 || x+dot>639)  return;  //若超过显示范围就返回
   if(y<0 || y+dot>479)  return;
   outportb(0x3c4, 2);
   outportb(0x3ce, 4);
   if(x&0x0007)   //若为在一般位置显示汉字
    { unsigned char ch, chL, chR; int i, j, k;
      unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*y+(x>>3);
      int  offset = x&0x0007;
      for(j=0; j<dot; j++)
       { chL = 0;
	 for(k=0; k<dot>>3; k++)
	  { int k;
	    ch = fgetc(fp);
	    chL = (ch>>offset);
	    chR = ch<<(8-offset);
	    for(k=0; k<4; k++)
	     { outportb(0x3cf, k);
	       outportb(0x3c5, 0x01<<k);
	       if(color&(0x01<<k))
		{ *ptr |= chL;
		  *(ptr+1) |= chR;
		}
	       else
		{ *ptr &= ~chL;
		  *(ptr+1) &= ~chR;
		}
	     }
	    ptr ++;
	  }
	 ptr += 80-(dot>>3);
       }
    }
   else    //若为在特殊位置显示汉字(横向象素值为8的倍数,显示更快)
    { unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*y+(x>>3);
      int k, j, K; unsigned char ch;
      for(j=0; j<dot; j++)
	{ for(k=0; k<dot>>3; k++)
	   { ch = fgetc(fp);
	     for(K=0; K<4; K++)
	       { outportb(0x3cf, K);
		 outportb(0x3c5, (char)(0x01<<K));
		 if(color&(0x01<<K))  *ptr |= ch;
		 else                 *ptr &= ~ch;
	       }
	     ptr ++;
	   }
	  ptr += 80-(dot>>3);
	}
    }
   outportb(0x3c5, 0x0f);
 }

void T_ShowHz::xshowhz(int x, int y, long order, int _color, char model)
 { fseek(fp, order*size, SEEK_SET);
   int color=_color==-1?getcolor():_color;
   unsigned char ch; int i, j, k;
   int X, Y, t, m, n;
   for(j=0; j<dot; j++)    //纵向点阵数
    { for(i=0; i<(dot>>3); i++)  //横向字节数
       { ch = fgetc(fp);
	 for(k=0; k<8; k++)  //横向点阵数
	  { if(ch&(0x80>>k))  //若有象素要显示
	     { if(fx==0)  { X = x+(8*i+k)*zoomx;  Y = y+j*zoomy; }  //若方向为横向
	       else       { Y = y+(8*i+k)*zoomy;  X = x+j*zoomx; }  //若方向为纵向
	       if(model==COPY_MODEL)  //若为直接显示
		{ if(zoomx==1 && zoomy==1)  putpixel(X, Y, color);
		  else
		   { for(n=0; n<zoomy; n++)
		       for(m=0; m<zoomx; m++)
			  putpixel(X+m, Y+n, color);
		   }
		}
	       else  //否则,若为异或显示
		{ if(zoomx==1 && zoomy==1)  putpixel(X, Y, color^getpixel(X,Y));
		  else
		   { for(n=0; n<zoomy; n++)
		       for(m=0; m<zoomx; m++)
			  putpixel(X+m, Y+n, color^getpixel(X+m,Y+n));
		   }
		}
	     }
	  }
       }
      if(_speed)  delay(_speed); //延缓时间
    }
 }

void T_ShowAsc::zoom(int _zoomx, int _zoomy)  //设置横向和纵向缩放比例
 { if(_zoomx!=CURSET)   zoomx = _zoomx;
   if(_zoomy!=CURSET)   zoomy = _zoomy;
 }

T_ShowAsc::T_ShowAsc(char *filename)
 { fp = fopen(filename, "rb");
   if(fp==NULL)  openflag = 0;
   else	 { openflag = 1; zoomx = 1; zoomy = 1; _speed = 0; }
   fseek(fp, (long)32*16, SEEK_SET);
   long  L = 16*(127-32);
   for(long k=0; k<L; k++)  asck[k] = fgetc(fp);
 }

void T_ShowAsc::showasc(int x, int y, long order, int _color)  //在指定位置显示ASC字符
 { fseek(fp, order<<4, SEEK_SET);
   int color=_color==-1?getcolor():_color;
   if(x<0 || x+8>639)  return;
   if(y<0 || y+16>479) return;
   outportb(0x3c4, 2);
   outportb(0x3ce, 4);
   if(x&0x0007)  //若为在一般位置显示
    { unsigned char ch, chL, chR; int i, j, k=1;
      unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*y+(x>>3);
      int  offset = x&0x0007;
      for(j=0; j<16; j++)
       { ch = fgetc(fp);
	 chL = (ch>>offset);
	 chR = ch<<(8-offset);
         for(k=0; k<4; k++)
           { outportb(0x3cf, k);
             outportb(0x3c5, 0x01<<k);
             if(color&(0x01<<k))
               { *ptr |= chL;
                 *(ptr+1) |= chR;
               }
             else
               { *ptr &= ~chL;
                 *(ptr+1) &= ~chR;
	       }
	    }
	 ptr += 80;
       }
    }
   else  //否则,若为在特殊位置显示
    { unsigned char far *ptr = (unsigned char far *)0xa0000000L+80*y+(x>>3);
      int k, j, K; unsigned char ch;
      for(j=0; j<16; j++)
	{ ch = fgetc(fp);
	  for(K=0; K<4; K++)
	    { outportb(0x3cf, K);
	      outportb(0x3c5, 0x01<<K);
	      if(color&(0x01<<K))  *ptr |= ch;
	      else                 *ptr &= ~ch;
	    }
	  ptr += 80;
	}
    }
   outportb(0x3c5, 0x0f);
 }

void T_ShowAsc::xshowasc(int x, int y, long order, int _color, char model)
//增强的在指定位置显示ASC字符
 { fseek(fp, order*16, SEEK_SET);
   int color=_color==-1?getcolor():_color;
   unsigned char ch; int i, j;
   int X, Y, t, m, n;
   for(j=0; j<16; j++)  //纵向点阵数
    { ch = fgetc(fp);
      for(i=0; i<8; i++)  //横向点阵数
	{ if(ch&(0x80>>i))  //若有象素要显示
	   { X = x+i*zoomx;  Y = y+j*zoomy;
	     if(model==COPY_MODEL)  //若为直接显示
	       { if(zoomx==1 && zoomy==1)  putpixel(X, Y, color);
		  else
		   { for(n=0; n<zoomy; n++)
		       for(m=0; m<zoomx; m++)
			  putpixel(X+m, Y+n, color);
		   }
		}
	     else  //否则,若为异或显示
		{ if(zoomx==1 && zoomy==1)  putpixel(X, Y, color^getpixel(X,Y));
		  else
		   { for(n=0; n<zoomy; n++)
		       for(m=0; m<zoomx; m++)
			  putpixel(X+m, Y+n, color^getpixel(X+m,Y+n));
		   }
		}
	     }
	  }
      if(_speed)  delay(_speed);
    }
 }



T_ShowStr::T_ShowStr(char *hzstr, char *ascstr)
 { dxy = 0; zoomx = zoomy = 1; pn_x = LEFT_TEXT; pn_y = TOP_TEXT;
   Hz = T_ShowHz(hzstr);  Asc = T_ShowAsc(ascstr);
 }

T_ShowStr::T_ShowStr(FILE *hzfp, FILE *ascfp)
 { dxy = 0; zoomx = zoomy = 1; pn_x = LEFT_TEXT; pn_y = TOP_TEXT;
   Hz = T_ShowHz(hzfp);  Asc = T_ShowAsc(ascfp);
 }

void T_ShowStr::showstr(int x, int y, char *str, int color)
 { int K = -1; int X = x, Y = y;
   while(str[++K])
    { if(str[K]&0x80)
       { if(str[K+1]=='\0')   break;
	 if((str[K+1]&0x80)==0)  goto ch_;
	 int Qm=(unsigned char)str[K]-161, Wm=(unsigned char)str[++K]-161;
	 Hz.showhz(X, Y, (long)Qm*94+Wm, color);
	 X += 16;
       }
      else ch_:
       { Asc.showasc(X, Y, (unsigned long)str[K], color);
	 X += 8;
       }
    }
 }

void T_ShowStr::xshowstr(int x, int y, char *str, int color, char model)
 { int K = -1; int X = x, Y = y;
   if(pn_x!=LEFT_TEXT)
    { int lenth = 0, K=-1, len = strlen(str);
      while(str[++K])  if(str[K]&0x80)  lenth ++;
      lenth = len*8*zoomx + (len/2+lenth-1)*dxy;
      if(pn_x==RIGHT_TEXT)  X -= lenth;
      else  X -= lenth/2;
    }
   if(pn_y!=TOP_TEXT)
    { if(pn_y==BOTTOM_TEXT)  Y -= 16*zoomy;
      else  Y -= 8*zoomy;
    }
   while(str[++K])
    { if(str[K]&0x80)
       { int Qm=(unsigned char)str[K]-161, Wm=(unsigned char)str[++K]-161;
	 Hz.xshowhz(X, Y, (long)Qm*94+Wm, color, model);
	 X += 16*zoomx+dxy;
       }
      else
       { Asc.xshowasc(X, Y, (unsigned long)str[K], color, model);
	 X += 8*zoomx+dxy;
       }
    }
 }


//时间
void T_Time::show(struct time *p)   //显示时间
 { struct time pp;
   if(p==NULL) { p = &pp;  gettime(p); }
   char str[10];
   int hour = p->ti_hour, min = p->ti_min, sec = p->ti_sec;
   sprintf(str, hour>=10?"%d:":"0%d:", hour);
   sprintf(str+3, min>=10?"%d:":"0%d:", min);
   sprintf(str+6, sec>=10?"%d":"0%d", sec);
   int flag=0;
   if(imouse_inbox(x-10, y-15, x+72, y+15))    { flag = 1;  mouse_off(); }
   _pn.qshowstr(x, y, str, WHITE, NOSEL, NOSEL);
   if(flag==1)  mouse_on();
 }

void T_Time::clear()      //清除时间
 { int flag=0;
   if(imouse_inbox(x-10, y-15, x+72, y+15))
      { flag = 1;  mouse_off(); }
   setfillstyle(1, backcolor);
   bar(x, y, x+72, y+15);
   if(flag==1)  mouse_on();
 }

void T_Time::run()       //检测时间以及时更新
 { if(runenable==false)  return;
   gettime(&_T);
   if(_T.ti_sec == T0.ti_sec)  return;
   T0 = _T;  show(&_T);
 }


//按钮

T_Button::T_Button(int x, int y, char *_str, char Kind, char _backcolor, char _forcolor)
 { x1 = x;  y1 = y;  str = _str;
   x2 = x1+strlen(str)*8+10;   y2 = y1+20;
   A = no;  F = no;  W = no;  T = no;  H = no;  Aflag=up;  C = no;
   if(Kind!=DEF)
     { if(Kind&Tx)  T = yes;       if(Kind&Fx)  F = yes;
       if(Kind&Ax)  A = yes;       if(Kind&Wx)  W = yes;
       if(Kind&Hx)  H = yes;       if(Kind&Cx)  C = yes;
     }
   backcolor = _backcolor;  forcolor = _forcolor;
   if(F==yes&&C==no)  { backcolor = BLACK;  forcolor = YELLOW; }
   if(H==no)  show(A);
 }

T_Button::T_Button(int _x1, int _y1, int _x2, int _y2, char *_str,
    char Kind, char _backcolor, char _forcolor)
 { x1 = _x1;  y1 = _y1;  str = _str;
   if(_x2==DEF)  x2 = x1+strlen(str)*8+10;  else x2 = _x2;
   if(_y2==DEF)  y2 = y1+20;  else y2 = _y2;
   A = no;  F = no;  W = no;  T = no;  H = no;  Aflag=up;  C = no;
   if(Kind!=DEF)
     { if(Kind&Tx)  T = yes;       if(Kind&Fx)  F = yes;
       if(Kind&Ax)  A = yes;       if(Kind&Wx)  W = yes;
       if(Kind&Hx)  H = yes;       if(Kind&Cx)  C = yes;
     }
   backcolor = _backcolor;  forcolor = _forcolor;
   if(F==yes&&C==no)  { backcolor = getbkcolor();  forcolor = YELLOW; }
   if(H==no)  show(A);
 }

void T_Button::attrib(char Kind)
 { A = no;  F = no;  W = no;  T = no;  H = no;  Aflag=up;  C = no;
   if(Kind!=DEF)
     { if(Kind&Tx)  T = yes;       if(Kind&Fx)  F = yes;
       if(Kind&Ax)  A = yes;       if(Kind&Wx)  W = yes;
       if(Kind&Hx)  H = yes;       if(Kind&Cx)  C = yes;
     }
   show(curset);
 }

void T_Button::hide(int color)
 { GRAPHGETTING
   if(color!=DEF)  setfillstyle(1, color);
   else            setfillstyle(0, 0);
   off();  bar(x1, y1, x2, y2);  on();
   H = yes;
 }

void T_Button::show(BOOL idown)
 { int upcolor, downcolor;
   if(idown!=curset)  A = idown;
   if(A==yes) { upcolor = DARKGRAY;  downcolor = LIGHTGRAY; }
   else       { upcolor = LIGHTGRAY; downcolor = DARKGRAY;  }
   H = no;
   mouse_off();
   GRAPHGETTING;
   setfillstyle(1, backcolor);
   bar(x1, y1, x2, y2);
   setlinestyle(0, 0, 1);
   setcolor(upcolor);
   int inbox = imouse_inbox(x1, y1, x2, y2);
   if(F==yes)
     { if(A==yes)
	{ line(x1, y1, x2, y1);  line(x1, y1, x1, y2); }
       else if(W==false)
	{ if(inbox)
	   { line(x1, y1, x2, y1);  line(x1, y1, x1, y2); Aflag = down; }
	  else  Aflag = up;
	}
     }
   else
     { line(x1, y1, x2, y1);  line(x1, y1, x1, y2);
       line(x1+1, y1+1, x2-1, y1+1);  line(x1+1, y1+1, x1+1, y2-1);
     }
   setcolor(downcolor);
   if(F==yes)
     { if(A==yes)
	 { line(x2, y2, x1, y2);  line(x2, y2, x2, y1); }
       else if(W==false)
	 { if(inbox)
	     { line(x2, y2, x1, y2);  line(x2, y2, x2, y1); }
	 }
     }
   else
     { line(x2, y2, x1, y2);  line(x2, y2, x2, y1);
       line(x2-1, y2-1, x1+1, y2-1);  line(x2-1, y2-1, x2-1, y1+1);
     }
   setcolor(W==no?forcolor:LIGHTGRAY);
   if(A==no)  print((x1+x2)/2-strlen(str)*4, (y1+y2)/2-8, str);
   else       print((x1+x2)/2-strlen(str)*4+1, (y1+y2)/2-7, str);
   mouse_on();
   GRAPHSETTING;
 }

BOOL T_Button::testclick()
 { if(W==yes||H==yes)  return no;
   int x, y; BOOL a = A; int inbox = imouse_inbox(x1, y1, x2, y2);
   BOOL  click = no;
   if(F==yes&&A==no)
     { if(Aflag==up&&inbox || Aflag==down&&!inbox) { show(A);  return false; } }
   if(inbox)
     while(getmousebutton()==LEFTBUTTON)
      { if(imouse_inbox(x1, y1, x2, y2))
	 { if(A!=(BOOL)(1-a))  show((BOOL)(1-a)); }
	else if(A!=a)     show(a);
      }
   if(A!=a)  click = yes;
   if(T==no)
    { if(A==yes)        show(no);
      else if(F==yes&&click)   show(a);
    }
   if(click==yes)  Sound();
   return  click;
 }

void  T_Button::moveto(int x, int y)
 { hide();
   int dx=x1-x, dy=y1-y;
   x1 -= dx;   y1 -= dy;
   x2 -= dx;   y2 -= dy;
   show();
 }



//文本框
T_Text::T_Text(int _x, int _y, int _width, char *_str, char _attrib,
     char _backcolor, char _forcolor, BOOL _ch)
 { x = _x;   y = _y;
   if(_width==DEF)  width = strlen(_str)*8+4;
   else             width = _width;
   strcpy(ptr, _str);  ch = _ch;
   if(_attrib&Lock)  L = yes;  else L = no;
   if(_attrib&Tx)    T = no;   else T = yes;
   backcolor = _backcolor;  forcolor = _forcolor;
   K = 0;  begin = 0; show();  bptr[0] = '\0';
 }

char *T_Text::str(char *str)
 { if(str==NULL)  return ptr;
   strcpy(bptr, ptr);  strcpy(ptr, str);
   Len = strlen(str);
   show();
 }

BOOL T_Text::edit()
 { char ch;  strcpy(bptr, ptr);  BOOL ret = true;
   GRAPHGETTING;
   while(1)
    { switch(ch=shines())
       { case 0:
	   switch(getch())
	     { case LEFT_KEY:  if(--K<0)  showstr(ptr); break;
	       case RIGHT_KEY: if(++K>strlen(ptr))  K = strlen(ptr);
			       else if(K>(width-4)>>3)
				 { K = (width-4)>>3; begin ++; showstr(ptr); }
			       else if(K+begin>strlen(ptr))  K = strlen(ptr)-begin;
			       break;
	       case HOME_KEY:  K = 0;  begin = 0; showstr(ptr); break;
	       case END_KEY:   K = strlen(ptr);  showstr(ptr);  break;
	       case DELETE_KEY: int j=strlen(ptr);
		    for(int i=K+begin; i<j; i++)  ptr[i] = ptr[i+1];
		    if(begin&&strlen(ptr)-begin<((width-4)>>3)) { begin --; K ++; }
		    showstr(ptr);  break;
	     }
	   break;
	 case BACKSPACE_KEY: if(begin+K==0)  break;
	      int j=strlen(ptr);
	      if(begin)  begin --;   else if(K)  K--;
	      for(int i=K+begin; i<j; i++)  ptr[i] = ptr[i+1];
	      showstr(ptr);  break;
	 case ENTER_KEY:  ret = true;  goto end;
	 case ESC_KEY:    strcpy(ptr, bptr); ret = false; goto end;
	 default: if(ch<32&&ch>=0)  break;  if(strlen(ptr)>48)  break;
	   if(ch>0)
	    { for(int i=strlen(ptr); i>=K+begin; i--)  ptr[i+1] = ptr[i];
	      ptr[begin+K++] = ch;
	      if(K>(width-4)>>3)  { K --;  begin ++; }
	      showstr(ptr);
	    }
	   else
	    { for(int i=strlen(ptr); i>=K+begin; i--)  ptr[i+2] = ptr[i];
	      ptr[begin+K++] = ch;  ptr[begin+K++] = getch();
	      if(K>(width-4)>>3)  { K -= 2;  begin += 2; }
	      showstr(ptr);
	    }
       }
    } end:;
   if(ch==true)  strupr(ptr);  begin = 0;  showstr(ptr);
   GRAPHSETTING;   return ret;
 }

void  T_Text::showstr(char *str)
 { if(K<0) { K = 0;  begin --; if(begin<0)  begin = 0; }
   if(K>((width-4)>>3))
     { K = ((width-4)>>3);  begin = strlen(str)-K; }
   if(K+begin>strlen(str))  K = strlen(str)-begin;
   char ch = *(str+begin+((width-4)>>3));
   *(str+begin+((width-4)>>3)) = '\0';
   mouse_off();
   setfillstyle(1, backcolor);   bar(x+1, y+1, x+width-1, y+20-1);
   setcolor(forcolor);           print(x+2, y+2, str+begin);
   mouse_on();
   *(str+begin+((width-4)>>3)) = ch;
   Len = strlen(str);
 }


char T_Text::shines()
 { setlinestyle(0, 0, 1);
   redo:for(int k=0; k<2; k++)
    { setcolor(k==0?~backcolor:backcolor);
      int flag = imouse_inbox(x, y, x+width, y+20);
      if(flag)  mouse_off();
      int X1 = x+(K<<3)+2,  X2 = X1+5;
      if(X2>x+width)  X2 = x+width;
      line(X1, y+19, X2, y+19);
      if(flag)  mouse_on();
      long L = clock();
      while(clock()-L<3)
       { if(kbhit())
	   { if(k==0)  break;
	     return getch();
	   }
	 if(getmousebutton()==LEFTBUTTON)
	  { if(k==0)  break;
	    if(imouse_inbox(x, y, x+width, y+20))
	     { K = (getmouse_x()-x-4)>>3;
	       if(K>Len)  K = Len;
	     }
	    else  return ENTER_KEY;
	  }
       }
    } goto redo;
 }

void T_Text::show()
 { GRAPHGETTING;  mouse_off();
   bar3ds(x, y, x+width, y+20, T==yes?down:def, backcolor);
   setcolor(forcolor);
   showstr(ptr);
   mouse_on();   GRAPHSETTING;
 }

void T_Text::clear()
 { GRAPHGETTING;  mouse_off();
   bar3ds(x, y, x+width, y+20, def, backcolor);
   mouse_on();  GRAPHSETTING;
 }

BOOL T_Text::testclick()
 { if(getmousebutton()!=LEFTBUTTON)   return false;
   if(!imouse_inbox(x, y, x+width, y+20))  return false;
   K = (getmouse_x()-x-4)>>3;
   if(K>strlen(ptr))  K = strlen(ptr);
   return edit();
 }


//标签
T_Label::T_Label(int _x, int _y, int _width, char *_str, char _attrib,
	 char _backcolor, char _forcolor)
 { x = _x;  y = _y;
   ptr = new char[strlen(_str)+1];
   strcpy(ptr, _str);
   backcolor = _backcolor;  forcolor = _forcolor;
   if(_width==DEF)  width = (strlen(ptr)<<3)+4;
   else             width = _width;
   attrib = _attrib;
   show();
 }

char *T_Label::str(char *str)
 { if(*str==0)  return str;
   delete ptr;
   ptr = new char[strlen(str)+1];
   strcpy(ptr, str);
   show();
   return str;
 }

void T_Label::show()
 { GRAPHGETTING;  mouse_off();
   bar3ds(x, y, x+width, y+20, def, backcolor);
   setcolor(forcolor);
   int X = attrib==LEFT_TEXT?x+2:attrib==RIGHT_TEXT?
     x+width-2-strlen(ptr)*8:(width-strlen(ptr)*8)/2+x+2;
   print(X, y+2, ptr);
   mouse_on();   GRAPHSETTING;
 }

void T_Label::clear()
 { GRAPHGETTING;  mouse_off();
   bar3ds(x, y, x+width, y+20, def, backcolor);
   mouse_on();  GRAPHSETTING;
 }


//单选/复选框

T_Select::T_Select(int _x, int _y, char _attrib, char _backcolor, char _forcolor)
 { bx = x = _x;  by = y = _y;
   dy = 20;  Home = NULL;  PTR = Home;  pn = 0;  num = 0;
   SEL = 0;  F = no;
   if(_attrib&Fx)  F = yes;
   backcolor = _backcolor;   forcolor = _forcolor;
 }

void T_Select::reset()
 { T_Sel *ptr = Home, *ptr0;
   while(ptr!=NULL) { ptr0 = ptr; ptr=ptr->Tnext; delete ptr0; }
   Home = NULL;  PTR = Home;
   x = bx;  y = by;  num = 0;  SEL = 0;  pn = 0;
 }

void T_Select::showhead()
 { T_Sel *ptr = Home;
   while(ptr!=NULL)  { ptr->showhead();  ptr = ptr->Tnext; }
 }

void T_Select::add(char *str, char attrib, int _x, int _y)
 { if(_x!=DEF)  x = _x;
   if(_y!=DEF)  y = _y;
   T_Sel *Ptr=Home, *ptr;
   ptr = new T_Sel(x, y, str, attrib|(F==true?Fx:0), backcolor, forcolor);
   ptr->Tnext = NULL;
   y += dy;
   if(Ptr==NULL)  Home = ptr;
   else
    { while(Ptr->Tnext!=NULL)   Ptr = Ptr->Tnext;
      Ptr->Tnext = ptr;
    }
   if(F==no&&attrib&Ax)  select(true, str);
 }

BOOL T_Select::moveptr(char *str)
 { T_Sel  *ptr = Home;  char *s, *s0;
   while(ptr!=NULL)
    { s0 = ptr->str;  s = str;
      while(*s)
	{ if(*s0 != *s)  break;
	  s0++;   s++;
	}
      if(*s==0)  break;
      ptr = ptr->Tnext;
    }
   if(ptr==NULL)  return false;
   PTR = ptr;  return true;
 }

BOOL T_Select::del(char *str)
 { T_Sel  *ptr0 = Home;
   if(moveptr(str)==false)  return false;
   if(PTR==Home)  Home = PTR->Tnext;
   else
     { while(ptr0->Tnext!=PTR)  ptr0 = ptr0->Tnext;
       ptr0->Tnext = PTR->Tnext;
     }
   int   X = PTR->x, Y = PTR->y;
   delete PTR;
   T_Sel  *Ptr = Home;
   while(Ptr!=NULL)
    { int x = Ptr->x, y = Ptr->y;
      if(x==X && y>Y)
	{ Ptr->clear();
	  Ptr->y -= dy;
	  Ptr->show();
	}
      Ptr = Ptr->Tnext;
    }
   y -= dy;
 }

void T_Sel::clear()
 { GRAPHGETTING;  mouse_off();
   setfillstyle(1, backcolor);
   bar(x, y, x+20+(strlen(str)<<3), y+16);
   GRAPHSETTING;  mouse_on();
 }

T_Sel::T_Sel(int _x, int _y, char *_str, char attrib,
      char _backcolor, char _forcolor)
 { strcpy(str, _str);
   backcolor = _backcolor;  forcolor = _forcolor;
   if(attrib&Ax)  A = yes;  else A = no;
   if(attrib&Wx)  W = yes;  else W = no;
   if(attrib&Fx)  F = yes;  else F = no;
   x = _x;  y = _y;
   show();
 }

void T_Sel::showhead()
 { if(F==no)
    { setcolor(DARKGRAY);
      circle(x+8, y+8, 5);
      setcolor(LIGHTGRAY);
      arc(x+8, y+8, 225, 405, 5);
      if(A==yes)
       { setfillstyle(1, forcolor);  setcolor(forcolor);
	 pieslice(x+8, y+8, 0, 360, 2);
       }
    }
   else
    { int x1=x+2, y1=y+2, x2=x1+10, y2=y1+10;
      setlinestyle(0, 0, 1);
      setcolor(DARKGRAY);
      line(x1, y1, x2, y1);  line(x1, y1, x1, y2);
      setcolor(LIGHTGRAY);
      line(x2, y2, x1, y2);  line(x2, y2, x2, y1);
      if(A==yes)
       { setcolor(forcolor);
	 line(x1+1, y1+5, x1+4, y2-1);
	 line(x1+4, y2-1, x2-1, y1+1);
       }
    }
 }

void T_Sel::show()
 { GRAPHGETTING
   mouse_off();
   setfillstyle(1, backcolor);
   bar(x, y, x+20+(strlen(str)<<3), y+16);
   showhead();
   setcolor(W==yes?LIGHTGRAY:forcolor);
   print(x+16, y, str);
   mouse_on();
   GRAPHSETTING
 }

BOOL T_Sel::able(BOOL Y)
 { if(Y==def)  return (BOOL)(1-W);
   W = (BOOL)(1-Y);
   show();   return Y;
 }

BOOL T_Select::able(BOOL Y, char *str)
 { if((*str)==0)           return PTR->able(Y);
   if(moveptr(str)==true)  return PTR->able(Y);
   return false;
 }

BOOL T_Sel::select(BOOL AX)
 { if(AX==def)  return A;
   A = AX;  show();  return AX;
 }

void T_Sel::change(char *_str)
 { clear();  strcpy(str, _str);   show();  }

BOOL T_Select::change(char *str, char *_str)
 { if(moveptr(str)==false)  return false;
   PTR->change(_str);
   return true;
 }

BOOL T_Select::select(BOOL AX, char *str)
 { if(*str) if(moveptr(str)==false)  return false;
   if(AX==def)   return PTR->select(def);
   if(AX==false) return PTR->select(false);
   if(F==yes)    return PTR->select(true);
   T_Sel *ptr = Home;
   while(ptr!=NULL)
    { if(ptr->select(def)==true)  ptr->select(false);
      ptr = ptr->Tnext;
    }
   PTR->select(true);
 }

BOOL T_Select::testclick()
 { if(getmousebutton()!=LEFTBUTTON)  return false;
   T_Sel  *ptr = Home;
   int x1, y1, x2, y2;
   while(ptr!=NULL)
    { if(ptr->W==true)  { ptr = ptr->Tnext;  continue; }
      x1 = ptr->x;  y1 = ptr->y;
      x2 = x1+(strlen(ptr->str)<<3)+16; y2=y1+16;
      if(imouse_inbox(x1, y1, x2, y2))
	{ if(F==no&&ptr->select()==yes)  return curset;
	  select((BOOL)(1-(ptr->select())), ptr->str);
	  str = ptr->str;  Sound(4);
	  while(getmousebutton()!=NOBUTTON);
	  return true;
	}
      ptr = ptr->Tnext;
    }
   return false;
 }

//滚动条
T_Scroll::T_Scroll(int _x1, int _y1, int _x2, int _y2, int _max, int _flag,
	 char _backcolor, char _forcolor, BOOL _Y)
 { x1 = _x1;  y1 = _y1;  x2 = _x2;  y2 = _y2;
   backcolor = _backcolor;  forcolor = _forcolor;
   max = _max;  flag = _flag;
   if(flag>=max-1)   flag = max-1;
   Y = _Y;
   if(x2-x1>y2-y1)  { H = no;  lenth = x2-x1;  width = y2-y1; }
   else             { H = yes; lenth = y2-y1;  width = x2-x1; }
   sd = d = (double)(lenth-2*width)/max;
   if(sd<6.0&&max!=1) { sd = 6.0;  d = (double)(lenth-2*width-sd)/(max-1); }
   show();
 }

void T_Scroll::clear()
 { GRAPHGETTING;  mouse_off();
   setfillstyle(0, 0);
   bar(x1, y1, x2, y2);
   mouse_on();  GRAPHSETTING;
 }


void T_Scroll::show()
 { GRAPHGETTING;  mouse_off();
   char color = Y==true?forcolor:LIGHTGRAY;
   setfillstyle(1, backcolor);
   if(H==yes)
    { bar(x1, y1+width, x2, y2-width);
      int X1=x1, X2=x2;
      int Y1=y1+d*flag+width, Y2=Y1+sd;
      bar3ds(X1, Y1, X2, Y2, up, color);
      Y1 = y1;  Y2 = Y1+width;
      bar3ds(X1, Y1, X2, Y2, up, color);
      Y1 = y2-width;  Y2 = y2;
      bar3ds(X1, Y1, X2, Y2, up, color);
    }
   else
    { bar(x1+width, y1, x2-width, y2);
      int Y1=y1, Y2=y2;
      int X1=x1+d*flag+width, X2=X1+sd;
      bar3ds(X1, Y1, X2, Y2, up, color);
      X1 = x1;  X2 = X1+width;
      bar3ds(X1, Y1, X2, Y2, up, color);
      X1 = x2-width;  X2 = x2;
      bar3ds(X1, Y1, X2, Y2, up, color);
    }
   mouse_on();  GRAPHSETTING;
 }

void T_Scroll::setflag(int _flag, int _max, int Flag)
 { if(_max!=CURSET)   max = _max;
   if(max<=0)  max = 1;
   if(_flag!=CURSET)  flag = _flag;
   if(flag>=max-1)   flag = max-1;
   if(flag<0)        flag = 0;
   double bsd = sd;
   sd = d = (double)(lenth-2*width)/max;
   if(sd<6.0&&max!=1) { sd = 6.0;  d = (double)(lenth-2*width-sd)/(max-1); }
   if(Flag==0&&bsd!=sd || Flag==2)  show();
 }

BOOL T_Scroll::able(BOOL _Y)
 { if(_Y==def)  return (BOOL)(1-Y);
   Y = _Y;  show();  return _Y;
 }

#define testdown(X1, Y1, X2, Y2) \
  mouse_off();  bar3ds(X1, Y1, X2, Y2, down, forcolor);  mouse_on();    \
  while(getmousebutton()==LEFTBUTTON && imouse_inbox(X1, Y1, X2, Y2)); \
  while(getmousebutton()!=NOBUTTON);   \
  mouse_off();  bar3ds(X1, Y1, X2, Y2, up, forcolor);  mouse_on();

BOOL T_Scroll::testclick()
 { if(Y==false)  return false;
   if(getmousebutton()!=LEFTBUTTON)  return false;
   int X1, Y1, X2, Y2;
   if(H==yes)
    { X1 = x1;  Y1 = y1;  X2 = x2;  Y2 = Y1+width;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ testdown(X1, Y1, X2, Y2);
	  if(flag--<=0)  { flag = 0;  return no; }
	  show(); Sound(4);  return yes;
	}
      Y1 = y2-width;   Y2 = y2;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ testdown(X1, Y1, X2, Y2);
	  if(flag++>=max-1) { flag = max-1; return no; }
	  show();  Sound(4);  return yes;
	}
      Y1 = y1+width;   Y2 = Y1+d*flag;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ if(flag==0)  return no;
	  flag -= 3;  if(flag<0)  flag = 0;
	  show();  while(getmousebutton()!=NOBUTTON);
	  Sound(4);  return yes;
	}
      Y1 = y1+width+d*flag+sd;   Y2 = y2-width;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ if(flag==max-1)  return no;
	  flag += 3;  if(flag>max-1)  flag = max-1;
	  show();  while(getmousebutton()!=NOBUTTON);
	  Sound(4);  return yes;
	}
      Y1 = y1+width+d*flag;  Y2 = Y1+sd;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ double Y = getmouse_y();  int flag0 = flag;
	  while(getmousebutton()==LEFTBUTTON)
	   { int Y0 = getmouse_y();
	     if(Y0-Y>d/2 && flag<max-1)
	      { int k = (Y0-Y)/d;  if(flag+k>max-1)  flag = max-k-1;
		flag += k;  show();  Y += d*k;
	      }
	     else if(Y0-Y<-d/2 && flag>0)
	      { int k = (Y-Y0)/d;  if(flag-k<0)  flag = k;
		flag -= k;  show();  Y -= d*k;
	      }
	   }
	  if(flag0!=flag)  { Sound(4);  return yes; }  return no;
	}
    }
   else
    { Y1 = y1;  X1 = x1;  Y2 = y2;  X2 = X1+width;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ testdown(X1, Y1, X2, Y2);
	  if(flag--<=0)  { flag = 0;  return no; }
	  show(); Sound(4);  return yes;
	}
      X1 = x2-width;   X2 = x2;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ testdown(X1, Y1, X2, Y2);
	  if(flag++>=max-1) { flag = max-1; return no; }
	  show();  Sound(4);  return yes;
	}
      X1 = x1+width;   X2 = X1+d*flag;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ if(flag==0)  return no;
	  flag -= 3;  if(flag<0)  flag = 0;
	  show();  while(getmousebutton()!=NOBUTTON);
	  Sound(4);  return yes;
	}
      X1 = x1+width+d*flag+sd;   X2 = x2-width;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ if(flag==max-1)  return no;
	  flag += 3;  if(flag>max-1)  flag = max-1;
	  show();  while(getmousebutton()!=NOBUTTON);
	  Sound(4);  return yes;
	}
      X1 = x1+width+d*flag;  X2 = X1+sd;
      if(imouse_inbox(X1, Y1, X2, Y2))
	{ double X;  X = getmouse_x();  int flag0 = flag;
	  while(getmousebutton()==LEFTBUTTON)
	   { int X0 = getmouse_x();
	     if(X0-X>d/2 && flag<max-1)
	      { int k = (X0-X)/d;  if(flag+k>max-1)  flag = max-k-1;
		flag += k;  show();  X += d*k;
	      }
	     else if(X0-X<-d/2 && flag>0)
	      { int k = (X-X0)/d;  if(flag-k<0)  flag = k;
		flag -= k;  show();  X -= d*k;
	      }
	   }
	  if(flag0!=flag)  { Sound(4);  return yes; }  return no;
	}
    }
   return no;
 }

//菜单

T_La::T_La(char *_str, char attrib, char _group)
 { W = A = S = false;
   if(attrib!=DEF)
     { if(attrib&Wx)  W = true;
       if(attrib&Ax)  A = true;
       if(attrib&Sx)  S = true;
     }
   str = new char[strlen(_str)+1];
   strcpy(str, _str);
   if(*_str=='-')  W = true;
   menuptr = NULL;
   barflag = false;  group = _group;
 }

void T_La::show(int x, int y, char color, int wide)
 { if(*str!='-')
    {  setcolor(W==yes?LIGHTGRAY:color);
       print(x+15, y, str);
    }
   else
    { setcolor(LIGHTGRAY);  line(x+2, y+9, x+wide-2, y+9);
      setcolor(DARKGRAY);   line(x+2, y+8, x+wide-2, y+8);
    }
   setcolor(W==yes?LIGHTGRAY:~color);
   if(A==yes)         print(x+2, y, "√");
   if(menuptr!=NULL)  print(x+wide-9, y, ">");
   barflag = false;
 }

void T_La::setbar(int x, int y, int wide)
 { if(barflag==true)  return;
   char *ptr = new char[imagesize(x+2, y, x+wide-3, y+16)];
   mouse_off();
   getimage(x+2, y, x+wide-3, y+16, ptr);
   putimage(x+2, y, ptr, NOT_PUT);
   mouse_on();
   delete ptr;   barflag = true;
   Sound(3);
 }

void T_La::clearbar(int x, int y, int wide)
 { if(barflag==false)  return;
   char *ptr = new char[imagesize(x+2, y, x+wide-3, y+16)];
   mouse_off();
   getimage(x+2, y, x+wide-3, y+16, ptr);
   putimage(x+2, y, ptr, NOT_PUT);
   mouse_on();
   delete ptr;   barflag = false;
 }

T_Menu  *PTR;

T_La *T_Menu::findptr(char *str)
 { if(headptr==NULL)  return headptr;
   T_La *ptr = headptr, *ptr0;
   PTR = this;
   if(strcmp(str, "<")==0)  return headptr;
   if(strcmp(str, ">")==0)
    { while(ptr->Tnext!=NULL)  ptr = ptr->Tnext;
      return  ptr;
    }
   while(ptr!=NULL)
    { char *s0 = ptr->str, *s = str;
      while(*s)
	{ if(*s0 != *s)  break;
	  s0++;   s++;
	}
      if(*s==0)  return ptr;
      if(ptr->menuptr!=NULL && (ptr0=ptr->menuptr->findptr(str))!=NULL)  return ptr0;
      ptr = ptr->Tnext;
    }
   return NULL;
 }

void T_Menu::show(int x, int y)
 { if(showflag==true)  return;
   T_LineEdit::shines(off);
   int high = gethigh()*18+8;
   int wide = getwide()*8+30;
   if(high==8)  high = 26;
   if(wide==30) wide = 60;
   T_La *ptr = headptr;
   int k=0;
   if(x==DEF)  x = getmouse_x();
   if(y==DEF)  y = getmouse_y();
   if(x+wide>639)  x -= wide;
   if(y+high>479)  y -= high;
   x1 = x;  y1 = y;  x2 = x+wide;  y2 = y+high;
   GRAPHGETTING;   mouse_off();
   if(screenptr!=NULL)  delete screenptr;
   screenptr = new char[imagesize(x1, y1, x2, y2)];
   if(screenptr==NULL)  goto end;
   getimage(x1, y1, x2, y2, screenptr);
   bar3ds(x, y, x+wide, y+high, up, backcolor);
   if(ptr==NULL)
    { setcolor(LIGHTGRAY);
      print(x+15, y+6, "(空)");
    }
   else while(ptr!=NULL)
    { if(ptr->S==false)   ptr->show(x, y+4+18*k++, forcolor, wide);
      ptr = ptr->Tnext;
    }
   showflag = true;
   end: mouse_on();  Sound(1);
   GRAPHSETTING;
 }

void T_Menu::clearbar()
 { int x=x1, y=y1, k = 0;
   T_La *ptr = headptr;
   int  wide = getwide()*8+30;
   while(ptr!=NULL)
    { ptr->clearbar(x, y+4+18*k++, wide);
      ptr = ptr->Tnext;
    }
 }

void T_Menu::hide()
 { if(showflag==false)  return;
   T_La  *ptr = headptr;
   while(ptr!=NULL)
    { if(ptr->menuptr!=NULL)  ptr->menuptr->hide();
      ptr = ptr->Tnext;
    }
   mouse_off();
   if(screenptr!=NULL)  putimage(x1, y1, screenptr, COPY_PUT);
   mouse_on();
   if(screenptr!=NULL)  delete screenptr;
   screenptr = NULL;   showflag = false;
 }

int T_Menu::getwide()
 { T_La *ptr = headptr;
   int  wide = 0;
   while(ptr!=NULL)
    { if(ptr->S==false)
	if(strlen(ptr->str)>wide)  wide = strlen(ptr->str);
      ptr = ptr->Tnext;
    }
   return wide;
 }

int T_Menu::gethigh()
 { T_La *ptr = headptr;
   int high = 0;
   while(ptr!=NULL)
     { if(ptr->S==false)     high ++;
       ptr = ptr->Tnext;
     }
   return high;
 }

T_Menu *T_Menu::add(char *str, char *pstr, char attrib, char group)
 { T_La  *P;
   if(headptr==NULL)
    { headptr = new T_La(str, attrib, group);
      headptr->Tnext = NULL;
      P = headptr;
    }
   else if(strcmp(pstr, "<")==0)
    { T_La *H = headptr;
      headptr = new T_La(str, attrib, group);
      headptr->Tnext = H;
      P = headptr;
    }
   else
    { T_La *ptr = findptr(pstr);
      if(ptr==NULL)  return NULL;
      P = new T_La(str, attrib, group);
      P->Tnext = ptr->Tnext;
      ptr->Tnext = P;
    }
   if(attrib!=DEF&&(attrib&Mx)&&*str!='-')
    {  P->menuptr = new T_Menu(backcolor, forcolor);
       return P->menuptr;
    }
   else return (T_Menu*)1;
 }
BOOL T_Menu::askattrib(char *str, char _attrib, BOOL *ret)
 { T_La *ptr = findptr(str);  BOOL R=def;
   if(ptr==NULL)   { if(ret!=NULL) *ret=false;  return false; }
   if(_attrib==Ax)  R = ptr->A;
   else if(_attrib==Mx)  R = ptr->menuptr==NULL?false:true;
   else if(_attrib==Sx)  R = ptr->S;
   else if(_attrib==Wx)  R = ptr->W;
   if(ret!=NULL)  *ret = true;
   return R;
 }

BOOL T_Menu::attrib(char *str, char _attrib)
 { T_La *ptr = findptr(str);
   if(ptr==NULL)  return false;
   ptr->W = ptr->A = ptr->S = false;
   if(_attrib&Wx)  ptr->W = true;
   if(_attrib&Ax)  ptr->A = true;
   if(_attrib&Sx)  ptr->S = true;
   return true;
 }

BOOL T_Menu::select(char *str)
 { T_La *ptr = findptr(str);
   if(ptr==NULL)  return false;
   if(ptr->group<0)  ptr->A = true;
   else if(ptr->group>0)
    { T_La *p = PTR->headptr;
      while(p!=NULL)
       { if(p->group==ptr->group)   p->A = false;
	 p = p->Tnext;
       }
      ptr->A = true;
    }
   return true;
 }

BOOL T_Menu::del(char *str)
 { T_La *ptr = findptr(str);
   if(ptr==NULL)  return false;
   T_La *Nptr = ptr->Tnext;
   if(ptr==headptr)  headptr = Nptr;
   else
    { T_La *Ptr = headptr;
      while(Ptr->Tnext!=ptr)   Ptr = Ptr->Tnext;
      Ptr->Tnext = Nptr;
    }
   delete ptr;   return true;
 }

#define mousemove curset

BOOL T_Menu::_testclick()
 { static char flag;
   int Wide = getwide()*8+30;
   if(showflag==false)  return false;
   T_La *ptr = headptr;
   while(ptr!=NULL)
    { if(ptr->menuptr!=NULL && ptr->menuptr->showflag==true)
	{ flag ++;
	  BOOL result = ptr->menuptr->testclick();
	  flag --;
	  if(result==true || result==cls)
	     { ptr->menuptr->hide();
	       if(flag==0)   hide();
	     }
	  if(result!=false)  return result;
	  break;
	}
      ptr = ptr->Tnext;
    }
   if(inbox())
    { ptr = headptr; int k = 0, K = (getmouse_y()-4-y1)/18;
      while(ptr!=NULL)
       { if(ptr->S==true)  { ptr = ptr->Tnext;  continue; }
	 if(k == K)
	   { if(ptr->barflag==true)
	      { if(ptr->W==false && ptr->menuptr==NULL)
		  { if(getmousebutton()==LEFTBUTTON)
		      { strcpy(T_Menu::ptr, ptr->str);
			if(flag==0)  hide();
			if(ptr->group<0)  ptr->A = (BOOL)(1-ptr->A);
			else if(ptr->group>0)
			 { T_La *p = headptr;
			   while(p!=NULL)
			     { if(p->group==ptr->group)  p->A = false;
			       p = p->Tnext;
			     }
			   ptr->A = true;
			 }
			return true;
		      }
		    return  mousemove;
		  }
	      }
	     T_La *ptr0 = headptr;  int k0=0;
	     if(ptr->menuptr==NULL || ptr->menuptr->showflag==false)
	      while(ptr0!=NULL)
	       { if(ptr0->menuptr!=NULL && ptr0->menuptr->showflag==true)
		    ptr0->menuptr->hide();
		 ptr0->clearbar(x1, y1+4+18*k0++, Wide);
		 ptr0 = ptr0->Tnext;
	       }
	     if(ptr->W==true)  return  mousemove;
             ptr->setbar(x1, y1+4+K*18, Wide);
	     if(ptr->menuptr!=NULL && ptr->menuptr->showflag==false)
	       { int x=x2, y=y1+18*k;
		 int wide = ptr->menuptr->getwide()*8+30,
		   high = ptr->menuptr->gethigh()*18+8;
		 if(x+wide>639)  x = x1-wide;
		 if(y+high>479)  y = y1-high;
		 if(x<0)  x = 639-wide;   if(y<0)  y = 479-high;
		 long T = clock();
		 while(clock()-T<3)
		   { if(!inbox())  goto _end_;
		     int j = (getmouse_y()-4-y1)/18;
		     if(j!=K)  goto _end_;
		   }
		 ptr->menuptr->show(x, y);
		 _end_:;
	       }
	     return  mousemove;
	   }
	 k ++;
	 ptr = ptr->Tnext;
       }
      return mousemove;
    }
   if(getmousebutton()==LEFTBUTTON)
     { if(flag==0)  { hide();  return cls; }
       else   return  false;
     }
   return false;
 }
#undef  mousemove

T_xM::T_xM(char *str, int x, int y, int backcolor, int forcolor)
 { menu = T_Menu(backcolor, forcolor);
   button = T_Button(x, y, str, Fx|Tx|Cx, BLACK, LIGHTGREEN);
 }

T_xM::~T_xM()
 { GRAPHGETTING;  mouse_off();
   setfillstyle(1, button.backcolor);
   bar(button.x1, button.y1, button.x2, button.y2);
   mouse_on();  GRAPHSETTING;
 }

BOOL T_xMenu::attrib(char *str, char _attrib)
 { T_xM *ptr = Head;
   while(ptr!=NULL)
    { if(ptr->menu.attrib(str, _attrib)==true)   return true;
      ptr = ptr->Tnext;
    }
   return false;
 }

BOOL T_xMenu::askattrib(char *str, char _attrib)
 { T_xM *ptr = Head;  BOOL ret;
   while(ptr!=NULL)
    { BOOL R = ptr->menu.askattrib(str, _attrib, &ret);
      if(ret==true)   return R;
      ptr = ptr->Tnext;
    }
   return false;
 }

void T_xMenu::renew()
 { T_xM *ptr = Head;
   while(ptr!=NULL) { ptr->button.show();  ptr = ptr->Tnext; }
 }

BOOL T_xMenu::select(char *str)
 { T_xM *ptr = Head;
   while(ptr!=NULL)
    { if(ptr->menu.select(str)==true)   return true;
      ptr = ptr->Tnext;
    }
   return false;
 }

void T_xMenu::hide()
 { if(showflag == no)  return;
   T_xM *p = Head;
   while(p!=NULL)
    { p->hidemenu();  p->button.show(false);   p = p->Tnext; }
   showflag = no;
 }

T_Menu *T_xMenu::add(char *str, int x, int y)
 { T_xM *ptr = new T_xM(str, x, y, backcolor, forcolor);
   ptr->Tnext = NULL;
   if(Head==NULL)  Head = ptr;
   else
    { T_xM *p = Head;
      while(p->Tnext!=NULL)   p = p->Tnext;
      p->Tnext = ptr;
    }
   return &ptr->menu;
 }

BOOL T_xMenu::del(char *str)
 { if(Head==NULL)  return false;
   T_xM *ptr = Head, *ptr0=Head;
   while(ptr!=NULL)
    { if(strcmp(str, ptr->button.str)==0)  break;
      ptr0 = ptr;
      ptr = ptr->Tnext;
    }
   if(ptr==NULL)   return false;
   if(ptr==Head)  { Head = ptr->Tnext;  delete ptr; }
   else { ptr0->Tnext = ptr->Tnext;  delete ptr; }
   return true;
 }

BOOL T_xMenu::testclick()
 { T_xM  *ptr = Head, *ptr0;
   while(ptr!=NULL)
     { if(ptr->testclick()==true)
	 if(ptr->button.idown())
	  { hide();
	    ptr->button.show(down);
	    ptr->showmenu();  showflag = yes;  PTR = &ptr->menu;
	    return true;
	  }
	 else { hide();  return true; }
       if(showflag==yes && ptr->inbox())
	{ if(ptr->menu.ishow())  return false;
	  hide();  showflag = yes;  Sound();
	  ptr->button.show(down);   PTR = &ptr->menu;
	  ptr->showmenu();   return true;
	}
       if(&ptr->menu == PTR)  ptr0 = ptr;
       ptr = ptr->Tnext;
     }
   if(showflag==yes)
    { if(PTR->testclick())
       { str = PTR->str();  ptr0->button.show(up);  showflag = false;
	 while(getmousebutton()!=NOBUTTON);  Sound(2);  return click;
       }
      if(PTR->ishow()==false)   hide();
    }
   return false;
 }

//列表框

T_List::T_List(int _x1, int _y1, int _x2, int _y2, char attrib, char _backcolor, char _forcolor, char flag)
 { x1 = _x1;  y1 = _y1;  x2 = _x2;  y2 = _y2;
   if(attrib&Ff)  F = yes;   else F = no;
   if(attrib&Wl)  L = yes;   else L = no;
   backcolor = _backcolor;   forcolor = _forcolor;
   max = (y2-y1)/17;   Yy = 0;  Home = NULL;  num = 0;  begin = 0;
   No = -1;    if(flag==0)  showall(0);
 }

void T_List::order(BOOL ord, BOOL sight)
 { T_Li  *ptr = Home;
   while(ptr!=NULL)
    { T_Li *p = ptr->Tnext;
      while(p!=NULL)
       { if(ord==down&&strcmp(ptr->str, p->str)>0 ||
	     ord==up&&strcmp(ptr->str, p->str)<0)
	   { T_Li b = *ptr; *ptr = *p;  *p = b; }
	 p = p->Tnext;
       }
      ptr = ptr->Tnext;
    }
   if(sight==true)  show();
 }

void T_List::reset()
 { T_Li *ptr = Home, *ptr0;
   while(ptr!=NULL)  { ptr0 = ptr;  ptr = ptr->Tnext; delete ptr0; }
   str = NULL;  Home = NULL;
 }

BOOL T_List::move(int k)
 { if(No+k<0||No+k>=max)  return false;
   No += k;
   T_Li *ptr = Home;
   for(int i=0; i<No; i++)  ptr = ptr->Tnext;
   select(ptr->str);
   return true;
 }

void T_List::showall(int flag)
 { T_Li::x = x1;
   T_Li::width = x2-x1;  T_Li::begin = begin;
   T_Li::buttom = y2;  T_Li::top = y1;
   T_Li::backcolor = backcolor;  T_Li::forcolor = forcolor;
   GRAPHGETTING;  mouse_off();
   setfillstyle(1, backcolor);
   if(L==no) bar3ds(x1, y1, x2, y2, down, backcolor);
   else      bar(x1, y1, x2, y2);
   show();
   if(flag==0)
     scroll = T_Scroll(x2-10, y1+1, x2-2, y2-1, 1, 0, backcolor==LIGHTGRAY?CYAN:LIGHTGRAY);
   else scroll = T_Scroll(x2-10, y1+1, x2-2, y2-1, scroll.max, scroll.flag, backcolor==LIGHTGRAY?CYAN:LIGHTGRAY);
   mouse_on();  GRAPHSETTING;
 }

void T_List::add(char *_str, char attrib, char flag)
 { if((attrib&Ax) && F==false)
    { T_Li *ptr = Home;
      while(ptr!=NULL)
       { if(ptr->select(def)==true)   ptr->A = false;
	 ptr = ptr->Tnext;
       }
    }
   T_Li  *ptr = new T_Li(_str, attrib,
       backcolor, forcolor, x1, y1+Yy, x2-x1, begin, y2, y1, flag, this);
   Yy += 17;
   ptr -> Tnext = NULL;
   if(Home==NULL)  Home = ptr;
   else
    { T_Li *Ptr = Home;
      while(Ptr->Tnext!=NULL)  Ptr = Ptr->Tnext;
      Ptr->Tnext = ptr;
    }
   if(++num>max)  scroll.setflag(CURSET, num-max+1, flag);
   else           scroll.setflag(0, 1, flag);
 }

T_Li::T_Li(char *_str, char  attrib, char _backcolor,
     char _forcolor, int _x, int _y, int _width, int _begin,
	int _buttom, int _top, char flag, T_List *p)
 { strcpy(str, _str);
   backcolor = _backcolor;   forcolor = _forcolor;
   x = _x;  y = _y;  width = _width;  begin = _begin;  buttom = _buttom;
   A = W = no;   top = _top;  begin = 0;
   if(attrib&Ax)  { A = yes; p->str = str; }  if(attrib&Wx)  W = yes;
   if(flag==0)  show();
 }

void T_Li::show()
 { int X = x, Y = y-begin*17;
   if(Y+17>buttom)  return;
   if(Y<top)        return;
   mouse_off();
   clear();
   if(A==true)
    { setfillstyle(1, ~backcolor);
      bar(X+1, Y+2, X+width-11, Y+18);
    }
   if(W==yes)      setcolor(LIGHTGRAY);
   else if(A==no)  setcolor(forcolor);
   else            setcolor(backcolor);
   char ch = 0;
   if((strlen(str)<<3)+8>width)
     { ch = str[(width>>3)-1];  str[(width>>3)-1] = '\0'; }
   print(X+1, Y+2, str);
   if(ch)  str[(width>>3)-1] = ch;
   mouse_on();
 }

BOOL T_Li::select(BOOL _A)
 { if(_A==def)  return A;
   A = _A;      show();
   return A;
 }

BOOL T_Li::able(BOOL _W)
 { if(_W==def)  return W;
   W = _W;
   show();
   return W;
 }

void T_Li::clear()
 { int X = x, Y = y-begin*17;
   if(Y+17>buttom)  return;
   if(Y<top)        return;
   mouse_off();
   setfillstyle(1, backcolor);
   bar(X+1, Y+2, X+width-11, Y+18);
   mouse_on();
 }

BOOL T_List::del(char *str, char flag)
 { if(moveptr(str)==false)  return false;
   T_Li *ptr = Home;
   if(PTR==Home)  Home = PTR->Tnext;
   else
    { while(ptr->Tnext!=PTR)  ptr = ptr->Tnext;
      ptr->Tnext = PTR->Tnext;
    }
   int Y = PTR->y;
   delete PTR;
   while(ptr!=NULL)
    { if(ptr->y>Y)
	{ if(flag==0)  ptr->clear();
	  ptr->y -= 17;
	  if(flag==0)  ptr->show();
	}
      ptr = ptr->Tnext;
    }
   Yy -= 17;
   if(--num>max)  scroll.setflag(CURSET, num-max+1, flag);
   return true;
 }

BOOL T_List::select(BOOL _A, char *str)
 { if(moveptr(str)==false)  return false;
   if(_A==def)  return PTR->select(def);
   if(_A==true && F==false)
    { T_Li *ptr = Home;
      while(ptr!=NULL)
	{ if(ptr->select(def)==true)  ptr->select(false);
	  ptr = ptr->Tnext;
	}
    }
   PTR->select(true);
   str = PTR->str;
   return true;
 }

BOOL T_List::moveptr(char *str)
 { T_Li  *ptr = Home;  char *s, *s0;
   while(ptr!=NULL)
    { s0 = ptr->str;  s = str;
      while(*s)
	{ if(*s0 != *s)  break;
	  s0++;   s++;
	}
      if(*s==0)  break;
      ptr = ptr->Tnext;
    }
   if(ptr==NULL)  return false;
   PTR = ptr;  return true;
 }

void T_List::show()
 { T_Li *ptr = Home;
   mouse_off();
   while(ptr!=NULL)
     { ptr->show();  ptr = ptr->Tnext; }
   mouse_on();
 }

BOOL T_List::testclick()
 { if(getmousebutton()!=LEFTBUTTON)   return false;
   if(!imouse_inbox(x1, y1, x2, y2))  return false;
   T_Li::x = x1;
   T_Li::width = x2-x1;  T_Li::begin = begin;
   T_Li::buttom = y2;  T_Li::top = y1;
   T_Li::backcolor = backcolor;  T_Li::forcolor = forcolor;

   int flag = 0;
   if(getmousebutton()==LEFTBUTTON && scroll.inbox())  flag = 1;
   if(scroll.testclick())
    { begin = scroll.flag;
      T_Li::begin = begin;
      show();  return false;
    }
   if(flag==1)  return false;

   int y = getmouse_y();
   int K = (y-y1)/17+begin;
   T_Li *ptr=Home;
   for(int i=0; i<K; i++)
    { if(ptr==NULL)  return false;
      ptr = ptr->Tnext;
    }
   No = K;
   if(ptr->W==true)  return false;
   if(F==true)   ptr->select((BOOL)(1-ptr->A));
   else
    { T_Li  *Ptr = Home;
      while(Ptr!=NULL)
	{ if(Ptr->A==true)  Ptr->select(false);  Ptr = Ptr->Tnext; }
      ptr->select(true);
    }
   str = ptr->str;
   while(getmousebutton()!=NOBUTTON);
   Sound(2);   return  true;
 }


T_Combo::T_Combo(int x, int y, int width, int high, char *_str, char backcolor, char forcolor)
 { x1 = x;  y1 = y;   maxhigh = high;
   x2 = x+width;  y2 = y+20;   ptr = NULL;
   showflag = 0;  str = _str;  max = 0;
   text = T_Text(x, y, width-15, str, 0, backcolor, forcolor);
   list = T_List(x-1, y+22, x+width, y+22+20, Wl, backcolor, forcolor, NOSIGHT);
   button = T_Button(x+width-13, y, x+width, y+20, "↓", Tx|Fx|Cx, backcolor, forcolor);
 }

BOOL T_Combo::move(int k)
 { if(list.No+k<0||list.No+k>=max)  return false;
   list.No += k;
   T_Li *ptr = list.Home;
   for(int i=0; i<list.No; i++)  ptr = ptr->Tnext;
   T_Li *p = list.Home;
   while(p!=NULL)
    { if(p!=ptr)  p->A = false;
      else { p->A = true;  text.str(p->str);  str = p->str; }
      p = p->Tnext;
    }
   return true;
 }

void T_Combo::add(char *_str, char attrib)
 { int H = ++max*17;
   if(H>maxhigh)  H = maxhigh;   list.setpn(H);
   list.add(_str, attrib, NOSIGHT);
   if(attrib&Ax)
     { text.str(_str);  str = _str;  list.No = max-1; }
 }

void T_Combo::findorder()
 { T_Li *p = list.Home;   int N = 0;
   while(p!=NULL) { if(strcmp(p->str, str)==0) break; N++; p=p->Tnext; }
   if(p!=NULL)
     { list.No = N;   T_Li *P = list.Home;
       while(P!=NULL)  { P->A = false;  P = P->Tnext; }
       p->A = true;
     }
 }

void msg(char *str="提示", char backcolor=CYAN, char forcolor=WHITE, int Ti=10);
BOOL T_Combo::testclick()
 { if(button.testclick())
    { if(list.Home==NULL)
	{  msg("无选项！", RED, YELLOW); button.show(up); return false; }
      if(showflag==1)
	{ off();  putimage(list.x1, list.y1, ptr, COPY_PUT);  on();
	  if(ptr!=NULL)  delete ptr;  ptr = NULL;  showflag = 0;
	  button.show(up);  return false;
	}
      showflag = 1;    T_LineEdit::shines(off);
      int x1=list.x1, y1=list.y1, x2=list.x2, y2=list.y2;
      if(ptr!=NULL)  delete ptr;
      ptr = new char[imagesize(x1, y1, x2, y2+2)];
      if(ptr==NULL)  return false;
      mouse_off();  getimage(x1, y1, x2, y2+2, ptr);  mouse_on();
      list.showall(NOSIGHT);  return false;
    }
   if(getmousebutton()!=LEFTBUTTON)   return false;
   if(imouse_inbox(x1, y1, x2, y2))
    { if(showflag==1)
	 { off();  putimage(list.x1, list.y1, ptr, COPY_PUT);  on();
	   if(ptr!=NULL)  delete ptr;  ptr = NULL;  showflag = 0;
	   button.show(up);
	 }
      char S[50];  strcpy(S, text.ptr);
      BOOL RES = text.edit();
      if(RES==true)  if(strcmp(S, text.ptr)==0)  RES = false;
      if(RES==true) { str = text.ptr;  findorder(); }
      return RES;
    }
   if(!imouse_inbox(list.x1, list.y1, list.x2, list.y2))
    { if(showflag==0)  return false;
      off(); putimage(list.x1, list.y1, ptr, COPY_PUT); on();  button.show(up);
      if(ptr!=NULL)  delete ptr;  ptr = NULL; showflag = 0;  return false;
    }
   else
    { if(showflag==0)  return false;
      BOOL RES = list.testclick();
      if(RES==true)
	{ off();  putimage(list.x1, list.y1, ptr, COPY_PUT);  on();
	  if(ptr!=NULL)  delete ptr;  ptr = NULL; showflag = 0;
	  button.show(up);   str = list.str;   text.str(str);
	}
       return RES;
    }
 }


//编辑框

BOOL _lineflag_ = false;
void formatshines(char *ptr)
 { if(T_LineEdit::K==0)  return;
   if(T_LineEdit::K>strlen(ptr))  T_LineEdit::K=strlen(ptr);
   BOOL flag = false;
   for(int k=0; k<T_LineEdit::K; k++)
     if(ptr[k]&0x80)  flag = (BOOL)(1-flag);
   if(flag==true)  T_LineEdit::K--;
 }

void T_LineEdit::show()
 { if(y+15>=T_EditBox::y2 || y<T_EditBox::y1)  return;
   mouse_off();  T_LineEdit::shines(off);
   setfillstyle(1, backcolor);
   bar(2, y, 628, y+15);
   if(str!=NULL)   _pn.qshowstr(7, y, str, forcolor, begin, end);
   if(Sf==true)
    { setwritemode(XOR_PUT);
      if(showkind==LINE)
       { setcolor(forcolor);
	 line(7, y+15, 7+(strlen(str)<<3), y+15);
       }
      else if(showkind==HIDE)
       { setcolor(forcolor);  setlinestyle(2, 0, 1);
	 line(7, y+15, 7+(strlen(str)<<3), y+15);
	 setlinestyle(0, 0, 1);
       }
      setwritemode(COPY_PUT);
     }
   setcolor(forcolor);
   mouse_on();
 }

void T_LineEdit::clear()
 { mouse_off();  T_LineEdit::shines(off);
   setfillstyle(1, backcolor);
   bar(2, y, 628, y+15);
   mouse_on();
 }

void T_LineEdit::del(int k)
 { T_LineEdit::shines(off);
   int len = strlen(ptr);
   if(k>=len)  return;
   for(int i=k; i<len; i++)   ptr[i] = ptr[i+1];
   moveimage(k+1, len, k);
   setfillstyle(1, backcolor);
   bar(len*8, y, len*8+7, y+15);
   T_EditBox::ichange = true;
 }

int T_LineEdit::findstr(char *s, int k, BOOL unupsee)
 { int len = strlen(s), LEN = strlen(str);
   if(LEN==0 || LEN<=k)  return -1;
   for(; k<LEN; k++)
    { int j = k;
      for(int i=0; i<len; i++)
	{ if(unupsee==false) { if(str[j++]!=s[i])  break; }
	  else if(toupper(str[j++])!=toupper(s[i]))  break;
	}
      if(i==len)  break;
    }
   if(k==LEN)  return -1;
   else        return k;
 }

BOOL T_EditBox::findstr(char *s, BOOL unupsee)
 { int N = now, L;
   T_LineEdit *ptr = PTR;
   while(ptr!=NULL)
    { if((L=ptr->findstr(s, ptr==PTR?T_LineEdit::K+1:0, unupsee))!=-1)
       { PTR = ptr;	 gotolinenum(N);
	 T_LineEdit::shines(off);  T_LineEdit::K = L;
	 PTR->setpn(y1+2+((now-begin)<<4));   return true;
       }
      ptr = ptr->Tnext;
      N ++;
    }
   return false;
 }

void T_LineEdit::add(char ch, int k)
 { T_LineEdit::shines(off);
   int len = strlen(ptr);
   for(int i=len; i>=k; i--)   ptr[i+1] = ptr[i];
   ptr[k] = ch;
   if(Sight==true)
    { if(k!=len) moveimage(k, len, k+1);
      setfillstyle(1, backcolor);
      bar(k*8+8, y, k*8+14, y+15);
    }
   char C[2] = { ch, 0 };
   if(end!=NOSEL)  end ++;
   if(begin!=NOSEL&&end==NOSEL || begin==NOSEL&&end!=NOSEL || k>=begin&&k<=end)
      _pn.qshowstr(k*8+7, y, C, forcolor, 0, 0);
   else   _pn.qshowstr(k*8+7, y, C, forcolor);
   if(Sf==true && Sight==true)
    { setwritemode(XOR_PUT);
      if(showkind==LINE)
	{ setcolor(forcolor);  line(7+(k<<3)+1, y+15, 15+(k<<3), y+15); }
      else if(showkind==HIDE)
	{ setcolor(forcolor);  setlinestyle(2, 0, 1);
	  line(7+(k<<3)+1, y+15, 15+(k<<3), y+15);
	  setlinestyle(0, 0, 1);
	}
      setwritemode(COPY_PUT);
    }
   T_EditBox::ichange = true;
 }

void T_LineEdit::addstr(char *s, int k)
 { T_LineEdit::shines(off);
   int len = strlen(ptr);
   for(int i=len; i>=k; i--)   ptr[i+2] = ptr[i];
   ptr[k] = *s;  ptr[k+1] = *(s+1);
   if(Sight==true)
    { if(k!=len)  moveimage(k, len, k+2);
      setfillstyle(1, backcolor);
      bar(k*8+8, y, k*8+22, y+15);
    }
   if(end!=NOSEL)  end += 2;
   if(begin!=NOSEL&&end==NOSEL || begin==NOSEL&&end!=NOSEL || k>=begin&&k<=end)
      _pn.qshowstr(k*8+7, y, s, forcolor, 0, 1);
   else  _pn.qshowstr(k*8+7, y, s, forcolor);
   if(Sf==true && Sight==true)
    { setwritemode(XOR_PUT);
      if(showkind==LINE)
	{ setcolor(forcolor);  line(7+(k<<3)+1, y+15, 23+(k<<3), y+15); }
      else if(showkind==HIDE)
	{ setcolor(forcolor);  setlinestyle(2, 0, 1);
	  line(7+(k<<3)+1, y+15, 23+(k<<3), y+15);
	  setlinestyle(0, 0, 1);
	}
      setwritemode(COPY_PUT);
    }
   T_EditBox::ichange = true;
 }


void T_LineEdit::moveimage(int k1, int k2, int k)
 { T_LineEdit::shines(off);
   int x1 = k1*8+8,  x2 = k2*8+8, y1 = y,  y2 = y+15;
   int X = k*8+8, Y = y;
   char far *ptr = new char[imagesize(x1, y1, x2, y2)];
   if(ptr==NULL)  return;
   getimage(x1, y1, x2, y2, ptr);
   putimage(X, Y, ptr, COPY_PUT);
   delete ptr;
 }

void T_LineEdit::shines(BOOL kind)
 { static long T = clock();   static BOOL dbflag;
   static k;
   if(kind==def)  { if((clock()-T)<T_EditBox::TIME)  return;  }
   else if(kind==on)  { if(dbflag==true)  return; }
   else { if(dbflag==false)  return; }
   dbflag = (BOOL)(1-dbflag);
   if(kind!=def)    dbflag = kind;
   setlinestyle(0, 0, 1);
   setcolor(~backcolor);
   if(dbflag==on)  k = K;
   int flag = imouse_inbox(0, y, 639, y+20);
   if(flag)  mouse_off();
   int X1 = (k<<3)+9,  X2 = X1+5;
   setwritemode(XOR_PUT);
   line(X1, y+15, X2, y+15);
   line(X1, y+16, X2, y+16);
   setwritemode(COPY_PUT);
   if(flag)  mouse_on();
   T = clock();
 }

void formatstr(char *ptr, int k=DEF)
 { if(k==DEF)  k = 0;
   for(int i=strlen(ptr)-1; i>=k; i--)
    { if(ptr[i]==32)  ptr[i] = '\0';
      else            break;
    }
 }

int T_LineEdit::edit()
 { static char key;   if(!kbhit()&&!key)  return 0;
   shines(off);
   GRAPHGETTING;  mouse_off();
   int  ch = key==0?getch():key;   key = 0;
   if(ch==0)  switch(ch=getch())
     { case LEFT_KEY:  if(K<=0)  break;
		       if(ptr[--K]&0x80 && ptr[K-1]&0x80 && K)  K --;
		       if(Sf==true && K>=strlen(ptr)-1 && ptr[K]==32)
			 { setcolor(BLACK); line(7+(K<<3), y+15, 15+(K<<3), y+15);
			   ptr[K] = '\0';
			 }
		       ch = 0;     break;
       case RIGHT_KEY: BOOL B = T_EditBox::ichange;
		       if(K<strlen(ptr))
			 { if(str[K++]&0x80 && ptr[K+1]&0x80)  K ++; }
		       else { if(strlen(ptr)<77)  add(' ', K++); }
		       ch = 0;  T_EditBox::ichange = B; break;
       case DELETE_KEY: if(ptr[K]=='\0')  { ch += 1000; break; }
		       if(ptr[K]&0x80 && ptr[K+1]&0x80) { del(K);  del(K); }
		       else del(K);  ch = 0;  break;
       case END_KEY:   formatstr(ptr);  K = strlen(ptr);  show(); ch = 0; break;
       case HOME_KEY:  K = 0; formatstr(ptr); show(); ch = 0;  break;
       case UP_KEY: case DOWN_KEY: case PAGEUP_KEY: case PAGEDOWN_KEY:
		       formatstr(ptr);  show();  ch += 1000;
		       if(K>strlen(ptr))  K = strlen(ptr);  break;
     }
   else switch(ch)
     { case ENTER_KEY:  case ESC_KEY:   ch += 1000;  break;
       case BACKSPACE_KEY: if(K!=0)
			     { if(ptr[K-1]&0x80 && ptr[K-2]&0x80 && K>1)
				 { del(--K);  del(--K); }
			       else del(--K);  ch = 0;
			     }
			   else ch+=1000; break;
       case 255:/*Ctrl+Delete*/  ch += 1000;  break;
       case TAB_KEY: for(int k=0; k<8; k++)
		       { if(strlen(ptr)<77) add(' ', K++); formatstr(ptr, K); }
		     ch = 0;  break;
       default: if(ch>=0 && ch<32)  break;
	      if(ch==32 && bioskey(2)&0x04)  {  ch = SPACE_KEY+1000;  break; }
	      if((ch&0x80)==0)
		 { if(strlen(ptr)<77)  { add(ch, K++);  ch = 0; }
		   else { key = ch;  ch = ENTER_KEY+1000; _lineflag_ = true; break; }
		 }
	      else
		{ if(strlen(ptr)<76)
		     { if(!kbhit()) break;
		       char s[3] = { ch, getch(), '\0' };
		       addstr(s, K);  K += 2;  ch = 0;
		     }
		   else { key = ch;  ch = ENTER_KEY+1000; _lineflag_ = true; break; }
		}
     } ret:
   mouse_on();   GRAPHSETTING;   return ch;
 }


void T_EditBox::gotolinenum(int num)
 { if(num>=begin&&num<=begin+high)  { setline(num); return; }
   begin = num-high/2;
   if(begin<0)  begin = 0;
   else if(begin+high>=max)  begin = max-high-1;
   T_LineEdit  *ptr = Head;
   for(int k=0; k<begin; k++) { if(ptr==NULL) break;  ptr = ptr->Tnext; }
   for(k=0; k<high; k++)
    { if(ptr==NULL)  break;
      T_LineEdit::setpn(y1+k*16+2);
      ptr->show();   ptr = ptr->Tnext;
    }
   setline(num);
 }

void T_EditBox::page(BOOL p)
 { int _begin = begin;
   if(max>=high)
     { if(p==down)  { begin += high-1;  now += high-1; }
       else         { begin -= high-1;  now -= high-1; }
     }
   else
     { if(p==down)  gotolinenum(max-1);
       else         gotolinenum(0);  return;
     }
   if(begin<0)  begin = 0;   else if(begin+high>=max)  begin = max-high;
   if(now<0)    now = 0;     else if(now>=max)  now = max-1;
   if(begin==_begin)  { setline(now);  return; }
   T_LineEdit  *ptr = Head;
   for(int k=0; k<begin; k++) { if(ptr==NULL) break;  ptr = ptr->Tnext; }
   for(k=0; k<high; k++)
    { if(ptr==NULL)  break;
      T_LineEdit::setpn(y1+k*16+2);
      ptr->show();   ptr = ptr->Tnext;
    }
   setline(now);  scroll.setflag(begin, max-high+1, 2);
 }

T_EditBox::T_EditBox(int _y1, int _y2, int _backcolor, int _forcolor, BOOL lock )
 { y1 = _y1;   y2 = _y2;
   backcolor = _backcolor;  forcolor = _forcolor;
   Head = NULL;    PTR = Head;
   begin = 0;  now = 0;  max = 0;  T_LineEdit::Sf = false;
   high = (y2-y1-2)/16;    L = lock;  la = true;  T_EditBox::key = 0;
   show();   Isel = false;    Line = true;    TIME = 2;
 }

void T_EditBox::reset(int color)
 { if(color!=DEF)  forcolor = color;
   mouse_off();  GRAPHGETTING;
   setfillstyle(1, backcolor);
   bar(0, y1+2, 639, y2-2);
   T_LineEdit *ptr = Head, *ptr0;
   while(ptr!=NULL)  { ptr0 = ptr;  ptr = ptr->Tnext;  delete ptr0; }
   Head = NULL;  begin = max = now = 0;  PTR = NULL;  T_LineEdit::K = 0;
   show();
   GRAPHSETTING;   mouse_on();
 }

void T_EditBox::free()
 { T_LineEdit *ptr = Head, *ptr0;
   while(ptr!=NULL)  { ptr0 = ptr;  ptr = ptr->Tnext;  delete ptr0; }
   Head = NULL;  begin = max = now = 0;  PTR = NULL;  T_LineEdit::K = 0;
 }

void T_EditBox::definebegin()  //定义块首
 { T_LineEdit *ptr = Head;
   while(ptr)
    { if(ptr==PTR)  ptr->begin = T_LineEdit::K;
      else          ptr->begin = NOSEL;
      ptr = ptr->Tnext;
    }
   showsel();
 }

void T_EditBox::defineend()     //定义块尾
 { T_LineEdit *ptr = Head;
   while(ptr)
    { if(ptr==PTR)  ptr->end = T_LineEdit::K;
      else          ptr->end = NOSEL;
      ptr = ptr->Tnext;
    }
   if(Isel==false)  Head->begin = 0;
   showsel();
 }

void T_EditBox::showsel()
 { T_LineEdit *ptr = Head;  int now = 0;
   while(ptr)
    { if(ptr->begin!=NOSEL) { Isel = true;  break; }
      if(ptr->end!=NOSEL)
	{ T_LineEdit *ptr = Head;
	  while(ptr)
	    { ptr->begin = NOSEL;  ptr->end = NOSEL;
	      ptr = ptr->Tnext;
	    }
	  Isel = false;  break;
	}
      ptr = ptr->Tnext;
    }
   ptr = Head;
   while(ptr)
    { if(ptr->begin!=NOSEL)  break;
      if(now-begin>=0&&now-begin<high)
       { T_LineEdit::setpn(y1+(now-begin)*16+2);
	 ptr->show();
       }
      ptr = ptr->Tnext;  now ++;
    }
   while(ptr)
    { if(ptr->begin==NOSEL)  ptr->begin = 0;
      if(now-begin>=0&&now-begin<high)
       { T_LineEdit::setpn(y1+(now-begin)*16+2);
	 ptr->show();
       }
      if(ptr->end!=NOSEL)  break;
      ptr = ptr->Tnext;  now ++;
    }
   if(ptr)  ptr = ptr->Tnext;  now ++;
   while(ptr)
    { ptr->begin = NOSEL;  ptr->end = NOSEL;
      if(now-begin>=0&&now-begin<high)
       { T_LineEdit::setpn(y1+(now-begin)*16+2);
	  ptr->show();
       }
      ptr = ptr->Tnext;  now ++;
    }
 }

void T_EditBox::addstr(char *ptr)   //添加字符串
 { mouse_off();  char ch;   Sight = false;
   formatstr(PTR->str);  T_LineEdit::shines(off);
   while(*ptr)
    { ch = *ptr++;
      if(ch=='\n')
	{ edit(ENTER_KEY);  T_LineEdit::K = 0;  continue; }
      if(ch==9)
	{ for(int k=0; k<8; k++)
	   if(T_LineEdit::K<77)   PTR->add(' ', T_LineEdit::K++);
	    else
	     { edit(ENTER_KEY);
	       T_LineEdit::K=0; PTR->add(' ', T_LineEdit::K++);
	     }
	}
      else if((ch&0x80)==0)
	 { if(T_LineEdit::K<77)   PTR->add(ch, T_LineEdit::K++);
	   else
	    { edit(ENTER_KEY);
	      T_LineEdit::K=0; PTR->add(ch, T_LineEdit::K++);
	    }
	 }
      else
	{ char s[3] = { ch, *ptr++, '\0' };
	  if(T_LineEdit::K)  PTR->addstr(s, T_LineEdit::K);
	  else
	   { T_LineEdit::K=0;  edit(ENTER_KEY);
	     PTR->addstr(s, T_LineEdit::K);
	   }
	  T_LineEdit::K += 2;
	}
    }
   PTR->end = T_LineEdit::K;
   T_LineEdit  *p = Head;
   for(int k=0; k<begin; k++) { if(p==NULL) break;  p = p->Tnext; }
   for(k=0; k<high; k++)
    { if(p==NULL)  break;
      T_LineEdit::setpn(y1+k*16+2);
      p->show();   p = p->Tnext;
    }
   mouse_on();   Sight = true;
   resetsel();
   gotolinenum(now);
 }

BOOL T_EditBox::ishinesinblock()   //光标是否在块内
 { if(PTR->begin==NOSEL && PTR->end==NOSEL)  return false;
   int begin = PTR->begin==NOSEL?0:PTR->begin,
       end = PTR->end==NOSEL?100:PTR->end;
   int K = T_LineEdit::K;
   if(K<=end && K>=begin)  return true;
   return false;
 }

void T_EditBox::resetline()    //刷新显示行
 { T_LineEdit  *ptr = Head;
   int K = 0;
   while(ptr) {  if(ptr==PTR)  break;  K ++;  ptr = ptr->Tnext; }
   T_LineEdit::setpn(y1+(K-begin)*16+2);
   ptr->show();
 }

void T_EditBox::resetsel()       //块取消
 { T_LineEdit  *ptr = Head;
   while(ptr)
    { ptr->begin = NOSEL;
      ptr->end   = NOSEL;
      ptr = ptr->Tnext;
    }
   showsel();
   Isel = false;
 }

void T_EditBox::selectall()     //全选
 { T_LineEdit  *ptr = Head;
   while(ptr)
    { if(ptr==Head)  ptr->begin = 0;
      else           ptr->begin = NOSEL;
      if(ptr->Tnext==NULL)  ptr->end = strlen(ptr->str);
      else           ptr->end = NOSEL;
      ptr = ptr->Tnext;
    }
   showsel();
 }

void T_EditBox::delstr()       //删除选定内容
 { if(Isel==false)  return;
   T_LineEdit  *ptr = Head, *Pshines=Head;
   while(ptr)
    { if(ptr->begin!=NOSEL)  break;
      ptr = ptr->Tnext;
    }
   char S[80];  int k, K=0;
   for(k=0; k<80; k++)
     if(k<ptr->begin || k>ptr->end)
       if( (S[K++]=ptr->str[k])=='\0')  break;
   if(ptr->end==NOSEL)  S[ptr->begin] = '\0';
   strcpy(ptr->str, S);
   if(ptr->end!=NOSEL)  goto drec;
   if(ptr)  ptr = ptr->Tnext;
   while(ptr)
    { if(ptr->end!=NOSEL) break;
      if(ptr==Head)  Head = Head->Tnext;
      else
       { T_LineEdit *p = Head;
	 while(p->Tnext!=ptr)   p = p->Tnext;
	 p->Tnext = ptr->Tnext;
       }
      delete ptr;  max --;
      ptr = ptr->Tnext;
    }
   if(ptr)
    { if(ptr==PTR)
	{ T_LineEdit::K -= ptr->end+1;
	  if(T_LineEdit::K<0)  T_LineEdit::K = 0;
	}
      int K=0;
      for(k=0; k<=80; k++)
	 if(k>ptr->end)
	    if( (S[K++]=ptr->str[k]) == '\0')  break;
      strcpy(ptr->str, S);
    }
   Pshines = ptr;
   drec:;
   mouse_off();    T_LineEdit::shines(false);
   setfillstyle(0, 0);    bar(6, y1+2, 628, y2-2);   show();
   if(begin+high>=max)  begin = max-high-1;
   if(begin<0)          begin = 0;
   ptr = Head;  int L = 0;
   while(ptr)
    { ptr->begin = NOSEL;  ptr->end = NOSEL;
      if(L-begin>=0&&L-begin<high)
       { T_LineEdit::setpn(y1+(L-begin)*16+2);
	 ptr->show();
       }
      ptr = ptr->Tnext;  L ++;
    }
   mouse_on();
   T_EditBox::ichange = true;
   Isel = false;

   int Line = 0;   ptr = Head;
   while(ptr)  { if(ptr==PTR)  break;    Line ++;  ptr = ptr->Tnext; }
   if(ptr==NULL)
    { ptr = Head;  Line = 0;
      while(ptr)  { if(ptr==Pshines)  break;  Line ++; ptr = ptr->Tnext; }
    }
   gotolinenum(Line);   formatshines(PTR->str);
 }

int T_EditBox::charnum()    //选中字符个数
 { T_LineEdit *ptr = Head;  int num = 0;
   while(ptr)
    { if(ptr->begin!=NOSEL)  break;
      ptr = ptr->Tnext;
    }
   while(ptr)
    { if(ptr->begin==NOSEL)  break;
      num += strlen(ptr->str)+1;
      ptr = ptr->Tnext;
    }
   return num;
 }

char *T_EditBox::selptr()     //获取选定字符串
 { if(Isel==false)  return NULL;
   char *str = new char[charnum()];
   str[0] = '\0';
   T_LineEdit *ptr = Head;
   while(ptr)
    { if(ptr->begin!=NOSEL)  break;
      ptr = ptr->Tnext;
    }
   while(ptr)
    { strcat(str, ptr->str + ptr->begin);
      strcat(str, "\n");
      if(ptr->Tnext->begin==NOSEL)  break;
      ptr = ptr->Tnext;
    }
   if(ptr!=NULL&&ptr->end!=NOSEL)
     str[strlen(str)-(strlen(ptr->str)-(ptr->end))] = '\0';
   return str;
 }

#define _Up  *ptr-- = *ptr0--
#define _Dn  *ptr++ = *ptr0++
void T_EditBox::moveline(int k, BOOL kind, int k0)
 { int Y1, Y2;
   T_LineEdit::shines(off);
   outportb(0x3c4, 2);    outportb(0x3c5, forcolor);
   outportb(0x3ce, 4);
   for(int i=0; i<4; i++)
     if(forcolor&(0x01<<i))  { outportb(0x3cf, i);  break; }
   mouse_off();
   if(kind==down)
    { Y1 = (k+1)*16+2+y1;
      if(k0==DEF)  Y2 = y2-2;   else  Y2 = (k0+1)*16+2+y1;
      if(Y1>Y2)  { int t=Y1;  Y1=Y2;  Y2 = t; }
      char far *ptr = (char far*)0xA0000000L+80*Y2, far *ptr0 = ptr-(80<<4);
      int lines = (Y2-Y1)<<3, k;
      for(register int j=0; j<lines; j++)
	  { _Up; _Up; _Up; _Up; _Up; _Up; _Up; _Up; _Up; _Up; }
    }
   else
    { if(k0==DEF)  Y1 = y1+2;  else  Y1 = (k0+1)*16+2+y1;
      Y2 = k*16+y1+2;
      if(Y1>Y2)  { int t=Y1;  Y1=Y2;  Y2 = t; }
      char far *ptr = (char far*)0xA0000000L+80*Y1, far *ptr0 = ptr+(80<<4);
      int lines = (Y2-Y1)<<3, k;
      for(register int j=0; j<lines; j++)
	  { _Dn; _Dn; _Dn; _Dn; _Dn; _Dn; _Dn; _Dn; _Dn; _Dn; }
    }
   outportb(0x3c5, 0x0f);
   mouse_on();
 }
#undef _Dn
#undef _Up


void T_EditBox::delline(int num)
 { if(Head==NULL)  return;
   T_LineEdit *ptr = go(num), *ptr0;
   if(ptr==NULL)  return;
   if(ptr==Head)  Head = ptr->Tnext;
   else
    { ptr0 = Head;
      while(ptr0->Tnext!=ptr)  ptr0 = ptr0->Tnext;
      ptr0->Tnext = ptr->Tnext;
    }
   delete ptr;
   moveline(num-begin, up, high-2);
   if(max-begin>=high)
     { ptr = go(begin+high-1);
       T_LineEdit::setpn(y2-18);
       if(ptr==NULL)  T_LineEdit::clear();
       else           ptr->show();
       if(begin>0&max-begin==high)  move(down);
     }
   else  if(begin>0)   move(down);
   max --;
   PTR = go(now);  if(PTR==NULL)  PTR = go(--now);
   if(now<0)  now = 0;  scroll.setflag(begin, max-high+1, 2);
 }

void T_EditBox::addline(char *str, BOOL pn, int k, int ishow, SHOWKIND kind, BOOL selflag)
 { if(Head==NULL)     pn = def;
   if(ishow!=QUICK && now-begin>=0&&now-begin<high-1)
    { if(pn==up)         moveline(now-begin, down);
      else if(pn==down)  moveline(now-begin+1, down);
    }
   T_LineEdit::setpn(y1+(pn==down?++now-begin:pn==up?now++-begin:now-begin)*16+2);
   T_LineEdit *ptr;
   if(now-begin>=0 && now-begin<high)
      ptr = new T_LineEdit(str, backcolor, forcolor, ishow, kind, selflag?0:NOSEL, NOSEL);
   else ptr = new T_LineEdit(str, backcolor, forcolor, NOSIGHT, kind, selflag?0:NOSEL, NOSEL);
   max ++;
   if(Head==NULL)   { ptr->Tnext = NULL;  Head = ptr; }
   else if(pn==up)
    { if(Head==PTR)
	{ ptr->Tnext = Head;   Head = ptr; }
       else
	{ T_LineEdit *first = Head;
	  while(first->Tnext!=PTR);   first = first->Tnext;
	  first->Tnext = ptr;  ptr->Tnext = PTR;
	}
    }
   else
    { T_LineEdit *next = PTR->Tnext;
      PTR->Tnext = ptr;  ptr->Tnext = next;
    }
   PTR = ptr;
   if(k!=DEF)  T_LineEdit::K = k;
   if(T_LineEdit::K>strlen(PTR->str))  T_LineEdit::K = strlen(PTR->str);
   PTR->beginedit();
   scroll.setflag(begin, max-high+1, kind);
   if(selflag==true)
     { T_LineEdit *ptr = Head;
       while(ptr)
	{ if(ptr!=PTR)  ptr->end = NOSEL;
	  ptr = ptr->Tnext;
	}
     }
 }

void T_EditBox::setline(int num)
 { if(Head==NULL)  return;
   int flag = 0;
   T_LineEdit::shines(off);
   if(num<0)  num = 0;
   else if(num>=max)  num = max-1;
   if(num-begin>=high) { begin++;  moveline(high-1, up); flag = 1; }
   if(num-begin<0)     { begin--;  moveline(0, down);  flag = 1; }
   PTR = Head;  now = 0;
   for(int k=0; k<num; k++)
    { if(PTR->Tnext==NULL)  break;
      now ++; PTR = PTR->Tnext;
    }
   if(T_LineEdit::K>strlen(PTR->str))  T_LineEdit::K = strlen(PTR->str);
   PTR->beginedit();
   if(flag==1)
    { T_LineEdit::setpn(y1+(now-begin)*16+2);
      PTR->show();
    }
   scroll.setflag(begin, max-high+1, 2);
   formatshines(PTR->str);
 }

void T_EditBox::show()
 { T_LineEdit::shines(off);
   setcolor(LIGHTGRAY);   rectangle(1, y1, 638, y2);
   setcolor(DARKGRAY);    rectangle(2, y1+1, 637, y2-1);
   scroll = T_Scroll(639-10, y1+1, 639-1, y2-1, 1, 0, CYAN, LIGHTBLUE);
   T_LineEdit *ptr = Head;  int k = 0;
   while(ptr!=NULL)
    { ptr->setpn(y1+16*(k++)+2);
      ptr->show();
      ptr = ptr->Tnext;
    }
 }

void T_LineEdit::changestr(char *_str, int ishow)
  { strcpy(str, _str);
    T_EditBox::ichange = true;
    if(ishow!=NOSIGHT)  show();
  }

void T_EditBox::replacestr(char *_str, int len)
 { char S[200];  strcpy(S, PTR->str);
   char *ptr0 = S + T_LineEdit::K;
   char *ptr = ptr0;
   while( (*ptr++ = *(ptr+len)) != '\0');  ptr --;
   int Len = strlen(_str);
   while(ptr>=ptr0) { *(ptr+Len) = *ptr;  ptr --; }
   for(int k=0; k<Len; k++)  ptr0[k] = _str[k];
   if(strlen(S)>77)
     { int L = T_LineEdit::K;
       char ch = S[77]; S[77] = '\0'; strcpy(PTR->str, S);
       S[77] = ch; PTR->show();
       T_LineEdit::K = strlen(PTR->str);  edit(ENTER_KEY);
       strcpy(PTR->str, S+77);  PTR->show();
       gotolinenum(now-1);  T_LineEdit::K = L+1;
     }
   else { strcpy(PTR->str, S); T_LineEdit::K+=Len; PTR->show(); }
 }

void T_LineEdit::showattrib(SHOWKIND kind)
  { showkind = kind;  T_LineEdit::Sf = true;
    T_EditBox::ichange=true;  show();
  }

T_LineEdit *T_EditBox::go(int num)
 { T_LineEdit *ptr = Head;
   for(int k=0; k<num; k++)
    { if(ptr==NULL)   return NULL;
      ptr = ptr->Tnext;
    }
   return ptr;
 }

void T_EditBox::move(BOOL kind)
 { if(kind==up)
     { moveline(high-1, up);
       T_LineEdit::setpn(y1+(high-1)*16+2);
       T_LineEdit *ptr = go(begin+high);
       T_LineEdit::setpn(y2-18);
       ptr->show();   begin ++;
     }
   else
     { if(begin==0)  return;
       moveline(0, down);
       T_LineEdit *ptr = go(--begin);
       T_LineEdit::setpn(y1+2);
       ptr->show();
     }
   scroll.setflag(begin, max-high+1, 2);
 }


void T_EditBox::edit(int KEY)
 { int ch;
   if(KEY==-1)
    { if(Head==NULL)  { addline("", NOSIGHT);  PTR = Head; }
      int flag = scroll.flag;
      if(scroll.testclick())
       { begin = scroll.flag;
	 T_LineEdit  *ptr = Head;
	 for(int k=0; k<begin; k++) { if(ptr==NULL) break;  ptr = ptr->Tnext; }
	 for(k=0; k<high; k++)
	  { if(ptr==NULL)  break;
	    T_LineEdit::setpn(y1+k*16+2);
	    ptr->show();   ptr = ptr->Tnext;
	  }
	 now += begin-flag;  setline(now);
	 return;
       }
     if(L == no)
      { if(getmousebutton()==LEFTBUTTON)
	 { if(imouse_inbox(7, y1+2, 639, y2))
	    { T_LineEdit::shines(off);
	      int N = (getmouse_y()-y1-2)/16;
	      int K = (getmouse_x()-8)/8;
	      if(N+begin!=now || K!=T_LineEdit::K)
	       { setline(begin+N);
		 T_LineEdit::K = K;
		 formatshines(PTR->str);  return;
	       }
	    }
	 }
	PTR->setpn(y1+2+((now-begin)<<4));
	T_LineEdit::shines();
	T_EditBox::key = ch = PTR->edit();
      }
     else if(kbhit())
      { T_EditBox::key = ch = getch();
	if(ch==PAGEUP_KEY || ch==PAGEDOWN_KEY)  ch += 1000;
      }
    }
   else ch = KEY + 1000;
   if(ch<1000)  return;
   switch(ch-1000)
    { case UP_KEY:   setline(now-1);  break;
      case DOWN_KEY: setline(now+1);  break;
      case ENTER_KEY:  if(la==false)  break;
	 char S[100]="", p[100];  int k = 0,  K=T_LineEdit::K;
	 strcpy(p, PTR->str);  char *pstr = PTR->str;
	 if(_lineflag_==false&&Line==true)  while(p[k]==32)  S[k++] = 32;
	 S[k] = '\0';
	 if(K<strlen(PTR->str))
	  { int m=-1;  while((p+K)[++m]==32);
	    strcat(S, p+K+m);   p[K+m] = '\0';
	    if(now-begin>=0&&now-begin<high&&Sight==true)  PTR->changestr(p);
	    else                              PTR->changestr(p, NOSIGHT);
	  }
	 BOOL selflag = PTR->begin!=NOSEL?true:false;
	 if(now-begin>=high-1 || Sight==false)
	    addline(S+((!Head||!Head->Tnext)&&*S==32), down, k, NOSIGHT, PTR->showkind, selflag);
	 else
	    addline(S+((!Head||!Head->Tnext)&&*S==32), down, k, 0, PTR->showkind, selflag);
	 if(now-begin>=high && Sight==true)  move(up);
	 scroll.setflag(begin, max-high+1, 2);  
	 if(_lineflag_ && strlen(pstr)<76)
	   { setline(now-1);  T_LineEdit::K = strlen(PTR->str); }
	 _lineflag_ = false; T_EditBox::ichange = true;
	 formatshines(PTR->str);  break;
      case DELETE_KEY:  if(la==false)  break;
	 T_LineEdit *ptr0 = PTR->Tnext;
	 if(ptr0==NULL)  break;  p[0] = p[1] = '\0';
	 char S0[200] = "";  strcpy(S0, PTR->str);
	 int n=-1;   while(ptr0->str[++n]==32);
	 strcat(S0, ptr0->str+n);
	 if(strlen(S0)>77)
	   { int k=0; int flag = 0;
	     for(k=0; k<77; k++)  if(S0[k]&0x80)  flag = 1-flag;
	     p[0] = S0[77-flag];  S0[77-flag] = '\0'; strcat(p, S0+78-flag); }
	 PTR->changestr(S0);
	 if(*p!='\0')
	   { T_LineEdit::setpn(y1+2+(now-begin+1)*16);
	     ptr0->changestr(p);
	   }
	 else  delline(now+1);   T_EditBox::ichange = true;
	 break;
      case BACKSPACE_KEY:  if(la==false)  break;
	 if(PTR==Head)  break;
	 T_LineEdit *ptr=Head;   p[0] = p[1] = '\0';
	 while(ptr->Tnext!=PTR)   ptr = ptr->Tnext;
	 ptr0 = ptr->Tnext;
	 strcpy(S0, ptr->str);	 strcat(S0, ptr0->str);
	 if(strlen(S0)>77)
	   { int k=0; int flag = 0;
	     for(k=0; k<77; k++)  if(S0[k]&0x80)  flag = 1-flag;
	     p[0] = S0[77];  S0[77] = '\0';  strcat(p, S0+78);
	   }
	 T_LineEdit::K = strlen(ptr->str);
	 T_LineEdit::setpn(y1+2+(now-begin-1)*16);
	 ptr->changestr(S0);
	 if(*p!='\0')
	   { T_LineEdit::setpn(y1+2+(now---begin)*16);
	     ptr0->changestr(p);
	   }
	 else  delline(now--);	 PTR = ptr;  break;
      case PAGEUP_KEY:    page(up);    break;
      case PAGEDOWN_KEY:  page(down);  break;
      default: T_EditBox::key = ch-1000;
    }
   PTR->beginedit();
 }

BOOL T_EditBox::read(char *file, char *path, BOOL flag)
 { char filename[50], str[80];
   sprintf(filename, "%s\\%s", path, file);
   FILE *fp = fopen(filename, "rt");
   if(fp==NULL)  return false;
   reset();   SHOWKIND kind = ALL;
   T_LineEdit::Sf = flag==curset||flag==false?false:true;
   if(flag!=false)
     { begin:
       char ch = fgetc(fp);  if(ch==EOF)  goto end;
       switch(ch)
	 { case 'A': kind = ALL;  break;
	   case 'L': kind = LINE; break;
	   case 'H': kind = HIDE; break;
	   default: fseek(fp, -1, SEEK_CUR);  kind = ALL;
	 }
       if((kind==HIDE&&flag!=true || kind==LINE&&flag==true) && flag!=all)
	 { int ch;
	   while((ch=fgetc(fp))!=EOF)  if(ch=='\n')  goto begin;
	   goto end;
	 }
     }
   while(TRUE)
    { int K = 0;  int ch;  BOOL flag0 = false;
      for(int k=0; k<77; k++)
       { ch = fgetc(fp);
	 if(ch==TAB_KEY)
	  { for(int i=0; i<8; i++, k++)
	     { if(k<77) str[K++] = ' '; }
	    k --;
	  }
	 else  if(ch==EOF||ch=='\n')  break;
	 else  str[K++] = ch;
	 if(ch&0x80)  flag0 = (BOOL)(1-flag0);
	 if(k==75&&flag0==false)
	  { char C = fgetc(fp);  fseek(fp, -1, SEEK_CUR);
	    if(C&0x80)   break;
	  }
       }
      str[K] = '\0';
      if(!(ch==EOF&&str[0]=='\0'))  addline(str, QUICK, kind);
      if(ch==EOF)  break;
      if(k==77&&fgetc(fp)!='\n')  fseek(fp, -1, SEEK_CUR);
      if(flag!=false)
	{ begin0:
	  char ch = fgetc(fp);  if(ch==EOF)  break;
	  switch(ch)
	    { case 'A': kind = ALL;  break;
	      case 'L': kind = LINE; break;
	      case 'H': kind = HIDE; break;
	      default: fseek(fp, -1, SEEK_CUR);  kind = ALL;
	    }
	  if((kind==HIDE&&flag!=true || kind==LINE&&flag==true)&&flag!=all)
	   { int ch;
	     while((ch=fgetc(fp))!=EOF)  if(ch=='\n')  goto begin0;
	     goto end;
	   }
	}
    }  end:;
   setline(0);   fclose(fp);
   T_LineEdit::K = 0;
   T_LineEdit::backcolor = backcolor;  T_LineEdit::forcolor = forcolor;
   return true;
 }

BOOL T_EditBox::save(char *file, char *path, BOOL flag)
 { if(Head==NULL)  return false;
   char filename[50];
   sprintf(filename, "%s\\%s", path, file);
   FILE *fp = fopen(filename, "wt");
   if(fp==NULL)   return false;
   T_LineEdit *ptr = Head;
   while(ptr!=NULL)
     { if(PTR!=ptr)  formatstr(ptr->str);
       if(flag==true) putc(ptr->showkind==ALL?'A':ptr->showkind==LINE?'L':'H' , fp);
       fputs(ptr->str, fp);  fputc('\n', fp);
       ptr = ptr->Tnext;
     }
   fclose(fp);   return true;
 }

#define _Wr  fputc(*ptr++, fp)
BOOL T_EditBox::savetoimage(char *file, char *path)
 { char filename[50];
   sprintf(filename, "%s\\%s", path, file);
   FILE *fp = fopen(filename, "wb");
   if(fp==NULL)  return false;
   mouse_off();    T_LineEdit::shines(off);
   outportb(0x3ce, 4);
   for(int k=0; k<4; k++)
     if(forcolor&(0x01<<k))  { outportb(0x3cf, k);  break; }
   char far *ptr = (char far *)0xa0000000L+y1*80;
   int lines = (y2-y1+1)*8;
   for(register int i=0; i<lines; i++)
    { _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; }
   fclose(fp);  mouse_on();
   shines_k = T_LineEdit::K;    ptr_k = T_LineEdit::ptr;
   return true;
 }

#define _Re  *ptr++ = fgetc(fp)
BOOL T_EditBox::readinimage(char *file, char *path)
 { char filename[50];
   sprintf(filename, "%s\\%s", path, file);
   FILE *fp = fopen(filename, "rb");
   if(fp==NULL)  return false;
   mouse_off();    T_LineEdit::shines(off);
   setfillstyle(0, 0);    bar(6, y1+2, 628, y2-2);
   outportb(0x3c4, 2);    outportb(0x3c5, forcolor);
   char far *ptr = (char far *)0xa0000000L+y1*80;
   int lines = (y2-y1+1)*8;
   for(register int i=0; i<lines; i++)
    { _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; }
   outportb(0x3c5, 0x0f);
   fclose(fp);
   setcolor(LIGHTGRAY);   rectangle(1, y1, 638, y2);
   setcolor(DARKGRAY);    rectangle(2, y1+1, 637, y2-1);
   scroll = T_Scroll(639-10, y1+1, 639-1, y2-1, 1, 0, CYAN, LIGHTBLUE);
   mouse_on();  T_LineEdit::K = shines_k;  T_LineEdit::ptr = ptr_k;
   T_LineEdit::backcolor = backcolor;  T_LineEdit::forcolor = forcolor;
   scroll.setflag(begin, max-high+1, 2);
   return true;
 }
#undef _Wr
#undef _Re


//图形初始化
T_Tx::T_Tx(int backcolor, int frontcolor)
 { graphdriver = DETECT;  graphmode;
   registerbgidriver(EGAVGA_driver);
   initgraph(&graphdriver, &graphmode, BGIPATH);
   setbkcolor(backcolor);  setcolor(frontcolor);
   cleardevice();   initmouse();
 }

T_Tx::~T_Tx() { mouse_off();  closegraph(); }

