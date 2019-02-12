#include <stdio.h>
#include <bios.h>
#include <graphics.h>
#include <dos.h>
#include <dir.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <alloc.h>
#include <mouse.h>
#include <key.h>
#include "tclass.h"

enum EXAMFLAG { Program, Change };
enum MODEL    { Testmodel, Editmodel };
enum EXAM     { Closed, Opened };

BOOL msgbox(char *str, char backcolor=CYAN, char forcolor=YELLOW, int kind=0);

void msg(char *str="��ʾ", char backcolor=CYAN, char forcolor=WHITE, int Ti=10);
char * getfilename(char *filename, char *searchpath=".", char *head="", char *hz="*", char *te=NULL);
void  bar3ds(int x1, int y1, int x2, int y2, BOOL flag=down, int color=-1);
void addgroups(T_List *list, char *str=NULL);
void  buildlist(), examchange(), showhead(int);

extern char exampath[20];   //����·��
extern char deskfile[30];   //�����ļ�

extern EXAMFLAG Examflag;
extern MODEL    Model;
extern EXAM     Exam;
extern BOOL msgsee, msgboxsee, autosave, waitclick;
extern T_Time _time;
extern int autoreset, compilemodel, process;

extern T_Tx  screen();
void  savesourcefile(char *s);
BOOL  windowdeal(int i), Tcedit(), windowdeal(T_Button &);
BOOL  readin(T_Button *win);
int   xmenudeal(char *str);
extern BOOL  start;
BOOL  Compile(), runexefile();
int   readcompilemsg(char *filename);
void  editforlinenumber(char *str), timedeal();
BOOL  saverunscreen(char *filename), readrunscreen(char *filename);
BOOL  showrunscreen();

extern BOOL  ucdosflag, bucdosflag;
extern BOOL  Haveseen,  Havechange;
extern BOOL  isound;
extern char  *blockptr;

extern T_xMenu  *xmenu;
extern T_Button _compile, _run,  _reset,  _left,  _right;
extern T_Button _showwindow, _tcwindow, _editwindow, _compilewindow,
   _runwindow,  _helpwindow;


extern T_Button *winptr[];
void  groupattrib_1(BOOL Fa), groupattrib_2(BOOL Fa);
extern T_Combo  *list;

extern int  Num;
extern T_EditBox editbox, showbox, helpbox, deskbox, compilebox;
extern T_Select select;

void waitforclick()
 { while(!kbhit() && getmousebutton()==NOBUTTON);
   while(kbhit())  getch();  while(getmousebutton()!=NOBUTTON);
 }

void showbuttons()
 { if(getmouse_y()>450)
    { mouse_off();  setfillstyle(0, 0);
      bar(0, 452, 639, 479);  mouse_on();
    }
   _compile.show();  _run.show();   _reset.show();
   for(int k=0; k<6; k++)  winptr[k]->show();
   T_Label(390, 455, "ִ��->", LIGHTGREEN);
   T_Label(6, 455, "����->", LIGHTCYAN);
 }

void showallbuttonsup()
 { for(int k=0; k<6; k++)
    { if(winptr[k]->idown()==down)
	if(ucdosflag==false)  winptr[k]->show(up);
	else                  winptr[k]->A = up;
    }
 }

void showbutton(T_Button &button, BOOL _A)
 { if(ucdosflag==false)  button.show(_A);
   else  button.A = _A;
   if(Model==Editmodel&&Exam==Opened&&(strcmp(button.str, "�༭")==0) )
      { xmenu->attrib("�����ʽ", _A?0:Wx);
	xmenu->attrib("��ע��", _A?0:Wx);
      }
 }

void _able(T_Button &button, BOOL _W)
 { if(ucdosflag==false)  button.able(_W);
   else button.W = (BOOL)(1-_W);
  }

void addfiles(T_List *list, char *hz, T_Text &text)
 { struct ffblk Ffblk;  int flag = 0;
   if(findfirst("*.*", &Ffblk, FA_DIREC|FA_ARCH)==-1)  return;
   do { char name[20];
	if(Ffblk.ff_attrib&FA_DIREC)  sprintf(name, "<%s>", Ffblk.ff_name);
	else
	 { strcpy(name, Ffblk.ff_name);
	   if(strcmp(hz,"*")!=0)
	     { int len = strlen(name), k;
	       for(k=0; k<len; k++)
		  if(name[k]=='.')  break;
	       if(k==len)  continue;
	       if(strcmp(name+k+1, hz))  continue;
	     }
	 }
	list->add(name);
	if(!(Ffblk.ff_attrib&FA_DIREC) && flag++==0)
	  { list->select(name); text.str(name); }
      } while(findnext(&Ffblk)!=-1);
 }
