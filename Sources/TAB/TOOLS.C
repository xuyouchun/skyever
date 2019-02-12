#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dos.h>
#include <time.h>
#include <bios.h>
#include <alloc.h>
#include "mouse.h"
#include "tools.h"
#include "extern.h"
#include "tab.h"
#include "hz.h"

#define  reset_FA()  FA.Speed=10

struct _FA  FA;
struct _BALLIN  ballin;
double  _Speed, _Rad, _Speed0, _Rad0;
double  XX, YY;
long  sysT = 0;

void  DELAY(long code)
 { long i, k, T=sysT/5;
   for(k=0; k<code; k++)  for(i=0; i<T; i++);
 }

void  test_sysT()
 { long T = clock();
   while(clock()-T<5)  sysT ++;
   sysT /= 5;
 }

void  resetball()
 { int k;
   get_points();
   mouse_off();
   show_ball();
   mouse_on();
 }

void  far  _outtextxy(int x, int y, char *format, ...)
 { char str[200];
   va_list  arglist;
   va_start(arglist, format);
   vsprintf(str, format, arglist);
   va_end(arglist);
   outtextxy(x, y, str);
 }

double Rad1, Rad2, Rad3, Rad4;
double V1, V2, V3, V4;
double V1x, V1y, V2x, V2y, V3x, V3y, V4x, V4y;
double  _X1, _X2, _Y1, _Y2, _X0, _Y0, _X, _Y;
double r, K, Rad;
double _dx_, _dy_;

int  ball_click(int n1, int n2)
 { _X1 = ball[n1].X; _Y1 = ball[n1].Y;
   _X2 = ball[n2].X; _Y2 = ball[n2].Y;
   Rad1 = ball[n1].Rad; Rad2 = ball[n2].Rad;
   V1 = ball[n1].Speed; V2 = ball[n2].Speed;
   _dx_ = _X1-_X2;    _dy_ = _Y1-_Y2;   r = sqrt(_dx_*_dx_ + _dy_*_dy_);
   if(r > 16)  return 0;

  /*  begin  */
   if(V1==0&&V2==0)   V1 = V2 = 0.1;
   if(_X1==_X2)  Rad = _Y2>_Y1?M_PI/2:-M_PI/2;
   else          Rad = _X2>_X1?atan((_Y2-_Y1)/(_X2-_X1)):
			   atan((_Y2-_Y1)/(_X2-_X1))+M_PI;
   V1y = V1*sin(Rad1-Rad); V1x = V1*cos(Rad1-Rad);
   V2y = V2*sin(Rad2-Rad); V2x = V2*cos(Rad2-Rad);
   if(V1x<V2x)  return 0;
   V3y = V1y;  V4y = V2y;
   V3x = V2x;  V4x = V1x;
   V3 = sqrt(V3x*V3x+V3y*V3y);
   V4 = sqrt(V4x*V4x+V4y*V4y);
   if(V3x==0)   Rad3 = (V3y>0?M_PI/2:-M_PI/2)+Rad;
   else         Rad3 = V3x>0?atan(V3y/V3x)+Rad: atan(V3y/V3x)+Rad+M_PI;
   if(V4x==0)   Rad4 = (V4y>0?M_PI/2:-M_PI/2)+Rad;
   else         Rad4 = V4x>0?atan(V4y/V4x)+Rad: atan(V4y/V4x)+Rad+M_PI;
   ball[n1].Speed = V3;   ball[n1].Rad = Rad3;
   ball[n2].Speed = V4;   ball[n2].Rad = Rad4;

   if(ball[n1].Speedflag==1)
	 { if(ball[n1].Doub==n2)
		{ ball[n1].Speedflag = 0;
		  ball[n1].Speed = ball[n1].Bkspeed;
		  ball[n1].Rad   = ball[n1].Bkrad;
		}
	   else
		{ register int k;
		  for(k=0; k<16; k++)  ball[k].Speedflag = 0;
		}
     }
   if(ball[n2].Speedflag==1)
	 { if(ball[n2].Doub==n1)
		{ ball[n2].Speedflag = 0;
		  ball[n2].Speed = ball[n2].Bkspeed;
		  ball[n2].Rad   = ball[n2].Bkrad;
		}
	   else
		{ register int k;
		  for(k=0; k<16; k++)  ball[k].Speedflag = 0;
		}
     }
 /*  end  */
   return 1;
 }

void  reset_speedflag()
 { int k;
   for(k=0; k<16; k++)
     ball[k].Speedflag = 0;
 }

void  reset_ballin()
 { ballin.WHITE = ballin.BLACK = ballin.RED = ballin.BLUE = 0;
   ballin.BLUEFLAG = ballin.REDFLAG = 0;
 }

#define  X0   320
#define  Y0   ((Y2-Y1)/2)

