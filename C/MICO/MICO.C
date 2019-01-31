#include <graphics.h>
#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <alloc.h>
#include "mouse.h"
#include "key.h"

FILE  *fp;
int   wide, high, buildflag;
void  write_screen(), save_ico(), barptr(), kind_ptr();
void  write_image(), write_ico(), bkcodo(int code), saveimage();
char  shines(int x, int y);
int  input_num(int x, int y, float *N, int n, int point);
int Inword(int x, int y, char *S, int Num);
#define  redo()  bkcodo(1)
#define  undo()  bkcodo(-1)
int   mouse_click();
char  F(long offset);
void  mouse_move(), barxy(int x1, int y1, int x2, int y2);
#define  clear_bar()  barxy(0,0,0,0)
#define  X    120
#define  Y    50
#define  BEGIN   while(1){
#define  WEND    }
int   XX, YY;
char  far  *ptr = (char far*)0xa0000000+Y*(640/8)+(X/8);
#define  dth  5
int   kind, kind0 = 1, forcolor=15, bkcolor=0;
char  far *p[10];
int   pptr, nump, maxp;
char  filename[20];

main(int argc, char **argv)
 { int graphdriver = DETECT, graphmode;
   int code, click = 0, write = 0;
   int i, j, k, m, n; int _X, _Y, BX=0, BY=0;
   char *pt;
   if(argc != 2)
	{ printf("Please input file name: ");
	  scanf("%s", filename);
	}
   else strcpy(filename, argv[1]);
   fp = fopen(filename, "rb+");
   if(fp == NULL)
	 { fp = fopen(filename, "wb");
	   if(fp==NULL)
		{ printf("Can't open file \"%s\" !", filename);
		  fcloseall();  exit(1);
		}
	   do{  printf("Please input wide of the image: ");
			scanf("%d", &wide);
		 }  while(wide>256||wide<8);
	   do{  printf("Please input high of the image: ");
			scanf("%d", &high);
		 }  while(wide>256||wide<0);
	   wide -= wide%8;
	 }
   else
	 { wide = (unsigned)fgetc(fp);
	   high = (unsigned)fgetc(fp);
	   buildflag = 1;
	 }
   XX = (640-wide*dth)/2;
   YY = (480-high*dth)/2;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(triplex_font);

   initgraph(&graphdriver, &graphmode, "C:\\TC");
   write_screen();
   if(buildflag==1)  { write_image();  write_ico(); }
   initmouse();
   mouse_on();
   for(k=0; k<10; k++)
	 p[k] = (char far*)farmalloc(imagesize(X, Y, X+wide-1, Y+high-1));
   saveimage();
BEGIN
   kind_ptr();
   while(getmousebutton()!=LEFTBUTTON&&!kbhit())
	 {  if(click==1 && write==1)  saveimage();
		click = 0; write = 0; mouse_move();
		if(getmousebutton()==RIGHTBUTTON)
		  { kind = kind==0?kind0:0;   kind_ptr();
			while(getmousebutton()!=NOBUTTON);
		  }
	 }
   if(kbhit())
	 { switch(getch())
		{ case 9 : j = bkcolor;  bkcolor = forcolor; forcolor = j;
				   barptr();     break;
		  case 60: save_ico(); break;
		  case 'Y': case 'y': case 25: redo(); break;
		  case 'Z': case 'z': case 26: undo(); break;
		  case 'p': case 'P':  kind = kind0 = 0; kind_ptr(); break;
		  case 'o': case 'O':  kind = kind0 = 1; kind_ptr(); break;
		  case 'r': case 'R':  kind = kind0 = 2; kind_ptr(); break;
		}
	 }
   else
	{ getmouse_xy(&_X, &_Y);
	  if(_X==BX&&_Y==BY&&click==1)  continue;
	  BX = _X;  BY = _Y;  click = 1;
	  code = mouse_click();
	  if(code>=0&&code<=15)
		{ switch(kind)
			{ case 0: case 2:
					  if(bioskey(2)&0x08) bkcolor = code;
					  else                forcolor = code;
					  barptr(); break;
			  case 1: setfillstyle(1, bkcolor=code);
					  for(j=0; j<high; j++)  for(i=0; i<wide; i++)
						bar(XX+i*dth+1, YY+j*dth+1,
								XX+(i+1)*dth-1, YY+(j+1)*dth-1);
					  write_ico(); barptr(); saveimage(); break;
			}
		}
	  else if(code>=100)
	   { int  MX, MY, flag = 0;
		 switch(kind)
		  { case 0: i = (code-100)%wide;  j = (code-100)/wide;
				  if(bioskey(2)&0x04)
				   { if(bioskey(2)&0x08)
						{ bkcolor = getpixel(XX+i*dth+2, YY+j*dth+2); }
					 else
						{ forcolor = getpixel(XX+i*dth+2, YY+j*dth+2); }
					 barptr(); break;
				   }
				 setfillstyle(1, bioskey(2)&0x08?bkcolor:forcolor);
				 mouse_off();
				 bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1, YY+(j+1)*dth-1);
				 write_ico();
				 mouse_on();  write = 1;
				 break;
            case 2: setwritemode(XOR_PUT);
				 setlinestyle(1, 0, 1);
				 setcolor(YELLOW);
				 MX = BX;  MY = BY;
				 do
				  { getmouse_xy(&_X, &_Y);
					if(kbhit()&&getch()==27)  { flag = 1;  break; }
					if(BX==_X&&BY==_Y)  continue;
					mouse_off();
					rectangle(MX, MY, BX, BY);
					rectangle(MX, MY, _X, _Y);
					mouse_on();
					BX = _X;  BY = _Y;
				  } while(getmousebutton()==LEFTBUTTON);
				 mouse_off(); rectangle(MX, MY, BX, BY); mouse_on();
				 setwritemode(COPY_PUT);  setlinestyle(0, 0, 1);
				 if(flag == 1)  break;
				 setfillstyle(1, bioskey(2)&0x08?bkcolor:forcolor);
				 mouse_off();
				 for(j=0; j<high; j++)  for(i=0; i<wide; i++)
				  { int x, y, maxx, minx, maxy, miny;
					x = (XX+i*dth+1 + XX+(i+1)*dth-1)/2;
					y = (YY+j*dth+1 + YY+(j+1)*dth-1)/2;
					maxx = MX>BX?MX:BX;  minx = MX<BX?MX:BX;
					maxy = MY>BY?MY:BY;  miny = MY<BY?MY:BY;
					if(x<=maxx&&x>=minx && y<maxy&&y>miny)
					  bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
							YY+(j+1)*dth-1);
				  }  write_ico();  mouse_on(); saveimage();
				 break;
		  }
	   }
	  else
		{ while(getmousebutton()!=NOBUTTON);
		  switch(code)
		   { case 20: case 21: case 22:
					  kind0 = kind = code-20; break;
			 case 23: mouse_off();
					  setfillstyle(1, bioskey(2)&0x08?forcolor:bkcolor);
					  for(j=0; j<high; j++)  for(i=0; i<wide; i++)
						if(getpixel(XX+i*dth+2, YY+j*dth+2) ==
							  (bioskey(2)&0x08?bkcolor:forcolor) )
							bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
									  YY+(j+1)*dth-1);
					  write_ico();  mouse_on();  saveimage(); break;
			 case 24: mouse_off();
					  for(j=0; j<high; j++)  for(i=0; i<wide; i++)
						{ setfillstyle(1, 15-getpixel(XX+i*dth+2, YY+j*dth+2) );
						  bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
									  YY+(j+1)*dth-1);
						}
					  write_ico();  mouse_on();  saveimage(); break;
			 case 25: pt = (char *)malloc(k=wide*high);
					  if(pt==NULL)  break;
					  for(j=0; j<high; j++)  for(i=0; i<wide; i++)
						pt[j*wide+i] = getpixel(XX+i*dth+2, YY+j*dth+2);
					  for(j=0; j<high; j++)  for(i=0; i<wide; i++)
						{ setfillstyle(1, i*high+j>k?BLACK:pt[i*high+j]);
						  bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
									  YY+(j+1)*dth-1);
						}
					  write_ico();  saveimage();   free(pt);
					  break;
			 case 26: if(bioskey(2)&0x08)
					  for(i=0; i<wide; i++)  for(j=0; j<high/2; j++)
					   { m = getpixel(XX+i*dth+2, YY+j*dth+2);
						 n = getpixel(XX+i*dth+2, YY+(high-j-1)*dth+2);
						 setfillstyle(1, n);
						 bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
									  YY+(j+1)*dth-1);
						 setfillstyle(1, m);
						 bar(XX+i*dth+1, YY+(high-j-1)*dth+1, XX+(i+1)*dth-1,
									  YY+(wide-j)*dth-1);
					   }
					  else
					  for(j=0; j<high; j++)  for(i=0; i<wide/2; i++)
					   { m = getpixel(XX+i*dth+2, YY+j*dth+2);
						 n = getpixel(XX+(wide-i-1)*dth+2, YY+j*dth+2);
						 setfillstyle(1, n);
						 bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1,
									  YY+(j+1)*dth-1);
						 setfillstyle(1, m);
						 bar(XX+(wide-i-1)*dth+1, YY+j*dth+1, XX+(wide-i)*dth-1,
									  YY+(j+1)*dth-1);
					   }
					  write_ico();  saveimage();  free(pt);
					  break;
			 case 27: write_image();  write_ico();  saveimage(); break;
			 case 28: if(bioskey(2)&0x08)  redo();
					  else                 undo();
					  break;
			 case 29: save_ico(); break;
			 case 30: goto end;
		   }
		}
	}
