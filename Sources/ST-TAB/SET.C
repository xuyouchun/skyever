#include <alloc.h>
#include <stdio.h>
#include <graphics.h>
#include <bios.h>
#include <math.h>
#include "extern.h"
#include "set.h"
#include "mouse.h"
#include "hz.h"
#include "tools.h"
#include "key.h"
#include "click.h"
#include "st-tab.h"
int  bx1, bx2, by1, by2;
int  B=5, E=5;
int  maxfilenum = 10;

ST_MSG  *add_point(ST_MSG far *P)
 {  ST_MSG  *ptr ;  int k;
    ptr = (ST_MSG *)malloc(sizeof(ST_MSG));
    if(ptr==NULL)  return NULL;
    ptr->Next = P->Next;  P->Next = ptr;
    ptr->Xh = 0;  ptr->Name[0] = '\0';
    ptr->Dy = (float*)malloc(DYnum*sizeof(float));
    ptr->Zy = (float*)malloc(ZYnum*sizeof(float));
    ptr->Fj = (float*)malloc(FJnum*sizeof(float));
    for(k=0; k<DYnum; k++)    ptr->Dy[k] = 0;
    for(k=0; k<ZYnum; k++)    ptr->Zy[k] = 0;
    for(k=0; k<FJnum; k++)    ptr->Fj[k] = 0;
    return ptr;
 }

void  free_all(int code)
 { ST_MSG *ptr = Home->Next;
   while(ptr!=NULL)
     { free(ptr->Dy);  free(ptr->Zy);  free(ptr->Fj);
       free(ptr);
       ptr = ptr->Next;
     }
   Home->Next = NULL;  End = Home;  STnum = 0;
   if(DYnum==0&&ZYnum==0&&FJnum==0)  return;
   if(code==0)  {  DYnum = ZYnum = FJnum = 0;  }
 }

int  mouse_move(int code)
 { int k ;  int X, Y;
   static bmousex=320, bmousey=240;
   getmouse_xy(&X, &Y);
   if(X==bmousex&&Y==bmousey&&code==0)  return 0;
   bmousex = X;  bmousey = Y;
   if(help==1)
     { test_in(250, 425, 390, 446);
       if(!imouse_inbox(250, 425, 390, 446))  clear_bar();
       return 0;
     }
   if(kind==0||kind==1)
      { float len; int B;
	for(k=0; k<3; k++)  test_inbox(130+k*180, 27, 235+k*180, 46);
	if(STnum>15)
	  { len = (float)(Y2-Y1-140)/(STnum-15+1);
	    B = Begin+15>STnum?STnum-15:Begin;
	    test_inbox(X2-8, Y1+70+B*len, X2-3, Y1+70+(B+1)*len);
	  }
	if(MAXnum>MAX)
	  { B = Beginx[msg]+MAX>MAXnum?MAXnum-MAX:Beginx[msg];
	    len = (float)(X2-X1-140)/(MAXnum-MAX+1);
	    test_inbox(X1+70+B*len, Y1+3,X1+70+(B+1)*len, Y1+8);
	  }
	test_inbox(X1+12, Y1+2, X1+60, Y1+9);
	test_inbox(X2-60, Y1+2, X2-12, Y1+9);
	test_inbox(X2-9, Y1+12, X2-2, Y1+60);
	test_inbox(X2-9, Y2-60, X2-2, Y2-12);
      }
   if(maxfilenum && namenum)  test_inbox(2, 31, 95, 51);
   for(k=0; k<2; k++)  test_inbox(2, 61+20*k, 95, 81+20*k);
   for(k=0; k<5; k++)  test_inbox(2, 111+20*k, 95, 131+20*k);
   for(k=0; k<4; k++)  test_inbox(2, 223+20*k, 95, 243+20*k);
   for(k=0; k<4; k++)  test_inbox(2, 317+20*k, 95, 337+20*k);
   if(kind==3||kind==2)
     { test_inbox(170, 25, 270, 47);  test_inbox(425, 25, 525, 47); }
   if(kind==0)
     { for(k=0; k<6; k++)
	test_inbox(100+80*k+8*(k==0)-8*(k==5), 425, 100+80*k+50+8*(k==0)-8*(k==5), 446);
     }
   else if(kind==1)
     {  test_inbox(100, 425, 245, 446);
	for(k=0; k<2; k++)  { test_inbox(260+125*k, 425, 260+125*k+70, 446); }
     }
   else if(kind==2)
     for(k=0; k<2; k++)  { test_inbox(252+95*k, 425, 252+95*k+70, 446); }
   else if(kind==3)
     for(k=0; k<2; k++)  { test_inbox(252+95*k, 425, 252+95*k+70, 446); }
   if(kind==3&&Home->Next)
     { if((k=scroll_move())>=0)
	 { int x1 = X1+20+38, x2 = X2-21-80-(k==4?60:40), num=0, i, y1, H=0;
	   float len = (float)(x2-x1)/(exist[k]-(k==4?5:8)+1);
	   for(i=0;i<5;i++)  if(exist[i])  num++;
	   for(i=0;i<k;i++)  if(exist[i])  H++;
	   y1 = (Y2-Y1-num*60)/2+Y1+15;
	   test_inbox(x1+bsee[k]*len,y1+H*60+30,x1+(bsee[k]+1)*len,y1+H*60+40);
	 }
     }
   if(kind==1||kind==0)   if(_mouse_move()==1)  return 1;
   if(kind==0&&bx2>X1+10&&bx2<X2-10&&by2>Y1+45&&by2<Y2-5)  return 1;
   clear_bar();  return 0;
 }