void  get_points()
 { static  long  num = 0;
   double  X[15], Y[15];
   int  k, i, K=0, F, G;
   unsigned int ZT = 0x00;
   #define  read_flag(x)  (ZT&((unsigned)0x01<<x))
   #define  set_flag(x)    ZT|=((unsigned)0x01<<x)
   for(k=0; k<5; k++)
    { X[K  ] = X0+k*2*(R+0.1)*cos(M_PI/6);
      Y[K++] = Y0-k*2*(R+0.1)*sin(M_PI/6);
      for(i=0; i<k; i++)
       { X[K  ] = X[K-1];
	 Y[K++] = Y[K-1]+2*(R+0.1);
       }
    }
  ball[0].X = 40;           ball[0].Xn = 32;
  ball[0].Y = (Y2-Y1)/2;    ball[0].Yn = (Y2-Y1)/2-8;
  ball[0].Speed = 0;           ball[0].Rad = 0;
  ball[0].Color = WHITE;
  ball[0].Life = INLIFE;
  randomize();
  for(k=1; k<=15; k++)
   { F = random(16-k);
     G = 0;
     for(i=0; i<=14; i++)
       if(read_flag(i)==0 && G++==F)  break;
     set_flag(i);
     ball[k].X = X[i];   ball[k].Xn = (int)(X[i]+0.5)-8;
     ball[k].Y = Y[i];   ball[k].Yn = (int)(Y[i]+0.5)-8;
     ball[k].Speed = 0;
     ball[k].Rad = 0 ;
     ball[k].Color = (k<=7?RED:k==8?DARKGRAY:BLUE);
     ball[k].Life  = INLIFE;
   }
  if(num++==0)
    for(k=0; k<16; k++)
      { lastball[k].Xn = ball[k].Xn;
	lastball[k].Yn = ball[k].Yn;
      }
  reset_speedflag();
 }

void  get_ball(unsigned *B)
 { int i, j;
   for(j=0; j<=15; j++)
    { B[j] = 0;
      for(i=0; i<=15; i++)
       { if((int)(sqrt((j-8)*(j-8)+(i-8)*(i-8))+0.5)<8)
	   B[j] |= (unsigned)0x8000>>i;
       }
    }
 }

void  move_ball()
 { int k, j, i;
   test_ballin();
   for(k=0; k<16; k++)
    { if(ball[k].Life == OUTLIFE)  continue;
      ball[k].Speed -= a;
      if(ball[k].Speed <= 0)  { ball[k].Speed = 0; continue; }
      ball[k].X += ball[k].Speed*(cos(ball[k].Rad));
      ball[k].Y += ball[k].Speed*(sin(ball[k].Rad));
      if(ball_border(k)) continue;
      ball[k].Xn = (int)(ball[k].X+0.5)-8;
      ball[k].Yn = (int)(ball[k].Y+0.5)-8;
    }
   for(j=0; j<16; j++)
     for(i=0; i<16; i++)
       { if(i==j)  continue;
	 if(ball[i].Life==OUTLIFE || ball[j].Life==OUTLIFE)  continue;
	 ball_click(i, j);
       }
   show_ball();
 }

int  ball_border(int n)
 { int code = 0;
   if(ball[n].X<=8)
     { ball[n].Xn = 0;
       ball[n].X  = 16-ball[n].X;
       ball[n].Rad = M_PI - ball[n].Rad;
       code ++;
     }
   if(ball[n].X>=471)
     { ball[n].Xn = 463;
       ball[n].X  = 942-ball[n].X;
       ball[n].Rad = M_PI - ball[n].Rad;
       code ++;
     }
   if(ball[n].Y<=8)
     { ball[n].Yn = 0;
       ball[n].Y  = 16-ball[n].Y;
       ball[n].Rad = -ball[n].Rad;
       code ++;
     }
    if(ball[n].Y>=231)
     { ball[n].Yn = 223;
       ball[n].Y  = 462-ball[n].Y;
       ball[n].Rad = -ball[n].Rad;
       code ++;
     }
    return code;
  }


int   test_speed()
 { register int k;
   for(k=0; k<16; k++)
     if(ball[k].Life==INLIFE && ball[k].Speed)  return 1;
   reset_speedflag();
   return 0;
 }

int  test_ballin()
 { register int k;
   int code = 0, flag;
   for(k=0; k<16; k++)
    { flag = 0;
      if(ball[k].Life == OUTLIFE)  continue;
      if(ball[k].Yn<=RR-8)
	{ if(ball[k].Xn<=RR-8 || ball[k].Xn>=480-RR-8 ||
	     ball[k].Xn<=240+RR-8&&ball[k].Xn>=240-RR-8)
	   { code = 1; flag = 1; }
	}
      else if(ball[k].Yn>=240-RR-8)
	{ if(ball[k].Xn<=RR-8 || ball[k].Xn>=480-RR-8 ||
		 ball[k].Xn<=240+RR-8&&ball[k].Xn>=240-RR-8)
	    { code = 1; flag = 1; }
	}
      if(flag == 1)
	{ if(k==0)   ballin.WHITE++;
	  else  if(k<=7)   { ballin.RED++; if(ballin.REDFLAG==1)  ballin.REDFLAG=2; }
	  else  if(k==8)   { ballin.BLACK++; ballin.BLUEFLAG=ballin.REDFLAG=1; }
	  else             { ballin.BLUE++; if(ballin.BLUEFLAG==1)  ballin.BLUEFLAG=2; }
	  show_in(k);
	  ball[k].Life = OUTLIFE;
	}
    }
   if(code==1)   write_ballnum();
   return code;
 }