WEND
end:
   fcloseall();
   for(k=0; k<10; k++)  farfree(p[k]);
   closegraph();
 }

void  write_screen()
 { int i, j;
   char  str[11][20] =
	{ "PENCIL", "OVERWRITE", "RECTANGLE", "CHANGE COLOR",
	  "COLOR ROTAL", "IMAGE ROTAL", "MIRROR", "RESET",
	  "UNDO/REDO", "SAVE", "QUIT"
	} ;
   char s[30];
   setfillstyle(1, BLUE);
   bar(0, 0, 639, 479);
   setcolor(YELLOW);
   rectangle(20-1, 30-1, 20+25+1, 30+16*25);
   for(j=1; j<16; j++)  line(20, 30+j*25, 20+25, 30+j*25);
   for(j=0; j<16; j++)
	 { setfillstyle(1, j);
	   bar(20+1, 30+25*j+2, 20+25-1, 30+25*(j+1)-2);
	 }
   rectangle(XX-2, YY-2, XX+wide*dth+2, YY+high*dth+2);
   setfillstyle(1, BLACK);
   bar(XX, YY, XX+wide*dth, YY+high*dth);
   bar(X, Y, X+wide-1, Y+high-1);
   setcolor(LIGHTGRAY);
   settextstyle(0, 0, 1);
   for(j=0; j<11; j++)  outtextxy(530, 100+j*15, str[j]);
   settextstyle(1, 0, 4);
   outtextxy(500, 20, "Skyever");
   setcolor(RED);
   outtextxy(502, 22, "Skyever");
   barptr();  kind_ptr();
   settextstyle(1, 0, 3);
   sprintf(s, "Wide : %d   High : %d",  wide, high);
   setcolor(RED);
   outtextxy(200, 50, s);
   setcolor(YELLOW);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(320, 20, filename);
   settextjustify(LEFT_TEXT, TOP_TEXT);
 }