int  _mouse_move()
 { int j, i, num=STnum-Begin+1 - kind ;
   int N = MAXnum-Beginx[msg] - kind;
   int M = msg==1?39:msg==0?61:86;
   if(num>=15)  num=15;
   test_inbox(X1+11, Y1+11, X1+38, Y1+45);
   test_inbox(X1+42, Y1+11, X1+89, Y1+45);
   if(bioskey(2)&0x1f)   return -1;
   for(i=0; i<N+1; i++)  test_inbox(X1+93+M*i, Y1+11, X1+89+M*(i+1), Y1+45);
   if(kind==0||kind==1)
     for(j=0; j<num; j++)
	if(test_in(X1+42, Y1+49+j*21, X1+89, Y1+45+(j+1)*21)==1)
	     { nowx = 1; nowy = j ; return 1; }
  if(kind==0)
    for(j=0; j<num; j++)
     { if(test_in(X1+11, Y1+49+j*21, X1+38, Y1+45+(j+1)*21)==1)
	     { nowx = 0; nowy = j ; return 1; }
       for(i=0; i<N; i++)
	  if(test_in(X1+93+M*i, Y1+49+21*j, X1+89+M*(i+1), Y1+45+21*(j+1))==1)
	     { nowx = i+2; nowy = j ; return 0; }
     }
    return 0;
 }

int  mouse_click()
  { int k, i, j, num=STnum-Begin+1-kind;
    int N = MAXnum-Beginx[msg] - kind;
    int M = msg==1?39:msg==0?61:86;
    if(N>MAX)  N = MAX;
    if(num>=15)  num=15;
    if(help==1)
     { if(imouse_inbox(250, 425, 390, 446))  return 0; else  return 1; }
    if(kind==0||kind==1)
      { float len;  int B;
	for(k=0; k<3; k++)
	 if(imouse_inbox(130+k*180, 27, 235+k*180, 46))  return  k;
	if(STnum>15)
	  { len = (float)(Y2-Y1-140)/(STnum-15+1);
	    B = Begin+15>STnum?STnum-15:Begin;
	    if(imouse_inbox(X2-8, Y1+70+B*len, X2-3, Y1+70+(B+1)*len))
		return 801;
	    if(imouse_inbox(X2-8, Y1+70, X2-3, Y1+70+B*len))  return 805;
	    if(imouse_inbox(X2-8, Y1+70+(B+1)*len, X2-3, Y2-70)) return 806;
	  }
	if(MAXnum>MAX)
	  { B = Beginx[msg]+MAX>MAXnum?MAXnum-MAX:Beginx[msg];
	    len = (float)(X2-X1-140)/(MAXnum-MAX+1);
	    if(imouse_inbox(X1+70+B*len, Y1+3, X1+70+(B+1)*len, Y1+8))
		return 802;
	    if(imouse_inbox(X1+70, Y1+3, X1+70+B*len, Y1+8))  return 807;
	    if(imouse_inbox(X1+70+(B+1)*len, Y1+3, X2-70, Y1+8))  return 808;
	  }
	if(imouse_inbox(X1+12, Y1+2, X1+60, Y1+9))  return 830;
	if(imouse_inbox(X2-60, Y1+2, X2-12, Y1+9))  return 831;
	if(imouse_inbox(X2-9, Y1+12, X2-2, Y1+60))  return 832;
	if(imouse_inbox(X2-9, Y2-60, X2-2, Y2-12))  return 833;
      }
    if(imouse_inbox(2, 31, 95, 51))  return 5;
    for(k=0; k<2; k++)
       if(imouse_inbox(2, 61+20*k, 95, 81+20*k))    return 10+k;
    for(k=0; k<5; k++)
       if(imouse_inbox(2, 111+20*k, 95, 131+20*k))  return 20+k;
    for(k=0; k<4; k++)
       if(imouse_inbox(2, 223+20*k, 95, 243+20*k))  return 30+k;
    for(k=0; k<4; k++)
       if(imouse_inbox(2, 317+20*k, 95, 337+20*k))  return 40+k;
    if(kind==3)
      { if(imouse_inbox(170, 25, 270, 47))   return 90;
	if(imouse_inbox(425, 25, 525, 47))   return 91;
      }
    if(kind==3&&Home->Next)
     { if((k=scroll_move())>=0)
	 { int x1 = X1+20+38, x2 = X2-21-80-(k==4?60:40), num=0, i, y1, H=0;
	   float len = (float)(x2-x1)/(exist[k]-(k==4?5:8)+1);
	   for(i=0;i<5;i++)  if(exist[i])  num++;
	   for(i=0;i<k;i++)  if(exist[i])  H++;
	   y1 = (Y2-Y1-num*60)/2+Y1+15;
	   if(imouse_inbox(x1+bsee[k]*len,y1+H*60+30,x1+(bsee[k]+1)*len,y1+H*60+40))
	      return  850+k;
	   if(imouse_inbox(x1,y1+H*60+30,x1+bsee[k]*len,y1+H*60+40))  return 860+k;
	   if(imouse_inbox(x1+(bsee[k]+1)*len,y1+H*60+30,x2,y1+H*60+40))  return  870+k;
	 }
	else  if(imouse_inbox(X1,Y1,X2,Y2))  return 880;
      }
    if(kind==2)
      { if(imouse_inbox(170, 25, 270, 47))   return 95;
	if(imouse_inbox(425, 25, 525, 47))   return 96;
	if(imouse_inbox(X1,Y1,X2,Y2))  return 881;
      }
    if(kind==0||kind==1)
      { if(imouse_inbox(X1+11, Y1+11, X1+38, Y1+45))    return 101;
	if(imouse_inbox(X1+42, Y1+11, X1+89, Y1+45))    return 102;
	for(i=0; i<(N+1>=MAX?MAX:N+1); i++)
	  if(imouse_inbox(X1+93+M*i, Y1+11, X1+89+M*(i+1), Y1+45))
	    return  110+i;
       for(j=0; j<num; j++)
	  if(imouse_inbox(X1+42, Y1+49+j*21, X1+89, Y1+45+(j+1)*21))  return 400+j;
       if(kind==0)
	for(j=0; j<num; j++)
	 { if(imouse_inbox(X1+11, Y1+49+j*21, X1+38, Y1+45+(j+1)*21))  return 200+j;
	   for(i=0; i<N; i++)
	       if(imouse_inbox(X1+93+M*i, Y1+49+21*j, X1+89+M*(i+1), Y1+45+21*(j+1)) )
		  return 1000+j*100+i;
	 }
       }
   if(kind==0)
     { for(k=0; k<6; k++)
	if(imouse_inbox(100+80*k+8*(k==0)-8*(k==5), 425,
		100+80*k+50+8*(k==0)-8*(k==5), 446))
	   return 50+k;
     }
   else if(kind==1)
     { if(imouse_inbox(100, 425, 245, 446))   return 7;
       for(k=0; k<2; k++)
	 if(imouse_inbox(260+125*k, 425, 260+125*k+70, 446)) return 60+k;
     }
   else if(kind==2)
     { for(k=0; k<2; k++)
	 if(imouse_inbox(252+95*k, 425, 252+95*k+70, 446))   return 70+k;
     }
   else if(kind==3)
     { for(k=0; k<2; k++)
	 if(imouse_inbox(252+95*k, 425, 252+95*k+70, 446))   return 80+k;
     }
    if(imouse_inbox(540, 427, 639, 447))  return 835;
    return -1;
  }