int  test_line(double x1, double y1, double x2, double y2, double x, double y)
 {  double  K1, K2;
    double  x0, y0;
    double  A,  B;
    int  code = 0;
    /**** (1) ****/
    if(fabs(x1-x2)<0.001)
     {  x0 = x1;  y0 = y;
	A = min(y1,y2); B=max(y1,y2);
	if(y0>=A&&y0<=B)  code=1;
     }
    else if(fabs(y1-y2)<0.001)
     {  x0 = x;   y0 = y1;
	A = min(x1,x2); B=max(x1,x2);
	if(x0>=A&&x0<=B)  code=1;
     }
    else
     {  K2=(y2-y1)/(x2-x1);
		K1=-1/K2;
		x0 = ((K2*x2-K1*x)+(y-y2))/(K2-K1);
		y0 = (K1*K2*(x2-x)+(K2*y-K1*y2))/(K2-K1);
		A = min(y1,y2); B=max(y1,y2);
		if(y0>=A&&y0<=B)  code = 1;
     }
    if(code==1)
       if(sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<=16)  return 1;
    /**** (2) ****/
    if(sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1))<=16 ||
       sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2))<=16 )   return 1;
    return 0;
 }

double  _get_rad(double x0, double y0, double x, double y)
 { if(x0==x)  return y>y0?M_PI/2:-M_PI/2;
   else       return x>x0?atan((y-y0)/(x-x0)):atan((y-y0)/(x-x0))+M_PI;
 }

