#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include "mouse.h"
#include "tools.h"
#include "extern.h"


int  _color_ = DARKGRAY;
int  _fillcolor_ = RED;

#define INT(x)   (int)(x+0.5)

void  getpoints()
 { int k;
   for(k=0; k<4; k++)
    if(MSG[k].Active == 1)
      _getpoints(k);
 }

void  _getpoints(int code)
 {  double  R, Rad;
    switch(code)
     { case 0:
	/* M_SQRT2/3 == 0.47140452 */
	R = 0.47140452 * r;
	/* 3*M_PI/4 == 2.35619449 */
	Rad = MSG[0].Round==1?
	   0.785398163-MSG[0].Rad:2.35619949+MSG[0].Rad;
	MSG[0].X[0] = MSG[0].XX + R*cos(Rad);
	MSG[0].Y[0] = MSG[0].YY - R*sin(Rad);
	R = r;
	Rad = MSG[0].Round==1?
	   M_PI-MSG[0].Rad: MSG[0].Rad;
	MSG[0].X[1] = MSG[0].X[0] + R*cos(Rad);
	MSG[0].Y[1] = MSG[0].Y[0] - R*sin(Rad);
	R = r;
	/*M_PI/2 == 1.570796327 */
	Rad = MSG[0].Round==1?
	   4.712388981-MSG[0].Rad: MSG[0].Rad-1.570796327;
	MSG[0].X[2] = MSG[0].X[0] + R*cos(Rad);
	MSG[0].Y[2] = MSG[0].Y[0] - R*sin(Rad);
       case 1:
       /* X=(M_SQRT2-4)/(2*M_SQRT2+1) == -0.675417437
	  Y=(6-2*M_SQRT2)/(6*M_SQRT2+3) == 0.276142374
	  sqrt(X*X+Y*Y) == 0.729687141
	  atan(-Y/X) == 0.388109734  */
	R = 0.729687141 * r;
	Rad = MSG[1].Round==1? 2.753482919-MSG[1].Rad : 0.388109734+MSG[1].Rad;
	MSG[1].X[0] = MSG[1].XX + R*cos(Rad);
	MSG[1].Y[0] = MSG[1].YY - R*sin(Rad);
	/* M_SQRT2-1 == 0.414213562 */
	R = 0.414213562 * r;
	/* M_PI/4 == 0.785398163 */
	Rad = MSG[1].Round==1?
	   2.356194491-MSG[1].Rad:0.785398163+MSG[1].Rad;
	MSG[1].X[1] = MSG[1].X[0] + R*cos(Rad);
	MSG[1].Y[1] = MSG[1].Y[0] - R*sin(Rad);
	/*  sqrt(7-4*M_SQRT2) == 1.158941651 */
	R = 1.158941651 * r;
	/*  5*M_PI/4 - asin(1/sqrt(7-4*M_SQRT2)) == 2.88609728 */
	Rad = MSG[1].Round==1?
	    0.255495373-MSG[1].Rad: 2.88609728+MSG[1].Rad;
	MSG[1].X[2] = MSG[1].X[0] + R*cos(Rad);
	MSG[1].Y[2] = MSG[1].Y[0] - R*sin(Rad);
	/* sqrt(5) == 2.236067978 */
	R = 2.236067978 * r;
	/* 5*M_PI/4 - asin(2/sqrt(5)) == 2.819842099 */
	Rad = MSG[1].Round==1?
	   5.961437753-MSG[1].Rad: MSG[1].Rad-2.819842099;
	MSG[1].X[3] = MSG[1].X[0] + R*cos(Rad);
	MSG[1].Y[3] = MSG[1].Y[0] - R*sin(Rad);
	R = r;
	/* M_PI/4 == 0.785398163 */
	Rad = MSG[1].Round==1?
	   3.926990817-MSG[1].Rad: MSG[1].Rad-0.785398163;
	MSG[1].X[4] = MSG[1].X[0] + R*cos(Rad);
	MSG[1].Y[4] = MSG[1].Y[0] - R*sin(Rad);
       case 2:
	/*  X=(6-3*M_SQRT2)/(4*M_SQRT2-2) == 0.480565861
	    Y=(7*M_SQRT2-6)/(12*M_SQRT2-6) == 0.355450766
	    sqrt(X*X+Y*Y) == 0.597736391
	    atan(-Y/X)+M_PI == 2.504748214  */
	R = 0.597736391 * r;
	Rad = MSG[2].Round==1? 0.63684444-MSG[2].Rad : 2.504748214+MSG[2].Rad;
	MSG[2].X[0] = MSG[2].XX + R*cos(Rad);
	MSG[2].Y[0] = MSG[2].YY - R*sin(Rad);
	/* M_SQRT2-1 == 0.414213562 */
	R = 0.414213562 * r;
	/* 5*M_PI/4 == 3.926990817 */
	Rad = MSG[2].Round==1?
	   -0.785398163-MSG[2].Rad: MSG[2].Rad+3.926990817;
	MSG[2].X[1] = MSG[2].X[0] + R*cos(Rad);
	MSG[2].Y[1] = MSG[2].Y[0] - R*sin(Rad);
	/* sqrt(4-2*M_SQRT2) == 1.082392200 */
	R = 1.082392200 * r;
	/* asin(1/sqrt(4-2*M_SQRT2) == 1.178097245 */
	Rad = MSG[2].Round==1?
	   4.319689899-MSG[2].Rad: MSG[2].Rad-1.178097245;
	MSG[2].X[2] = MSG[2].X[0] + R*cos(Rad);
	MSG[2].Y[2] = MSG[2].Y[0] - R*sin(Rad);
	R = M_SQRT2 * r;
	Rad = MSG[2].Round==1? M_PI-MSG[2].Rad: MSG[2].Rad;
	MSG[2].X[3] = MSG[2].X[0] + R*cos(Rad);
	MSG[2].Y[3] = MSG[2].Y[0] - R*sin(Rad);
       case 3:
	/* X=(26-12*M_SQRT2)/(12*M_SQRT2-3) == 0.646318792
	   Y=(6*M_SQRT2-1)/(12*M_SQRT2-3) == 0.535789539;
	   sqrt(X*X + Y*Y) == 0.839522728
	   atan(-Y/X)+M_PI == 2.449425225  */
	R = 0.839522728 * r;
	Rad = MSG[3].Round==1? 0.692167428-MSG[3].Rad : 2.449425225+MSG[3].Rad;
	MSG[3].X[0] = MSG[3].XX + R*cos(Rad);
	MSG[3].Y[0] = MSG[3].YY - R*sin(Rad);
	R = M_SQRT2 * r;
	/* 5*M_PI/4 == 3.926990817 */
	Rad = MSG[3].Round==1?
	  -0.785398163-MSG[3].Rad: MSG[3].Rad+3.926990817;
	MSG[3].X[1] = MSG[3].X[0] + R*cos(Rad);
	MSG[3].Y[1] = MSG[3].Y[0] - R*sin(Rad);
	/*  sqrt(10-4*M_SQRT2) == 2.084021533 */
	R = 2.084021533 * r;
	/*  atan(1/(2*M_SQRT2)-1) == 0.500474036  */
	Rad = MSG[3].Round==1?
	   3.64206669-MSG[3].Rad: MSG[3].Rad-0.500474036;
	MSG[3].X[2] = MSG[3].X[0] + R*cos(Rad);
	MSG[3].Y[2] = MSG[3].Y[0] - R*sin(Rad);
	/*  2*M_SQRT2-1 == 1.828427125  */
	R = 1.828427125 * r;
	Rad = MSG[3].Rad;
	Rad = MSG[3].Round==1? M_PI-MSG[3].Rad: MSG[3].Rad;
	MSG[3].X[3] = MSG[3].X[0] + R*cos(Rad);
	MSG[3].Y[3] = MSG[3].Y[0] - R*sin(Rad);
     }
  }

