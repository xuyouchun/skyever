#include <showhz.h>
#include <mouse.h>
#include <graphics.h>
#include <stdio.h>

main()
 { int graphdriver=DETECT, graphmode;
   initgraph(&graphdriver, &graphmode, "c:\\tc");
   open_hzk("c:\\ucdos\\asc16", "c:\\ucdos\\hzk16");
   show_str(50, 240, "走在乡间的小路上,暮归的老牛是我同伴...");
   close_hzk();
   mouse_on();
   getch();
   closegraph();
 }