#define t_line(x,y)  test_line(_X1,_Y1,_X,_Y,x,y)
int  get_sr(int n, int n0, int code)
 {  int k;  double  _R = (double)RR+4;
    double  _Rad_, __Rad, K;
    double  _X0, _Y0, _X1, _Y1, _X, _Y, farxy, HW;
    if(n==n0)
      if(code==0)  { _Speed = _Speed0; _Rad = _Rad0; return 1; }
      else           return 0;
    _X1 = ball[n].X;  _Y1 = ball[n].Y;
    switch(code)
     { case 0:_X0 = ball[n0].X; _Y0 = ball[n0].Y; _Rad_ = _Rad0;
	      _X  = _X0-2*R*cos(_Rad_);
	      _Y  = _Y0-2*R*sin(_Rad_);
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
		 if(k==n||k==n0)  continue;
		 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      if(t_line(239.5, 0  )==1) return 0;
		  if(t_line(239.5, 240)==1) return 0;
	      break;
       case 1:_X0 = ball[n0].X;  _Y0 = 16-ball[n0].Y;  _Rad_=-_Rad0;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (8-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R || HW>=479-_R&&HW<=479+_R)
		      return 0;
	      for(k=0; k<16; k++)
		   { if(ball[k].Life==OUTLIFE)  continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
			 if(k==n0) continue;
			 if(t_line(ball[k].X,16-ball[k].Y)==1)  return 0;
		   }
	      break;
       case 2:_X0 = ball[n0].X;  _Y0 = 462-ball[n0].Y;  _Rad_=-_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (231-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R ||
		     HW>=479-_R&&HW<=479+_R)  return 0;
	      for(k=0; k<16; k++)
		{ if(ball[k].Life==OUTLIFE)  continue;
		  if(k==n)  continue;
                  if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		  if(k==n0) continue;
                  if(t_line(ball[k].X,462-ball[k].Y)==1)  return 0;
		}
	      break;
       case 3:_X0 = 16-ball[n0].X;  _Y0 = ball[n0].Y;  _Rad_=M_PI-_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (8-_X)*K+_Y;
		  if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
		{ if(ball[k].Life==OUTLIFE)  continue;
		  if(k==n) continue;
                  if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		  if(k==n0) continue;
                  if(t_line(16-ball[k].X,ball[k].Y)==1)  return 0;
		}
	      break;
       case 4:_X0 = 942-ball[n0].X;  _Y0 = ball[n0].Y;  _Rad_=M_PI-_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (471-_X)*K+_Y;
		  if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
		 if(k==n)  continue;
                 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		 if(k==n0) continue;
                 if(t_line(942-ball[k].X,ball[k].Y)==1) return 0;
	       }
	      break;
       case 5:_X0 = 16-ball[n0].X;  _Y0 = 16-ball[n0].Y;  _Rad_=M_PI+_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (8-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R || HW>=479-_R&&HW<=479+_R
		 || HW>=-233.5-_R&&HW<=-213.5+_R || HW<=718.5-_R&&HW>=718.5+_R)
		       return 0;
	      HW = (8-_X)*K+_Y;
	      if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
		 if(k==n)  continue;
		 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
                 if(k==n0) continue;
		 if(t_line(ball[k].X,16-ball[k].Y)==1) return 0;
		 if(t_line(16-ball[k].X,ball[k].Y)==1) return 0;
		 if(t_line(16-ball[k].X,16-ball[k].Y)==1) return 0;
	       }
	      break;
	case 6:_X0 = 16-ball[n0].X;  _Y0 = 462-ball[n0].Y;  _Rad_=M_PI+_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (231-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R || HW>=479-_R&&HW<=479+_R
			 || HW>=-233.5-_R&&HW<=-213.5+_R || HW<=718.5-_R&&HW>=718.5+_R)
		       return 0;
	      HW = (8-_X)*K+_Y;
	      if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
		 if(k==n)  continue;
		 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
                 if(k==n0) continue;
		 if(t_line(ball[k].X, 462-ball[k].Y)==1) return 0;
		 if(t_line(16-ball[k].X, ball[k].Y)==1)  return 0;
		 if(t_line(16-ball[k].X,462-ball[k].Y)==1) return 0;
	       }
	      break;
	case 7:_X0 = 942-ball[n0].X;  _Y0 = 16-ball[n0].Y;  _Rad_=M_PI+_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (8-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R || HW>=479-_R&&HW<=479+_R
		 || HW>=-233.5-_R&&HW<=-213.5+_R || HW<=718.5-_R&&HW>=718.5+_R)
		       return 0;
	      HW = (471-_X)*K+_Y;
	      if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE) continue;
		 if(k==n)  continue;
		 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
                 if(k==n0)  continue;
		 if(t_line(ball[k].X,16-ball[k].Y)==1)  return 0;
		 if(t_line(942-ball[k].X,ball[k].Y)==1)  return 0;
		 if(t_line(942-ball[k].X,16-ball[k].Y)==1) return 0;
	       }
	      break;
	case 8:_X0 = 942-ball[n0].X;  _Y0 = 462-ball[n0].Y;  _Rad_=M_PI+_Rad0;
	      _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
	      _X =  _X0-2*R*cos(_Rad_);
	      _Y =  _Y0-2*R*sin(_Rad_);
	      if(fabs(_X-_X1)<0.1)  return 0;
	      K = (_Y-_Y1)/(_X-_X1);
	      HW = (231-_Y)/K+_X;
		  if(HW>=239.5-_R*2&&HW<=239.5+_R*2 || HW<=_R&&HW>=-_R || HW>=479-_R&&HW<=479+_R
		 || HW>=-233.5-_R&&HW<=-213.5+_R || HW<=718.5-_R&&HW>=718.5+_R)
		       return 0;
	      HW = (471-_X)*K+_Y;
	      if( HW<=_R&&HW>=-_R || HW>=239-_R&&HW<=239+_R)  return 0;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE) continue;
		 if(k==n)  continue;
		 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
                 if(k==n0)  continue;
                 if(t_line(ball[k].X,462-ball[k].Y)==1)  return 0;
		 if(t_line(942-ball[k].X,ball[k].Y)==1)  return 0;
		 if(t_line(942-ball[k].X,462-ball[k].Y)==1) return 0;
	       }
	      break;
     }
    __Rad = fabs(_get_rad(_X,_Y,_X0,_Y0) - _get_rad(_X,_Y,_X1,_Y1) );
    _Rad_ = _Rad_>=2*M_PI?_Rad_-2*M_PI:_Rad_<0?_Rad_+2*M_PI:_Rad_;
    if(__Rad<=M_PI/2 || __Rad>=3*M_PI/2)    return 0;
	_Rad = _get_rad(_X1, _Y1, _X, _Y);
    _Speed = _Speed0/cos(_Rad-_Rad_);
    farxy = sqrt((_X-_X1)*(_X-_X1)+(_Y-_Y1)*(_Y-_Y1));
	_Speed = sqrt(2*a*farxy+_Speed*_Speed);
	if(test_L(n0, code)==0)  return 0;
    return 1;
 }

#define dM  (M_PI/6)
int  test_L(int n, int code)
 { double x = ball[n].X, y = ball[n].Y;
   double Rad = _Rad;
   if(code==2||code==4)   Rad = M_PI-Rad;
   if(code==1||code==3)   Rad = -Rad;
   if(code>=5)            Rad += M_PI;
   Rad = Rad>=2*M_PI?Rad-2*M_PI:Rad<0?Rad+2*M_PI:Rad;
   if(x<R+10)
	 if(fabs(Rad-M_PI)<dM)     return 0;
   if(x>718.5-R-10)
	 if(fabs(Rad)<dM)          return 0;
   if(y<R+10)
	 if(fabs(Rad-3*M_PI/2)<dM) return 0;
   if(y>239.5-R-10)
	 if(fabs(Rad-M_PI/2)<dM)   return 0;
   return 1;
 }

