#include <graphics.h>
#include <dos.h>

#define X 320
#define Y 240
int k, color=1;
int main()
{ int graphdriver=VGA,  graphmode=VGAHI;
  registerbgidriver(EGAVGA_driver);
  registerbgifont(triplex_font);
  initgraph(&graphdriver,&graphmode,"C:\\tc");
  setcolor(YELLOW);
  circle(X,240,100);
  arc(X,165,215,325,60);
  arc(X,270,120,215,80);
  arc(X,270,325,420,80);
  setcolor(DARKGRAY);
  arc(285,270,30,150,16);
  arc(355,270,30,150,16);
  circle(285,260,3);
  circle(355,260,3);
  arc(X, 290,225,315,15);
  arc(X,301,215,325,8);
  settextstyle(1, 0, 4);
  settextjustify(CENTER_TEXT, CENTER_TEXT);
  setcolor(YELLOW);
  outtextxy(320, 450, "Made by Su Xiao Li");
  setcolor(RED);
  outtextxy(323, 453, "Made by Su Xiao Li");

  while(!kbhit())
   { for(k=150; k<=250; k+=10)
	  { setcolor( color++==15?1:color );
		circle(X, Y, k);
		sound(k*3);		delay(1500);
		nosound();      delay(500);
	  }
	 setcolor(0);
	 for(k=150; k<=250; k+=10)
	   { circle(X, Y, k);
		 sound( (350-k)*3 );		 delay(1500);
		 nosound();                  delay(500);
	   }
     setcolor(YELLOW);
	 outtextxy(320, 450, "Made by Su Xiao Li");
	 setcolor(RED);
	 outtextxy(323, 453, "Made by Su Xiao Li");
   }
  closegraph();
  exit(0);
 }