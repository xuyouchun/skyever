#include <tx.h>
#include <string.h>
#include <time.h>
#include <alloc.h>
#include <conio.h>
#include "extern.h"

void  writescreen(),  showtitle(), conioclick(), Exit(int);
int   starttest(int select);
void  dosystem()  { conioclick(); }
void  test_end(), write_hb(), showTITLE();

BUTTON  button[4];
MENU    menu[4];
FILE  * fp = NULL;
char  * ptr = NULL;
char  *PA=NULL, *PG=NULL, *PQ=NULL, *PU=NULL, *PE=NULL, *PR=NULL, *TITLE=NULL;
int   titnum=-1, selnum=-1;
int   sel = 0;
char  ifcontinue=1, ifopen=0, ifstart=0, ifend=0,
	ifenablepause=1, iffileopen = 0, ishowqk, bkifcontinue;
extern int ifsound;
SEL * titptr = NULL;
long  T, T0;
int   second, asecond;
BUTTON  but_begin = { 320-4*16, 430, 0, 0, "����˴���ʼ����", LIGHTRED, BLACK, BF|BT };
#define ENDCOLOR  LIGHTGREEN
BUTTON  but_end[4] =
 { { 100, 430, 0, 0, "����������", ENDCOLOR, BLACK, BF },
   { 200, 430, 0, 0, "�鿴����ϸ��", ENDCOLOR, BLACK, BF },
   { 330, 430, 0, 0, "���ܽ�", ENDCOLOR, BLACK, BF },
   { 420, 430, 0, 0, "��ѯ�������", ENDCOLOR, BLACK, BF }
 };

XMENU  xmenu =
 { 30, 30, -1, -1, button, menu, 4, "����\0ѡ��\0����\0��", 20, 0 };

LIST  list =
 { 50, 100, 230, 400, NULL, {0,0,0,0,YELLOW,RED,0,0}, -1, -1, 0, {0,0,0,0}, 0 };

BUTTON  button2[4] =
 { { 415, 30, 0, 0, "��һ��", -1, -1, BT },
   { 479, 30, 0, 0, "��һ��", -1, -1, BT },
   { 543, 30, 0, 0, "����˵��", -1, -1, BT },
   { 335, 30, 0, 0, "�������", -1, -1, 0 }
 };

TOOLBOX  toolbox;
BUTTON   *blist;
BUTTON   renew = { 30,0,130,0,"<ˢ��ѡ��>", LIGHTBLUE, BLACK, BT|BF|BY },
	 complete = { 150,0,250,0,"<�������>", LIGHTBLUE, BLACK, BT|BF|BY },
	 allend = { 270,0,370,0,"<���ڽ���>", LIGHTBLUE, BLACK, BT|BF|BY };
int  X , Y ;
char ch;
int  nowselect, bkcolor=BLUE;
char ifsave, ifft, ifhelp;

#define  FREESTR(x)   if(x!=NULL)  free(x);  x = NULL
#define  xp(y)        xmenup(xmenu,y)
int main()
 { int k;
   registerbgifont(triplex_font);
   inittx();
   open_hzk("examasc", "examhzk");
   xp(0) = (char*)malloc(300);
   xp(1) = (char*)malloc(1500);
   xp(2) = (char*)malloc(550);
   xp(3) = (char*)malloc(550);
   strcpy(xp(0), "X�˳�������ϵͳ");  // 1�Ų˵�����
   strcpy(xp(1), "^@(���κβ�����)|-1|Y��һ�ײ�����|X��һ�ײ�����|fͨ���ļ�����|j�����ļ�{^L����ļ��б�}-|^Z��ǰ����������");  //���Ų˵�����
   strcpy(xp(2), "X����...|-|^@δ���ְ����ļ�|-|Y���ߵĻ�..." ); //���Ų˵�����
   strcpy(xp(3), "^A�����趨...{^A��ǰ�ļ�����|^B��ǰ�ļ�����}"
		 "^B������...{^A���������|^B���������|-|^C�����׼��|"
		 "^D��Ϊ��׼��}"
		    "^D�ر�Ŀ¼|E��Ŀ¼...|-|Fִ  ��..."
		      "{Aˢ����Ļ|B������ת��|C�ģϣ�����״̬}"
		    "G��  ��..."
		      "{A��������ļ�������|DԤ���ļ���"
			"{AĿ¼�ļ���|Bͨ�ú������ļ���|Cͨ�ú궨���ļ���|D�����ļ���}"
		      "B�˳�ʱ�Զ���������|E�ı䱳����ɫ"
			  "{A��   ɫ"
			    "{0��  ɫ|1��  ɫ|2��  ɫ|3��  ɫ|4��  ɫ|5���ɫ|6��  ɫ|7ǳ��ɫ}"
			  "B��   ɫ"
			    "{0���ɫ|1����ɫ|2����ɫ|3����ɫ|4����ɫ|5�����|6��  ɫ|7��  ɫ}"
			  "-|CĬ�ϱ�����ɫ}C��������}-|"
		    "H�����ڲ�����");
   getset();
   writescreen();  show_about();  initmouse();
   
   formatxmenu(&xmenu);
   button[3].x1 = 10; button[3].x2 = 35;  menu[3].x = 10;
   setxmenucolor(&xmenu, WHITE, BLACK);
   button[3].forcolor = LIGHTGREEN;
   showxmenu(&xmenu);
   sethzborder(LEFT, RIGHT);
   for(k=0; k<4; k++)
     { formatbutton(button2+k);  showbutton(button2+k); }
   for(k=0; k<4; k++)  formatbutton(but_end+k);
   
   readinmenu(dirfile);
   readinhelp(helpfile);
   if(ifft==1)
    { close_hzk();  open_hzk("examasc", "examhzkf");
      renew_screen(0);
    }
   mouse_on();
   showTITLE();
   while(1)   conioclick();
 }

