#include <math.h>
#include <graphics.h>
#include <stdio.h>
#include "extern.h"


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