void  renew_names()
 { int j, i, k ;
   int M = msg==1?39:msg==0?61:86;
   int begin = Beginx[msg], end = begin+MAX;
   if(end>MAXnum)  end = MAXnum;
   setfillstyle(1, papercolor);
   mouse_off();
   for(i=0; i<MAX; i++)  bar(X1+93+M*i, Y1+11, X1+89+M*(i+1), Y1+45);
   sethz_color(BLACK);
   if(msg==1)
    { for(k=begin; k<end; k++)
	xshow_hzs(X1+104+39*(k-begin), Y1+38, ZY[k].Name);
    }
   if(msg==0)
    { for(k=begin; k<end; k++)
	xshow_hzs(X1+114+61*(k-begin), Y1+38, DY[k].Name);
    }
   if(msg==2)
    { for(k=begin; k<end; k++)
       xshow_hzs(X1+126+86*(k-begin), Y1+38, FJ[k].Name);
    }
   mouse_on();
 }

void  show_linemsgy(int N)
 { int j;
   ST_MSG *ptr = find_point(Begin);
   int KD = msg==0?61:msg==1?39:86;
   N -= Beginx[msg];
   show_date(1);
   setfillstyle(1, papercolor);
   mouse_off();
   bar(X1+93+KD*N, Y1+11, X1+89+KD*(N+1), Y1+45);
   for(j=0; j<15; j++)
     bar(X1+93+KD*N, Y1+49+21*j, X1+89+KD*(N+1), Y1+45+21*(j+1));
   if(N+Beginx[msg]>=MAXnum)  return;
   sethz_color(BLACK);  setcolor(BLACK);
   switch(msg)
     { case 1: xshow_hzs(X1+104+39*N, Y1+38, ZY[N+Beginx[msg]].Name); break;
       case 0: xshow_hzs(X1+114+61*N, Y1+38, DY[N+Beginx[msg]].Name); break;
       case 2: xshow_hzs(X1+126+86*N, Y1+38, FJ[N+Beginx[msg]].Name); break;
     }
   for(j=0; j<15; j++)
     { if(ptr==NULL)  break;
       show_nums(X1+(msg==0?102:msg==1?93:105)+KD*N, Y1+50+j*21,
	    msg==0?ptr->Dy[N+Beginx[msg]]:msg==1?ptr->Zy[N+Beginx[msg]]:
	      ptr->Fj[N+Beginx[msg]], msg==0?4:msg==1?4:6, msg==0?1:msg==1?1:3);
       ptr = ptr->Next;
     }
 }


