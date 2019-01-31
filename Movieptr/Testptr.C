#include <dos.h>
#include <stdio.h>
#include <alloc.h>
#include <graphics.h>

#include "mouse.h"


/*以下为测试代码*/
void write_screen()
 { int k;
   setcolor(CYAN);
   for(k=0; k<480; k+=10)
     { setcolor(k/10%15+1); line(0, k, 640, k); }
   for(k=0; k<640; k+=10)
     { setcolor(k/10%15+1); line(k, 0, k, 480); }
   setfillstyle(1, BLUE);
/*   bar(0, 0, 640, 480);  */
 }



main()
 { int graphdriver=DETECT, graphmode, k;
   initgraph(&graphdriver, &graphmode, "c:\\tc");
   write_screen();

   start_mouse();
   mouse_on();


   while(1)
    { if(kbhit())  switch(getch())
	{ case 32: goto _break;
	  case 27: mouse_off(); break;
	  case 13: mouse_on();  break;
	  case '1': setmouse_staticptr(DEFAULT_STATICPTR);  break;
	  case '2': setmouse_movieptr(DEFAULT_MOVIEPTR);  break;
	  case '3': setmouse_staticptr(CROSS_STATICPTR); break;
	  case '4': setmouse_movieptr(WATER_MOVIEPTR); break;
	  case '5': curptr_mirrorx();  break;
	  case '6': curptr_mirrory();  break;	  
	}
      switch(iclick())
	{ case DOUBLECLICK: if(imouse())  mouse_off();  else mouse_on(); break;
	  case LEFTCLICK:   curptr_mirrorx(); break;
	  case RIGHTCLICK:  curptr_mirrory(); break;
	}      
    }
_break:
   exit_mouse();
   closegraph();
 }
