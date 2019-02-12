#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include "hz.h"
#include "screen.h"
#include "extern.h"

void screen(int code)
 { int i;
   sethz_file(hzk24fp);
   sethz_dot(24);
   sethz_fx(Y_ROUND);
   sethz_zoomxy(2, 1);
   sethz_speed(code==1?3:0);
   sethz_color(YELLOW);
   writehzs(10, 20, 0, 7);
   if(code==1)  delay(200);
   sethz_zoomxy(4, 4);
   sethz_speed(code==1?4:0);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_dxy(15);
   sethz_color(LIGHTGREEN);
   writehzs(320, 240, 8, 11);
   if(code==1)  delay(200);
   sethz_zoomxy(2, 1);
   sethz_text(LEFT_HZ, MIDDLE_HZ);
   sethz_speed(code==1?3:0);
   sethz_dxy(0);
   sethz_color(YELLOW);
   writehzs(300, 400, 12, 13);
   writehzs(440, 400, 14, 15);

   setcolor(LIGHTGREEN);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   if(code==1)
    { setwritemode(XOR_PUT);
      outtextxy(520, 30, "Skyever");
      for(i=1; i<5; i++)
       { settextstyle(1, 0, i);
	 outtextxy(520, 30, "Skyever");
	 settextstyle(1, 0, i+1);
	 outtextxy(520, 30, "Skyever");
	 delay(150);
       }
      setwritemode(COPY_PUT);
    }

   settextstyle(1, 0, 5);
   outtextxy(520, 30, "Skyever");
   setcolor(RED);
   outtextxy(523, 33, "Skyever");

   sethz_zoomxy(1, 1);
   sethz_color(LIGHTGREEN);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_speed(code==1?2:0);
   writehzs(320, 460, 16, 25);

   sleep(3);

   if(code==1)
    { double  A, I;
      setcolor(BLACK);
      setlinestyle(0, 0, 3);
      for(I=0; I<360; I+=0.1)
       { A = I*M_PI/180;
	 line(320, 240, 320+450*cos(A), 240+450*sin(A));
       }
    }
   setlinestyle(0, 0, 1);
   cleardevice();
 }