void  show_linemsg(int N)
 { ST_MSG *ptr;
   int begin=Beginx[msg], end=Beginx[msg]+MAX;
   int N0=N-Begin, M = msg==1?39:msg==0?61:86;
   int i;  int KD = msg==1?39:msg==0?61:86;
   if(end>MAXnum)  end = MAXnum;
   ptr = find_point(N);
   show_date(1);
   setfillstyle(1, papercolor);
   mouse_off();
   bar(X1+11, Y1+49+N0*21, X1+38, Y1+45+(N0+1)*21);
   bar(X1+42, Y1+49+N0*21, X1+89, Y1+45+(N0+1)*21);
   for(i=0; i<MAX; i++)
     bar(X1+93+M*i, Y1+49+21*N0, X1+89+M*(i+1), Y1+45+21*(N0+1));
   if(ptr==NULL||N>=STnum)  return;
   setcolor(BLACK);   sethz_color(BLACK);
   for(i=begin; i<end; i++)
     show_nums(X1+(msg==0?102:msg==1?93:105)+KD*(i-begin), Y1+50+N0*21,
	 msg==0?ptr->Dy[i]:msg==1?ptr->Zy[i]:ptr->Fj[i],
	     msg==0?4:msg==1?4:6, msg==0?1:msg==1?1:3);
   show_nums(X1+12, Y1+50+N0*21, ptr->Xh, 3, 0);
   show_hzs(X1+42, Y1+66+N0*21, ptr->Name);
   mouse_on();
 }

int  _test_in(int _X1, int _Y1, int _X2, int _Y2, int clear)
 { if(!imouse_inbox(_X1, _Y1, _X2, _Y2)&&clear==0)   return -1;
   if(bx1==_X1&&by1==_Y1&&bx2==_X2&&by2==_Y2)  return 1;
   not_bar(bx1, by1, bx2, by2);
   if(bx2>=540&&by2>=427)  show_time(1);
   bx1 = _X1; by1=_Y1; bx2=_X2; by2=_Y2;
   not_bar(bx1, by1, bx2, by2);
   if(bx2>=540&&by2>=427)  show_time(1);
   return 1;
 }

void  not_bar(int x1, int y1, int x2, int y2)
 { void  far *ptr;
   if(!x1&&!y1&&!x2&&!y2)  return;
   ptr = (void far *)farmalloc(imagesize(x1, y1, x2, y2));
   if(ptr == NULL)  return;
   mouse_off();
   getimage(x1, y1, x2, y2, ptr);
   putimage(x1, y1, ptr, NOT_PUT);
   mouse_on();
   farfree(ptr);
 }

void  show_line()
 { int k;
   if(INPUT || kind>1)  return;
   setlinestyle(0, 0, 1);
   setcolor(RED);  line(X1+10, Y1+47, X2-10, Y1+47);
   setcolor(MAGENTA); line(X1+40, Y1+10, X1+40, Y2-10);
   setcolor(RED);  line(X1+91, Y1+10, X1+91, Y2-10);
   setcolor(BLUE);
   for(k=0; k<14; k++) line(X1+10, Y1+68+21*k, X2-10, Y1+68+21*k);
   for(k=1; k<MAX; k++)
     line(X1+91+k*(msg==1?39:msg==0?61:86), Y1+10,
		  X1+91+k*(msg==1?39:msg==0?61:86), Y2-10);
 }

void  show_msg()
 { int  k;
   clear_bar();
   mouse_off();
   show_paper(X1+10, Y1+10, X2-10, Y2-9);
   show_line();
   sethz_color(BLACK);
   show_hzs(X1+10, Y1+38, "学号");
   show_hzs(X1+50, Y1+38, "姓名");
   renew_names();
   renew_xms();
   renew_xhs();
   renew_data();
   if(kind==0) barxy(nowx, nowy);
   write_fb();
   show_scroll(X1+70, Y1+3, X2-70, Y1+8, MAXnum-MAX+1, Beginx[msg]);
   show_scroll(X2-8, Y1+70, X2-3, Y2-70, STnum-15+1,   Begin);
   show_ptr2(msg);
   mouse_on();
 }

void  renew_data()
 { int j, i ;  ST_MSG *ptr;
   int begin=Beginx[msg], end=Beginx[msg]+MAX;
   int KD = msg==1?39:msg==0?61:86;
   if(end>MAXnum)  end = MAXnum;
   ptr = find_point(Begin);
   setcolor(BLACK);
   for(j=0; j<15; j++)
     { if(ptr==NULL)  break;
       for(i=begin; i<end; i++)
	 show_nums(X1+(msg==0?102:msg==1?93:105)+KD*(i-begin), Y1+50+j*21,
	    msg==0?ptr->Dy[i]:msg==1?ptr->Zy[i]:ptr->Fj[i],
		msg==0?4:msg==1?4:6, msg==0?1:msg==1?1:3);
       ptr = ptr->Next;
     }
 }

