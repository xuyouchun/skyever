#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <dos.h>
#include <time.h>
#include <alloc.h>

#define  TRUE   1
#define  FALSE  0
#define  EQ    -1

int MaxX, MayY, MaxColors;
int S=0, F=0, M=0, H=0;
int N=0, Y=0, R=0, _N=0, n, _N0=1;
int  Ifs=1, Ret=1, IfBes=1, BkIfs=1, IfBe=0, AutoSave=0;
int Sq=0, Outit=1, Step=20, Arc=1;
int  Backcolor=0;
int  Ms, Ss;
FILE  *SqFile;

char *buffer, *cp, S0[3], F0[3], M0[3];
struct time NowTime, NowTime0;
struct date NowDate, NowDate0;
struct Ti
 { int SS;
   int SF;
   int SM;
   int Bes;
   struct Ti *Next;
   struct Ti *Last;
 };
struct Ti *Pre, *Pre0, *Start, *St, *St0, *St1, *St2;
void  Clock(void);
void  ChangeTime(void);
void  GetDate(void);
void  GetTime(void);
void  ChangeDate(void);
void Circles(void);
void  WriteifChange(int Cord);
void  Shines(int Cord);
void  SetTime(void);
void  SetDate(void);
void  SetKb(int code);
void  BellKb(int Cord);
void  WriteScreen();
void  ReWrite(char *Cp);
void  Save();
void  LockKb(int Ch);
void  Rew(char *Cp);
void  OutKb(int Ch);
void  Order();
void  Boxline(int x1, int y1, int x2, int y2);
int   Cmp(struct Ti *MNM0, struct Ti *MM1);
char  MakeStr(char *buf, int a, int b, int c);

main()
 { int a, b, c, d;
   int GraphDriver=DETECT, GraphMode;
   initgraph(&GraphDriver, &GraphMode, "C:\\TC");
   if((SqFile=fopen("Sq", "r"))==NULL)
    { printf("Can't open file \"Sq\"!\n");
      exit(0);
    }
    fscanf(SqFile, "%d,%d,%d,%d,%d,%d,%d,%d\n",
	 &Sq, &Outit, &Ifs, &Step, &IfBes, &_N, &IfBes, &AutoSave);
    BkIfs = Ifs;
    buffer = (char *)malloc(15*sizeof(char));
    cp     = (char *)malloc(15*sizeof(char));

    Start = (struct Ti *)malloc(sizeof(struct Ti));
    fscanf(SqFile, "%d, %d, %d, %d\n", &a, &b, &c, &d);
    Start->SS=a; Start->SF=b; Start->SM=c; Start->Bes=d;

    Pre=(struct Ti *)malloc(sizeof(struct Ti));
    fscanf(SqFile, "%d, %d, %d, %d", &a, &b, &c, &d);
    Pre->SS=a; Pre->SF=b; Pre->SM=c; Pre->Bes=d;

    Start->Next = Pre;
    Pre->Last = Start;
    Start->Last = Pre;
    Pre->Next = Start;

    for(n=0; n<_N-2; n++)
     { Pre0 = Pre;
       Pre = (struct Ti *)malloc(sizeof(struct Ti));
       Pre->Next = Start;
       Start->Last = Pre;
       Pre->Last = Pre0;
       Pre0->Next = Pre;
       fscanf(SqFile, "%d, %d, %d, %d\n", &a, &b, &c, &d);
       Pre->SS=a; Pre->SF=b; Pre->SM=c; Pre->Bes=d;
     }
    Pre = Start;
    fclose(SqFile);

    /*********************************************/
    /***********/  Clock();  /********************/
    /*********************************************/

    if(AutoSave==1)  Save();

    closegraph();
 }