//��ȡ�ļ���
char * getfilename(char *filename, char *searchpath, char *head, char *hz, char *te)
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-110, __x2=320+110, __y1=240-123, __y2=240+123;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   print(__x1+5, __y1+5, "������:");
   T_List *drivername = new T_List(__x1+10, __y1+30, __x1+80, __y2-60);
   int  disk = getdisk();
   char path[200];  getcurdir(0, path);
   for(int disks=0; disks<26; disks++)
    { if(disks==1)  continue;
      setdisk(disks);  char driver[5] = "[X:]";
      if(disks==getdisk())
	{ driver[1] = 'A'+disks;
	  if(disk==getdisk())  drivername->add(driver, Ax);
	  else                 drivername->add(driver);
	}
    }
   setdisk(disk);  chdir(searchpath);
   setcolor(WHITE);  print(__x1+85, __y1+5, "�ļ��м��ļ�:");
   T_List *file = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
   setcolor(YELLOW);
   print(__x1+10, __y2-25, *head?head:"ѡ���ļ�");
   setcolor(WHITE);  print(__x1+10, __y2-50, "�ļ���:");
   T_Text text(__x1+80, __y2-50, __x2-__x1-90, te?te:"", 0, CYAN);
   T_Button _ok(__x1+120, __y2-25, "ȷ��", DEF, CYAN);
   T_Button _no(__x1+170, __y2-25, "ȡ��", DEF, CYAN);
   addfiles(file, hz, text);
   mouse_on();
   if(!te)  text.edit();
   while(1)
    { timedeal();
      char ch=0;  if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY || ch==SPACE_KEY)
	{ char path[200];  getcurdir(0, path);
	  if(text.str()[0]=='\0'||text.str()==NULL)  filename = NULL;
	  else sprintf(filename, *path?"%c:\\%s\\%s":"%c:%s\\%s",
		   getdisk()+'A', path, text.str());
	  for(int k=0; k<strlen(text.str()); k++)
	     if(text.str()[k]=='.') break;
	  if(k==strlen(text.str()))
	    { strcat(filename, ".");  strcat(filename, hz); }
	  break;
	}
      if(_no.testclick() || ch==ESC_KEY) { filename=NULL; break; }
      if(drivername->testclick())
       { setdisk(drivername->str[1]-'A');
	 delete file;
	 file = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
	 if(!te) text.str("");  addfiles(file, hz, text);
       }
      if(file->testclick())
       { char str[15];
	 if(file->str[0]=='<')
	   { strcpy(str, file->str+1); str[strlen(str)-1]='\0'; chdir(str);
	     delete file;
	     file = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
	     if(!te) text.str("");  addfiles(file, hz, text);
	   }
	 else if(!te) text.str(file->str);
       }
      if(te==NULL)  text.testclick();
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);
   delete ptr; setdisk(disk); chdir("\\"); chdir(path);
   mouse_on();   GRAPHSETTING;
   delete file;  delete drivername;
   return filename;
 }

void adddirs(T_List *list)
 { struct ffblk Ffblk;
   findfirst("*.*", &Ffblk, FA_DIREC);
   do
    { if(Ffblk.ff_name[0]!='.'&&Ffblk.ff_attrib&FA_DIREC)
	 list->add(Ffblk.ff_name);
    } while(findnext(&Ffblk)!=-1);
 }
//��ȡĿ¼��
char * getdirname(char *dirname, char *head, EXAMFLAG &Examflag, BOOL iedit)
 { GRAPHGETTING;  mouse_off();  EXAMFLAG bE = Examflag;
   const int __x1=320-110, __x2=320+110, __y1=240-123, __y2=240+123;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   char Epath[50];  sprintf(Epath, "exam\\%s", exampath);  chdir(Epath);
   chdir(Examflag==Program?"Program":"Change");
   print(__x1+5, __y1+5, "����:");
   T_Select kind(__x1+5, __y1+40, 0, CYAN, WHITE);
   kind.add("�����", Examflag==Program?Ax:0);
   kind.add("�Ĵ���", Examflag==Change?Ax:0);
   setcolor(WHITE);  print(__x1+85, __y1+5, "�ļ��м��ļ�:");
   T_List *dir = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
   setcolor(YELLOW);
   print(__x1+10, __y2-25, *head?head:"ѡ������");
   setcolor(WHITE);  print(__x1+10, __y2-50, "�ļ���:");
   T_Text text(__x1+80, __y2-50, __x2-__x1-90, "", 0, CYAN);
   T_Button _ok(__x1+120, __y2-25, "ȷ��", DEF, CYAN);
   T_Button _no(__x1+170, __y2-25, "ȡ��", DEF, CYAN);
   adddirs(dir);   mouse_on();   if(iedit==true) text.edit();
   while(1)
    { timedeal();
      char ch = 0;  if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY || ch==SPACE_KEY)
	{ if(text.str()[0]=='\0'||text.str()==NULL)  dirname = NULL;
	  else  strcpy(dirname, text.str());  break;
	}
      if(_no.testclick() || ch==ESC_KEY) { dirname=NULL; Examflag = bE; break; }
      if(dir->testclick())  text.str(dir->str);
      if(kind.testclick())
       { if(strcmp(kind.str, "�����")==0)  Examflag = Program;
	 else                               Examflag = Change;
	 chdir("..");   chdir(Examflag==Program?"Program":"Change");
	 delete dir;  dir = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
	 adddirs(dir);
       }
      if(iedit==true)  text.testclick();
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);
   delete ptr; chdir("..\\..\\..");
   mouse_on();   GRAPHSETTING;   delete dir;
   xmenu->select(Examflag==Program?"�����":"�Ĵ���");
   return dirname;
 }


void addgroups(T_List *list, char *str)
 { if(str)  strupr(str);
   struct ffblk Ffblk;  int K = 0;
   findfirst("*.*", &Ffblk, FA_DIREC);
   do
    { if(Ffblk.ff_name[0]!='.'&&Ffblk.ff_attrib&FA_DIREC)
	{ if(str!=NULL && strcmp(str, Ffblk.ff_name)==0)
	     list->add(Ffblk.ff_name, Ax);
	  else if(str==NULL && K++==0)
	     list->add(Ffblk.ff_name, Ax);
	  else list->add(Ffblk.ff_name);
	}
    } while(findnext(&Ffblk)!=-1);
 }