void  Exit(int code)
 { mouse_off();  close_hzk();
   freeall();
   closegraph();  freexmenuptr(&xmenu);
   if(ifsave==1)  saveset();
   exit(code);
 }

void  writescreen()
 { int k;
   mouse_off();
   cleardevice();
   setbkcolor(bkcolor);
   setcolor(YELLOW);
   show_str(20, 4, "��");
   showTITLE();  showtitle();
   setlinestyle(0, 0, 3);
   setcolor(WHITE);
   line(0, 24, 639, 24);
   setlinestyle(0, 0, 1);
   mouse_on();
   write_hb();
 }

void  showTITLE()
 { setfillstyle(0, 0);
   off(); bar(40, 4, 200, 20); on();
   setcolor(YELLOW);
   if(strlen(TITLE)>20)  TITLE[20] = '\0';
   off(); show_str(40, 4, TITLE==NULL?"���ϵ�в�������ϵͳ":TITLE); on();
 }

void  showtitle()
 { char S[60] = "�� ";
   if(iffileopen==0)  strcat(S, "��ѡ�� (��Ҫʱ���Ķ�������Ϣ)");
   else
    { if(PA!=NULL)  strcat(S, PA);
      else  strcat(S, "�ޱ���");
    }
   strcpy(S+48, "..."); off();
   setfillstyle(0, 0);
   bar(190, 4, 639, 20);
   setcolor(LIGHTGREEN);
   show_str(180+(640-190-strlen(S)*8)/2, 4, S);
   on();
 }

void  addstartmenu();

