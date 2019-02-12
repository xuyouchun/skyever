#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include <stdlib.h>
#include "hz.h"
#include "screen.h"
#include "extern.h"

void screen()
 { int i;
   sethz_file(hzk24fp);
   sethz_dot(24);
   sethz_speed(0);
   sethz_fx(Y_ROUND);
   sethz_zoomxy(2, 1);
   sethz_color(BLUE);
   writehzs(10, 20, 0, 3);
   delay(50);
   sethz_zoomxy(2, 4);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   sethz_dxy(5);
   sethz_color(GREEN);
   writehzs(320, 240, 4, 13);
   delay(50);

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
      delay(20);
    }
   setwritemode(COPY_PUT);
   settextstyle(1, 0, i);
   outtextxy(520, 30, "Skyever");
   setcolor(RED);
   outtextxy(523, 33, "Skyever");

   sethz_zoomxy(1, 1);
   sethz_color(BLUE);
   sethz_text(MIDDLE_HZ, MIDDLE_HZ);
   writehzs(320, 460, 14, 25);
   sleep(1);  setcolor(BLACK);
   randomize();
   switch(random(4))
    { case 0: for(i=0;i<320;i++)
	       { line(320+i, 0, 320+i, 479);
		 line(320-i, 0, 320-i, 479);  delay(1);
	       } break;
      case 1: for(i=0;i<320;i++)
	       { line(i,0,i,479);
		 line(639-i,0,639-i,479);   delay(1);
	       } break;
      case 2: for(i=0;i<240;i++)
	       { line(0, 240+i, 639, 240+i);
		 line(0, 240-i, 639, 240-i);  delay(1);
	       } break;
      case 3: for(i=0;i<240;i++)
	       { line(0, i, 639, i);
		 line(0, 479-i, 639, 479-i);  delay(1);
	       }
     }
   cleardevice();
 }