void  renew_xms()
 { int k; ST_MSG *ptr;
   setfillstyle(1, papercolor);
   for(k=0; k<15; k++)  bar(X1+42, Y1+49+k*21, X1+89, Y1+45+(k+1)*21);
   ptr = find_point(Begin);
   sethz_color(BLACK);
   for(k=0; k<15; k++)
     { if(ptr==NULL)  break;
       show_hzs(X1+42, Y1+66+k*21, ptr->Name);
       ptr = ptr->Next;
     }
 }

void  renew_xhs()
 { int k;  ST_MSG *ptr;
   setfillstyle(1, papercolor);
   for(k=0; k<15; k++)  bar(X1+11, Y1+49+k*21, X1+38, Y1+45+(k+1)*21);
   ptr = find_point(Begin);
   setcolor(BLACK);
   for(k=0; k<15; k++)
     { if(ptr==NULL)  break;
       show_nums(X1+12, Y1+50+(k)*21, ptr->Xh, 3, 0);
       ptr = ptr->Next;
     }
 }

void  xshow_hzs(int x, int y, char *str)
 { int len = strlen(str), k;
   char S[5];
	  if(len == 2)  show_hzs(x, y, str);
   else   if(len == 4)  show_hzs(x-8, y, str);
   else   if(len == 6)
     { for(k=0; k<4; k++)  S[k]=str[k];  S[k]='\0'; show_hzs(x-8, y-8, S);
       for(   ; k<6; k++)  S[k-4]=str[k];S[k-4]='\0'; show_hzs(x, y+8, S);
     }
   else   if(len == 8)
     { for(k=0; k<4; k++)  S[k]=str[k];  S[k]='\0'; show_hzs(x-8, y-8, S);
       for(   ; k<8; k++)  S[k-4]=str[k];S[k-4]='\0'; show_hzs(x-8, y+8, S);
     }
 }

int  save(int code)
 { FILE *fp;  int k;   ST_MSG  *ptr;
   if(strcmp(filename, "NONAME")==0)  return -3;
   fp = fopen(filename, "rb");
   if(fp!=NULL&&!code&&
       msg_box(320, 240, "发现重名文件：", "是否覆盖掉该文件？", 0)-1) return -2;
   if(fp!=NULL)  fclose(fp);
   fp = fopen(filename, "wb");
   add_name();
   if(fp==NULL)
     { msg_box(320, 240, "保存文件错误：", "无法保存指定文件！", 1);  return -1; }
   fputc('S', fp); fputc('X', fp); fputc('\n', fp);
   fprintf(fp, "%d,%d,%d,%d,%d\n",
	    PTR[0].num, PTR[1].num, PTR[2].num, PTR[3].num, PTR[4].num );
   fprintf(fp, "%s\n%s\n%s\n%s\n%s\n",
	    PTR[0].str, PTR[1].str, PTR[2].str, PTR[3].str, PTR[4].str );
   fprintf(fp, "%s\n", Head[0]==0?"NULL":Head);
   fprintf(fp, "%s\n", Man[0]==0?"NULL":Man);
   fprintf(fp, "%d,%d,%d,%d\n", DYnum, ZYnum, FJnum, STnum);
   for(k=0; k<DYnum; k++)  fprintf(fp, "%s\n", DY[k].Name[0]==0?"NULL":DY[k].Name);
   for(k=0; k<ZYnum; k++)  fprintf(fp, "%s\n%d,%f\n",
	     ZY[k].Name[0]==0?"NULL":ZY[k].Name,ZY[k].Attrib, ZY[k].Xf);
   for(k=0; k<FJnum; k++)  fprintf(fp, "%s\n", FJ[k].Name[0]==0?"NULL":FJ[k].Name);
   fprintf(fp, "Order = %d, %d\n", bcode, order);
   fprintf(fp, "/*************  BEGIN *************/\n");
   ptr = Home->Next;
   while(ptr!=NULL)
    { fprintf(fp, "%d\n", ptr->Xh);
      fprintf(fp, "%s\n", ptr->Name[0]==0?"NULL":ptr->Name);
      for(k=0; k<DYnum; k++)  fprintf(fp, "%f\n", ptr->Dy[k]);
      for(k=0; k<ZYnum; k++)  fprintf(fp, "%f\n", ptr->Zy[k]);
      for(k=0; k<FJnum; k++)  fprintf(fp, "%f\n", ptr->Fj[k]);
      ptr = ptr->Next;
    }
   fprintf(fp, "/************* END ************/\n");
   fclose(fp);
   saveflag = 0;  jsflag = 0;  add_name();
   return 1;
 }