//������������
char * getgroupname(char *groupname, char *head)
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-70, __x2=320+70, __y1=240-100, __y2=240+100;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   chdir("exam");
   T_List *dir = new T_List(__x1+10, __y1+30, __x2-10, __y2-60);
   setcolor(YELLOW);
   print(__x1+30, __y1+10, head);
   T_Button _ok(__x2-120, __y2-25, "ȷ��", DEF, CYAN);
   T_Button _no(__x2-60, __y2-25, "ȡ��", DEF, CYAN);
   T_Text text(__x1+10, __y2-50, __x2-__x1-20, exampath, 0, CYAN);
   addgroups(dir, strupr(exampath));   mouse_on();
   while(1)
    { timedeal();
      char ch = 0;  if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY || ch==SPACE_KEY)
	{ if(text.str()[0]=='\0'||text.str()==NULL)  groupname = NULL;
	  else  strcpy(groupname, text.str());  break;
	}
      if(_no.testclick() || ch==ESC_KEY) { groupname=NULL; break; }
      if(dir->testclick())  text.str(dir->str);
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);
   delete ptr; chdir("..");
   mouse_on();   GRAPHSETTING;   delete dir;
   xmenu->select(Examflag==Program?"�����":"�Ĵ���");
   return groupname;
 }

#define NUMERROR 0x8fff
//��������
int getnum(char *Head, char *str)
 { int x1=320-100, x2=320+100, y1=240-40, y2=240+40;  int num=NUMERROR;
   char *ptr = new char[imagesize(x1, y1, x2+5, y2+5)];
   if(ptr==NULL)  return 0xffff;
   mouse_off();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, CYAN);      bar(x1, y1, x2, y2);
   setcolor(WHITE);            rectangle(x1, y1, x2, y2);
   print(x1+10, y1+10, Head);
   T_Text  text(x1+10, y1+30, x2-x1-20, "", 0, CYAN);
   T_Button _ok (320-95, y2-25, " ȷ �� ", 0, CYAN, BLUE);
   T_Button _no (320+30, y2-25, " ȡ �� ", 0, CYAN, BLUE);
   mouse_on();
   text.edit();
   while(1)
    { timedeal();
      int ch = 0;  if(kbhit())   ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY )
	if(str==NULL)
	   { if(sscanf(text.str(), "%d", &num)==EOF)  num = NUMERROR; break; }
	else { strcpy(str, text.str()); num = 0; break; }
      if(_no.testclick() || ch==ESC_KEY )  { num = NUMERROR;  break; }
      text.testclick();
    }
   mouse_off();
   putimage(x1, y1, ptr, COPY_PUT);   delete ptr;
   mouse_on();   return num;
 }


void xline(int x1, int y1, int x2, int y2)
 { int color = getcolor();
   setcolor(LIGHTGRAY);  line(x1, y1, x2, y2);
   setcolor(DARKGRAY);
   if(y1==y2)       line(x1, y1+1, x2, y2+1);
   else if(x1==x2)  line(x1+1, y1, x2+1, y2);
   setcolor(color);
 }

void clearfile(char *path, char *arc)
 { struct ffblk Ffblk;
   char str[50];
   sprintf(str, "%s\\%s", path, arc);
   if(findfirst(str , &Ffblk, 0)==-1)  return;
   do
    { sprintf(str, "%s\\%s", path, Ffblk.ff_name);
      if(!(Ffblk.ff_attrib&FA_RDONLY))  unlink(str);
    } while(findnext(&Ffblk)!=-1);
 }

#define _Wr  fputc(*ptr++, fp)
void savescreen(char *file)
 { FILE *fp = fopen(file, "wb");
   if(fp==NULL)  return;
   mouse_off();
   outportb(0x3ce, 4);
   for(int k=0; k<4; k++)
    { outportb(0x3cf, k);
      char far *ptr = (char far *)0xa0000000L;
      for(register int i=0; i<480*8; i++)
       { _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; _Wr; }
    }
   fclose(fp);  mouse_on();
 }

#define _Re  *ptr++ = fgetc(fp)
void writescreen(char *file)
 { FILE *fp = fopen(file, "rb");
   if(fp==NULL)  return;
   mouse_off();
   outportb(0x3c4, 2);
   for(int k=0; k<4; k++)
    { outportb(0x3c5, 0x01<<k);
      char far *ptr = (char far *)0xa0000000L;
      for(register int i=0; i<480*8; i++)
       { _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; _Re; }
    }
   outportb(0x3c5, 0x0f);
   fclose(fp);  mouse_on();
 }
#undef _Re
#undef _Wr


int xstrcmp(char *ptr1, char *ptr2)
 { while(*ptr2)  if(*ptr1++!=*ptr2++)  return 0;  return 1; }


BOOL msgbox(char *str, char backcolor, char forcolor, int kind)
 { T_LineEdit::shines(off);
   char S[10][40];  int K=0, j=0, k=0;  BOOL flag = false;
   while(str[K])
    { S[j][k++] = str[K];
      if(str[K]&0x80)  flag = (BOOL)(1-flag);
      if(k==38-(j?0:4)&&flag==false || k==39-(j?0:4))
	{ S[j++][k] = '\0';
	  k = 0;
	}
      K ++;
    }
   S[j][k] = '\0';
   int width = 40*8+10, high = (j+1)*20+50;
   GRAPHGETTING;   mouse_off();
   int x1 = 320-width/2, x2 = x1+width, y1 = 240-high/2, y2 = y1+high;
   char *ptr = new char[imagesize(x1, y1, x2+5, y2+5)];
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, backcolor); bar(x1, y1, x2, y2);
   setcolor(WHITE);  rectangle(x1, y1, x2, y2);
   setcolor(forcolor);
   for(int i=0; i<=j; i++) print(x1+12+(i?0:32), y1+15+i*20, S[i]);
   T_Button _ok (320-95, y2-25, kind==0?" < �� > ":kind==1?" ȷ �� ":" �� װ ", 0, CYAN, BLUE);
   T_Button _no (320+30, y2-25, kind==0?" < �� > ":" ȡ �� ", 0, CYAN, BLUE);
   mouse_on();   BOOL RES;
   while(1)
    { timedeal();
      if(_ok.testclick()) { RES = yes; break; }
      if(_no.testclick()) { RES = no;  break; }
      if(kbhit())
       { char ch = getch();
	 if(ch==ESC_KEY)                  { RES = no; break; }
	 if(ch==ENTER_KEY||ch==SPACE_KEY) { RES = yes;  break; }
       }
    }
   mouse_off();
   putimage(x1, y1, ptr, COPY_PUT);
   delete ptr;
   mouse_on();    GRAPHSETTING;   return RES;
 }