void  conioclick()
 { int k, i, code;
   if(ifstart==1)   if(starttest(-2)==-1)  return;
   if( but_end[3].attrib & (BD|BY) )  asktest();
   if(!ixmenuopen(xmenu) && kbhit() && iffileopen==1)
    { switch(getch())
       { case SPACE_KEY: if(ifend==1)  break;
			 if(ifstart==0)  { addstartmenu(); break; }
	 case DOWN_KEY: case RIGHT_KEY: case PAGEDOWN_KEY:
			if(nowselect==titnum)  break;
			if(ifend==1)
			 { nowselect++; showflag = 0;  asktit(); }
			else if(ifstart==1)
			 { starttest(++nowselect); test_buttonenable(); }
			break;
	 case PAGEUP_KEY: case UP_KEY: case LEFT_KEY:
			if(nowselect==0)  break;
			if(ifend==1)
			  { nowselect--; showflag = 0; asktit(); }
			 else if(ifstart==1)
			  { starttest(--nowselect); test_buttonenable(); }
			 break;
	 case HOME_KEY: if(nowselect==0)  break;
			if(ifend==1)
			  { nowselect=0; showflag = 0; asktit(); }
			else if(ifstart==1)
			  { starttest(nowselect=0); test_buttonenable(); }
			break;
	 case END_KEY: if(nowselect==titnum)  break;
			if(ifend==1)
			  { nowselect=titnum;  showflag = 0; asktit(); }
			else if(ifstart==1)
			  { starttest(nowselect=titnum); test_buttonenable(); }
			break;
       }
    }
   code = xmenuclick(&xmenu);
   if(ifhelp==1)   code = 0;
   switch(code)
    { case 0: { char str[5];
		strcpy(str, getselect());
		but_begin.caption = "����˴���ʼ����";
		switch(str[0])
		 { case 'L': if(ifstart==1)
			       if(ask_box("�����رգ�", "��ȷ��Ҫ�ر����ڲ��Եĸ���������")!=1)  break;
			     freeall();
			     iffileopen = 0;  showtitle();
			     autoxmenu(xmenu, 0, "X");
			     delx(0, "SELK12CWweplnrma456x");
			     ifopen = 0;  reset_qk();
			     autox(1, "ABCDEFGHIJKLMNOPQXYfj");
			     unablex(1, "Z");
			     cleardesktop(0);
			     but_begin.attrib &= ~BY;
			     ifstart = 0;
			     for(k=0; k<4; k++) unablebutton(button2+k);
			     for(k=0; k<4; k++) but_end[k].attrib&=~(BY|BD);
			     show_about();
			     setfillstyle(0, 0);  bar(0, 461, 200, 479);
			     unablexmenu(xmenu, 3, "BA"); unablexmenu(xmenu, 3, "BB");
			     unablexmenu(xmenu, 3, "BC"); unablexmenu(xmenu, 3, "BD");
			     unablexmenu(xmenu, 3, "AA"); unablexmenu(xmenu, 3, "AB");
			     reset_qk();   line_msg(""); break;
		   case 'X': Exit(0); break;
		   case 'x': if(ifend==0)
			       { if(ask_box("������λ��", "��ȷ��Ҫ��λ���ڲ��Եĸ���������")
				   !=1)  break;
			       }
			     dataptr = 0;  maxdata = 50;
			     delx(0, "SELK12CWweplnx456rma");
			     unablebutton(button2);  unablebutton(button2+1);
			     enablebutton(button2+2); enablebutton(button2+3);
			     ifstart = 0;  ifend = 0;   nowselect = 0;
			     line_msg("");
			     addxmenu(xmenu, 0, "X<",
			       "S��ʼ����|^E��ͣ����|L�رո�������|K�鿴����˵��|-1|");
			     autox(1, "ABCDEFGHIJKLMNOPQXYfj");
			     autox(0, "KX");
			     for(k=0; k<4; k++) but_end[k].attrib&=~(BY|BD);
			     for(k=0; k<=titnum; k++)
			       { titptr[k].time = titptr[k].time0;
				 titptr[k].passtime = 0;
				 for(i=0; i<titptr[k].sel.num; i++)
				  titptr[k].sel.text[i].attrib &= ~BD;
			       }
			     cleardesktop(0);
			     mouse_on();  showPG();
			     unablexmenu(xmenu, 3, "BA"); autoxmenu(xmenu, 3, "BB");
			     autoxmenu(xmenu, 3, "BC");   unablexmenu(xmenu, 3, "BD");
			     break;
		   case 'S': addstartmenu();   break;
		   case 'E': replacexmenu(xmenu, 0, "E", ifcontinue==1?
				"E��������...|":"E��ͣ����|");
			     if(ifcontinue==1)  T0 = clock();
			     else    { T += clock()-T0; aT += clock()-T0; }
			     ifcontinue = 1-ifcontinue;
			     break;
		   case 'K': test:
			     T0 = clock();
			     if(ifstart==0)
			       but_begin.caption = "����˴���ʼ����";
			     else
			      { but_begin.caption = "����˴���������";
				unablex(0, "KEC");
				addxmenu(xmenu, 0, "K>", "W��������...|");
				bkifcontinue = ifcontinue; ifcontinue = 0;
			      }
			     but_begin.attrib |= BY;
			     showPG();
			     break;
		  case 'W':  addstartmenu();  break;
		  case 'C':  _allend:
			     if(ask_box("����������", "ȷ���޴���Ҫ������")!=1)  break;
			     test_end();  dataptr = 0;  _DATA = 0;  spaceshow=0;
			     if(PR!=NULL)
			      { setcolor(WHITE);
				_write_str(0, 462, 639, 479, PR, 0, 462);
			      } spaceshow = 1;  break;
		  case 'r':  delx(0, "SELK12CWweplnx456rma");
			     unablebutton(button2);  unablebutton(button2+1);
			     enablebutton(button2+2); enablebutton(button2+3);
			     ifstart = 1;  ifend = 0;   nowselect = 0;
			     dataptr = 0;  maxdata = 50;
			     line_msg("");
			     addxmenu(xmenu, 0, "X<",
			       "^S��ʼ����|E��ͣ����|L�رո�������|K�鿴����˵��|-1|");
			     if(ifenablepause==0)  unablexmenu(xmenu, 0, "E");
			     addxmenu(xmenu, 0, "1>", aTime>0?"C��ǰ����|-2|":"C���ڽ���|-2|");
			     addxmenu(xmenu, 0, "C>", "x��λ|");
			     addxmenu(xmenu, 0, "L<", "-4|^l��ѯǰһ��|^n��ѯ��һ��|^m����Ŀ���ݲ�ѯ|-5|");
			     for(k=0; k<4; k++) but_end[k].attrib&=~(BY|BD);
			     for(k=0; k<=titnum; k++)
			       { titptr[k].time = titptr[k].time0;
				 titptr[k].passtime = 0;
				 for(i=0; i<titptr[k].sel.num; i++)
				  titptr[k].sel.text[i].attrib &= ~BD;
			       }
			     autoxmenu(xmenu, 3, "BA"); unablexmenu(xmenu, 3, "BB");
			     unablexmenu(xmenu, 3, "BC"); unablexmenu(xmenu, 3, "BD");
			     cleardesktop(0);
			     setcolor(LIGHTCYAN);
			     show_str(5, 462, "��ʣ��ʱ��:");
			     if(aTime<0)  show_str(105, 462, "(������)");
			     else
			       { sprintf(str, "%d��%d��", aTime/60,  aTime%60);
				 show_str(105, 462, str);
			       }
			     mouse_on();
			     aT = clock();  asecond = aTime;
			     starttest(0);  test_buttonenable();
			     break;
		  case 'p':  but_end[1].attrib|=BD; showPP();  break;
		  case 'e':  but_end[2].attrib|=BD; showPE();  break;
		  case 'w':  but_end[0].attrib|=BD; showPQ();  break;
		  case 'a':  but_end[3].attrib|=BD; asktit();  break;
		  case 'l':  if(ifend==1)
			      { resetsel(nowselect--); showflag=0; asktit(); }
			     else if(ifstart==1)
			      { starttest(--nowselect); test_buttonenable(); }
			     break;
		  case 'n':  if(ifend==1)
			      { resetsel(nowselect++); showflag=0; asktit(); }
			     else if(ifstart==1)
			      { starttest(++nowselect); test_buttonenable(); }
			     break;
		  case 'm':  selectfortitle();  break;
		 }
	      }	break;
      case 1: { char filename[50];
		 if(getselect()[0]<='N'&&getselect()[0]>='A')
		  { if(!getlabelname(menuptr, getselect(), filename))
		     { msg_box("����",
			 "δ����ָ�����ļ���������Ŀ¼�ļ���ָ�����ļ����Ƿ���ȷ��");
		       break;
		     }
		    if((k=openfile(filename))==0)
		      { msg_box("����", "δ����ָ�����ļ�������ָ�����ļ��Ƿ���ڣ�");
			break;
		      }
		    if(k==-1)
		      { freeall();  break; }
		    showtitle();  showPG();
		  }
		 else if(getselect()[0]=='f')
		  { if((k=openforfile())==0)
		     { msg_box("����", "δ����ָ�����ļ�������ָ�����ļ��Ƿ���ڣ�");
		       break;
		     }
		    if(k==-1)  { freeall();  break; }
		    if(k==-2)  break;
		    showtitle();  showPG();
		  }
		 else if(getselect()[0]=='X' || getselect()[0]=='Y')
		  { char filename[50], k, order=Order;
		    if(!_getfilename(getselect()[0]=='X'?++Order:--Order, filename))
		     { msg_box("����",
			 "δ����ָ�����ļ���������Ŀ¼�ļ���ָ�����ļ����Ƿ���ȷ��");
		       Order = order;
		       break;
		     }
		    if((k=openfile(filename))==0)
		      { msg_box("����", "δ����ָ�����ļ�������ָ�����ļ��Ƿ���ڣ�");
			break;
		      }
		    if(k==-1)
		      { freeall();  break; }
		    showtitle();  showPG();
		  }
		 else if(getselect()[0]=='Z')         //��ǰ����������
		   { char str[100];
		     if(aTime<0)  sprintf(str, "�ļ�����%s   ��Ŀ������%d  ����ʱ�䣺������", nameptr, titnum+1);
		     else sprintf(str, "�ļ�����%s  ��Ŀ������%d ����ʱ�䣺%d��%d��", aTime/60, aTime%60);
		     msg_box("��ǰ���������ϣ�", str);
		   }
		 else if(getselect()[0]=='j')       //�����ļ��б�
		   { if(getselect()[1] == 'L')
		       { listnum = 0;  renewfilelist(); }
		     else
		       { if((k=openfile(filelist[getselect()[1]-'0']))==0)
			   { msg_box("����", "δ����ָ�����ļ�������ָ�����ļ��Ƿ���ڣ�");
			     break;
			   }
			 if(k==-1)
			   { freeall();  break; }
			 showtitle();  showPG();
		       }
		   }
	      } break;
       case 2: if(getselect()[0]=='X')
		 msg_box("���ڣ�", "ȫ���ܶ��ò�������ϵͳ  ����ߣ����Ѵ�"
		   "(���ϴ�ѧ����ϵ)  ���������������  �����һ��ʮ������"
		   "���ڼ���  �汾�ţ�1.0");
	      else
	       { char labelname[50]; int c; char label[21];
		 char *str;
		 if(getselect()[0]<='N'&&getselect()[0]>='A')
		  { char bifjm=ifjm;  ifjm=0;  readptr=NULL;
		    if(!getlabelname(helpptr, getselect(), labelname))
		     { msg_box("����",
			 "δ����ָ���������ݣ���������ļ��е���д�Ƿ���ȷ��");
		       break;
		     }
		    fp = fopen(helpfile, "rt");
		    if(fp==NULL)
		     { msg_box("��ȡ�����ļ�����",
			  "δ���ְ����ļ�������ָ����·���Ƿ���ȷ��");
		       ifjm=bifjm; break;
		      }
		     while((c=fgetc(fp))!=EOF)
		      { if(c!='/')  continue;
			if(fgetc(fp)!='(')  continue;
			fseek(fp, -1, SEEK_CUR);
			getlabel(label);
			if(strcmp(label, labelname)==0)  break;
		       }
		      if(c==EOF)
		       { msg_box("����", "δ���ָ����������İ������ݣ�");
			 fclose(fp);
			 ifjm = bifjm; break;
		       }
		      getstr();  str = ptr;  cleardesktop(0);
		      top_title("������Ϣ");
		      setcolor(LIGHTGRAY);
		      write_str(LEFT, TOP, RIGHT, BOTTOM, str);
		      free(ptr);  ifhelp=1;  fclose(fp);
		      ifjm = bifjm;
		      while(!kbhit()&&getmousebutton()==NOBUTTON);
		      ifhelp = 0;  renew_screen(1);
		  }
	       } break;
       case 3: switch(getselect()[0])
		 { case 'B': switch(getselect()[1])     //���������
			       { case 'A': saverel(0); break;
				 case 'B': userel(0);  break;
				 case 'C': userel(1);  break;
				 case 'D': saverel(1); break;
			       } break;
		   case 'A': switch(getselect()[1])   //���������
			       { case 'A': if(filejm()==1)  break;
				 case 'B': if(filejm()==1)  break;
			       } break;      		   
		   case 'H': saveset();   break;      //�����ڲ�����
		   case 'E': opendir();   break;
		   case 'D': closedir();  break;
		   case 'F': switch(getselect()[1])
			      { case 'A':  renew_screen(0);  break;  //ˢ����Ļ
				case 'B':  ifft = 1-ifft;
					   close_hzk(); open_hzk("examasc", ifft?"examhzkf":"examhzk");
					   renew_screen(0);  break;
				case 'C':  restorecrtmode();
					   textcolor(LIGHTBLUE);
					   cprintf("\n\n              Press");
					   textattr(LIGHTRED+BLINK);
					   cprintf(" \"Exit\" ");
					   textattr(LIGHTBLUE+(BLACK<<4));
					   cprintf("to return to Exam");
					   textattr(LIGHTGREEN+BLINK);
					   cprintf("...");
					   textattr(LIGHTBLUE+(BLACK<<4));
					   system("c:\\command.com");  mouse_off();
					   setgraphmode(VGAHI);
					   renew_screen(0); mouse_on(); break;
			      } break;
		   case 'G': switch(getselect()[1])
			      { case 'A': { char str[4];
					    sprintf(str, "%d", maxlistnum);
					    if(input_box("�����뱣�������ļ����ĸ�����(1-10)", str, 2)<=0)  break;
					    if(str[0]<'0' || str[0]>'9')  break;
					    maxlistnum = strtonum(str);
					  } break;
				case 'B': ifsave = 1-ifsave;
					  replacexmenu(xmenu, 3, "GB", ifsave==0?"B�˳�ʱ�Զ���������(OFF)|":
						   "B�˳�ʱ�Զ���������(ON)|");
					  msg_box("������ϣ�", ifsave==0?"�Ѿ��ر����Զ����濪�أ�":
					      "�Ѿ������Զ����濪�أ�");
					  saveset();
					  break;
				case 'C': ifsound = 1-ifsound;
					  replacexmenu(xmenu, 3, "GC", ifsound==0?
					    "C��������(�ѹر�)":"C��������(�Ѵ�)");
					  break;
				case 'D':  switch(getselect()[2])       //Ԥ�����ļ���
					    { case 'A': input_box("������Ŀ¼�ļ�����", dirfile, 20);  break;
					      case 'B': input_box("����������궨���ļ�����", commandfile, 20); break;
					      case 'C': input_box("������ͨ�ú궨���ļ�����", includefile, 20); break;
					      case 'D': input_box("����������ļ�����", helpfile, 20);  break;
					    } break;
				case 'E': switch(getselect()[2])  //���ñ�����ɫ
					   { case 'A': setbkcolor(bkcolor=getselect()[3]-'0'); break;
					     case 'B': setbkcolor(bkcolor=getselect()[3]-'0'+8); break;
					     case 'C': setbkcolor(bkcolor=BLUE); break;
					   }  break;
			      } break;
		 }
     }
   if(ixmenuopen(xmenu))  return;
   for(k=0; k<4; k++)
     { if(button_click(button2+k))
	 { switch(k)
	     { case 0: if(ifend==1)
			{ resetsel(nowselect--);  showflag = 0;  asktit(); }
		       else if(ifstart==1)
			{ starttest(--nowselect); test_buttonenable(); }
		       break;
	       case 1: if(ifend==1)
			{ resetsel(nowselect++);  showflag = 0; asktit(); }
		       else if(ifstart==1)
			{ starttest(++nowselect); test_buttonenable(); }
		       break;
	       case 2: goto  test;
	       case 3: show_qk(); break;
	     }
	 }
     }
   if(ifhelp==1)  return;

   for(k=0; k<4; k++)
     { if(button_click(but_end+k))
	{ int i;  unablex(0, "lnm");
	  for(i=0; i<3; i++)
	    if(i!=k) { but_end[i].attrib &= ~BD; showbutton(but_end+k); }
	  switch(k)
	   { case 0: showPQ(); break;
	     case 1: showPP(); break;
	     case 2: showPE(); break;
	     case 3: asktit(); break;
	   }
	}
    }
   if(button_click(&but_begin))   addstartmenu();
   if(list.attrib&BY)
    if((k=selectlist(&list))>=0)
      { reset_qk();
	if(ifend==1)
	 { nowselect = k;  showflag = 0;
	   list.attrib = 0;  asktit();
	 }
	else if(ifstart==1)
	 { starttest(nowselect=k); test_buttonenable(); }
	while(getmousebutton()!=NOBUTTON);
      }
    else if(k==-2)
      { if(ifend==1)  { list.attrib = 0;  asktit(); }
	else if(ifstart==1)
	 { starttest(nowselect); test_buttonenable(); }
      }
   if(ishowqk==1 && !(list.attrib&BY) )
    { for(k=0; k<=titnum; k++)
       if( button_click(blist+k) )
	 {  if(ifend==1)
	       { nowselect = k;  showflag = 0;  asktit(); }
	    else
	       { starttest(nowselect=k); test_buttonenable(); }
	    break;
	 }
    }
   if(ifstart && !ishowqk && button2[3].attrib&BY && !(list.attrib&BY))
    { if(button_click(&renew) && titptr[nowselect].time)
      { for(k=0; k<titptr[nowselect].sel.num; k++)
	{ titptr[nowselect].sel.text[k].attrib &= ~BD;
	  test_select(&titptr[nowselect].sel, k);
	}
      }
      if(button_click(&complete))
      { starttest(++nowselect); test_buttonenable(); }
      if(button_click(&allend))   goto  _allend;
    }
 }

