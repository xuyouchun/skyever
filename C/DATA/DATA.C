#include <graphics.h>
#include <math.h>

char  wait();

#define  X  50
#define  Y  440
#define  dx 45
#define  dy 35

#define  width  30

int  data[10] = { 60, 80, 50, 60, 100, 150, 140, 123, 250, 340 };
int  i, j, k;
char  buffer[20];

#define   R  200
double rad, rad0=0;
double sum = 0;

int x0, y0;

int main()
 { int graphdriver=VGA, graphmode=VGAHI;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "c:\\tc");
   begin:

   cleardevice();
   setcolor(BLUE);
   line(X, Y, X+490, Y);
   line(X+490, Y, X+490-5, Y-5);
   line(X+490, Y, X+490-5, Y+5);
   line(X, Y, X, Y-400);
   line(X, Y-400, X-5, Y-400+5);
   line(X, Y-400, X+5, Y-400+5);

   setcolor(RED);
   for(k=1; k<=10; k++)   line(X+k*dx, Y-4, X+k*dx, Y+4);
   for(k=1; k<=10; k++)   line(X-1, Y-k*dy, X+1, Y-k*dy);

   setcolor(YELLOW);
   settextstyle(0, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   for(k=1; k<=10; k++)
	{ sprintf(buffer, "%d", 1990+k);
	  outtextxy(X+k*dx, Y+10, buffer);
	}
   settextjustify(RIGHT_TEXT, CENTER_TEXT);
   for(k=0; k<=10; k++)
	{ sprintf(buffer, "%d", 35*k);
	  outtextxy(X-10, Y-k*35, buffer);
	}

   setfillstyle(3, GREEN);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   for(k=1; k<=10; k++)
	{ int x1, y1, x2, y2;
	  x1 = X+k*dx-width/2;
	  x2 = x1+width;
	  y1 = Y-data[k-1];
	  y2 = Y-1;
	  setcolor(GREEN);
	  bar3d(x1, y1, x2, y2, 5, 1);
	  sprintf(buffer, "%d", data[k-1]);
	  setcolor(LIGHTRED);
	  outtextxy(X+k*dx, y1-10, buffer);
	}
   if(wait()==27)  goto end;

   /**************************************************/

   cleardevice();
   rad0 = sum = 0;
   settextstyle(0, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   for(k=0; k<10; k++)   sum += data[k];
   for(k=0; k<10; k++)
	 { int x, y;  double radt;
	   rad = ((double)data[k]/sum) * 360 ;
	   setfillstyle(k%5+3, k%5+3);
	   setcolor(BLUE);
	   pieslice(320, 280, (int)(rad0+0.5), (int)(rad+rad0+0.5), R);
	   setcolor(YELLOW);
	   radt = (rad0+rad/2)*M_PI/180;
	   x = 320+(R+20)*cos(radt);   y = 280-(R+20)*sin(radt);
	   sprintf(buffer, "%d", 1991+k);
	   outtextxy(x, y, buffer);
	   setcolor(WHITE);
	   x = 320+2*R/3*cos(radt);   y = 280-2*R/3*sin(radt);
	   sprintf(buffer, "%d", data[k]);
	   outtextxy(x, y, buffer);
	   rad0 += rad;
	 }
   if(wait()==27)  goto end;

   /**************************************/

   cleardevice();
   setcolor(BLUE);
   line(X, Y, X+490, Y);
   line(X+490, Y, X+490-5, Y-5);
   line(X+490, Y, X+490-5, Y+5);
   line(X, Y, X, Y-400);
   line(X, Y-400, X-5, Y-400+5);
   line(X, Y-400, X+5, Y-400+5);

   setcolor(RED);
   for(k=1; k<=10; k++)   line(X+k*dx, Y-4, X+k*dx, Y+4);
   for(k=1; k<=10; k++)   line(X-1, Y-k*dy, X+1, Y-k*dy);

   setcolor(YELLOW);
   settextstyle(0, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   for(k=1; k<=10; k++)
	{ sprintf(buffer, "%d", 1990+k);
	  outtextxy(X+k*dx, Y+10, buffer);
	}
   settextjustify(RIGHT_TEXT, CENTER_TEXT);
   for(k=0; k<=10; k++)
	{ sprintf(buffer, "%d", 35*k);
	  outtextxy(X-10, Y-k*35, buffer);
	}
   x0 = X;  y0 = Y;
   for(k=1; k<=10; k++)
	{ int x, y;
	  x = X+dx*k;  y = Y-data[k-1];
	  setcolor(WHITE);
	  circle(x, y, 2);
	  setcolor(GREEN);
	  setlinestyle(0, 0, 1);
	  line(x0, y0, x, y);
	  setlinestyle(1, 0, 1);
	  setcolor(DARKGRAY);
	  line(x, y, X, y);   line(x, y, x, Y);
	  x0 = x; y0 = y;
	}
   setlinestyle(0, 0, 1);
   if(wait()!=27)  goto begin;

   end:
   closegraph();
   exit(0);
 }

char  wait()
 { settextstyle(2, 0, 6);
   setcolor(GREEN);  settextjustify(CENTER_TEXT, TOP_TEXT);
   outtextxy(320, 20, "Press any key to continue . . .");
   return getch();
 }