/************************* BEGIN ************************/
void  Clock()
 { int i, j, k;
   int Number = 0, _SM;
   unsigned char Ch;

   GetTime();  Ms = M; Ss = S;
   GetDate();
   WriteScreen();

   Order();
   setcolor(5);
   circle(320, 240, 180);
   setcolor(YELLOW);
   settextjustify(LEFT_TEXT,  CENTER_TEXT);
   settextstyle(0, 0, 1);
   outtextxy(0, 390, "Space:SET/BELL/LOCK/OUT");
   setcolor(YELLOW);
   outtextxy(491, 31, "Skyever");
   setcolor(GREEN);
   outtextxy(490, 30, "Skyever");
   ChangeDate();
   ChangeTime();
   St = (struct Ti *)malloc(sizeof(struct Ti));
   Pre = Start;
   for(i=0; i<_N; i++)
    { if(Cmp(Pre, St)==TRUE)  break;
      else Pre = Pre->Next;
    }
   Pre0 = Pre;
   free(St);
   WriteifChange(Outit);
   while(1)
    { Circles();
      if(Ret == 1)
       { GetTime();
	 if(M-Ms)  { Ms = M;  ChangeTime(); }
	 if(S-Ss)  { Ss = S;  GetDate();  ChangeDate(); }
       }
      else { SetTime(); Ms = M; }
      if(kbhit())
       { if((Ch=getch())==0)  Ch = getch();
	 if(Ch-32&&Ch-60&&Ch-9&&Ch-'-'&&Ch-'+'&&Outit==0 || Ch==27)  return;
	 if(IfBe==1)  { IfBe = 0;  nosound(); Ifs=BkIfs; }
	 switch(Ch)
	  { case 60: Save(); break;
	    case 32:
	    case '+': Outit = ++Outit==4?0:Outit;  WriteifChange(Outit); break;
	    case '-': Outit = --Outit==-1?3:Outit; WriteifChange(Outit); break;
	  }
	 switch(Outit)
	  { case 0: OutKb(Ch); break;
	    case 1: SetKb(Ch); break;
	    case 2: BellKb(Ch); break;
	    case 3: LockKb(Ch); break;
	  }
       }
     if(S==Pre->SS&&F==Pre->SF&&M==Pre->SM)
      { if(IfBes==1&&Pre->Bes==1)
	  { IfBe = 1;   Number = 0;  _SM=M-1==-1?59:M-1;  Ifs = 0; }
	St = (struct Ti*)malloc(sizeof(struct Ti));
	St->SS=S;  St->SF=F; St->SM=M;
	for(n=0; n<_N; n++)
	 { if(Cmp(Pre, St)==TRUE)  break;
	   else  Pre = Pre->Next;
	 }
	free(St);
	Pre0 = Pre;
	if(++_N0>_N)  _N0 = 1;
	if(Outit==2)  ReWrite("Now:");
      }
     if(_SM==M)  { IfBe=0; nosound(); Ifs=BkIfs; }
     if(IfBe==1&&++Number==1)  sound(1400);
     if(Number==75&&IfBe==1)   nosound();
     if(Number==125&&IfBe==1)  sound(1400);
     if(Number==200&&IfBe==1)  nosound();
     if(Number==450&&IfBe==1)  Number=0;
     Shines(Outit);
   }
 }