void  addstartmenu()
  { char  str[30];
    if(titnum<=0)
      { msg_box("����", "���ײ�����û�з����κ����⣬�����ļ���д�Ƿ���ȷ��"
	 "����д�������Ժ������´򿪣�");
	return;
      }
    but_begin.attrib &= ~BY;
    if(ifstart==0)
     { if(ifenablepause==1)  autoxmenu(xmenu, 0, "E");
       unablex(0, "SX");
       addxmenu(xmenu, 0, "1>", aTime>0?"C��ǰ����|-2|":"C���ڽ���|x��λ|-2|");
       addxmenu(xmenu, 0, "L<", "-4|^l��ѯǰһ��|^n��ѯ��һ��|^m����Ŀ���ݲ�ѯ|-5|");
       cleardesktop(0);
       top_title("���Խ�����");
       unablex(1, "ABCDEFGHIJKLMNOPQXYfj");
       enablebutton(button2+2);
       enablebutton(button2+3);
       autoxmenu(xmenu, 0, "K");
       autoxmenu(xmenu, 3, "BA"); unablexmenu(xmenu, 3, "BB");
       unablexmenu(xmenu, 3, "BC"); unablexmenu(xmenu, 3, "BD");
       mouse_off();
       setfillstyle(0, 0);
       bar(0, 461, 200, 479);
       setcolor(LIGHTCYAN);
       show_str(5, 462, "��ʣ��ʱ��:");
       if(aTime<0)  show_str(105, 462, "(������)");
       else
	{ sprintf(str, "%d��%d��", aTime/60,  aTime%60);
	  show_str(105, 462, str);
	}
       mouse_on();
       aT = clock();  asecond = aTime;
       ifstart = 1;  starttest(0);
       ifcontinue = bkifcontinue = 1;
     }
    else
     { char str[30];
       cleardesktop(0);
       but_begin.caption = "����˴���ʼ����";
       top_title("���Խ�����");
       showtit(nowselect);
       setfillstyle(0, 0);
       bar(30, 80, 30+20*8, 80+16);
       sprintf(str, "���%d��(��%d��)", nowselect+1, titnum+1);
       setcolor(YELLOW);
       mouse_off();
       show_str(30, 80, str);
       bar(450, 80, 620, 96);
       show_str(450, 80, "ʣ��ʱ��:");
       setcolor(LIGHTGREEN);
       if(titptr[nowselect].time<0)
	 show_str(450+75, 80, "(������)");
       else if(titptr[nowselect].time==0)
	 show_str(450+75, 80, "(ʱ���ѵ�)");
       else
	 { sprintf(str, "%d��%d��", second/60, second%60);
	   show_str(450+75, 80, str);
	 }
       mouse_on();
       delxmenu(xmenu, 0, "W");
       autox(0, "KEC");
       enablebutton(button2+2);
       enablebutton(button2+3);
       T += clock()-T0;  test_buttonenable();
       ifcontinue = bkifcontinue;
     }
 }

