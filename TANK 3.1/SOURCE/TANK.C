#include <graphics.h>
#include <math.h>
#include <bios.h>
#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include <alloc.h>
#include "tools.h"
#include "hz.h"
#include "screen.h"
#include "extern.h"
#include "tank.h"
#include <key.h>
#include <mouse.h>

TANKDEF  Tank[12];
SHOTDEF  Shot[MAXSHOTNUM];
ROADDEF  Road[28][36], Roadtank[28][36];
ROADATTRIBDEF  Roadattrib[7] = {
 //          istoptank   ihard    istopshot    icoverd   ihua   iend
 /*block*/{    YES     ,   NO    ,   YES    ,     0    ,   0  ,  NO },
 /*Ice*/  {    NO      ,   0     ,   NO     ,     NO   ,   YES,  NO },
 /*Iron*/ {    YES     ,   YES   ,   YES    ,     0    ,   0  ,  NO },
 /*Sea*/  {    YES     ,   0     ,   NO     ,     NO   ,   0  ,  NO },
 /*Tree*/ {    NO      ,   0     ,   NO     ,     YES  ,   NO ,  NO },
 /*Road*/ {    NO      ,   0     ,   NO     ,     NO   ,   NO ,  NO },
 /*Head*/ {    YES     ,   1     ,   YES    ,     0    ,   0  ,  YES } };

unsigned char  Roadptr[8][4][16*2];
unsigned char  Treebit[4][16*2];
unsigned char  Tankptr[5][4][4][32*4];
unsigned char  Tankbit[5][4][32*4];
unsigned char  Breakptr[5][4][32*4];
unsigned char  Breakbit[5][32*4];
unsigned char  Exptr[9][4][32*6];
unsigned char  Exbit[9][32*6];
unsigned char  Headptr[4][32*4];
unsigned char  Overptr[4][32*4];
unsigned char  Picbefore[2] = {0,0};
unsigned char  Picptr[4][32][36*2];
unsigned char  Picend[2] = {0,0};
unsigned char  Shotptr[3][4][16];
unsigned char  Shotbit[3][16];
unsigned char  Kindptr[8][32*4];
struct _EAT  Eat;
extern  char  overflag, kindflag, isound, idirectrun, idefaultrun;
extern  char  wave[13], stage[14];

int  ishowscreen = 1;
FILE *hzk16fp, *hzk24fp;

void  writeintank0(TANKDEF *Tankp);
void  writeinroad();
void  writeineat();
void  writeinshot();
void  writeintree();
void  writeonscreen();
void  writeintank();
void  interrupt  (*olddiskerror)();
void  interrupt    diskerror() { /* do nothing */ }

int main(int argc, char **argv)
 { int i, j;
   int graphdriver = VGA, graphmode=VGAHI;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);
   registerbgifont(sansserif_font);
   registerbgifont(small_font);
   registerbgifont(gothic_font);
   initgraph(&graphdriver, &graphmode, BGIPATH);
   build(argc, argv);
   disable();
   olddiskerror = getvect(0x24);
   setvect(0x24, diskerror);
   enable();
   restorecrtmode();  getpic();   setgraphmode(VGAHI);
   setbkcolor(BLUE);
   if(ishowscreen)  screen(ishowscreen);
   setbkcolor(BLACK);
   while(selectfa()!=4);
   Exit(0);   return 0;
 }