void  linkpoints(double *x, double *y, int num)
 { int k;
   for(k=0; k<num-1; k++)
     line(INT(x[k]), INT(y[k]), INT(x[k+1]), INT(y[k+1]));
   line(INT(x[0]), INT(y[0]), INT(x[k]), INT(y[k]));
 }

void  link(int code)
 { int color = getcolor();
   setcolor(_color_);
   mouse_off();
   linkpoints(MSG[code].X, MSG[code].Y, MSG[code].Points);
   mouse_on();
   setcolor(color);
 }

void  flood(int code)
 { int Points[12];
   int  k;
   int  color = getcolor();
   setcolor(_color_);
   setfillstyle(1, _fillcolor_);
   for(k=0; k<MSG[code].Points; k++)
    { Points[2*k] = INT(MSG[code].X[k]);
      Points[2*k+1] = INT(MSG[code].Y[k]);
    }
   Points[2*k] = INT(MSG[code].X[0]);
   Points[2*k+1] = INT(MSG[code].Y[0]);
   mouse_off();
   fillpoly(MSG[code].Points+1, Points);
   mouse_on();
   setcolor(color);
 }

int   idotin3(double *X, double *Y, double X0, double Y0)
 { int i, j;
   double t;
   double k;
   double _Y0, _Y1;
   double Y1, Y2, Y3;
   for(j=0; j<3; j++)  for(i=j+1; i<3; i++)
     if(X[j]>X[i])
       { t=X[j]; X[j]=X[i]; X[i]=t;
	 t=Y[j]; Y[j]=Y[i]; Y[i]=t;
       }
   if(X0<X[0] || X0>X[2])  return OUT;
   if(fabs(X[1]-X[0])>0.001)
     { k = (Y[1]-Y[0])/(X[1]-X[0]);  Y1 = k*(X0-X[0])+Y[0]; }
   if(fabs(X[2]-X[1])>0.001)
     { k = (Y[2]-Y[1])/(X[2]-X[1]);  Y2 = k*(X0-X[1])+Y[1]; }
   if(fabs(X[0]-X[2])>0.001)
     { k = (Y[0]-Y[2])/(X[0]-X[2]);  Y3 = k*(X0-X[2])+Y[2]; }
   _Y1 = Y3;
   if(fabs(X[1]-X[0])<=0.001)   _Y0 = Y2;
   else if(fabs(X[1]-X[2])<=0.001)  _Y0 = Y1;
   else if(X0>X[1])             _Y0 = Y2;
   else                         _Y0 = Y1;
   if(_Y0>_Y1) { t=_Y0; _Y0=_Y1; _Y1=t; }
   if(Y0>_Y0 && Y0<_Y1)  return  IN;
   return  OUT;
 }