int  get_sin(int n, int code)
 {  int k;
    double  _X, _Y, _X1, _Y1, farxy;
    _X1 = ball[n].X;  _Y1 = ball[n].Y;
    switch(code)
     { case 0:_X = XX; _Y = YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
       case 1:_X = XX;  _Y = 16-YY;
	      for(k=0; k<16; k++)
		{ if(ball[k].Life==OUTLIFE)  continue;
		  if(k==n)  continue;
		  if(t_line(ball[k].X,16-ball[k].Y)==1)  return 0;
		  if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		}
	      break;
       case 2:_X = XX;  _Y = 462-YY;
	      for(k=0; k<16; k++)
		{ if(ball[k].Life==OUTLIFE)  continue;
		  if(k==n)  continue;
		  if(t_line(ball[k].X,462-ball[k].Y)==1)  return 0;
		  if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		}
	      break;
       case 3:_X = 16-XX;  _Y = YY;
	      for(k=0; k<16; k++)
		{ if(ball[k].Life==OUTLIFE)  continue;
		  if(k==n) continue;
		  if(t_line(16-ball[k].X,ball[k].Y)==1)  return 0;
		  if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
		}
	      break;
       case 4:_X = 942-XX;  _Y = YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
			 if(k==n)  continue;
			 if(t_line(942-ball[k].X,ball[k].Y)==1) return 0;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
       case 5:_X = 16-XX;  _Y = 16-YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X,16-ball[k].Y)==1) return 0;
			 if(t_line(16-ball[k].X,ball[k].Y)==1) return 0;
			 if(t_line(16-ball[k].X,16-ball[k].Y)==1) return 0;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
	   case 6:_X = 16-XX;  _Y = 462-YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE)  continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X, 462-ball[k].Y)==1) return 0;
			 if(t_line(16-ball[k].X, ball[k].Y)==1)  return 0;
			 if(t_line(16-ball[k].X,462-ball[k].Y)==1) return 0;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
	   case 7:_X = 942-XX;  _Y = 16-YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE) continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X,16-ball[k].Y)==1)  return 0;
			 if(t_line(942-ball[k].X,ball[k].Y)==1)  return 0;
			 if(t_line(942-ball[k].X,16-ball[k].Y)==1) return 0;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
	   case 8:_X = 942-XX;  _Y = 462-YY;
	      for(k=0; k<16; k++)
	       { if(ball[k].Life==OUTLIFE) continue;
			 if(k==n)  continue;
			 if(t_line(ball[k].X,462-ball[k].Y)==1)  return 0;
			 if(t_line(942-ball[k].X,ball[k].Y)==1)  return 0;
			 if(t_line(942-ball[k].X,462-ball[k].Y)==1) return 0;
			 if(t_line(ball[k].X,ball[k].Y)==1)  return 0;
	       }
	      break;
     }
   _Rad = _get_rad(_X1, _Y1, _X, _Y);
   farxy = sqrt( (_X1-_X)*(_X1-_X) + (_Y1-_Y)*(_Y1-_Y) );
   _Speed = sqrt(2*a*farxy+_Speed0*_Speed0);
   return 1;
 }

void get_psn(int code)
 { switch(code)
    { case 0: XX=8;     YY=8;   break;
      case 1: XX=239.5; YY=8;   break;
      case 2: XX=471;   YY=8;   break;
      case 3: XX=8;     YY=231; break;
      case 4: XX=239.5; YY=231; break;
      case 5: XX=471;   YY=231; break;
    }
 }

int  test_life(int color)
 { int k, begin, end;
	if(color==RED)   { begin = 1; end = 7;  }
   else if(color==BLUE)  { begin = 9; end = 15; }
   else if(color==BLACK) { begin = 8; end = 8;  }
   else                  { begin = 0; end = 0;  }
   for(k=begin; k<=end; k++)
      if(ball[k].Life==INLIFE)  return 1;
   return 0;
 }