#define  test_null(x)  (strcmp(x, "NULL")==0?1:0)
int  get_msg(int code)
 { FILE *fp;  ST_MSG *ptr; int k, i;
   fp = fopen(filename, "rb");
   if(fp==NULL)
     { if(code==0) msg_box(320, 240, "打开文件错误：", "无法打开指定文件！", 1);  return -1; }
   if(fgetc(fp)!='S'||fgetc(fp)!='X')
     { if(code==0) msg_box(320, 240, "读取文件错误：", "该文件不是这种类型的文件！", 1);
       fclose(fp); return -1;
     }
   fgetc(fp);
   fscanf(fp, "%d,%d,%d,%d,%d\n",
	&PTR[0].num, &PTR[1].num, &PTR[2].num, &PTR[3].num, &PTR[4].num);
   PTR[0].str = (char*)realloc(PTR[0].str, PTR[0].num);
   PTR[1].str = (char*)realloc(PTR[1].str, PTR[1].num);
   PTR[2].str = (char*)realloc(PTR[2].str, PTR[2].num);
   PTR[3].str = (char*)realloc(PTR[3].str, PTR[3].num);
   PTR[4].str = (char*)realloc(PTR[4].str, PTR[4].num);
   fscanf(fp, "%s\n%s\n%s\n%s\n%s\n",
	       PTR[0].str, PTR[1].str, PTR[2].str, PTR[3].str, PTR[4].str);
   fscanf(fp, "%s\n", Head);  if(test_null(Head))  clear_str(Head);
   fscanf(fp, "%s\n", Man);   if(test_null(Man))   clear_str(Man);
   fscanf(fp, "%d,%d,%d,%d\n", &DYnum, &ZYnum, &FJnum, &STnum);
   DY = (struct _DY*)malloc(DYnum*sizeof(struct _DY));
   ZY = (struct _ZY*)malloc(ZYnum*sizeof(struct _ZY));
   FJ = (struct _FJ*)malloc(FJnum*sizeof(struct _FJ));
   for(k=0; k<DYnum; k++)
     { fscanf(fp, "%s\n", DY[k].Name);
       if(test_null(DY[k].Name))  clear_str(DY[k].Name);
     }
   for(k=0; k<ZYnum; k++)
     { fscanf(fp, "%s\n%d,%f\n", ZY[k].Name, &ZY[k].Attrib, &ZY[k].Xf);
       if(test_null(ZY[k].Name))  clear_str(ZY[k].Name);
     }
   for(k=0; k<FJnum; k++)
     { fscanf(fp, "%s\n", FJ[k].Name);
       if(test_null(FJ[k].Name))  clear_str(FJ[k].Name);
     }
   fscanf(fp, "Order = %d, %d\n", &bcode, &order);
   fscanf(fp, "/*************  BEGIN *************/\n");
   for(i=0; i<STnum; i++)
    { ptr = add_point(End);
      if(ptr == NULL)  return 2;
      End = ptr;
      fscanf(fp, "%d\n", &ptr->Xh);
      fscanf(fp, "%s\n", ptr->Name);
      if(test_null(ptr->Name))  clear_str(ptr->Name);
      for(k=0; k<DYnum; k++)	fscanf(fp, "%f\n", &ptr->Dy[k]);
      for(k=0; k<ZYnum; k++)	fscanf(fp, "%f\n", &ptr->Zy[k]);
      for(k=0; k<FJnum; k++)	fscanf(fp, "%f\n", &ptr->Fj[k]);
    }
   fscanf(fp, "/************* END ************/\n");
   fclose(fp);  add_name();
   return 1;
 }

ST_MSG  *find_point(int n)
 { ST_MSG *ptr = Home;   int k;
   for(k=0; k<n+1; k++)
     { if(ptr==NULL)  return NULL;
       ptr = ptr->Next;
     }
   return ptr;
 }

void  move_text(int code)
 { char far *ptr; int k;
   int KD = msg==1?39:msg==0?61:86;
   ptr = (char far*)farmalloc(
      code<2?imagesize(X1, Y1, X2, Y1+21):imagesize(0, 0, KD, Y2-Y1-21));
   mouse_off();   clear_bar();
   if(code==0)
     { for(k=0; k<14; k++)
	{ getimage(X1+10, Y1+k*21+66, X2-10, Y1+(k+1)*21+66, ptr);
	  putimage(X1+10, Y1+(k-1)*21+66, ptr, COPY_PUT);
	}
       show_linemsg(14+Begin);
     }
   else if(code==1)
     { for(k=13; k>=0; k--)
	{ getimage(X1+10, Y1+(k-1)*21+66, X2-10, Y1+k*21+66, ptr);
	  putimage(X1+10, Y1+k*21+66, ptr, COPY_PUT);
	}
       show_linemsg(Begin);
     }
   else if(code==2)
     { for(k=1; k<MAX; k++)
	{ getimage(X1+91+KD*k, Y1+10, X1+89+KD*(k+1), Y2-9, ptr);
	  putimage(X1+91+KD*(k-1), Y1+10, ptr, COPY_PUT);
	}
       show_linemsgy(MAX+Beginx[msg]-1);
     }
   else if(code==3)
     { for(k=MAX-1; k>0; k--)
	{ getimage(X1+91+KD*(k-1), Y1+10, X1+89+KD*k, Y2-9, ptr);
	  putimage(X1+91+KD*k, Y1+10, ptr, COPY_PUT);
	}
       show_linemsgy(Beginx[msg]);
     }
   write_fb();          show_line();
   show_scroll(X1+70, Y1+3, X2-70, Y1+8, MAXnum-MAX+1, Beginx[msg]);
   show_scroll(X2-8, Y1+70, X2-3, Y2-70, STnum-15+1,   Begin);
   mouse_on();
   farfree(ptr);
 }

