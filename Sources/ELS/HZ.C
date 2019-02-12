#include <graphics.h>
#include <stdio.h>
#include <stdarg.h>
#include <dos.h>
#include "mouse.h"
#include "hz.h"
#include "extern.h"

struct _HZ   HZ = { X_ROUND, LEFT_HZ, TOP_HZ, COPY_MODEL, WHITE,
		    1, 1, 0, 16, X_ROUND };
FILE   *hz_fp = NULL;
int    hz_speed = 0;

int i, j, k, _i, _j;
int x0, y0;
int m, n, color, mode;
unsigned char c;
int flag;

void writehz(int x, int y, int begin)
 { m = HZ.text_zoomx;
   n = HZ.text_zoomy;
   flag = 0;
   color = HZ.text_color;
   mode = HZ.text_mode;
   fseek(hz_fp, (long)begin*HZ.text_dot/8*HZ.text_dot, SEEK_SET);
   if(imouse_inbox(x-25, y-25, x+HZ.text_dot*m+35, y+HZ.text_dot*n+35))
     { mouse_off(); flag = 1; }
   for(j=0; j<HZ.text_dot; j++)
    { for(i=0; i<HZ.text_dot/8; i++)
       { c = fgetc(hz_fp);
         if(hz_speed)  delay(hz_speed);
	 for(k=0; k<8; k++)
	  { if(!(c&(0x80>>k)))  continue;
	    if(HZ.text_hz_round == X_ROUND)
	     { x0 = (i*8+k)*m+x;
	       y0 = j*n+y;
	     }
	    else
	     { y0 = (i*8+k)*n+y;
	       x0 = j*m+x;
	     }
	    for(_j=0; _j<n; _j++)
	      for(_i=0; _i<m; _i++)
		 putpixel(x0+_i, y0+_j, mode==COPY_MODEL?
		    color: color^getpixel(x0+_i, y0+_j));
	   }
	}
     }
    if(flag==1) mouse_on();
  }

void hzswh(int *x0, int *y0, int x, int y, int num)
 { int dx, dy;
   if(HZ.text_round==X_ROUND)
     { dx = num*HZ.text_dot*(HZ.text_zoomx)+(num-1)*HZ.text_dxy;
       dy = HZ.text_dot*(HZ.text_zoomy);
     }
   else
     { dy = num*HZ.text_dot*(HZ.text_zoomy)+(num-1)*HZ.text_dxy;
       dx = HZ.text_dot*(HZ.text_zoomx);
     }
   *x0 = x; *y0 = y;
   if(HZ.text_x==RIGHT_HZ)    *x0 = x-dx;
   if(HZ.text_x==MIDDLE_HZ)   *x0 = x-dx/2;
   if(HZ.text_y==BOTTOM_HZ)   *y0 = y-dy;
   if(HZ.text_y==MIDDLE_HZ)   *y0 = y-dy/2;
 }

void writehzs(int x, int y, int begin, int end)
 { int x0, y0, k, x1, y1;
   hzswh(&x1, &y1, x, y, end-begin+1);
   for(k=begin; k<=end; k++)
    { x0 = x1 + (k-begin)*(HZ.text_dot*HZ.text_zoomx+HZ.text_dxy)*
			     (HZ.text_round==X_ROUND);
      y0 = y1 + (k-begin)*(HZ.text_dot*HZ.text_zoomy+HZ.text_dxy)*
			     (HZ.text_round==Y_ROUND);
      writehz(x0, y0, k);
    }
 }

/*void x_writehzs(int x, int y, ...)
 { int k, x0, y0, num=0;
   int x1, y1;
   va_list arglist;
   va_start(arglist, format);
   k = va_arg(arglist, int);
   while(k>=0)
    { num++;
      k = va_arg(arglist, int);
    };
   va_end(arglist);
   if(!num)  return;
   hzswh(&x1, &y1, x, y, num);
   va_start(arglist, format);
   num = 0;
   k = va_arg(arglist, int);
   while(k>=0)
    { x0 = x1 + (num  )*(HZ.text_dot*HZ.text_zoomx+HZ.text_dxy)*
					 (HZ.text_round==X_ROUND);
      y0 = y1 + (num++)*(HZ.text_dot*HZ.text_zoomy+HZ.text_dxy)*
					 (HZ.text_round==Y_ROUND);
      writehz(x0, y0, k);
      k = va_arg(arglist, int);
    }
   va_end(arglist);
 }*/


int gethz_wide(int begin, int end)
 { int  num = end-begin+1;
   if(HZ.text_round==X_ROUND)
      return num*HZ.text_dot*(HZ.text_zoomx)+(num-1)*HZ.text_dxy;
   else
      return HZ.text_dot*(HZ.text_zoomx);
 }

int gethz_high(int begin, int end)
 { int  num = end-begin+1;
   if(HZ.text_round==X_ROUND)
      return   HZ.text_dot*(HZ.text_zoomy);
   else
      return   num*HZ.text_dot*(HZ.text_zoomy)+(num-1)*HZ.text_dxy;
 }