void msg(char *str, char backcolor, char forcolor, int Ti)
 { if(msgsee==false)  return;
   int lenth = strlen(str)*8+20;
   int x1 = 320-lenth/2, x2 = 320+lenth/2, y1 = 240-15, y2 = 240+15;
   char *ptr = new char[imagesize(x1, y1, x2+5, y2+5)];
   mouse_off();  GRAPHGETTING;
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, backcolor); bar(x1, y1, x2, y2);
   setcolor(WHITE);  rectangle(x1, y1, x2, y2);
   setcolor(forcolor);  print(x1+10, 240-8, str);
   mouse_on();
   if(Ti>=0)  { long T = clock();  while(clock()-T<(long)Ti); }
   mouse_off();
   if(Ti>=0)  putimage(x1, y1, ptr, COPY_PUT);
   delete ptr;
   mouse_on();
 }
inline void msg(char *str, int Ti)  { msg(str, CYAN, WHITE, Ti); }

void saveall();
void askforsave()       //ѯ���Ƿ����
 { if(T_EditBox::ichange==true && Model==Editmodel)
     if(msgbox("��ǰ������޸Ĳ���δ���棬�Ƿ����ڱ��棿")==true)  saveall();
 }

extern int backcolor, headcolor;

void renewxmenu()
 { xmenu->attrib("��ʾʱ��", _time.runenable==true?Ax:0);
   xmenu->attrib("��������", isound==true?Ax:0);
   xmenu->attrib("�Զ�����", T_EditBox::Line==true?Ax:0);
   xmenu->select(T_EditBox::TIME==1?"����":T_EditBox::TIME==2?"����":"����");
   xmenu->attrib("��ʾ��ǩ��ʾ", msgsee==true?Ax:0);
   xmenu->select(autoreset==0?"������ʱѯ���Ƿ��ʼ��":autoreset==1?
	   "������ʱ�Զ���ʼ��":"������ʱ����ʼ��");
   xmenu->attrib("��ʾ��Ϣ����ʾ", msgboxsee==true?Ax:0);
   xmenu->select(autosave==true?"�˳�ʱ�Զ�����":"�˳�ʱ���Զ�����");
   xmenu->select(waitclick==false?"��������":"�ȴ������򵥻�");
   xmenu->select(compilemodel==0?"С��":"΢��");
   showhead(headcolor);
 }

void resetmsg();
void readmsg()      //��ȡ��Ϣ
 { int code = 0;
   if(access("temp\\process\\set.msg", 0)==0)  code = 1;
   if(code==0)  msg("��ȡ����...");
   FILE *fp = fopen(code==0?"set.msg":"temp\\process\\set.msg", "rt");
   if(fp==NULL)
     { mouse_on();  msgboxsee = false;
       msgbox("���������ļ�δ�ҵ�����ʹ��ϵͳĬ�����ã�", CYAN, WHITE, 1);
       resetmsg();  mouse_off();   msgboxsee = true;    return;
     }
   fscanf(fp, "EXAMPATH: %s\n", &exampath);
   fscanf(fp, "SHOWTIME: %d\n", &_time.runenable);
   fscanf(fp, "SOUND: %d\n", &isound);
   fscanf(fp, "AUTOLINE: %d\n", &T_EditBox::Line);
   fscanf(fp, "SHINESPEED: %d\n", &T_EditBox::TIME);
   fscanf(fp, "MSGSEE: %d\n", &msgsee);
   fscanf(fp, "MSGBOXSEE: %d\n", &msgboxsee);
   fscanf(fp, "AUTORESET: %d\n", &autoreset);
   fscanf(fp, "AUTOSAVE: %d\n", &autosave);
   fscanf(fp, "DESKFILE: %s\n", deskfile);

   fscanf(fp, "DESKBOXCOLOR: %d\n", &deskbox.forcolor);
   fscanf(fp, "EDITBOXCOLOR: %d\n", &editbox.forcolor);
   fscanf(fp, "SHOWBOXCOLOR: %d\n", &showbox.forcolor);
   fscanf(fp, "COMPILEBOXCOLOR: %d\n", &compilebox.forcolor);
   fscanf(fp, "HELPBOXCOLOR: %d\n", &helpbox.forcolor);

   fscanf(fp, "BACKCOLOR: %d\n", &backcolor);
   fscanf(fp, "WAITFORCLICK: %d\n", &waitclick);
   fscanf(fp, "COMPILEMODEL: %d\n", &compilemodel);
   if(backcolor!=getbkcolor())   setbkcolor(backcolor);

   fscanf(fp, "HEADCOLOR: %d\n", &headcolor);
   fclose(fp);  renewxmenu();
   process = code;  if(code==1)  unlink("temp\\process\\set.msg");
 }