void  write_image()
 { int i, j, k;  int color; char ch;
   int S = high*(wide/8);
   for(j=0; j<high; j++)
	for(i=0; i<wide; i++)
	 { color = 0;
	   for(k=0; k<4; k++)
		{ ch = F(j*(wide/8)+i/8 + S*k);
		  if(ch&(0x80>>i%8))  color |= (0x01<<k);
		}
	   setfillstyle(1, color);
	   bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1, YY+(j+1)*dth-1);
	}
 }

void  write_ico()
  { int i, j, color;
	mouse_off();
	for(j=0; j<high; j++)
	 for(i=0; i<wide; i++)
		putpixel(X+i, Y+j, getpixel(XX+i*dth+2, YY+j*dth+2));
	mouse_on();
  }

char F(long offset)
 { fseek(fp, offset+2L, SEEK_SET);
   return  fgetc(fp);
 }

void  save_ico()
 { int i, j, k;
   rewind(fp);
   fputc((char)wide, fp);
   fputc((char)high, fp);
   for(k=0; k<4; k++)
	{ outport(0x3ce, 4);
	  outport(0x3cf, k);
	  for(j=0; j<high; j++)
		for(i=0; i<wide/8; i++)
		  fputc( *(ptr+j*(640/8)+i), fp);
	}
   outportb(0x3cf, 0);
   fprintf(fp, "wide:%d  high:%d", wide, high);
 }

int  mouse_click()
 { int i, j;   int _X, _Y;
   getmouse_xy(&_X, &_Y);
   if(imouse_inbox(20, 30, 20+25, 30+16*25))
	 return (_Y-30)/25;
   if(imouse_inbox(XX, YY, XX+wide*dth, YY+high*dth))
	 return ((_Y-YY)/dth)*wide+(_X-XX)/dth + 100;
   if(imouse_inbox(525, 95, 630, 95+11*15))
	 return (_Y-95)/15+20;
   return -1;
 }

void  barptr()
 { char  colorstr[16][20] =
	 { "BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "LIGHTGRAY",
	   "DARKGRAY", "LIGHTBLUE", "LIGHTGREEN", "LIGHTCYAN", "LIGHTRED",
	   "LIGHTMAGENTA", "YELLOW", "WHITE"
	 };
   setcolor(CYAN);
   settextstyle(1, 0, 3);
   mouse_off();
   outtextxy(56, 410, "FrontColor:");
   setfillstyle(1, forcolor);
   bar(190, 415, 250, 435);
   rectangle(189, 414, 251, 436);
   setfillstyle(1, BLUE);
   bar(255, 410, 270+88, 410+30);
   outtextxy(62, 440, "BackColor:");
   setfillstyle(1, bkcolor);
   bar(190, 445, 250, 465);
   rectangle(189, 444, 251, 466);
   setfillstyle(1, BLUE);
   bar(255, 440, 270+88, 440+30);
   settextstyle(0, 0, 1);
   outtextxy(260, 420, colorstr[forcolor]);
   outtextxy(260, 450, colorstr[bkcolor]);
   mouse_on();
 }