int  starttest(int select)
 {
/*************  ��ʾ��Ŀ�����  *************************/
  if(select>=0)
    { char str[30];
      setfillstyle(0, 0);
      bar(LEFT, 105, RIGHT, BOTTOM);
      top_title("���Խ�����");
      showtit(nowselect);
      setfillstyle(0, 0);
      bar(30, 80, 30+20*8, 80+16);
      sprintf(str, "���%d��(��%d��)", nowselect+1, titnum+1);
      setcolor(YELLOW);
      mouse_off();
      show_str(30, 80, str);
      bar(450, 80, 620, 96);
      show_str(450, 80, "ʣ��ʱ��:");
      setcolor(LIGHTGREEN);
      if(titptr[nowselect].time0<0)
	show_str(450+75, 80, "(������)");
      else if(titptr[nowselect].time==0)
	show_str(450+75, 80, "(ʱ���ѵ�)");
      else
	{ sprintf(str, "%d��%d��",
	    titptr[nowselect].time/60, titptr[nowselect].time%60);
	  show_str(450+75, 80, str);
	}
      mouse_on();
      T = clock();  second = titptr[nowselect].time;
      test_buttonenable();
    }
/*****************  ��ʾ��Ŀ����ν���  ********************/

/*****************  ���ʱ�����ο�ʼ  ********************/
  if(select==-2&&second!=0)
   { if((clock()-T>18L||second==0) && ifcontinue==1)
      { char str[20];
	long C = clock();
	second -= (C-T)/18L;
	titptr[nowselect].passtime += (C-T)/18L;
	T = clock() + (C-T)%18L;
	if(second<0 && titptr[nowselect].time>=0)  second = 0;
	titptr[nowselect].time = second;  //����ʣ��ʱ��
     //��ʾʱ�俪ʼ
	if(titptr[nowselect].time0>=0 && !ishowqk)
	 { mouse_off();
	   setfillstyle(0, 0);
	   bar(450+75, 80, 620, 96);
	   setcolor(LIGHTGREEN);
	   sprintf(str, "%d��%d��", second/60, second%60);
	   show_str(450+75, 80, str);
	   mouse_on();
     //��ʾʱ�����
	  if(second<=10&&second>0)  showsound(0);
	  if(second==0)
	   { showsound(1);
	     mouse_off();
	     setfillstyle(0, 0);
	     bar(450+75, 80, 620, 96);
	     setcolor(LIGHTGREEN);
	     show_str(450+75, 80, "(ʱ���ѵ�)");
	     renew.attrib &= ~BY;
	     if(button2[3].attrib&BY)  showbutton(&renew);
	     mouse_on();
	     return 1;
	   }
	 }
       }
     }
  /*****************  ���ʱ�����ν���  *****************/

   if(!ixmenuopen(xmenu)&&second&&!ishowqk&&(button2[3].attrib&BY))
      select_click(&titptr[nowselect].sel);
   if(aTime>0 && clock()-aT>18L && ifcontinue==1)
    { char str[20];
      long C = clock();
      asecond -= (C-aT)/18L;
      aT = clock() + (C-aT)%18L;
      if(asecond<0)  asecond = 0;
      mouse_off();
      setfillstyle(0, 0);
      bar(105, 461, 200, 479);
      setcolor(LIGHTCYAN);
      sprintf(str, "%d��%d��", asecond/60,  asecond%60);
      show_str(105, 462, str);
      mouse_on();
      if(asecond==remindtime)
       { char str[100];
	 sprintf(str, "�����ʱ�仹��%d��%d�룡", asecond/60, asecond%60);
	 showsound(4);
	 msg_box("��ע�⣺", str);
       }
      if(asecond<=0)
       { showsound(4);
	 msg_box("ʱ���ѵ�", "�밴�������������˳�����...");
	 bar(0, 461, 200, 479);
	 second = 0;    ifend = 1;  ifstart = 0;  test_end();
       }
     }
   return 1;
 }