void  getpic()
 {  int i, j, k, n;   FILE *fp;
    char  tankfilename[][7] = { "Tank", "Tank0", "Tank1", "Tank2", "Tank3" };
    char  roadfilename[][10] = { "Block", "Ice", "Iron", "Sea", "Tree", "Firetr0", "Firetr1", "Firetr2" };
    char  expfilename[][10] = { "Bomb", "Pause", "Rect", "Tank", "Actv", "Gun", "Star", "Ship", "Fire" };
    char  breakfilename[][10] = { "Break0", "Break1", "Break2", "Appear0", "Appear1" };
    char  shotfilename[][10] = { "Shot", "Shbreak", "Shappear" };
    char  kindfilename[][10] = { "addjust", "Eatings" };
    char  round[4] = { 2, 1, 3, 0 };
    // ***************************************************************
    for(k=0; k<5; k++)
     { char  tankstr[30];
       void roundimage(unsigned char *ptr0, unsigned char *ptr1);
       sprintf(tankstr, "Pic\\Tankpic\\%s", tankfilename[k] );
       if( (fp=fopen(tankstr, "rb"))==NULL )
	 { printf("Error: Can't find file \"%s\"!\n", tankstr);
	   Exit(1);
	 }
       fseek(fp, 2, SEEK_SET);
       for(i=0; i<4; i++)
       for(n=0; n<32*4; n++)
	 Tankptr[k][round[0]][i][n] = (unsigned char)fgetc(fp);
       fclose(fp);
       for(j=0; j<3; j++) for(n=0; n<4; n++)
	 roundimage(Tankptr[k][round[j]][0]+n*32*4,Tankptr[k][round[j+1]][0]+n*32*4);
       for(j=0; j<4; j++)  for(n=0; n<32*4; n++)
	 Tankbit[k][j][n] = ~(Tankptr[k][j][0][n] | Tankptr[k][j][1][n]
		    | Tankptr[k][j][2][n] | Tankptr[k][j][3][n]);
     }
//**********************************************************************
    for(k=0; k<2; k++)
     { char  kindstr[30];
       sprintf(kindstr, "Pic\\Kindpic\\%s", kindfilename[k]);
       if( (fp=fopen(kindstr, "rb"))==NULL )
	{ printf("Error: Can't find file \"%s\"!\n", kindstr);
	  Exit(1);
	}
       fseek(fp, 2, SEEK_SET);
       for(n=0; n<32*4; n++)  Kindptr[k*4+round[0]][n]=(unsigned char)fgetc(fp);
       for(j=0; j<3; j++) roundimage(Kindptr[k*4+round[j]], Kindptr[k*4+round[j+1]]);
       fclose(fp);
      }
//**************************************************************************
   for(k=0; k<5; k++)
     { char  breakstr[30];
       sprintf(breakstr, "Pic\\Breakpic\\%s", breakfilename[k] );
       if( (fp=fopen(breakstr, "rb"))==NULL )
	 { printf("Error: Can't find file \"%s\"!\n", breakstr);
	   Exit(1);
	 }
       fseek(fp, 2, SEEK_SET);
       for(i=0; i<4; i++)
	for(n=0; n<32*4; n++)
	  Breakptr[k][i][n] = (unsigned char)fgetc(fp);
       fclose(fp);
       for(n=0; n<32*4; n++)
	 Breakbit[k][n] = ~(Breakptr[k][0][n] | Breakptr[k][1][n]
	     | Breakptr[k][2][n] | Breakptr[k][3][n]);
      }
// ******************************************************************
    for(k=0; k<8; k++)
     {	char  roadstr[30];
	sprintf(roadstr, "Pic\\Roadpic\\%s", roadfilename[k]);
	if( (fp=fopen(roadstr, "rb")) == NULL)
	  { printf("Error: Can't find file \"%s\"!\n", roadstr);
	    Exit(1);
	  }
	fseek(fp, 2, SEEK_SET);
	for(i=0; i<4; i++)
	 for(n=0; n<16*2; n++)
	   Roadptr[k][i][n] = (unsigned char)fgetc(fp);
	fclose(fp);
	if(k>=4)
	  for(n=0; n<16*2; n++)
	    Treebit[k-4][n] = ~(Roadptr[k][0][n]|Roadptr[k][1][n]|Roadptr[k][2][n]
			       |Roadptr[k][3][n]);
      }
   // **************************************************************
     for(k=0; k<9; k++)
       { char  expstr[30];
	 sprintf(expstr, "Pic\\Expic\\%s", expfilename[k]);
	 if( (fp=fopen(expstr, "rb"))==NULL )
	   { printf("Error: Can't find file \"%s\"!\n", expstr);
	     Exit(1);
	   }
	 fseek(fp, 2, SEEK_SET);
	 for(i=0; i<4; i++)
	  for(n=0; n<32*6; n++)
	    Exptr[k][i][n] = (unsigned char)fgetc(fp);
	 fclose(fp);
	 for(n=0; n<32*6; n++)
	   Exbit[k][n] = ~(Exptr[k][0][n]|Exptr[k][1][n]|
		     Exptr[k][2][n]|Exptr[k][3][n]) ;
       }
//********************************************************************
     if( (fp=fopen("Pic\\Roadpic\\Head", "rb")) == NULL )
       { printf("Error: Can't find file \"Pic\\Roadpic\\Head\"!\n");
	 Exit(1);
       }
     fseek(fp, 2, SEEK_SET);
     for(i=0; i<4; i++)
       for(n=0; n<32*4; n++)
	 Headptr[i][n] = (unsigned char)fgetc(fp);
     fclose(fp);
//*******************************************************************
     if( (fp=fopen("Pic\\Roadpic\\Gameover", "rb")) == NULL )
       { printf("Error: Can't find file \"Pic\\Roadpic\\Gameover\"!\n");
	 Exit(1);
       }
     fseek(fp, 2, SEEK_SET);
     for(i=0; i<4; i++)
       for(n=0; n<32*4; n++)
	 Overptr[i][n] = (unsigned char)fgetc(fp);
     fclose(fp);
   // *****************************************************************
   for(k=0; k<3; k++)
     { char shotstr[30];
       sprintf(shotstr, "Pic\\Breakpic\\%s", shotfilename[k]);
       if( (fp=fopen(shotstr, "rb")) == NULL )
	 { printf("Error: Can't find file \"%s\"!\n", shotstr);
	   Exit(1);
	 }
       fseek(fp, 2, SEEK_SET);
       for(i=0; i<4; i++)
	 for(n=0; n<8*2; n++)
	   Shotptr[k][i][n] = (unsigned char)fgetc(fp);
       fclose(fp);
       for(n=0; n<8*2; n++)  Shotbit[k][n] = ~(Shotptr[k][0][n]|Shotptr[k][1][n]
	      |Shotptr[k][2][n]|Shotptr[k][3][n]);
     }
   //*******************************************************************
   fp = fopen("msg\\kindsave", "rb");
   if(fp==NULL)
    { printf("Error: Cant find file \"kindsave\"!");  Exit(1); }
   readkind(fp);  fclose(fp);
  }