/*  Change time  */
void  ChangeTime()
 { float radius;
   int x1[3], y1[3], x2[3], y2[3], x3[3], y3[3];

   /********************************************************/
   if(S>=12)    radius=(float)((M_PI/2)-(M_PI/6)*(S-12)-(M_PI/(30*12))*F);
   else         radius=(float)((M_PI/2)-(M_PI/6)*S-(M_PI/(30*12))*F);
   x1[0] = 320-(int)(15*cos(radius)+0.5);  y1[0] = 240+(int)(15*sin(radius)+0.5);
   x2[0] = 320+(int)(100*cos(radius)+0.5); y2[0] = 240-(int)(100*sin(radius)+0.5);
   x3[0] = 320+(int)(80*cos(radius)+0.5);  y3[0] = 240-(int)(80*sin(radius)+0.5);
   radius=(float)((M_PI/2)-(M_PI/30)*F-(M_PI/1800)*M);
   x1[1] = 320-(int)(20*cos(radius)+0.5);  y1[1] = 240+(int)(20*sin(radius)+0.5);
   x2[1] = 320+(int)(140*cos(radius)+0.5); y2[1] = 240-(int)(120*sin(radius)+0.5);
   x3[1] = 320+(int)(120*cos(radius)+0.5); y3[1] = 240-(int)(120*sin(radius)+0.5);
   radius=(float)((M_PI/2)-(M_PI/30)*M);
   x1[2] = 320-25*cos(radius);  y1[2] = 240+25*sin(radius);
   x2[2] = 320+172*cos(radius); y2[2] = 240-172*sin(radius);
   x3[2] = 320+(int)(150*cos(radius)+0.5);  y3[2] = 240-(int)(150*sin(radius)+0.5);
   setfillstyle(0, 0);
   floodfill(320, 240, 5);
   /*******************************************************/
   /***** S *****/
   setcolor(YELLOW);
   setlinestyle(0, 1, 3);
   line(x1[0]-2, y1[0]-2, x2[0]-2, y2[0]-2);
   line(x1[0], y1[0], x2[0], y2[0]);
   line(x1[0]+2, y1[0]+2, x2[0]+2, y2[0]+2);
   /***** F *****/
   setcolor(LIGHTRED);
   line(x1[1], y1[1], x2[1], y2[1]);
   /***** M *****/
   setcolor(LIGHTGREEN);
   setlinestyle(0, 1, 1);
   line(x1[2], y1[2], x2[2], y2[2]);

   /*****************************************************/
   setcolor(LIGHTBLUE);
   circle(x3[0], y3[0], 6);
   setfillstyle(1, LIGHTBLUE);
   floodfill(x3[0], y3[0], LIGHTBLUE);
   setcolor(GREEN);
   circle(x3[1], y3[1], 6);
   setfillstyle(1, GREEN);
   floodfill(x3[1], y3[1], GREEN);
   setcolor(RED);
   circle(x3[2], y3[2], 6);
   setfillstyle(1, RED);
   floodfill(x3[2], y3[2], RED);
   /*****************************************************/

   /******** Circle and String *********/
   setcolor(1);
   circle(320, 240, 7);
   setfillstyle(1, 1);
   floodfill(320, 240, 1);
   setcolor(GREEN);
   circle(320, 240, 3);
   setfillstyle(1, GREEN);
   floodfill(320, 240, GREEN);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 4);
   setfillstyle(0, 0);
   setcolor(0);
   bar(0, 0, 180, 50);
   setcolor(YELLOW);
   if(S<10) { S0[0]='0'; S0[1]=(char)(S+48); }
   else   itoa(S, S0, 10);
   if(F<10) { F0[0]='0'; F0[1]=(char)(F+48); }
   else   itoa(F, F0, 10);
   if(M<10) { M0[0]='0'; M0[1]=(char)(M+48); }
   else   itoa(M, M0, 10);
   sprintf(buffer, "%s:%s:%s", S0, F0, M0);
   outtextxy(90, 30, buffer);
   settextstyle(0, 0, 1);
   sprintf(buffer, "%d", S==0?12:S>12?S-12:S);
   outtextxy(x3[0], y3[0], buffer);
   sprintf(buffer, "%d", F);
   outtextxy(x3[1], y3[1], buffer);
   sprintf(buffer, "%d", M);  outtextxy(x3[2], y3[2], buffer);
   if(Ifs==0)  return;
   if(M)  { sound(1000); delay(800); nosound(); }
   else
     { sound(1000);  delay(800);
       sound(1500);  delay(800);
       sound(2000);  delay(800);
     }
 }

void  Circles()
 { static Colors=0, Rad=6;
   setcolor(Colors++);
   circle(570, 420, Rad);
   if(Colors==16)  Colors=0;
   Rad+=6;
   if(Rad>55)  Rad=6;
 }

void  ChangeDate()
 { setfillstyle(0, 0);  bar(420, 450, 530, 470);
   sprintf(buffer, "%d", Sq-500);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   getdate(&NowDate);
   sprintf(buffer, "%d.%d.%d", N, Y, R);
   setcolor(YELLOW);
   settextstyle(1, 0, 2);
   outtextxy(480, 460, buffer);
 }

void  GetDate()
 { getdate(&NowDate);
   N = (int)NowDate.da_year;
   Y = (int)NowDate.da_mon;
   R = (int)NowDate.da_day;
 }

void  GetTime()
 { gettime(&NowTime);
   S = NowTime.ti_hour;
   F = NowTime.ti_min;
   M = NowTime.ti_sec;
 }