void  test_end()
 { /***********  ��Ŀ����������´���  ***********/
   int k;
   ifend = 1; ifcontinue = 1; ifstart = 0;
   delx(0, "SEC42x");  cleardesktop(0);
   unablebutton(button2+3);  unablexmenu(xmenu, 0, "K");
   addxmenu(xmenu, 0, "l<", "w����������|p�鿴����ϸ��|"
     "e���ܽ�|a��ѯ�������|-4|r���²���|-6|");
   addxmenu(xmenu, 0, "r>", "x��λ|");
   unablex(0, "lnm");
   for(k=0; k<4; k++) but_end[k].attrib |= BY;
   if(PQ==NULL)
    { unablexmenu(xmenu, 0, "w"); but_end[0].attrib &= ~BY; }
   if(PU==NULL)
    { unablexmenu(xmenu, 0, "p"); but_end[1].attrib &= ~BY; }
   if(PE==NULL)
    { unablexmenu(xmenu, 0, "e"); but_end[2].attrib &= ~BY; }
    for(k=0; k<4; k++)  showbutton(but_end+k);
    nowselect = 0;      show_end();
    setfillstyle(0, 0); bar(0, 461, 200, 479);
   /************ ��Ŀ����������ϴ��� *************/
 }


void  write_hb()
 { int k;
   setcolor(LIGHTGREEN);
   off();
   for(k=0; k<37; k++)
    { show_word(16*(k+1)+7, 55, "��");
      show_word(16*(k+1)+7, 449, "��");
    }
   for(k=0; k<23; k++)
    { show_word(2, 75+16*k, "��");
      show_word(621, 75+16*k, "��");
    }
   show_word(7, 60, "��");  show_word(614, 60, "��");
   show_word(7, 443, "��"); show_word(614, 443, "��");
   on();
 }