void  set_home()
 { Home = (ST_MSG *)farmalloc(sizeof(ST_MSG));
   Home->Next = NULL;  End = Home;
 }

#define XX   250
#define YY   150
int   avr_input(int x, int y, float *N, int n, int point)
 { double  get_pj(float *, int n);
   float  DATA[201], data=0.0, bN=*N ;
   int num=0, flag=1, N0=0, begin=0, Start; double sum=0.0;
   int x1, y1, x2, y2, k, code, start=0;
   char far *ptr;  char ch, scrflag = 0;
   for(k=0; k<201; k++)  DATA[k] = 0.0;
   if(x+XX>620)  { x1 = x-XX; x2 = x; }
   else          { x1 = x; x2 = x+XX; }
   if(y+YY>460)  { y1 = y-YY; y2 = y; }
   else          { y1 = y; y2 = y+YY; }
   ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return -1;
   mouse_off();
   clear_bar(); clear_scroll();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+4, x2+5, y2+5);
   setfillstyle(1, GREEN);
   bar(x1, y1, x2, y2);
   setlinestyle(0, 0, 1);
   setcolor(YELLOW);
   rectangle(x1+2, y1+2, x2-2, y2-2);
   line(x1+90, y1+4, x1+90, y2-5);
   sethz_color(YELLOW);
   show_hzs(x1+98, y2-10, "确定 取消  删除");
   sethz_color(LIGHTCYAN);
   show_hzs(x1+98, y1+25, "个数：");
   show_hzs(x1+98, y1+45, "总分：");
   show_hzs(x1+98, y1+65, "平均：");
   sethz_color(RED);
   show_hzs(x1+98, y1+90, "在此输入数据");
   setfillstyle(1, DARKGRAY);
   bar(x1+98, y1+95, x2-10, y1+115);
   setcolor(WHITE);  bkcolor = DARKGRAY;
   INPUT = 1;
   show_nums(x1+150, y1+10, 0, 3, 0);
   show_nums(x1+150, y1+30, 0, 8, 4);
   show_nums(x1+150, y1+50, 0, 8, 4);
   sethz_color(MAGENTA);
   show_hzs(x1+6, y1+40, "注意：在计");
   show_hzs(x1+6, y1+60, "算平均值时");
   show_hzs(x1+6, y1+80, "将删掉 个");
   show_hzs(x1+6, y1+100, "最大值和 ");
   show_hzs(x1+6, y1+120,"个最小值！");
   setcolor(WHITE);
   show_nums(x1+58, y1+63, B, 3, 0);
   show_nums(x1+74, y1+83, E, 3, 0);
   fcolor = WHITE;
 BEGIN
   key = 0;   mouse_on();   show_time(0);
	if(test_in(x1+92, y2-28, x1+135, y2-8)==1)  code = 0;
   else if(test_in(x1+138,y2-28, x1+184, y2-8)==1)  code = 1;
   else if(test_in(x1+205,y2-28, x1+247, y2-8)==1)  code = 2;
   else if(test_in(x1+98, y1+95, x2-10, y1+115)==1) code = 3;
   else if(imouse_inbox(x1+85, y1+4, x1+95, y2-3))
	    { float len;
	      if(scrflag==0)  clear_bar();
	      if(num)
		{ len = (float)(y2-y1-7)/num;
		  if(scrflag==0)
		    { scroll_see(x1+85, y1+4, x1+95, y2-3, 1, num, num-begin-1);
		      scrflag = 1;
		    }
		  Start = num-begin-1;
		  if(imouse_inbox(x1+85, y1+4, x1+95, y1+(int)(len*Start+0.5)))  code = 7;
		  else if(imouse_inbox(x1+85, y1+4+(int)(len*(Start+1)+0.5), x1+95, y2-3)) code = 9;
		  else if(test_in(x1+85, y1+4+(int)(len*Start+0.5), x1+95, y1+4+(int)(len*(Start+1))+0.5))  code = 8;
		  if(code != 8)  clear_bar();
		}
	    }
   else if(start==1)
	  { for(k=0; k<7; k++)
	     { if(begin+(6-k)>=num)  continue;
	       if(test_in(x1+5, y1+4+20*k, x1+87, y1+4+20*(k+1))==1)
		{ code = 10+k; break; }
	     }
	    if(k==7)  { code = -1; clear_bar(); }
	  }
   else if(code!=-1) { clear_bar(); clear_scroll(); scrflag = 0; code = -1; }
   if(!imouse_inbox(x1+85, y1, x1+95, y2)&&scrflag==1)
      { clear_scroll(); scrflag = 0; }
   if(!kbhit()&&getmousebutton()==NOBUTTON)  continue;
   mouse_off();  clear_bar();  mouse_on();  start = 1;
   if(kbhit())   { clear_scroll(); scrflag = 0; }
 if(getmousebutton()==LEFTBUTTON)
   switch(code)
    { case 0: *N = get_pj(DATA, num); goto end;
      case 1: *N = bN;  flag = -1; goto end;
      case 2: if(num==0)  break;
	      if(data != DATA[N0])  break;
	      for(k=N0; k<num; k++)  DATA[k] = DATA[k+1];
	      sum -= (double)data;
	      num--;  break;
      case 3: if(num>=199)  break;
	      mouse_off();
	      setfillstyle(1, DARKGRAY);  bar(x1+105, y1+95, x2-10, y1+115);
	      mouse_on();
	      if(input_num(x1+98,y1+95,&data,n,point)==-1)  break;
	      num++;  sum += (double)data;
	      for(k=198; k>=0; k--)    DATA[k+1] = DATA[k];
	      DATA[0] = (float)data;  begin = 0;  break;
      case 7: begin += 3; if(begin>num-1) begin=num-1; break;
      case 8: Start = num-begin-1;
	      Start = scroll(x1+85, y1+4, x1+95, y2-3, num, Start);
	      begin = num-Start-1; break;
      case 9: begin -= 3; if(begin<0)  begin=0; break;
      default: if(code>=10)  data = DATA[6-(code-10)+begin]; N0 = 6-(code-10)+begin; break;
    }
 if((ch=bioskey(1))!=0)
   if( (ch<='9'&&ch>='0')||ch=='.'||ch=='-' )
    { if(num<199)
       { setfillstyle(1, DARKGRAY);  bar(x1+98, y1+95, x2-10, y1+115);
	 if(input_num(x1+105,y1+95,&data,n,point)!=-1)
	  { num++;  sum += (double)data;  
	    for(k=198; k>=0; k--)    DATA[k+1] = DATA[k];
	    DATA[0] = (float)data;  begin = 0;
	  }
       }
    }
  if(kbhit()) switch(getch())
      { case ENTER_KEY: if(num>=199)  break;
			setfillstyle(1, DARKGRAY);  bar(x1+98, y1+95, x2-10, y1+115);
			if(input_num(x1+105,y1+95,&data,n,point)==-1)  break;
			num++;  sum += (double)data;
			for(k=198; k>=0; k--)    DATA[k+1] = DATA[k];
			DATA[0] = (float)data;  begin = 0; break;
	case ESC_KEY:   *N = bN;  flag = -1;  goto end;
	case DOWN_KEY:  begin--; if(begin<0)  begin=0;  break;
	case UP_KEY:    begin++; if(begin>num-1) begin=num-1; break;
	case PAGEUP_KEY:begin+=6; if(begin>num-1) begin=num-1; break;
	case PAGEDOWN_KEY: begin-=6; if(begin<0) begin=0; break;
	case END_KEY:   begin=0; break;
	case HOME_KEY:  begin=num-1; break;
      }
   setfillstyle(1, GREEN);
   mouse_off();
   clear_scroll();
   bar(x1+140, y1+4, x2-5, y1+65);
   bar(x1+5, y1+4, x1+87, y2-5);
   setcolor(WHITE);
   show_nums(x1+150, y1+10, (float)num, 3, 0);
   show_nums(x1+150, y1+30, (float)sum, 8, 4);
   show_nums(x1+150, y1+50, get_pj(DATA, num), 8, 4);
   for(k=0; k<(num-begin>7?7:num-begin); k++)  show_nums(x1+10, y2-25-20*k, DATA[k+begin], 8, 4);
   setfillstyle(1, DARKGRAY);  bar(x1+98, y1+95, x2-10, y1+115);
   show_nums(x1+105, y1+95, data, n, point);
   mouse_on();
   if(imouse_inbox(x1+85,y1,x1+95,y2))
      scroll_see(x1+85, y1+4, x1+95, y2-3, 1, num, num-begin-1);
   while(getmousebutton()!=NOBUTTON) show_time(0);
 WEND
 end:
   while(getmousebutton()!=NOBUTTON) show_time(0);
   mouse_off(); clear_bar();  clear_scroll();
   putimage(x1, y1, ptr, COPY_PUT);
   farfree(ptr);
   mouse_on();
   INPUT = 0;
   fcolor = RED;
   return flag;
 }

