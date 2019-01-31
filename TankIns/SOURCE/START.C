#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <process.h>
#include <dos.h>
#include <string.h>
#include <bios.h>
#include <key.h>


void select(), writefk(), showdemo_tank(), showdemo_skyever(), doprogram();
void testpath();
int sel = 1;


#define MAX   5
#define BEGIN while(1){
#define WEND }
#define write_str()  \
 textcolor(CYAN);  gotoxy(5, 20);  cprintf("Please Select ... ")

int main()
 { int k;  unsigned char ch;  int bselup=MAX+4, bseldown=MAX+26;
   long Time=clock()-20, Time0=clock()-5;
   clrscr();
   writefk();   select();
   testpath();

BEGIN
   if(kbhit()) switch(ch=getch())
      { case ESC_KEY:   textcolor(LIGHTGRAY);  return 5;
	case ENTER_KEY: if(sel==5) { textcolor(LIGHTGRAY);  return 5; }
			else doprogram();
			clrscr();  writefk();  select();  break;
	case UP_KEY:    if(sel>MAX)
			 { sel -= 7;
			   if(sel<=MAX) { bselup = sel+7; sel = MAX; }
			   select();  break;
			 }
			else if(sel==1)
			 { sel = bseldown; select(); break; }
	case LEFT_KEY:  sel--; select(); break;
	case DOWN_KEY:  if(sel>MAX)
			 { sel += 7;
			   if(sel>=MAX+26+1)  { bseldown=sel-7;  sel = 1; }
			   select();  break;
			 }
			else if(sel==MAX)
			 { sel = bselup; select(); break; }
	case RIGHT_KEY: sel++; select(); break;
	default: if(ch<='Z'&&ch>='A')       sel = MAX+1+ch-'A';
		 else if(ch<='z'&&ch>='a')  sel = MAX+1+ch-'a';
		 else if(ch>='1'&&ch<='1'+MAX) sel = ch-'1'+1;
		 else break;
		 select();
      }
    if(clock()-Time>20)  { showdemo_tank();    Time=clock(); }
    if(clock()-Time0>1)  { showdemo_skyever(); Time0=clock(); }
WEND
 }

void select()
 { char *str[] =
    { "==========>>>> KIND 1 (Start...) <<<<==========",
      "==========>>>> KIND 2 (Continue) <<<<==========",
      "==========>>>> KIND 3 (Too Easy) <<<<==========",
      "==========>>>> KIND 4 (Too Hard) <<<<==========",
      "==========>>>> >>>> (EXIT) <<<<  <<<<=========="
    };
   char str_k[12];

   int k;
   if(sel==0)  sel = MAX+26;  else if(sel==MAX+26+1) sel = 1;
   for(k=1; k<=MAX; k++)
    { gotoxy(18, 6+k);
      textbackground(k==sel?LIGHTGRAY:BLACK);
      textcolor(k==sel?LIGHTBLUE:GREEN);
      cputs(str[k-1]);
    }
   for(;k<=26+MAX; k++)
    { gotoxy(3+((k-MAX-1)%7)*11, 13+(k-MAX-1)/7);
      textbackground(k==sel?LIGHTGRAY:BLACK);
      textcolor(k==sel?LIGHTBLUE:LIGHTCYAN);
      sprintf(str_k, "  KIND %c  ", 'A'+k-MAX-1);
      cputs(str_k);
    }
   textbackground(BLACK);
   write_str();
 }

void writefk()
 { int k;
   textcolor(WHITE);
   cputs("\n===>> Tank 3.1 Starter ==>>\n\r");
   textcolor(CYAN);
   cputs("様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様"
     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r"
   "様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様");
   textcolor(LIGHTGRAY);
   gotoxy(50, 24);  cputs("Email: Skyever_youer@sohu.com");
   gotoxy(1, 24);   cputs("2003.4.14 At LiaoCheng ShangDong (XuYouChun)");
   textcolor(DARKGRAY);
   gotoxy(45, 20);  cputs("(Enter: Start...    Esc: Quit...)");
   gotoxy(43, 21);  cputs("(Shift+Enter: Start without Sound)");
   gotoxy(41, 22);  cputs("(Left/Right/Up/Down arrow: Select)");
 }

void showdemo_tank()
 { static int color = 1;
   int k;
   gotoxy(12, 5);
   for(k=0; k<5; k++)
    { if(++color==16) color = 1;
      textcolor(color);
      cputs("  TANK 3.1  ");
    }
   write_str();
 }

void showdemo_skyever()
 { static int x = 1, changeflag=1;
   textcolor(LIGHTBLUE);
   gotoxy(x, 18); cprintf(" --->>> SKYEVER <<<--- ");
   x += changeflag;
   if(x==58)  changeflag = -1;
   if(x==1)   changeflag = 1;
   write_str();
 }

void doprogram()
 { char str_k[10];
   switch(sel)
    { case 1: system("Tank.exe");    break;
      case 2: system("Tank.exe #q"); break;
      case 3: case 4:
	      sprintf(str_k, "Tank.exe %c^%cq",
		 (bioskey(2)&0x03)?'!':' ',  sel==3?'d':'e');
	      system(str_k);   break;
      default: sprintf(str_k, "Tank.exe %c^%cq",
	      (bioskey(2)&0x03)?'!':' ', 'A'+sel-MAX-1);
	      system(str_k);   break;
    }
 }


void testpath()
 { char path[100];
   getcurdir(0, path);
   if(strcmp(path+strlen(path)-3, "MSG")==0)  chdir("..");
 }