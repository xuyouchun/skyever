#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include "hz.h"
#include "screen.h"
#include "extern.h"

void screen()
 { int i;
   sethz_file(hzk24fp);
   sethz_dot(24);
   sethz_fx(Y_ROUND);
   sethz_zoomxy(2, 1);
   sethz_speed(3);
   sethz_color(YELLOW);
   writehzs(10, 20, 0, 7);
   delay(100);
   sethz_zoomxy(4, 4);
   sethz_speed(5);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_dxy(8);
   sethz_color(LIGHTGREEN);
   writehzs(320, 240, 8, 12);
   delay(500);
   sethz_zoomxy(2, 1);
   sethz_text(LEFT_HZ, MIDDLE_HZ);
   sethz_speed(3);
   sethz_dxy(0);
   sethz_color(YELLOW);
   writehzs(300, 400, 13, 14);
   writehzs(440, 400, 15, 16);

   setcolor(LIGHTGREEN);
   setwritemode(XOR_PUT);
   settextstyle(1, 0, 1);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(520, 30, "Skyever");
   for(i=1; i<5; i++)
    { settextstyle(1, 0, i);
      outtextxy(520, 30, "Skyever");
      settextstyle(1, 0, i+1);
      outtextxy(520, 30, "Skyever");
      delay(150);
    }
   setwritemode(COPY_PUT);
   settextstyle(1, 0, i);
   outtextxy(520, 30, "Skyever");
   setcolor(RED);
   outtextxy(523, 33, "Skyever");

   sethz_zoomxy(1, 1);
   sethz_color(LIGHTGREEN);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_speed(3);
   writehzs(320, 460, 17, 26);

   sleep(3);
   setcolor(BLACK);
   setlinestyle(0, 0, 3);
 { double  A, I;
   for(I=0; I<360; I+=0.1)
    { A = I*M_PI/180;
      line(320, 240, 320+450*cos(A), 240+450*sin(A));
    }
 }
   setlinestyle(0, 0, 1);
   cleardevice();
 }