void  mouse_move()
 { int k;
   static int bk = -1;
   for(k=0; k<11; k++)
	 if(imouse_inbox(525, 95+k*15, 630, 95+(k+1)*15))
	   { barxy(525, 95+k*15, 630, 95+(k+1)*15); break; }
   if(k==11)  clear_bar();
   if(bk == k)  return;
   setfillstyle(1, BLUE);
   bar(360, 400, 639, 479);
   settextstyle(0, 0, 1);
   setcolor(YELLOW);
   switch(k)
	{ case 0: case 11:
			  outtextxy(360, 410, "Click in color bar: ");
			  outtextxy(360, 418, "           - Select front color");
			  outtextxy(360, 430, "Alt + Click in color bar:");
			  outtextxy(360, 438, "           - Select back color ");
			  outtextxy(360, 450, "Tab: Exchange back and front color");
			  outtextxy(360, 460, "F2 : Save");
			  break;
	  case 1: outtextxy(360, 420, "Click in color bar:");
			  outtextxy(360, 430, "           - Reset all color");
			  break;
	  case 2: outtextxy(360, 420, "Click: Fill with front color");
			  outtextxy(360, 430, "Alt+Click: Fill with back color");
			  break;
	  case 3: outtextxy(360, 420, "Exchange front color with back color");
			  outtextxy(360, 435, "Alt+Click:");
			  outtextxy(360, 443, "Exchange back color with front color");
			  break;
	  case 4: outtextxy(360, 420, "Exchange color with its opposite");
			  break;
	  case 5: outtextxy(360, 420, "Rotal image");
			  break;
	  case 6: outtextxy(360, 420, "Mirror image form left to right");
			  outtextxy(360, 430, "Alt + click:");
			  outtextxy(360, 438, "Mirror image form up to bottom");
			  break;
	  case 7: outtextxy(360, 430, "Reset image on file in disk");
			  break;
	  case 8: outtextxy(360, 420, "Click/Ctrl+Z/Z: Undo");
			  outtextxy(360, 435, "Alt+Click/Ctrl+Y/Y: Redo");
			  break;
	  case 9: outtextxy(360, 430, "Click : Save file now");
			  break;
	  case 10: outtextxy(360, 430, "Quit without saving");
			  break;
	}
   bk = k;
 }

void  barxy(int x1, int y1, int x2, int y2)
 { static int bx1, by1, bx2, by2;
   if(bx1==x1&&bx2==x2&&by1==y1&&by2==y2)  return;
   setwritemode(XOR_PUT);
   setlinestyle(1, 0, 1);
   setcolor(LIGHTCYAN^BLUE);
   mouse_off();
   rectangle(bx1, by1, bx2, by2);
   rectangle(x1, y1, x2, y2);
   mouse_on();
   setlinestyle(0, 0, 1);
   setwritemode(COPY_PUT);
   bx1 = x1;  by1 = y1;  bx2 = x2;  by2 = y2;
 }

void  kind_ptr()
 { static int bkind = -1;
   if(bkind == kind)  return;
   setcolor(BLUE);
   settextstyle(0, 0, 1);
   outtextxy(510, 100+bkind*15, "->");
   setcolor(RED);
   outtextxy(510, 100+kind*15, "->");
   bkind = kind;
 }

void  bkcodo(int code)
 { int i, j;
   if(nump+code>0 || nump+code<-maxp)  return;
   nump += code;   pptr += code;
   if(pptr<0)  pptr = 9;
   if(pptr>9)  pptr = 0;
   mouse_off();
   putimage(X, Y, p[pptr], COPY_PUT);
   for(j=0; j<high; j++)  for(i=0; i<wide; i++)
	{ setfillstyle(1, getpixel(X+i, Y+j) );
	  bar(XX+i*dth+1, YY+j*dth+1, XX+(i+1)*dth-1, YY+(j+1)*dth-1);
	}
   mouse_on();
 }

void  saveimage()
 { pptr ++;  if(pptr>9)  pptr = 0;
   maxp += nump;
   mouse_off();
   getimage(X, Y, X+wide-1, Y+high-1, p[pptr]);
   mouse_on();
   maxp ++;    if(maxp>10)  maxp = 10;
   nump = 0;
 }


