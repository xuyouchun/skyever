#include <graphics.h>
#include <dos.h>
#include <time.h>
#include "mouse.h"

#define  ibit(num,n)  (num&(0x01<<n))

union REGS regs;
int  _mouse_x, _mouse_y;
char _mouse_op = OFF;


int initmouse()
 { regs.x.ax = 0;
   int86(0x33, &regs, &regs);
   if(regs.x.ax == 0)  return FALSE;
   else                return TRUE;
 }


int  getmouse_xy(int *x, int *y)
 { int left, right;
   regs.x.ax = 3;
   int86(0x33, &regs, &regs);
   *x = regs.x.cx;
   *y = regs.x.dx;
   left = ibit(regs.x.bx, 0);
   right = ibit(regs.x.bx, 1);
   if(left  && !right)   return  LEFTBUTTON;
   if(right && !left )   return  RIGHTBUTTON;
   if(right && left  )   return  BOTHBUTTON;
   return  NOBUTTON;
 }

int  getmouse_x()
 { regs.x.ax = 3;
   int86(0x33, &regs, &regs);
   return regs.x.cx;
 }

int  getmouse_y()
  { regs.x.ax = 3;
    int86(0x33, &regs, &regs);
    return regs.x.dx;
  }

void  mouse_on()
 { regs.x.ax = 1;
   int86(0x33, &regs, &regs);
   _mouse_op = ON;
 }

void  mouse_off()
 { regs.x.ax = 2;
   int86(0x33, &regs, &regs);
   _mouse_op = OFF;
 }

void  setmouse_max_x(int left, int right)
 { regs.x.ax = 7;
   regs.x.cx = left;
   regs.x.dx = right;
   int86(0x33, &regs, &regs);
 }

void  setmouse_max_y(int top, int bottom)
 { regs.x.ax = 8;
   regs.x.cx = top;
   regs.x.dx = bottom;
   int86(0x33, &regs, &regs);
 }

void  setmousexy(int x, int y)
 { regs.x.ax = 4;
   regs.x.cx = x;
   regs.x.dx = y;
   int86(0x33, &regs, &regs);
 }

int  imouse_inbox(int x1, int y1, int x2, int y2)
 { int x, y, t;
   getmouse_xy(&x, &y);
   if(x1 < x2)  { t=x1; x1=x2; x2=t; }
   if(y1 < y2)  { t=y1; y1=y2; y2=t; }
   if(x>x2 && x<x1 && y>y2 && y<y1)  return YES;
   return NO;
 }
