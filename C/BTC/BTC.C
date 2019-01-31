#include <conio.h>
main()
 { int i;
   textbackground(BLUE);
   clrscr();
   textbackground(LIGHTGRAY);
   textcolor(BLACK);
   cputs("   File   Edit   Run   Compile"
   "   Project   Options   Debug   Break/watch        ");
   textcolor(RED);
   gotoxy(4, 1);   putch('F');
   gotoxy(11, 1);  putch('E');
   gotoxy(18, 1);  putch('R');
   gotoxy(24, 1);  putch('C');
   gotoxy(34, 1);  putch('P');
   gotoxy(44, 1);  putch('O');
   gotoxy(54, 1);  putch('D');
   gotoxy(62, 1);  putch('B');
   textcolor(LIGHTGRAY);  textbackground(BLUE);
   gotoxy(1, 2);
   cputs("ีออออออออออออออออออออออออออออออออออออ"
	 "      ออออออออออออออออออออออออออออออออออออธ");
   gotoxy(1, 22);
   cputs("รฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"
	 "       ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด");
   gotoxy(1, 24);
   cputs("ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"
	 "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู");
   for(i=3; i<=21; i++)
     { gotoxy(1, i);   putch('ณ');
       gotoxy(80, i);  putch('ณ'); }
   gotoxy(1, 23);  putch('ณ');
   gotoxy(80, 23); putch('ณ');
   gotoxy(8, 3);
   cputs("Line 1     Col 1   Insert Indent Tab Fill Unindent * C:E.C");
   gotoxy(38, 22);  cputs("Watch");
   textcolor(WHITE);
   gotoxy(39, 2);   cputs("Edit");
   textbackground(LIGHTGRAY);
   textcolor(BLACK);
   gotoxy(1, 25); delline();
   cputs(" F1-Help  F5-Zoom  F6-Switch  F7-Trace  F8-Step  F9-Make  F10-Menu");
   textcolor(RED);
   gotoxy(2, 25);   cputs("F1");
   gotoxy(11, 25);  cputs("F5");
   gotoxy(20, 25);  cputs("F6");
   gotoxy(31, 25);  cputs("F7");
   gotoxy(41, 25);  cputs("F8");
   gotoxy(50, 25);  cputs("F9");
   gotoxy(59, 25);  cputs("F10");
   textcolor(WHITE);  textbackground(BLUE);
   gotoxy(69, 25);  cputs(" NUM ");
   gotoxy(5, 4);

   getch();
   textbackground(BLACK);
   textcolor(LIGHTGRAY);
   clrscr();
 }