int  get_roads(int color, double endspeed)
 {  int  sel, begin, end;
    line_msg("Thinking . . .");
    reset_FA();
	if(color==RED)   { begin = 1; end = 7;  }
    else if(color==BLUE) { begin = 9; end = 15; }
    else                 { begin = 8; end = 8;  }
    reset_speedflag();
    for(sel=begin; sel<=end; sel++)
     { int rin;   show_time( _mktime(clock()-Time) );
       if(ball[sel].Life==OUTLIFE)  continue;
       for(rin=0; rin<6; rin++)
	{ int  roadl;
	  get_psn(rin);
	  for(roadl=0; roadl<9; roadl++)
	   { double speed0, rad0;  int seb, randnum;
	     _Speed0 = endspeed;
	     if(get_sin(sel, roadl)==0)  continue;
	     speed0 = _Speed;    rad0 = _Rad + 0.025*(9-pass)*((double)50-random(100))/50 ;
	     speed_msg(speed0, rad0);
	     for(seb=0; seb<16; seb++)
	      { double speed, rad; int road0;
		if(ball[seb].Life==OUTLIFE) continue;
		if(seb==sel)  continue;
		for(road0=0; road0<9; road0++)
		 { int roadb;
		   _Speed0 = speed0;   _Rad0 = rad0;
		   if(get_sr(seb, sel, road0)==0)  continue;
		   speed = _Speed;  rad = _Rad;
		   for(roadb=0; roadb<9; roadb++)
		    { double  speedx, radx;
		      _Speed0 = speed; _Rad0 = rad;
		      if(get_sr(0, seb, roadb)==0)  continue;
		      speedx = _Speed;  radx = _Rad;
		      if(speedx<FA.Speed)
		       { FA.Speed=speedx; FA.Rad=radx;
			 if(ifsound==0)  { sound(1000); DELAY(100); nosound(); }
			 reset_speedflag();
			 ball[sel].Speedflag=1;
			 ball[sel].Doub = seb;
			 ball[sel].Bkspeed = speed0;
			 ball[sel].Bkrad = rad0;
			 if(seb==0)  continue;
			 ball[seb].Bkspeed = speed;
			 ball[seb].Bkrad = rad;
			 ball[seb].Speedflag = 1;
			 ball[seb].Doub = 0;
			}
		     }
		  }
	       }
	    }
	 }
      }
    line_msg("");
    if(FA.Speed>8) { reset_speedflag();  return 0; }
    return 1;
  }


int   leave_speed()
 { int X, Y, BX, BY, MX, MY, flag=0, f=0;
   double  Rad, V;
   double  Len, Len0=0;
   if(getmousebutton()!=LEFTBUTTON) return -1;
   line_msg("Start . . .");
   MX = (int)(ball[0].X+0.5)+X1, MY = (int)(ball[0].Y+0.5)+Y1;
   BX = MX; BY = MY;
   getmouse_xy(&X, &Y);
   V = sqrt( ((double)X-MX)*(X-MX)+((double)Y-MY)*(Y-MY) )/25;
   if(bioskey(2)&0x04)  V*=2;
   if(bioskey(2)&0x08)  V/=2;
   if(V > 8)  V = 8;
   Len = V*V/(2*a);
   if(Len > 10000)  Len = 10000;

   while(getmouse_xy(&X,&Y)!=NOBUTTON)
    { show_time(_mktime(clock()-Time));
      if(kbhit() && getch()==27) { flag = 1; break; }
      if(getmousebutton()==BOTHBUTTON) { flag = 1; break; }
      V = sqrt( ((double)X-MX)*(X-MX)+((double)Y-MY)*(Y-MY) )/25;
      if(bioskey(2)&0x04)  V*=2;
      if(bioskey(2)&0x08)  V/=2;
      if(V > 8)  V = 8;
      Len = V*V/(2*a);  if(Len > 10000)  Len = 10000;
      if(X==BX && Y==BY && Len==Len0)  continue;
      mouse_off();
      setcolor(LIGHTGREEN);  setwritemode(XOR_PUT);
      setlinestyle(0, 0, 1);
      line(BX, BY, MX, MY);      line(X, Y, MX, MY);
      Rad = M_PI+_get_rad(MX, MY, BX, BY);
      if(f==1)  write_line(MX, MY, Rad, Len0);
      Rad = M_PI+_get_rad(MX, MY, X, Y);
      write_line(MX, MY, Rad, Len);
      speed_msg(V, Rad);
      mouse_on();
      BX = X; BY = Y; f=1; Len0 = Len;
    }
   mouse_off();
   setcolor(LIGHTGREEN);    setwritemode(XOR_PUT);
   setlinestyle(0, 0, 1);   line(BX, BY, MX, MY);
   Rad = M_PI+_get_rad(MX, MY, BX, BY);
   write_line(MX, MY, Rad, Len);
   mouse_on();
   line_msg("");

   if(flag==1)
     { line_msg("Esc . . ."); while(getmousebutton()!=NOBUTTON);
       line_msg(""); return 0;
     }
   _Speed = sqrt( ((double)X-MX)*(X-MX) + ((double)Y-MY)*(Y-MY) )/25;
   if(bioskey(2)&0x04)  _Speed*=2;
   if(bioskey(2)&0x08)  _Speed/=2;
   if(_Speed > 8)  _Speed = 8;
   _Rad   = M_PI+_get_rad(MX, MY, X, Y);
   setwritemode(COPY_PUT);
   return 1;
 }