void  WriteifChange(int Code)
 { if(Arc==0)
    { St0 = Start;
      St =  Start->Next;
      St->Last = Pre0;
      Pre0->Next = St;
      St0->Next=Pre0;
      Pre0->Last=St;
    }
   Order();
   Arc=1;
   St = (struct Ti*)malloc(sizeof(struct Ti));
   St->SS = S;  St->SF = F;  St->SM = M;
   Pre = Start;
   for(n=0; n<_N; n++)
    { if(Cmp(Pre, St)==TRUE)  break;
      else  Pre = Pre->Next;
    }
   Pre0 = Pre;
   free(St);

   setfillstyle(0, 0);
   bar(0, 395, 190, 405);  bar(0, 405, 200, 445);  bar(0, 445, 300, 480);
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   settextstyle(0, 0, 1);
   switch(Code)
    { case 1: setcolor(LIGHTRED);
	      outtextxy(90, 472, "Press some key to SET...");
	      setcolor(GREEN);
	      outtextxy(0, 400, "Enter:Pause Del:No sound");
	      outtextxy(0, 409, "Up/Down:Change Second");
	      outtextxy(0, 418, "Alt+Up/Down:Change Minute");
	      outtextxy(0, 427, "Ctrl+Up/Down:Change Hour");
	      outtextxy(0, 436, "left/Right;Change Date");
	      outtextxy(0, 445, "Alt+Left/Right:Change Moon");
	      outtextxy(0, 436, "F2:Save");
	      outtextxy(0, 472, "Esc:Quit");
	      if(Ifs==0)
	       { setcolor(LIGHTRED);
		 outtextxy(0, 400, "Enter:Pause");
	       }
	      break;
     case 0: setcolor(LIGHTRED);
	     outtextxy(120, 472, "Press any key to OUT...");
	     OutKb(0);  break;
     case 3: setcolor(LIGHTRED);
	     outtextxy(90, 472, "The time now be Locked!");
	     LockKb(0);  break;
     case 2: setcolor(LIGHTRED);
	     outtextxy(90, 472, "Set the Bell time...");
	     ReWrite("Now:");
	     setcolor(GREEN);
	     if(IfBes==1)  outtextxy(0, 450, "F12:Bell ON  Tab:En/Unable");
	     else
	      { setcolor(LIGHTRED);
		outtextxy(0, 450, "Tab:En/Unable");
	      }
	     setcolor(LIGHTGREEN);
	     outtextxy(0, 460, "Insert/Delete/PgUp/PgDn/Up/Down/Enter");
	     setcolor(YELLOW);
	     outtextxy(0, 420, "Now");
	     settextstyle(1, 0, 1);
	     MakeStr(buffer, Pre->SS, Pre->SF, Pre->SM);
	     outtextxy(30, 430, buffer);
	     settextstyle(0, 0, 1);
	     if(Pre->Bes==0)  outtextxy(110, 430, "OFF");
	     else             outtextxy(110, 430, "ON");
	     break;
    }
 }

void  Shines(int Code)
 { static int Bj=0;
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   settextstyle(0, 0, 1);
   if(Bj==0)
    { setcolor(YELLOW);
      outtextxy(48, 390, "SET/BELL/LOCK/OUT");
    }
   else
    { setcolor(LIGHTRED);
      switch(Code)
       { case 0: outtextxy(160, 390, "OUT"); break;
	 case 1: outtextxy(48,  390, "SET"); break;
	 case 2: outtextxy(120, 390, "LOCK"); break;
	 case 3: outtextxy(80, 390,  "BELL"); break;
       }
    }
  Bj = 1-Bj;
 }

void  SetDate()
 { NowDate.da_year = N;
   NowDate.da_mon = Y;
   NowDate.da_day = R;
   setdate(&NowDate);
 }

void  SetTime()
 { NowTime.ti_min = F;
   NowTime.ti_hour = S;
   NowTime.ti_sec = M;
   settime(&NowTime);
 }

void  WriteScreen()
 { int x, y, k;
   float radius;
   setcolor(GREEN);  circle(320, 240, 200);  circle(320, 240, 199);
   setcolor(YELLOW); circle(320, 240, 197);  circle(320, 240, 196);
   setcolor(BLUE);   circle(320, 240, 195);  circle(320, 240, 194);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 4);

   setlinestyle(0, 0, 3);
   for(k=1; k<12; k++)
    { radius = (float)(M_PI/3-(M_PI/6)*(k-1));
      x = 220*cos(radius);
      y = 220*sin(radius);
      itoa(k, buffer, 10);
      setcolor(YELLOW);
      line(320+200*cos(radius), 240-200*sin(radius),
	   320+180*cos(radius), 240-180*sin(radius)) ;
      setcolor(RED);     outtextxy(320+x-2, 240-y+2, buffer);
      setcolor(YELLOW);  outtextxy(320+x, 240-y, buffer);
    }
  setlinestyle(0, 0, 1);
  setcolor(GREEN);
  for(k=0; k<60; k++)
   { if(!(k%5))  continue;
     radius=(float)((M_PI/30)*k);
     line(320+195*cos(radius), 240-195*sin(radius),
	  320+185*cos(radius), 240-185*sin(radius) );
   }
 }