#define  readbit(j,i)  ((*(ptr0+(j)*4+(i)/8))&(unsigned char)0x80>>((i)%8))
#define  setbit(j,i)   (*(ptr1+(j)*4+(i)/8))|=((unsigned char)0x80>>((i)%8))
void roundimage(unsigned char *ptr0, unsigned char *ptr1)
 { int i, j;
   for(j=0; j<32; j++)   for(i=0; i<32; i++)
    if(readbit(31-i,j))  setbit(j, i);
 }

#define  Rep  *ptr0++ = *pic++
#define  Repall \
	for(i=0; i<4; i++)\
	  { Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep;\
	    Rep; Rep; Rep; Rep; Rep; Rep; Rep; Rep;\
	  } ptr0 += 8
#define  Rep0   *p0++ = *p++;  *p0 = *p++;  p0 += 71
#define  Rep1   *p0++ = 0; *p0 = 0; p0 += 71
#define  Rep2   *p0 = *p++;  p0 += 72;  p++
#define  Rep_   *p0 = 0;  p0 += 72;
#define  Rep3   *p0++ = *p++; *p0 = *p; p0 += 71; p += 3

unsigned  char  tankpic[5];
unsigned  char  tankbit[32*5];
int X, Y, tankY0, tankY1, kind, shotY0, shotY1, Y0, Y1, Line;
char kindlow, kindhi;
unsigned char *p, *p0, *ptank, *bitp, *Pp, *Bp;
char  far *ptr0, *pic;
TANKDEF  *Tankp;   SHOTDEF  *Shotp;
static  int   k;
void  setroad(int line)
 { Line = line;
   Y0 = (Line<<5)+TOP; Y1 = Y0+31;
   writeinroad();     //Road
   writeintank();     //Tank
   writeinshot();     //Shot
   writeintree();     //Tree
   writeineat();      //Eat
   writeonscreen();   //Screen
 }

void  writeintank()
 { Tankp = Tank;
   for(k=0; k<12; k++)
    { if(Tankp->life)  writeintank0(Tankp);
      Tankp ++;
    }
 }
extern long a;
void  Exit(int code)
 {  while(kbhit())  getch();
    fcloseall();    closegraph();
    while(kbhit())   getch();
    disable();    setvect(0x24, olddiskerror);    enable();
    exit(code);
 }