void resetmsg()    //�ָ�Ĭ������
 { if(msgboxsee==true &&
      msgbox("����ܽ�������ϵͳ���ûָ�ΪĬ�����ã�������������������"
       "����Ϣ���Ƿ������")==false)  return;
   _time.runenable = true;   isound = true;
   T_EditBox::Line = true;   T_EditBox::TIME = 2L;
   msgsee = true;            msgboxsee = true;
   autoreset = 0;            autosave = true;
   editbox.forcolor = GREEN;   deskbox.forcolor = CYAN;
   showbox.forcolor = YELLOW;  helpbox.forcolor = WHITE;
   compilebox.forcolor = LIGHTCYAN;   waitclick = true;
   compilemodel = 0;    headcolor = CYAN;
   renewxmenu();
   for(int k=0; k<6; k++)  if(winptr[k]->idown())  break;
   if(k==6)   readin(NULL);   else readin(winptr[k]);
   setbkcolor(backcolor=BLUE);  
 }


void savemsg(int code)     //������Ϣ
 { if(code==0)  msg("��������...");
   FILE *fp = fopen(code==0?"set.msg":"temp\\process\\set.msg", "wt");
   fprintf(fp, "EXAMPATH: %s\n", exampath);
   fprintf(fp, "SHOWTIME: %d\n", _time.runenable);
   fprintf(fp, "SOUND: %d\n", isound);
   fprintf(fp, "AUTOLINE: %d\n", T_EditBox::Line);
   fprintf(fp, "SHINESPEED: %d\n", T_EditBox::TIME);
   fprintf(fp, "MSGSEE: %d\n", msgsee);
   fprintf(fp, "MSGBOXSEE: %d\n", msgboxsee);
   fprintf(fp, "AUTORESET: %d\n", autoreset);
   fprintf(fp, "AUTOSAVE: %d\n", autosave);
   fprintf(fp, "DESKFILE: %s\n", deskfile);

   fprintf(fp, "DESKBOXCOLOR: %d\n", deskbox.forcolor);
   fprintf(fp, "EDITBOXCOLOR: %d\n", editbox.forcolor);
   fprintf(fp, "SHOWBOXCOLOR: %d\n", showbox.forcolor);
   fprintf(fp, "COMPILEBOXCOLOR: %d\n", compilebox.forcolor);
   fprintf(fp, "HELPBOXCOLOR: %d\n", helpbox.forcolor);

   fprintf(fp, "BACKCOLOR: %d\n", backcolor);
   fprintf(fp, "WAITFORCLICK: %d\n", waitclick);
   fprintf(fp, "COMPILEMODEL: %d\n", compilemodel);

   fprintf(fp, "HEADCOLOR: %d\n", headcolor);
   fclose(fp);
 }

BOOL testexam()
 { struct ffblk Ffblk;
   findfirst("exam\\*.*", &Ffblk, FA_DIREC);
   BOOL flag = false;
   do
    { if(Ffblk.ff_name[0]!='.' && Ffblk.ff_attrib&FA_DIREC)  flag = true;
    } while(findnext(&Ffblk)!=-1);
   return flag;
 }

BOOL installexams()              //�ڿ�ʼʱ��װ
 { void installexam();
   if(testexam()==true)  { xmenu->attrib("�༭ģʽ");  return false; }
   if(msgbox("δ�������⣬�����ԴӰ�װ�̻������ļ��а�װ���⣬������а�װ��"
     "�������ϲ��룬�������װ����ѡ����̣���ѡ�а�װ�ļ������ް�װ�̣�Ҳ��"
     "����ѡ������װ�ļ�����Ҳ���Ե����ȡ�����Լ����������⡣�����ڰ�װ��"
       , CYAN, WHITE, 2)==false)  return true;
   installexam();  xmenu->attrib("�༭ģʽ");  return true;
 }

void addhelpfiles(T_List *list, char *file)
 { struct ffblk Ffblk;  strupr(file);
   if(findfirst("*.*", &Ffblk, 0)==-1)  return;
   do { list->add(Ffblk.ff_name,
	    strcmp(file, Ffblk.ff_name)==0?Ax:0);
    } while(findnext(&Ffblk)!=-1);
 }

int  gethelpfiles(char *helpfile)
 { int code = 1;   char H[30] = "";
   GRAPHGETTING;  mouse_off();
   const int __x1=320-70, __x2=320+70, __y1=240-120, __y2=240+120;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   T_List *help = new T_List(__x1+10, __y1+30, __x2-10, __y2-80);
   setcolor(YELLOW);
   print(__x1+15, __y1+10, "ѡ����������");
   T_Button _ok(__x2-130, __y2-50, "ȷ��", DEF, CYAN);
   T_Button _no(__x2-70, __y2-50, "ȡ��", DEF, CYAN);
   T_Button _add(__x2-110, __y2-25, "����", DEF, CYAN);
   T_Button _del(__x2-50, __y2-25, "ɾ��", DEF, CYAN);
   for(int k=strlen(deskfile)-1; k>=0; k--)  if(deskfile[k]=='\\')  break;
   if(k!=-1)  strcpy(H, deskfile+k+1);
   T_Text text(__x1+10, __y2-75, __x2-__x1-20, H, 0, CYAN);
   addhelpfiles(help, H);   mouse_on();
   while(1)
    { timedeal();
      char ch = 0;  if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY || ch==SPACE_KEY)
	{ if(text.str()[0]=='\0'||text.str()==NULL)  helpfile = NULL;
	  else  strcpy(helpfile, text.str());  code = 1; break;
	}
      if(_no.testclick() || ch==ESC_KEY) { code = 0; break; }
      if(_add.testclick())  { code = 2;  break; }
      if(_del.testclick())
	 { strcpy(helpfile, text.str()); code = -1;  break; }
      if(help->testclick())  text.str(help->str);
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);
   delete ptr;
   mouse_on();   GRAPHSETTING;   delete helpfile;
   return code;
 }