void  SetKb(int Ch)
 { int n;
   switch(Ch)
    { case 13: if(!(Outit-2))  break;
	       Ret = 1-Ret;
	       if(Ret==1)  setcolor(GREEN);
	       else        setcolor(LIGHTRED);
	       outtextxy(0, 400, "Enter:Pause");
	       break;
      case 83: Ifs = 1-Ifs; nosound();  BkIfs = Ifs;
	       settextjustify(LEFT_TEXT, CENTER_TEXT);
	       settextstyle(0, 0, 1);
	       if(Ifs==0)  setcolor(LIGHTRED);
	       else        setcolor(GREEN);
	       outtextxy(96, 400, "Del:No sound");
	       break;
      case 80: if(Outit-2&&++M==60)  M=0;  SetTime(); ChangeTime(); break;
      case 72: if(Outit-2&&--M==-1)  M=59; SetTime(); ChangeTime(); break;
      case 160: if(Outit-2&&++F==60) F=0;  SetTime(); ChangeTime(); break;
      case 152: if(Outit-2&&--F==-1) F=59; SetTime(); ChangeTime(); break;
      case 141: if(Outit-2&&--S==-1) S=23; SetTime(); ChangeTime(); break;
      case 145: if(Outit-2&&++S==24) S=0;  SetTime(); ChangeTime(); break;
      case 115: if(Outit-2&&--N==-1) N=0;  SetDate(); ChangeDate(); break;
      case 116: if(Outit-2&&++N==10000) N=9999; SetDate(); ChangeDate(); break;
      case 155: if(Outit-2&&--Y==0)  Y=12; SetDate(); ChangeDate(); break;
      case 157: if(Outit-2&&++Y==13) Y=1;  SetDate(); ChangeDate(); break;
      case 75: if(Outit-2&&--R<1)
		if(Y==1||Y==3||Y==5||Y==7||Y==8||Y==10||Y==12)  R=31;
		else if(Y==2&&(N%4))  R=28;
		else if(Y==2&&!(N%4)) R=29;
		else R=30;
		SetDate();  ChangeDate();
		break;
      case 77:  if(!(Outit-2))  break;
		 { int Bz = 0;
		   if(++R>31 && (Y==1||Y==3||Y==5||Y==7||Y==8||Y==10||Y==12))
		     Bz = 1;
		   if(R>30 && (Y==4||Y==6||Y==9||Y==11))  Bz = 1;
		   if(R>28 && Y==2 && N%4)  Bz = 1;
		   if(R>29 && Y==2 && !(N%4))  Bz = 1;
		   if(Bz == 1)  R = 1;
		   SetDate();  ChangeDate();
		 }
		break;
      case 132: if(--Backcolor==1)  Backcolor=15; setbkcolor(Backcolor); break;
      case 118: if(++Backcolor==16) Backcolor=0;  setbkcolor(Backcolor); break;
    }
 }

