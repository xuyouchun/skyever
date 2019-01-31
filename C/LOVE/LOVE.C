#define  BEGIN  while(1){
#define  END    }



#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include <dos.h>
#include <string.h>

char S[10];
char * makestr(int num, int n);
void TIME();

char SS[30];
struct time Ti;
struct date Da;
char  BOY[21] = "BoyFriend";
char  GIRL[21] = "GirlFriend";
#define LOVE  "LOVE"

int main()
 {
   int graphdriver=DETECT, graphmode;
   int  color = 1, R = 0, radius = 0;
   int  i, dra = 2;
   int  a, b;
   FILE  *fp;
   double A;

   fp = fopen("love.msg", "rt");
   if(fp != NULL)
    { fgets(BOY,  20, fp);
      fgets(GIRL, 20, fp);
    }
   else
    { printf("Boyfriend\'s name:[%s] ", BOY);
      gets(BOY);
      if(strlen(BOY) == 0)  strcpy(BOY, "BoyFriend");
      printf("Grilfriend\'s name:[%s] ", GIRL);
      gets(GIRL);
      if(strlen(GIRL) == 0)  strcpy(GIRL, "GirlFriend");
      if(!(strcmp(BOY, "BoyFriend")==0 && strcmp(GIRL, "GirlFriend")==0))
	{ fp = fopen("love.msg", "wt");
	  if(fp != NULL)
	   { fputs(BOY, fp);
	     fputc('\n', fp);
	     fputs(GIRL, fp);
	     fclose(fp);
	   }
       }
    }

   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "C:\\TC");
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   BEGIN
   for(R=0; R<=240; R+=10)
    { setcolor(color = ++color==15? 1: color);
      for(i=0; i<360; i+=20)
       { A = (double)(i+radius)*(M_PI/180);
	 circle(320+R*cos(A), 240+R*sin(A), 2);
       }
      radius += dra;
     delay(600);
    }

    if(dra>0) { a = 120; b = 360; }
    else      { a = 360; b = 120; }
    setcolor(YELLOW);
    setwritemode(XOR_PUT);
    settextstyle(1, 0, 6);
    outtextxy(-100, a, BOY);
    outtextxy(740, b, GIRL);
    for(i=0; i<=41; i++)
     { outtextxy(-90+i*10-10, a, BOY);
       outtextxy(-90+i*10, a, BOY);
       outtextxy(730-i*10+10, b, GIRL);
       outtextxy(730-i*10, b, GIRL);
     }
    setwritemode(COPY_PUT);
    outtextxy(320, a, BOY);
    outtextxy(320, b, GIRL);
    setwritemode(XOR_PUT);
    setcolor(LIGHTRED);
    settextstyle(1, 0, 1);
    outtextxy(320, 240, LOVE);
    for(i=2; i<=10; i++)
     { settextstyle(1, 0, i);
       outtextxy(320, 240, LOVE);
       settextstyle(1, 0, i-1);
       outtextxy(320, 240, LOVE);
       delay(600);
     }
    settextstyle(1, 0, 10);
    outtextxy(320, 240, LOVE);
    setwritemode(COPY_PUT);
    settextstyle(1, 0, 9);
    outtextxy(320, 240, LOVE);
    delay(1500);

    TIME();

    for(R=0; R<=40; R+=8)
     { setcolor(color = ++color==15? 1: color);
       for(i=0; i<360; i+=60)
	{ A = (double)(i*(M_PI/180));
	  circle(60+R*cos(A), 50+R*sin(A), 2);
	  circle(580+R*cos(A), 50+R*sin(A), 2);
	}
       delay(1000);
     }

    delay(15000);

    for(R=40; R>=0; R-=8)
     { setcolor(BLACK);
       for(i=0; i<360; i+=60)
	{ A = (double)(i*(M_PI/180));
	  circle(60+R*cos(A), 50+R*sin(A), 2);
	  circle(580+R*cos(A), 50+R*sin(A), 2);
	}
       delay(1000);
     }

    cleardevice();
    R = 0; dra = -dra;
    if(kbhit())  { getch()==0? getch():0; break; }
    END
    closegraph();
    exit(0);
  }


char * makestr(int num, int n)
 { char S0[10];
   int  len, i;
   sprintf(S0, "%d", num);
   len = strlen(S0);
   for(i=0; i<n-len; i++)   S[i] = '0';
   S[i] = '\0';
   strcat(S, S0);
   return S;
 }

void TIME()
 {
  int i;
  getdate(&Da);
  strcpy(SS, makestr(Da.da_year, 4)); strcat(SS, ".");
  strcat(SS, makestr(Da.da_mon, 2)); strcat(SS, ".");
  strcat(SS, makestr(Da.da_day, 2)); strcat(SS, " ");
  gettime(&Ti);
  strcat(SS, makestr(Ti.ti_hour, 2)); strcat(SS, ":");
  strcat(SS, makestr(Ti.ti_min, 2)); strcat(SS, ":");
  strcat(SS, makestr(Ti.ti_sec, 2));
  setwritemode(XOR_PUT);
  setcolor(LIGHTGRAY);
  settextstyle(1, 0, 1);
  outtextxy(320, 440, SS);
  for(i=2; i<=7; i++)
   { settextstyle(1, 0, i);
     outtextxy(320, 440, SS);
     settextstyle(1, 0, i-1);
     outtextxy(320, 440, SS);
     delay(400);
   }
  setwritemode(COPY_PUT);
  settextstyle(1, 0, 7);
  outtextxy(320, 440, SS);
 }