void desktopedit()        //��������
 { if(msgboxsee==true &&
     msgbox("������ѡ��һЩ��Ȥ�ĸ�ʻ���������ɢ�ķ��������ϣ��Դ������"
     "�İ�����Ϣ�������Դӱ�׼Ŀ¼��ѡ��Ҳ����ѡȡһЩ����ļ����ڱ�׼"
     "Ŀ¼�£�Ҫ������")==false)  return;
   char file[30]; int code;   chdir("help\\desktop");
   while( (code=gethelpfiles(file))!=0)
    { if(code==1)
	{ sprintf(deskfile, "desktop\\%s", file);
	  showallbuttonsup();
	  deskbox.read(deskfile, "..");
	  xmenu->select("��ʾ����");
	  deskbox.savetoimage("..\\..\\temp\\window\\desk.win");
	  break;
	}
      else if(code==2)
	{ char file[100];
	  if(getfilename(file, "\\", "ѡ���ı��ļ�")==NULL) continue;
	  if(access(file, 0)==-1)
	    { msgbox("����δ�ҵ����ļ���");
	      continue;
	    }
	  char order[150];
	  sprintf(order, "copy %s . >NUL", file);
	  system(order);
	}
      else
	{ if(msgbox("ȷ��Ҫ����ɾ����")==true)  unlink(file); }
    }
   chdir("..\\..");
 }


char * getexamname(char *examname, char *head)   //��ȡ������
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-100, __x2=320+100, __y1=240-100, __y2=240+100;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   T_List *dir = new T_List(__x1+100, __y1+30, __x2-10, __y2-60);
   T_List *exam = new T_List(__x1+10, __y1+30, __x1+90, __y2-110);
   setcolor(YELLOW);
   print(__x1+30, __y1+10, head);
   T_Button _ok(__x2-160, __y2-25, "ȷ ��", DEF, CYAN);
   T_Button _no(__x2-80, __y2-25, "ȡ ��", DEF, CYAN);
   T_Text text(__x1+10, __y2-50, __x2-__x1-20, "", 0, CYAN);
   T_Select kind(__x1+10, __y2-100, 0, CYAN, WHITE);
   kind.add("�����", Examflag==Program?Ax:0);
   kind.add("�Ĵ���", Examflag==Change?Ax:0);
   chdir("exam");     addgroups(exam, strupr(exampath));
   chdir(exampath);   chdir(Examflag==Program?"program":"change");
   addgroups(dir);    mouse_on();
   while(1)
    { timedeal();
      char ch = 0;  if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY || ch==SPACE_KEY)
	{ if(text.str()[0]=='\0'||text.str()==NULL)  examname = NULL;
	  else  strcpy(examname, text.str());  break;
	}
      if(_no.testclick() || ch==ESC_KEY) { examname=NULL; break; }
      if(dir->testclick())
	{  char str[50];  sprintf(str, "%s\\%s\\%s", exam->str,
	      Examflag==Change?"change":"program", dir->str);
	   text.str(str);
	}
      if(exam->testclick() || kind.testclick())
       { chdir("..\\..");
	 if(strcmp(kind.str, "�����")==0)  Examflag = Program;
	 else                               Examflag = Change;
	 chdir(exam->str);  chdir(Examflag==Program?"program":"change");
	 delete dir;
	 T_List *dir = new T_List(__x1+100, __y1+30, __x2-10, __y2-60);
	 addgroups(dir);
	 char str[50];  sprintf(str, "%s\\%s\\%s", exam->str,
		 Examflag==Change?"change":"program", dir->str);
	 text.str(str);
       }
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);
   delete ptr; chdir("..\\..\\..");
   mouse_on();   GRAPHSETTING;   delete dir;  delete exam;
   xmenu->select(Examflag==Program?"�����":"�Ĵ���");
   buildlist();   return examname;
 }


extern BOOL findall, unupsee;
extern int  findflag;
char * getstr(char *str, char *head, char *str2)   //��ȡѰ���ַ���
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-100, __x2=320+100,
       __y1=240-55-(str2?15:0), __y2=240+55+(str2?15:0);
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   setcolor(YELLOW);   print(__x1+10, __y1+5, head);
   T_Button _ok(__x2-110, __y2-25, "ȷ��", DEF, CYAN);
   T_Button _no(__x2-50, __y2-25, "ȡ��", DEF, CYAN);
   T_Button _clear(__x2-190, __y2-25, "���", DEF, CYAN);
   T_Text *text2;
   T_Text text(__x1+10, __y1+30, __x2-__x1-20, str, 0, CYAN, WHITE, false);
   if(str2!=NULL) text2 = new T_Text(__x1+10, __y1+60, __x2-__x1-20, str2, 0, CYAN, WHITE, false);
   T_Select kind(__x1+5, __y2-50, Fx, CYAN, WHITE);
   kind.add("ȫ�̲���",   findall==false?0:Ax);
   kind.add("���Դ�Сд", unupsee==false?0:Ax, __x1+95, __y2-50);
   mouse_on();     text.edit();
   while(1)
    { timedeal();
      int ch = 0;
      if(kbhit())  ch = getch();
      if(_ok.testclick() || ch==ENTER_KEY)
	 { strcpy(str, text.str()); if(*str=='\0')  str = NULL;
	   if(str2!=NULL)  strcpy(str2, text2->str());
	   break;
	 }
      if(_no.testclick() || ch==ESC_KEY)  { str = NULL;  break; }
      if(kind.testclick())
       { if(strcmp("ȫ�̲���", kind.str)==0)  findall = (BOOL)(1-findall);
	 else           unupsee = (BOOL)(1-unupsee);
       }
      if(_clear.testclick())
       { text.str("");   if(text2!=NULL) text2->str(""); }
      text.testclick();
      if(str2!=NULL)  text2->testclick();
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);   delete ptr;
   mouse_on();   if(str!=NULL)   findflag = 0;  
   if(str2!=NULL)   delete  text2;   return str;
 }