void BellKb(int Ch)
 { switch(Ch)
   { case 134: IfBes = 1-IfBes;
	       setfillstyle(0, 0);  bar(0, 445, 100, 455);
	       settextstyle(0, 0, 1);
	       settextjustify(LEFT_TEXT, CENTER_TEXT);
	       if(IfBes==1)  { setcolor(GREEN); outtextxy(0, 450, "F12:Bell ON"); }
	       else          { setcolor(LIGHTRED); outtextxy(0, 450, "F12:BELL OFF"); }
	       break;
     case 73:  Pre0 = Pre0->Last;
	       _N0--;
	       if(_N0<1)  _N0 = N;
	       ReWrite("Up:");
	       break;
     case 81:  Pre0=Pre0->Next;
	       _N0++;
	       if(_N0>_N)  _N0=1;
	       ReWrite("Down:");
	       break;
     case 82:  if(Arc==0)  break;
	       _N++;  _N0=_N;
	       Pre0 = (struct Ti*)malloc(sizeof(struct Ti));
	       Pre0->SS=0;  Pre0->SF=0; Pre0->SM=0; Pre0->Bes=1;
	       setfillstyle(0, 0);
	       bar(0, 415, 200, 445);
	       setcolor(YELLOW);
	       outtextxy(0, 420, "Insert");
	       settextstyle(1, 0, 1);
	       settextjustify(LEFT_TEXT, CENTER_TEXT);
	       MakeStr(buffer, Pre0->SS, Pre0->SF, Pre0->SM);
	       outtextxy(30, 430, buffer);
	       settextstyle(0, 0, 1);
	       outtextxy(110, 430, "ON");
	       Arc = 0;
	       sprintf(buffer, "%d/%d", _N0, _N);
	       setcolor(LIGHTGREEN);
	       outtextxy(150, 430, buffer);
	       break;
     case 83:  if(Arc==0)  break;
	       if(_N<=3)   break;
	       St0 = Pre0->Last;  St = Pre0->Next;
	       St0->Next = St;    St->Last = St0;
	       if(Start==Pre0)  Start=St;
	       free(Pre0);
	       Pre0 = St;
	       _N --;
	       if(_N0>_N)  _N0 = _N;
	       ReWrite("Delete...");
	       break;
     case 80:  if(++Pre0->SM==60)  Pre0->SM=0;  Rew("Change...");  break;
     case 72:  if(--Pre0->SM==-1)  Pre0->SM=59; Rew("Change...");  break;
     case 160: if(++Pre0->SF==60)  Pre0->SF=0;  Rew("Change...");  break;
     case 152: if(--Pre0->SF==-1)  Pre0->SF=59; Rew("Change...");  break;
     case 141: if(--Pre0->SS==-1)  Pre0->SS=23; Rew("Change...");  break;
     case 145: if(++Pre0->SS==24)  Pre0->SS=0;  Rew("Change...");  break;
     case  9:  Pre0->Bes = 1-Pre0->Bes;  Rew("Change..."); break;
     case  13: if(Arc==0)
		{ St0=Start;
		  St = Start->Next;
		  St->Last = Pre0;
		  Pre0->Next = Pre0;
		  St0->Next = Pre0;
		  Pre0->Last=St0;
		  Arc = 1;
		}
	       Order();
		/***************************/
	       St=(struct Ti*)malloc(sizeof(struct Ti));
	       St->SS=S; St->SF=F; St->SM=M;
	       Pre=Start;
	       for(n=0; n<_N; n++)
		{ if(Cmp(Pre, St)==TRUE)  break;
		  else  Pre=Pre->Next;
		}
	       Pre0 = Pre;
	       free(St);
	       ReWrite("Now");
    }
 }

/******* End *******/
int Cmp(struct Ti *MM0, struct Ti *MM1)
 { if(MM0->SS>MM1->SS)  return TRUE;
   if(MM0->SS<MM1->SS)  return FALSE;
   if(MM0->SS==MM1->SS)
    { if(MM0->SF>MM1->SF)  return TRUE;
      if(MM0->SF<MM1->SF)  return FALSE;
      if(MM0->SF==MM1->SF)
       { if(MM0->SM>MM1->SM)  return TRUE;
	 if(MM0->SM<MM1->SM)  return FALSE;
	 if(MM0->SM==MM1->SM) return EQ;
       }
    }
 }

char  MakeStr(char *buf, int a, int b, int c)
 { char cc[3][3];
   if(a<10)  { cc[0][0]='0';cc[0][1]=(char)(a+48); cc[0][2]='\0'; }
   else      itoa(a, cc[0], 10);
   if(b<10)  { cc[1][0]='0';cc[1][1]=(char)(b+48); cc[1][2]='\0'; }
   else      itoa(b, cc[1], 10);
   if(c<10)  { cc[2][0]='0';cc[2][1]=(char)(c+48); cc[2][2]='\0'; }
   else      itoa(c, cc[1], 10);
   sprintf(buf, "%s:%s:%s", cc[0], cc[1], cc[2]);
   *(buf+8) = '\0';
 }