double  get_pj(float *DATA, int num)
 {  int k, i, j;
    float _DATA[200], t, sum=0;
    if(B+E>=num)  return 0;
    for(k=0; k<num; k++)  _DATA[k] = DATA[k];
    for(j=0; j<num; j++)
     { for(i=j+1; i<num; i++)
	if(_DATA[i]<_DATA[j])
	  { t = _DATA[i]; _DATA[i] = _DATA[j];  _DATA[j] = t; }
     }
    for(k=B; k<num-E; k++)
       sum += _DATA[k];
    return sum/(num-B-E);
 }

void  add_name()
 { int k, i, len = strlen(filename);
   char name[30];
   strcpy(name, filename);
   for(k=0; k<len; k++)
     if(name[k]<='z'&&name[k]>='a')  name[k] -= 'a'-'A';
   if(strcmp(name, "NONAME")==0)  return;
   if(maxfilenum==0)  return;
   for(k=0; k<namenum; k++)  if(strcmp(name, bname[k])==0)  break;
   if(k==namenum)
    { if(namenum>=maxfilenum)
       { for(i=0; i<9; i++)   strcpy(bname[i], bname[i+1]);
	 k = maxfilenum-1;
	 namenum = maxfilenum;
       }
      else   k = namenum++;
      strcpy(bname[k], name);
    }
 }