void  write_line(double x, double y, double rad, double len)
 { double  X, Y, _rad, len0;
   static  int   FLAG = 0;
   rad = rad>=2*M_PI?rad-2*M_PI:rad<0?rad+2*M_PI:rad;
   x -= X1; y -= Y1;
   setlinestyle(4, 0xf0f0, 1);
   if(FLAG==0)                   setcolor(YELLOW);
   else if(FLAG==1 || FLAG==2)   setcolor(LIGHTBLUE);
   else if(FLAG==3 || FLAG==4)   setcolor(DARKGRAY);
   else  { setlinestyle(4, 0xc0c0, 1);   setcolor(LIGHTGRAY); }
   setwritemode(XOR_PUT);
   while(1)
    { if(rad>=M_PI&&rad<2*M_PI)
	{ _rad = -rad;
	  if(rad==M_PI/2) { X = x; Y = 8; break; }
	  X = (8-y)/tan(rad) + x;  Y = 8;
	  if(X>8&&X<471) break;
	}
      if(rad<=M_PI&&rad>0)
	{ _rad = -rad;
	  if(rad==3*M_PI/2) { X = x; Y = 231; break; }
	  X = (231-y)/tan(rad) + x; Y = 231;
	  if(X>8&&X<471) break;
	}
      if(rad>M_PI/2&&rad<3*M_PI/2)
	{ _rad = M_PI-rad;
	  Y = (8-x)*tan(rad) + y; X = 8;
	  if(Y>8&&Y<231) break;
	}
      if(rad>3*M_PI/2||rad<M_PI/2)
	{ _rad = M_PI-rad;
	  Y = (471-x)*tan(rad) + y; X = 471;
	  if(Y>8&&Y<231) break;
	}
       break;
    }
   X += X1; Y += Y1; x += X1; y += Y1;
   len0 = sqrt( (X-x)*(X-x) + (Y-y)*(Y-y) );
   if(len > len0)
    { line(X,Y,x,y);  FLAG++;  write_line(X, Y, _rad, len-len0); FLAG--; }
   else  line(x,y,x+len*cos(rad),y+len*sin(rad));
   setwritemode(COPY_PUT);
   return;
 }

void  reset_inball(int n)
 { int k;
   int  ball_psn(double, double);
   double x0, y0, x, y;
   if(random(2)==0)  x0 = 60;
   else              x0 = 420;
   y0 = (Y2-Y1)/2;
   for(k=0; k<12; k++)
     {  if( !ball_psn((x=x0+k*4), (y=y0)) )  break;
		if( !ball_psn((x=x0-k*4), (y=y0)) )  break;
     }
   if(k==12)
    { x0 = 480-x0;
      for(k=0; k<12; k++)
		{ if( !ball_psn((x=x0+k*4), (y=y0)) )  break;
		  if( !ball_psn((x=x0-k*4), (y=y0)) )  break;
		}
	}
   ball[n].X = x; ball[n].Y = y;
   ball[n].Xn = (int)(x+0.5)-8;  ball[n].Yn = (int)(y+0.5)-8;
   show_in(n);
   ball[n].Life = INLIFE;
   ball[n].Speed = 0;
   mouse_off();   show_ball();  mouse_on();
 }

int  ball_psn(double x, double y)
 { int k;
   for(k=0; k<16; k++)
    { if(ball[k].Life == OUTLIFE)  continue;
      if( sqrt( (ball[k].X-x)*(ball[k].X-x) + (ball[k].Y-y)*(ball[k].Y-y) )
	      < 16.1)    return 1;
    }
   return 0;
 }

int  reset_black(int color)
 { int i;
   int begin = color==RED?  1:9,
       end   = color==BLUE? 7:15;
   if(test_life(color)==0)
     { if(color==RED&&ballin.REDFLAG==2 || color==BLUE&&ballin.BLUEFLAG==2)
	 { reset_inball(8);
	   for(i=begin; i<=end; i++)  if(ball[i].Life==OUTLIFE)  break;
	   if(i<=end)  reset_inball(i);
	   return NOTEND;
	 }
       if(color==RED&&ballin.REDFLAG==1 || color==BLUE&&ballin.BLUEFLAG==1)
	 return HAVEEND;
     }
   else
     { reset_inball(8);
       for(i=begin; i<=end; i++)  if(ball[i].Life==OUTLIFE)  break;
       if(i!=end+1)  reset_inball(i);
       return NOTEND;
     }
   return 0;
  }

char  *_mktime(long time)
 { int  hour, minute, second;
   static char str[10];
   static long time0;
   char Hour[3], Minute[3], Second[3];
   time /= 18.2;
   if(time==time0)  { str[0] = 'N'; return str; }
   time0 = time;
   hour = time/3600;
   time %= 3600;
   minute = time/60;
   time %= 60;
   second = time;
   Hour[0]=hour/10+'0'; Hour[1]=hour%10+'0'; Hour[2]='\0';
   Minute[0]=minute/10+'0'; Minute[1]=minute%10+'0'; Minute[2]='\0';
   Second[0]=second/10+'0'; Second[1]=second%10+'0'; Second[2]='\0';
   sprintf(str, "%s:%s:%s", Hour, Minute, Second);
   return  str;
 }

