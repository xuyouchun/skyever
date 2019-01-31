#include  <dos.h>
#include  <graphics.h>
#include  <stdio.h>
#include  "extern.h"
#include  "set.h"

#define  Rep   *(ptr0++) = *(p++)
#define  Repall  \
	   for(i=0; i<6; i++)\
	     { Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep; }\
	   ptr0 += 20
#define  Rep0    *p++=0; *p++=0; *p=0; p += 58

void  show_ball()
 { int K, k;
   unsigned long U;
   int M, N, M0, A;
   int X, Y, color, code;
   char far *p = (char far *)SET;

   for(K=0; K<16; K++)
     {  N = lastball[K].Yn;
	A = lastball[K].Xn;
	lastball[K].Yn = ball[K].Yn;
	lastball[K].Xn = ball[K].Xn;

	M = A>>3;  M0 = A-(M<<3);
	p = (char far *)SET + (N<<6)-(N<<2)+M;

	color = ball[K].Color;
	for(k=0; k<4; k++)
	  { if(!(color&(0x01<<k))) { p += 14400;  continue; }
	    Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0;
	    Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0;
	    p += 13440;
	  }
     }

   for(K=0; K<16; K++)
     {  if(ball[K].Life == OUTLIFE)  continue;
		N = ball[K].Yn;
		A = ball[K].Xn;
		M = A>>3;  M0 = A-(M<<3);
		p = (char far *)SET + (N<<6)-(N<<2)+M;
		color = ball[K].Color;
		for(k=0; k<4; k++)
		  { register int n;
		    if(!(color&(0x01<<k))) { p += 14400;  continue; }
		    for(n=0; n<16; n++)
		     { U = (long)B[n]<<(16-M0);
		       *(p++)|=U>>24; *(p++)|=U>>16; *(p)|=U>>8;
		       p += 58;
		     }
		    p += 13440;
		  }
     }
   p = (char far *)SET;
{  int k;
   char far *ptr0 = (char far *)0xa00023faL;
   for(k=0; k<4; k++)
    { outportb(0x3c4, 2);
      outportb(0x3c5, (0x01<<k));
      ptr0 = (char far *)0xa00023faL;
    { register i, j;
      for(j=0; j<24; j++)
       { Repall; Repall; Repall; Repall; Repall; Repall; Repall; Repall;
	 Repall; Repall;
       }
    }
    }
     outportb(0x3c4, 2);
     outportb(0x3c5, 0x0f);
}
  }