void  build(int argc, char **argv)
 { int k;  extern char STAGE[15];
   void far *address;
   char  deal[100] = "";
   for(k=1; k<argc; k++)
     { if(*argv[k]=='+')  strcpy(wave, strupr(argv[k]+1));
       else if(*argv[k]=='-')  strcpy(stage, strupr(argv[k]+1));
       else if(*argv[k]=='=')
	 { strcpy(STAGE, strupr(argv[k]+1)); idefaultrun = 1; }
       else strcat(deal, argv[k]);
     }
   for(k=0; k<strlen(deal); k++)
     { if(deal[k]=='q')  ishowscreen = 0;
       else if(deal[k]=='*')  ishowscreen = 2;
       else if(deal[k]=='!')  isound = 1;
       else if(deal[k]=='^')  idirectrun = 1;
       else if(deal[k]=='#')  idefaultrun = 1;
       else if(deal[k]<='z'&&deal[k]>='a')  kindflag = deal[k]-'a';
       else if(deal[k]<='Z'&&deal[k]>='A')  kindflag = deal[k]-'A';
       else if(deal[k]=='?')
	{ closegraph();
	  printf("\nFormat: TANK [q][*][!][^][#][?][A~Z] [+wave] [-stage] [=breakpoint]\n\n");
	  printf("     ==>>Skyever<<==\n");
	  printf("       Xu You Chun\n");
	  exit(0);
	}
     }

   address = getvect(0x33);
   setcolor(YELLOW);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   if(address==NULL || *(unsigned char *)address==0xcf)
    { outtextxy(320, 200, "Mouse driver not installed, please install it!");
      outtextxy(320, 230, "(Run \"mouse.com\")");
      outtextxy(320, 260, "Press any key to continue . . .");
      getch();      Exit(1);
    }
   initmouse();

   hzk24fp = fopen("msg\\tank.fnt", "rb");
   if(hzk24fp == NULL)
    { outtextxy(320, 230, "Can't open file \"TANK.FNT\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1);
    }
   hzk16fp = fopen("msg\\menu.fnt", "rb");
   if(hzk16fp == NULL)
    { outtextxy(320, 230, "Can't open file \"MENU.FNT\"!");
      outtextxy(320, 260, "Press any key to continue . . ."); getch();
      Exit(1);
    }
 }


void  write_screen()
 { cleardevice();
   setfillstyle(11, GREEN);
   bar(0, 0, LEFT-1, 479);
   bar(RIGHT+1, 0, 639, 479);
   bar(0, BOTTOM+1, 639, 479);
   bar(0, 0, 639, TOP-1);
   setcolor(BLUE);
   rectangle(LEFT-1, TOP-1, RIGHT+1, BOTTOM+1);
 }

void  writeintank0(TANKDEF *Tankp)
 {  int round = Tankp->round;  int i, j, m;  int flag = 0;
      int dxy = Tankp->dxy, life;
      if(Tankp->ienemy==YES)  life = Tankp->life;
      else life = 0;
      if(life>4)  life = Tankp->life = 4;
      tankY0 = ((Tankp->Y)<<4)+TOP;
      if(round>1)  tankY0 += dxy;    tankY1 = tankY0+31;
      if(tankY1<Y0 || tankY0>Y1)  return;
      X = Tankp->X;  Y = Tankp->Y;
      if(Tankp->state==3)
	{ Pp = Tankptr[life][round][0]; Bp = Tankbit[life][round]; }
      else if(Tankp->state<3)
	{ Pp = Breakptr[Tankp->state][0]; Bp = Breakbit[Tankp->state]; }
      else if(Tankp->state>3)
	{ Pp = Breakptr[3+(((int)(Tankp->state))&0x01)][0];
	  Bp = Breakbit[3+(((int)(Tankp->state))&0x01)];
	}
      if(round>1)
       { int begin, end, dy;
	 dy = tankY0-Y0;
	 begin = 32-dy;  end = 32+dy;
	 if(dy>=0)
	   for(m=0; m<4; m++)
	    { p0 = Picptr[m][dy]+(X<<1);
	      p  = Pp + (m<<7);      bitp = Bp;
	      for(j=0; j<begin; j++)
	       { *p0&=*bitp++;  *p0++|=*p++;  *p0&=*bitp++;  *p0++|=*p++;
		 *p0&=*bitp++;  *p0++|=*p++;  *p0&=*bitp++;  *p0  |=*p++;
		  p0 += 69;
	       }
	    }
	 else
	   for(m=0; m<4; m++)
	    { p0 = Picptr[m][0]+(X<<1);
	      p  = Pp + (m<<7)+(-dy<<2);   bitp = Bp + (-dy<<2);
	      for(j=0; j<end; j++)
	       { *p0&=*bitp++;  *p0++|=*p++;  *p0&=*bitp++;  *p0++|=*p++;
		 *p0&=*bitp++;  *p0++|=*p++;  *p0&=*bitp++;  *p0  |=*p++;
		  p0 += 69;
	       }
	    }
       }
      else
       { int begin, end, absd=abs(dxy), d=8-absd;
	 if(Y==(Line<<1))         { begin = 0;  end = 31; }
	 else if(Y==(Line<<1)-1)  { begin = 16; end = 31; }
	 else                     { begin = 0;  end = 15; }
	 if(dxy>=0)
	  { p  = Bp + (begin<<2);
	    p0 = tankbit;
	    for(j=begin; j<=end; j++)
	     { p0[4] = p[3]<<d;      p0[4]|=(0xff>>absd);
	       p0[3] = p[3]>>absd;   p0[3]|=(p[2]<<d);
	       p0[2] = p[2]>>absd;   p0[2]|=(p[1]<<d);
	       p0[1] = p[1]>>absd;   p0[1]|=(p[0]<<d);
	       p0[0] = p[0]>>absd;   p0[0]|=(0xff<<d);
	       p0 += 5;  p += 4;
	     }
	    for(m=0; m<4; m++)
	     { p0 = Picptr[m][Y==(Line<<1)?begin:16-begin] + (X<<1);
	       ptank = Pp + (m<<7)+(begin<<2);
	       bitp = tankbit;
	       for(j=begin; j<=end; j++)
		{ p = tankpic;
		  *p++ = *ptank++; *p++ = *ptank++;
		  *p++ = *ptank++; *p   = *ptank++;
		  tankpic[4] = tankpic[3]<<d;
		  tankpic[3]>>=absd;  tankpic[3]|=(tankpic[2]<<d);
		  tankpic[2]>>=absd;  tankpic[2]|=(tankpic[1]<<d);
		  tankpic[1]>>=absd;  tankpic[1]|=(tankpic[0]<<d);
		  tankpic[0]>>=absd;
		  p = tankpic;
		  *p0&=*bitp++; *p0++|=*p++;  *p0&=*bitp++; *p0++|=*p++;
		  *p0&=*bitp++; *p0++|=*p++;  *p0&=*bitp++; *p0++|=*p++;
		  *p0&=*bitp++; *p0  |=*p++;   p0 += 68;
		}
	     }
	   }
	 else
	  { p  = Bp + (begin<<2);
	    p0 = tankbit;
	    for(j=begin; j<=end; j++)
	     { p0[0] = p[0]>>d;      p0[0]|=0xff<<absd;
	       p0[1] = p[0]<<absd;   p0[1]|=(p[1]>>d);
	       p0[2] = p[1]<<absd;   p0[2]|=(p[2]>>d);
	       p0[3] = p[2]<<absd;   p0[3]|=(p[3]>>d);
	       p0[4] = p[3]<<absd;   p0[4]|=0xff>>d;
	       p0 += 5;  p += 4;
	     }
	    for(m=0; m<4; m++)
	     { p0 = Picptr[m][Y==(Line<<1)?begin:16-begin] + (X<<1) - 1;
	       ptank = Pp + (m<<7)+(begin<<2);
	       bitp = tankbit;
	       for(j=begin; j<=end; j++)
		{ p = tankpic + 1;
		  *p++ = *ptank++; *p++ = *ptank++;
		  *p++ = *ptank++; *p   = *ptank++;
		  tankpic[0] = tankpic[1]>>d;
		  tankpic[1]<<=absd;  tankpic[1]|=(tankpic[2]>>d);
		  tankpic[2]<<=absd;  tankpic[2]|=(tankpic[3]>>d);
		  tankpic[3]<<=absd;  tankpic[3]|=(tankpic[4]>>d);
		  tankpic[4]<<=absd;
		  p = tankpic;
		  *p0&=*bitp++; *p0++|=*p++;  *p0&=*bitp++; *p0++|=*p++;
		  *p0&=*bitp++; *p0++|=*p++;  *p0&=*bitp++; *p0++|=*p++;
		  *p0&=*bitp++; *p0  |=*p++;   p0 += 68;
		}
	     }
	  }
       }
//************************************************************ flash begin
     if(Tankp->state!=3)    return;
     if(Tankp->addjust==0)  goto next;
     if(((unsigned char)Tankp->addjust)&0x03)  goto next;
     Bp = Pp = Kindptr[Tankp->round];  flag = 0;  goto begin;
next:if(Tankp->eats==0)     return;
     if(Tankp->flashflag<3)  return;
     Bp = Pp = Kindptr[Tankp->round+4];  flag = 1;
begin:if(round>1)
       { int begin, end, dy;
	 dy = tankY0-Y0;
	 begin = 32-dy;  end = 32+dy;
	 if(dy>=0)
	   for(m=0; m<4; m++)
	    { p0 = Picptr[m][dy]+(X<<1);
	      p  = Pp;
	      for(j=0; j<begin; j++)
	       { *p0++|=*p++; *p0++|=*p++; *p0++|=*p++;
		 *p0  |=*p++;  p0 += 69;
	       }
	    }
	 else
	   for(m=0; m<4; m++)
	    { p0 = Picptr[m][0]+(X<<1);
	      p  = Pp + (-dy<<2);
	      for(j=0; j<end; j++)
	       { *p0++|=*p++;  *p0++|=*p++;  *p0++|=*p++;
		 *p0  |=*p++;   p0 += 69;
	       }
	    }
       }
      else
       { int begin, end, absd=abs(dxy), d=8-absd;
	 if(Y==(Line<<1))         { begin = 0;  end = 31; }
	 else if(Y==(Line<<1)-1)  { begin = 16; end = 31; }
	 else                     { begin = 0;  end = 15; }
	 if(dxy>=0)
	  { p  = Bp + (begin<<2);
	    p0 = tankbit;
	    for(j=begin; j<=end; j++)
	     { p0[4] = p[3]<<d;      p0[4]|=(0xff>>absd);
	       p0[3] = p[3]>>absd;   p0[3]|=(p[2]<<d);
	       p0[2] = p[2]>>absd;   p0[2]|=(p[1]<<d);
	       p0[1] = p[1]>>absd;   p0[1]|=(p[0]<<d);
	       p0[0] = p[0]>>absd;   p0[0]|=(0xff<<d);
	       p0 += 5;  p += 4;
	     }
	    for(m=0; m<4; m++)
	     { p0 = Picptr[m][Y==(Line<<1)?begin:16-begin] + (X<<1);
	       ptank = Pp + (begin<<2);
	       bitp = tankbit;
	       for(j=begin; j<=end; j++)
		{ p = tankpic;
		  *p++ = *ptank++; *p++ = *ptank++;
		  *p++ = *ptank++; *p   = *ptank++;
		  tankpic[4] = tankpic[3]<<d;
		  tankpic[3]>>=absd;  tankpic[3]|=(tankpic[2]<<d);
		  tankpic[2]>>=absd;  tankpic[2]|=(tankpic[1]<<d);
		  tankpic[1]>>=absd;  tankpic[1]|=(tankpic[0]<<d);
		  tankpic[0]>>=absd;
		  p = tankpic;
		  *p0++|=*p++;  *p0++|=*p++; *p0++|=*p++; *p0++|=*p++;
		  *p0  |=*p++;   p0 += 68;
		}
	     }
	   }
	 else
	  { p  = Bp + (begin<<2);
	    p0 = tankbit;
	    for(j=begin; j<=end; j++)
	     { p0[0] = p[0]>>d;      p0[0]|=0xff<<absd;
	       p0[1] = p[0]<<absd;   p0[1]|=(p[1]>>d);
	       p0[2] = p[1]<<absd;   p0[2]|=(p[2]>>d);
	       p0[3] = p[2]<<absd;   p0[3]|=(p[3]>>d);
	       p0[4] = p[3]<<absd;   p0[4]|=0xff>>d;
	       p0 += 5;  p += 4;
	     }
	    for(m=0; m<4; m++)
	     { p0 = Picptr[m][Y==(Line<<1)?begin:16-begin] + (X<<1) - 1;
	       ptank = Pp + (begin<<2);
	       bitp = tankbit;
	       for(j=begin; j<=end; j++)
		{ p = tankpic + 1;
		  *p++ = *ptank++; *p++ = *ptank++;
		  *p++ = *ptank++; *p   = *ptank++;
		  tankpic[0] = tankpic[1]>>d;
		  tankpic[1]<<=absd;  tankpic[1]|=(tankpic[2]>>d);
		  tankpic[2]<<=absd;  tankpic[2]|=(tankpic[3]>>d);
		  tankpic[3]<<=absd;  tankpic[3]|=(tankpic[4]>>d);
		  tankpic[4]<<=absd;
		  p = tankpic;
		  *p0++|=*p++;  *p0++|=*p++;  *p0++|=*p++; *p0++|=*p++;
		  *p0  |=*p++;   p0 += 68;
		}
	     }
	  }
       }
      if(flag==0)  goto next;
 }


void  writeinroad()
 { for(Y=0; Y<2; Y++)  for(X=0; X<36; X++)
     { kind = Road[(Line<<1)+Y][X];  kindlow = kind&0x0f;
       if(kindlow==5||kind==4)
	for(k=0; k<4; k++)
	  { register int j;
	    p0 = Picptr[k][Y<<4] + (X<<1);
	    for(j=0; j<2; j++)
	     { Rep1; Rep1; Rep1; Rep1; Rep1; Rep1; Rep1; Rep1; }
	  }
      else if(kindlow<=3&&kindlow || kind==0xf0 || kind==0x00)
	for(k=0; k<4; k++)
	  { register int j;
	    p = Roadptr[kindlow][k];   p0 = Picptr[k][Y<<4] + (X<<1);
	    for(j=0; j<2; j++)
	      { Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; Rep0; }
	  }
      else if(kindlow==0)
	for(k=0; k<4; k++)
	  { register int i;
	    for(i=0; i<4; i++)
	      { p0 = Picptr[k][Y<<4]+(X<<1) + (i>>1?576:0) + (i&0x01);
		if(kind&(0x80>>i))
		 { p  = Roadptr[kindlow][k] + ((i>>1)<<4) + (i&0x01);
		   Rep2; Rep2; Rep2; Rep2; Rep2; Rep2; Rep2; Rep2;
		 }
		else
		 { Rep_; Rep_; Rep_; Rep_; Rep_; Rep_; Rep_; Rep_; }
	      }
	  }
      else if(kindlow==6)
	for(k=0; k<4; k++)
	  { register j;
	    if(overflag==0)
	      p = Headptr[k] + ((kind<<1)&~0x3f) + (((kind>>4)&0x01)<<1);
	    else p = Overptr[k] + ((kind<<1)&~0x3f) + (((kind>>4)&0x01)<<1);
	    p0 = Picptr[k][Y<<4] + (X<<1);
	    for(j=0; j<2; j++)
	     { Rep3; Rep3; Rep3; Rep3; Rep3; Rep3; Rep3; Rep3; }
	  }
      }
  }

void  writeineat()
 { Y = Eat.Y;  X = Eat.X;
   if(Eat.kind!=NOEAT && Y>=(Line<<1)-1 && Y<=(Line<<1)+1)
    { int begin, end, kind=Eat.kind, start;
      if(Y==(Line<<1)-1)        { begin = 0;  end = 15; }
      else if(Y==Line<<1)       { begin = 0;  end = 31; }
      else if(Y==(Line<<1)+1)   { begin = 16; end = 31; }
      start = Y==(Line<<1)? begin: 16-begin;
      for(k=0; k<4; k++)
       { register j;
	 p0   = Picptr[k][begin]+(X<<1);
	 bitp = Exbit[kind]+(start<<2)+(start<<1);
	 p    = Exptr[kind][k] + (start<<2) + (start<<1);
	 for(j=begin; j<=end; j++)
	  { *p0 &= *bitp++;  *p0++ |= *p++;
	    *p0 &= *bitp++;  *p0++ |= *p++;
	    *p0 &= *bitp++;  *p0++ |= *p++;
	    *p0 &= *bitp++;  *p0++ |= *p++;
	    *p0 &= *bitp++;  *p0++ |= *p++;
	    *p0 &= *bitp++;  *p0   |= *p++;
	     p0 += 67;
	  }
       }
    }
 }


void  writeinshot()
 {  Shotp = Shot;
    for(k=0; k<15; k++)
     { int  dxy = Shotp->dxy;
       if(Shotp->life==0)  { Shotp++; continue; }
       shotY0 = ((int)(Shotp->Y)<<4)+4 - (Y&0x01);
       if(Shotp->round>1)   shotY0 += dxy;
       shotY1 = shotY0+8;
       if(shotY0>Y1 || shotY1<Y0)  { Shotp++; continue; }
       if(Shotp->state>=2)  { Pp = Shotptr[2][0];  Bp = Shotbit[2]; }
       else if(Shotp->state==1)  { Pp = Shotptr[0][0];  Bp = Shotbit[0]; }
       else  { Pp = Shotptr[1][0];  Bp = Shotbit[1]; }
       if(Shotp->round>1)
	{ int begin, end, Y, m, X=Shotp->X;
	  if(shotY0<Y0) { begin=Y0-shotY0-1; Y=0;  end=7; }
	  else if(shotY1>Y1)
	     { end=7-(shotY1-Y1);  Y=24+(shotY1-Y1); begin=0; }
	  else { begin=0; end=7; Y=shotY0-Y0; }
	  if(Y<0||Y>28) if(Shotp->state==1)  { Shotp++; continue; }
	  for(m=0; m<4; m++)
	   { register int j;
	     p0 = Picptr[m][Y]+(X<<1) - 1;
	     p  = Pp + (m<<4) + (begin<<1);
	     bitp = Bp + (begin<<1);
	     for(j=begin; j<=end; j++)
	       { *p0&=*bitp++; *p0++|=*p++;
		 *p0&=*bitp++; *p0  |=*p++;
		 p0 += 71;
	       }
	   }
	}
       else
	{ int begin, end, Y, X=Shotp->X, dx = dxy==0?0:dxy<0?-1:1;
	  register int m;
	  if(X<0||X>=36)   { Shotp++; continue; }
	  if(shotY0<Y0) { begin=Y0-shotY0-1; Y=0; end=6; }
	  else if(shotY1>Y1)
	   { end=7-(shotY1-Y1);  Y=24+(shotY1-Y1); begin=0; }
	  else { begin=0; end=7; Y=shotY0-Y0; }
	  for(m=0; m<4; m++)
	   { register int j;
	     p0 = Picptr[m][Y]+(X<<1) + dx;
	     p  = Pp + (m<<4) + (begin<<1);    bitp = Bp;
	     if(X==0) for(j=begin; j<=end; j++)
		{ p0++; bitp++; p++; *p0&=*bitp++; *p0|=*p++; p0+=71; }
	     else if(X==35) for(j=begin; j<=end; j++)
		{ *p0&=*bitp++; *p0++|=*p++; bitp++; p++; p0 += 71; }
	     else for(j=begin; j<=end; j++)
		{ *p0&=*bitp++; *p0++|=*p++; *p0&=*bitp++; *p0|=*p++; p0+=71; }
	   }
	}
       if(Tank[Shotp->which].life)
	 { X = Tank[Shotp->which].X;  Y = Tank[Shotp->which].Y;
	   if(abs(X-Shotp->X)<3 && abs(Y-Shotp->Y)<3)
	      writeintank0(Tank+Shotp->which);
	 }
       Shotp ++;
     }
  }

void  writeintree()
 { for(Y=0; Y<2; Y++)  for(X=0; X<36; X++)
     { kind = Road[(Line<<1)+Y][X];
       if((kind&0x0f)!=4)  continue;
       for(k=0; k<4; k++)
	{ register int j;
	  if(fire==0) { bitp = Treebit[0]; p = Roadptr[4][k]; }
	  else
	   { switch(((X+Y)%5+(firekind)>>1)%4)
	      { case 0: if(fire<70) { bitp=Treebit[0]; p = Roadptr[4][k]; break; }
		case 2: if(fire<35) { bitp=Treebit[0]; p = Roadptr[4][k]; break; }
			bitp = Treebit[0];  p = Roadptr[5][k]; break;
		case 1: bitp = Treebit[1]; p = Roadptr[6][k]; break;
		case 3: bitp = Treebit[2]; p = Roadptr[7][k]; break;
	      }
	   }
	  p0 = Picptr[k][Y<<4] + (X<<1);
	  for(j=0; j<16; j++)
	   { *p0 &= *bitp++;  *p0++ |= *p++;
	     *p0 &= *bitp++;  *p0   |= *p++;
	     p0 += 71;
	   }
	}
     }
 }

void  writeonscreen()
 { char  far *ptr = (char far*)(0xa0000000L+641+(Line<<11)+(Line<<9));
   for(k=0; k<4; k++)
    { ptr0 = ptr;  pic = (char *)Picptr[k];
      outportb(0x3c4, 2);
      outportb(0x3c5, 0x01<<k);
     {register int i, j;
      for(j=0; j<4; j++)
       { Repall; Repall; Repall; Repall; Repall; Repall; Repall; Repall; }
     }
    }
   outportb(0x3c5, 0x0f);
 }