int getfontcolor(int color)     //��ȡ��ɫ
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-60, __x2=320+60, __y1=240-100, __y2=240+100;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   setcolor(YELLOW);   print(__x1+10, __y1+5, "ѡ����ɫ:");
   T_Button _ok(__x1+10, __y2-25, "ȷ��", DEF, CYAN);
   T_Button _no(__x1+65, __y2-25, "ȡ��", DEF, CYAN);
   T_List list(__x1+10, __y1+25, __x2-10, __y2-50);
   char colorstr[][20] = { "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN",
     "LIGHTGRAY", "DARKGRAY", "LIGHTBLUE", "LIGHTGREEN", "LIGHTCYAN", "LIGHTRED",
     "LIGHTMAGENTA", "YELLOW", "WHITE" };
   for(int k=1; k<=15; k++)   list.add(colorstr[k-1], color==k?Ax:0);
   bar3ds(__x1+9, __y2-46, __x2-9, __y2-29, down);
   setfillstyle(1, color);   bar(__x1+10, __y2-45, __x2-10, __y2-30);
   mouse_on();
   while(1)
    { timedeal();
      if(_ok.testclick())  break;
      if(_no.testclick())  { color = -1;  break; }
      if(list.testclick())
       { setfillstyle(1, color=list.No+1);
	 bar(__x1+10, __y2-45, __x2-10, __y2-30);
       }
    }
   mouse_off();  putimage(__x1, __y1, ptr, COPY_PUT);   delete ptr;
   mouse_on();   return color;
 }


void  waitforstep()
 { int x1 = 320-150, x2 = 320+150, y1 = 300-10, y2 = 300+10;
   setfillstyle(1, DARKGRAY);  bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, GREEN);     bar(x1, y1, x2, y2);
   setcolor(WHITE);  rectangle(x1, y1, x2, y2);
   setcolor(RED);
   for(int k=x1+1; k<x2; k++)
     {  line(k, y1+1, k, y2-1);  delay(5); }
 }