void  ReWrite(char *Cp)
 { setfillstyle(0, 0);
   bar(0, 395, 190, 405);  bar(0, 405, 200, 445);
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   settextstyle(0, 0, 1);
   setcolor(GREEN);
   St = Pre0->Last;
   MakeStr(cp, St->SS, St->SF, St->SM);
   sprintf(buffer, "Last Time: %s", cp);
   outtextxy(0, 400, buffer);
   St = Pre0->Next;
   MakeStr(cp, St->SS, St->SF, St->SM);
   sprintf(buffer, "Next Time: %s", cp);
   outtextxy(0, 410, buffer);
   setcolor(YELLOW);
   outtextxy(0, 420, Cp);
   settextstyle(1, 0, 1);
   MakeStr(buffer, Pre0->SS, Pre0->SF, Pre0->SM);
   outtextxy(30, 430, buffer);
   settextstyle(0, 0, 1);
   if(Pre0->Bes==0)    outtextxy(110, 430, "OFF");
   else                outtextxy(110, 430, "ON");
   sprintf(buffer, "%d/%d", _N0, _N);
   setcolor(LIGHTGREEN);
   outtextxy(150, 430, buffer);
 }

void  Save()
 { if((SqFile=fopen("Sq", "w"))==NULL)  return;
   fprintf(SqFile, "%d,%d,%d,%d,%d,%d,%d,%d\n",
	 Sq, Outit, Ifs, Step, IfBe, _N, IfBes, AutoSave);
   for(n=0; n<_N; n++)
    { fprintf(SqFile, "%d,%d,%d,%d\n", Pre->SS, Pre->SF, Pre->SM, Pre->Bes);
      Pre=Pre->Next;
    }
   fclose(SqFile);
 }

void  Rew(char *Cp)
 { setfillstyle(0, 0);
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   bar(0, 415, 200, 445);
   setcolor(YELLOW);
   outtextxy(0, 420, Cp);
   settextstyle(1, 0, 1);
   MakeStr(buffer, Pre0->SS, Pre0->SF, Pre0->SM);
   outtextxy(30, 430, buffer);
   settextstyle(0, 0, 1);
   if(Pre0->Bes==1)   outtextxy(110, 430, "ON");
   else               outtextxy(110, 430, "OFF");
   sprintf(buffer, "%d/%d", _N0, _N);
   setcolor(LIGHTGREEN);
   outtextxy(150, 430, buffer);
 }

void  LockKb(int Cord)
 { static int K=1, J=0;
   int i, j, k;
   if(Cord==83)  { Ifs=1-Ifs;  nosound();  BkIfs=Ifs;  return; }
   setcolor(LIGHTRED);
   for(i=0; i<16; i++)
    { Boxline(2+12*i, 410, 2+12*i+12, 450);
      setfillstyle(K, J);
      floodfill(8+12*i, 430, LIGHTRED);
      if(++J==16)  J=0;
   }
  if(++K==12)  K=1;
 }

void  Boxline(x1, y1, x2, y2)
 int x1, y1, x2, y2;
 { line(x1, y1, x2, y1);
   line(x2, y1, x2, y2);
   line(x2, y2, x1, y2);
   line(x1, y2, x1, y1);
 }

void  OutKb(int Ch)
 { settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 4);
   setcolor(RED);
   outtextxy(100, 410, "Skyever");
   setcolor(YELLOW);
   outtextxy(102, 412, "Skyever");
   settextstyle(0, 0, 1);
   setcolor(GREEN);
   outtextxy(100, 445, "2000,.12.31 by XuYouChun");
   setcolor(LIGHTGREEN);
   outtextxy(150, 457, "Best wish to you...");
   if(Ch==9)  AutoSave=1-AutoSave;
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   if(AutoSave==1)  setcolor(GREEN);
   else             setcolor(LIGHTRED);
   outtextxy(0, 472, "Tab:Auto save");
 }

void  Order()
 { int i, j, k;
   St1 = Start->Last;
   for(j=0; j<_N-1; j++)
    { St = Start;
      for(i=0; i<_N-j; i++)
       { if(Cmp(St, St1)==TRUE)
	  { k=St->SS; St->SS=St1->SS; St1->SS=k;
	    k=St->SF; St->SF=St1->SF; St1->SF=k;
	    k=St->SM; St->SM=St1->SM; St1->SM=k;
	  }
	 St = St->Next;
       }
     St1 = St1->Last;
    }
 }