void  show_str(int x, int y, int size, char *MSG)
 { int k;
   setwritemode(XOR_PUT);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   mouse_off();
   outtextxy(x, y, MSG);
   for(k=1; k<size; k++)
    { settextstyle(1, 0, k);
      outtextxy(x, y, MSG);
      settextstyle(1, 0, k+1);
      outtextxy(x, y, MSG);
      delay(70);
    }
   setwritemode(COPY_PUT);
   outtextxy(x, y, MSG);
   mouse_on();
   return;
 }

void  show_in(int k)
 { int x = (int)ball[k].X, y = (int)ball[k].Y;
   int code;
   char far *ptr;
   char str[10];
   ptr = (char far*)farmalloc( imagesize(x-10, y, x+70, y+30) );
   if(ptr==NULL)  return;
   mouse_off();
   getimage(x-10+X1, y+Y1, x+70+X1, y+30+Y1, ptr);
   if(ball[k].Life == OUTLIFE)  { strcpy(str, "Back..."); code = 3; }
   else  if(k==0) { strcpy(str, "NO!"); code = 3; }
   else  if(k==8)
     { if(test_life(player[PLAY].color)) { strcpy(str, "BAD!"); code = 3; }
       else
	 { strcpy(str, "Great!"); code = 4;
	   if(player[PLAY].color==BLUE)  Bluen++;
	   else                          Redn ++;
	   write_rb();
	 }
     }
   else  if(player[PLAY].color != ball[k].Color) { strcpy(str, "Sorry!"); code = 2; }
   else  { strcpy(str, "OK!"); code = 1; }
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 2);
   setcolor(YELLOW);     outtextxy(x+30+X1, y+13+Y1, str);
   setcolor(RED);        outtextxy(x+28+X1, y+11+Y1, str);
   mouse_on();
   if(code==4)   line_msg(player[PLAY].color==RED?"Red Win !":"Blue Win !");
   Sound(code);
   if(code==4)   line_msg("Wait . . .");
   mouse_off();
   putimage(x-10+X1, y+Y1, ptr, COPY_PUT);
   mouse_on();
   farfree(ptr);
  }

void  line_msg(char *str)
 { setfillstyle(1, BLUE);
   mouse_off();
   bar(400, 55, 600, 98);
   setcolor(LIGHTGRAY);
   setwritemode(COPY_PUT);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy( (400+600)/2, (55+90)/2-3, str);
   mouse_on();
   return;
 }

void  speed_msg(double speed, double rad)
 { char str[30];
   sprintf(str, "%f<>%f", speed, fmod(720-rad/M_PI*180, 360) );
   setfillstyle(1, BLUE);
   bar(410, 87, 590, 98);
   setcolor(GREEN);
   setwritemode(COPY_PUT);
   settextstyle(0, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy( (420+580)/2, (87+98)/2, str);
   return;
 }

void  Sound(int code)
 { long T;
   int _delay[3], _hz[3];
   int k, flag = 0;
   T = clock();
   switch(code)
	{ case 0: break;
	  case 1: _delay[0] = 200;  _delay[1] = 200;  _delay[2] = 200;
			  _hz[0] = 500;      _hz[1] = 1000;     _hz[2] = 1500;
			  break;
	  case 2: _delay[0] = 200;  _delay[1] = 200;  _delay[2] = 200;
			  _hz[0] = 1300;     _hz[1] = 800;     _hz[2] =  300;
			  break;
	  case 3: _delay[0] = 300;  _delay[1] = 300;  _delay[2] = 300;
			  _hz[0] = 600;     _hz[1] =   500;   _hz[2] =  0;
			  break;
	  case 4: _delay[0] = 300;  _delay[1] = 300;  _delay[2] = 300;
			  _hz[0] = 800;     _hz[1] =   800;   _hz[2] = 800;
			  break;
	  case 5: _delay[0] = 300;  _delay[1] = 300;  _delay[2] = 300;
			  _hz[0] = 600;     _hz[1] =   600;   _hz[2] = 600;
	}
   for(k=0; k<3; k++)
     { if(ifsound==0) sound(_hz[k]);
       DELAY(_delay[k]);
       nosound();      DELAY(60);
     }
   if(code==4 && ifsound==0)
	{ DELAY(500);
	  for(k=2; k<10; k++)
	   { sound(k*120);  DELAY(150);
	     nosound();     DELAY(50);
	     if(kbhit()||getmousebutton()!=NOBUTTON) { flag=1; break; }
	   }
	  for(; k>2; k--)
	   { if(flag==1)    break;
	     sound(k*120);  DELAY(150);
	     nosound();     DELAY(50);
	     if(kbhit()||getmousebutton()!=NOBUTTON)  break;
	   }
	}
   else if(code==5 && ifsound==0)
	{ DELAY(200);
	  for(k=2; k<10; k++)
	   { sound(k*120);  DELAY(150);
	     nosound();     DELAY(50);
	     if(kbhit()||getmousebutton()!=NOBUTTON)  break;
	   }
	}
   nosound();
   DELAY(40);
   Time += clock()-T;
 }