void buildxmenu()
 { T_Label(0, 23, "��", WHITE);
   xmenu = new T_xMenu;
   T_Menu  *ptr = xmenu->add("�ļ�", 20, 23), *ptr0, *ptr1;
   ptr->add("����...   (Ctrl+N)", Wx);
   ptr->add("��...   (Ctrl+O|F3)", Wx);
   ptr->add("ȫ������  (Ctrl+A|F2)", Wx);
   ptr->add("����Ϊ...", Wx);
   ptr->add("�Ӵ������Ƴ�", Wx);
   ptr->add("-");
   ptr->add("�༭ģʽ (Ctrl+F12)", Wx, 1);
   ptr->add("����ģʽ (Ctrl+F12)", Ax, 1);
   ptr->add("-");
   ptr0 = ptr->add("�����ʽ(��ע���)", Mx|Wx);
   ptr0->add("��־(������ʾ) (F6)");
   ptr0->add("����(�𰸼���ע) (F7)");
   ptr0->add("-");
   ptr0->add("ȡ�������ʽ (Esc)");
   ptr->add("-");
   ptr->add("�˳����� (Shift+Esc)");

   ptr = xmenu->add("�༭", 65, 23);
   ptr0 = ptr->add("�����", Mx|Wx);
   ptr0->add("������� (F4)");
   ptr0->add("�����β (F5)");
   ptr0->add("-");
   ptr0->add("�鸴��");
   ptr0->add("���ƶ�");
   ptr0->add("��ɾ��");
   ptr0->add("��ȡ��");
   ptr0 = ptr->add("��д�ļ�", Mx|Wx);
   ptr0->add("���ļ�...");
   ptr0->add("��д�ļ�...");
   ptr0 = ptr->add("����ƶ�", Mx|Wx);
   ptr0->add("���� (Home)");
   ptr0->add("��β (End)");
   ptr0->add("-");
   ptr0->add("ҳ�� (^Up)");
   ptr0->add("ҳβ (^Down)");
   ptr0->add("-");
   ptr0->add("��ҳ (PgUp)");
   ptr0->add("��ҳ (PgDn)");
   ptr0->add("-");
   ptr0->add("ƪ�� (^Home)");
   ptr0->add("ƪĩ (^End)");
   ptr0->add("-");
   ptr0->add("ָ���� (F10)");
   ptr0 = ptr->add("Ѱ�����滻", Mx|Wx);
   ptr0->add("Ѱ��... (Ctrl+F)");
   ptr0->add("�滻... (Ctrl+R)");
   ptr0->add("-");
   ptr0->add("����    (Ctrl+L)");
   ptr->add("-");
   ptr->add("���� (Ctrl+X)", Wx);
   ptr->add("���� (Ctrl+C)", Wx);
   ptr->add("ճ�� (Ctrl+V)", Wx);
   ptr->add("ɾ�� (Ctrl+D)", Wx);
   ptr->add("-");
   ptr->add("ȫѡ", Wx);
   ptr->add("����ɾ��(^Y)", Wx);
   ptr->add("-");
   ptr->add("TC 2.0 �༭��", Wx);

   ptr = xmenu->add("����", 110, 23);
   ptr->add("�����", Ax, 1);
   ptr->add("�Ĵ���", 1);
   ptr->add("-");
   ptr->add("��һ�� (Ctrl+PgUp)", Wx);
   ptr->add("��һ�� (Ctrl+PgDn)");
   ptr->add("�����ѡ��... (F8)");
   ptr->add("-");
   ptr->add("����  (F2)", Wx);
   ptr->add("�ر�", Wx);
   ptr->add("����", Wx);
   ptr->add("-");
   ptr0 = ptr->add("����(��ʼ��)", Mx);
   ptr0->add("��ǰ����", Wx);
   ptr0->add("-");
   ptr0->add("���б����");
   ptr0->add("���иĴ���");
   ptr0->add("-");
   ptr0->add("ȫ������");

   ptr = xmenu->add("��", 155, 23);
   ptr->add("��ʶ��Ϣ��(�Ĵ���)", Wx);
   ptr->add("��ȥ��ʶ", Wx);
   ptr->add("-");
   ptr->add("����ο��� (^F1)", Wx);
   ptr->add("��ȥ�ο���", Wx);
   ptr->add("-");
   ptr0 = ptr->add("��ע��(�༭��)", Mx|Wx);
   ptr0->add("��־(������ʾ) (F6)");
   ptr0->add("����(�𰸼���ע) (F7)");
   ptr0->add("-");
   ptr0->add("ȡ�������ʽ (Esc)");
   ptr->add("-");
   ptr->add("������� (F1)", Wx);

   ptr = xmenu->add("����", 200, 23);
   ptr->add("Turbo C 2.0 �༭����ʽ (F12)", Wx);
   ptr->add("-");
   ptr->add("����...     (F9)", Wx);
   ptr->add("���г���... (Ctrl+F9)", Wx);

   ptr = xmenu->add("����", 245, 23);
   ptr->add("��������", Ax, -1);
   ptr->add("��ʾʱ��", Ax, -1);
   ptr->add("-");
   ptr0 = ptr->add("�༭������", Mx);
   ptr0->add("�Զ�����", Ax, -1);
   ptr1 = ptr0->add("�����˸", Mx);
   ptr1->add("����", 1);
   ptr1->add("����", Ax, 1);
   ptr1->add("����", 1);
   ptr0->add("������ɫ...");
   ptr0 = ptr->add("��Ϣ������", Mx);
   ptr0->add("��ʾ��Ϣ����ʾ", Ax, -1);
   ptr0->add("��ʾ��ǩ��ʾ", Ax, -1);
   ptr0 = ptr->add("�����ʼ������", Mx);
   ptr0->add("������ʱ�Զ���ʼ��", 1);
   ptr0->add("������ʱѯ���Ƿ��ʼ��", Ax, 1);
   ptr0->add("������ʱ����ʼ��", 1);
   ptr0 = ptr->add("����ѡ��", Mx);
   ptr1=ptr0->add("��ǰ����", Mx);
   ptr1->add("�˳�ʱ�Զ�����", Ax, 1);
   ptr1->add("�˳�ʱ���Զ�����", 1);
   ptr1->add("-");
   ptr1->add("���ڱ���");
   ptr1 = ptr0->add("���к�", Mx);
   ptr1->add("��������", 1);
   ptr1->add("�ȴ������򵥻�", Ax, 1);
   ptr1 = ptr0->add("����ģʽ", Mx);
   ptr1->add("С��", Ax, 1);
   ptr1->add("΢��", 1);
   ptr0->add("-");
   ptr0->add("������ɫ");
   ptr0->add("������ɫ");
   ptr->add("��������...");
   ptr->add("-");
   ptr->add("���浱ǰ����");
   ptr->add("�ָ�Ĭ������");

   ptr = xmenu->add("����", 290, 23);
   ptr->add("�༭���� (^F6)", Wx, 1);
   ptr->add("���봰�� (^F7)", Wx, 1);
   ptr->add("���д��� (^F5)", Wx);
   ptr->add("-");
   ptr->add("����˵������ (^F8)", Wx, 1);
   ptr->add("��˵������ (F1)", Wx, 1);
   ptr->add("-");
   ptr->add("��ʾ����(������Ϣ)", Ax, 1);
   ptr->add("Turbo C 2.0 ����", Wx);
   ptr->add("-");
   ptr->add("ˢ����Ļ (F11)");

   ptr = xmenu->add("����", 335, 23);
   ptr->add("��������");
   ptr->add("���������ļ�...");
   ptr->add("-");
   ptr->add("����Դ�ļ�...", Wx);
   ptr->add("���������ļ�...", Wx);
   ptr->add("-");
   ptr->add("��װ������...");
   ptr->add("�Ƴ�������...");
   ptr->add("����������...");
   ptr->add("ѡ��������...");
   ptr->add("-");
   ptr->add("�½�������...");
   ptr->add("��������...");
   ptr->add("�Ƴ�����...");
   ptr->add("-");
   ptr->add("DOS����״̬");

   ptr = xmenu->add("����", 380, 23);
   ptr->add("����...");
   ptr->add("����˵��");
   ptr->add("ʹ��ָ�� (Shift+F1)");
   ptr->add("��������...");
   ptr->add("-b");
   ptr->add("ף���ɹ�");
 }