int   idotin(int code, double X0, double Y0)
 { double X[3], Y[3];
   int k;
   for(k=0; k<=MSG[code].Points-3; k++)
    { X[0] = MSG[code].X[0];     Y[0] = MSG[code].Y[0];
      X[1] = MSG[code].X[k+1];   X[2] = MSG[code].X[k+2];
      Y[1] = MSG[code].Y[k+1];   Y[2] = MSG[code].Y[k+2];
      if(idotin3(X, Y, X0, Y0) == IN)  return IN;
    }
   return OUT;
 }

int   which_active(int X, int Y)
 {  int n=-1, k, layer0=-1;
    for(k=0; k<4; k++)
     { if(idotin(k, X, Y)==IN)
	if(MSG[k].Layer > layer0)
	  { n = k; layer0 = MSG[k].Layer; }
     }
    if(n==-1) return -1;
    set_layer(n);
    return  n;
 }
void  set_layer(int n)
 { int k, layer0=MSG[n].Layer;
   for(k=0; k<4; k++)
     if(MSG[k].Layer>layer0)
       MSG[k].Layer--;
    MSG[n].Layer = 3;
 }

double middle_rad(double middlex, double middley, double X, double Y)
{ double rad, k;
  if(X==middlex)
    { if(Y>middley)  return  3*M_PI/2;
      return  M_PI/2;
    }
  k = (Y-middley)/(X-middlex);
  rad = atan(k);
  if(X<middlex)  rad+=M_PI;
  rad = 2*M_PI - rad;
  if(rad>2*M_PI) rad-=2*M_PI;
  return  rad;
 }

void middle_round(double *X, double *Y, double rad)
 { double rad0, R;
   R = sqrt((*X-middlex)*(*X-middlex) + (*Y-middley)*(*Y-middley));
   rad0 = middle_rad(middlex, middley, *X, *Y)+rad;
   *X = middlex + R*cos(rad0);
   *Y = middley - R*sin(rad0);
 }

