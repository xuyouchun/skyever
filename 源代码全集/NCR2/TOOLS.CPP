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

void msg(char *str="提示", char backcolor=CYAN, char forcolor=WHITE, int Ti=10);
char * getfilename(char *filename, char *searchpath=".", char *head="", char *hz="*", char *te=NULL);
void  bar3ds(int x1, int y1, int x2, int y2, BOOL flag=down, int color=-1);
void addgroups(T_List *list, char *str=NULL);
void  buildlist(), examchange(), showhead(int);

extern char exampath[20];   //试题路径
extern char deskfile[30];   //桌面文件

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
   T_Label(390, 455, "执行->", LIGHTGREEN);
   T_Label(6, 455, "窗口->", LIGHTCYAN);
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
   if(Model==Editmodel&&Exam==Opened&&(strcmp(button.str, "编辑")==0) )
      { xmenu->attrib("特殊格式", _A?0:Wx);
	xmenu->attrib("标注答案", _A?0:Wx);
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
//获取文件名
char * getfilename(char *filename, char *searchpath, char *head, char *hz, char *te)
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-110, __x2=320+110, __y1=240-123, __y2=240+123;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   print(__x1+5, __y1+5, "驱动器:");
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
   setcolor(WHITE);  print(__x1+85, __y1+5, "文件夹及文件:");
   T_List *file = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
   setcolor(YELLOW);
   print(__x1+10, __y2-25, *head?head:"选择文件");
   setcolor(WHITE);  print(__x1+10, __y2-50, "文件名:");
   T_Text text(__x1+80, __y2-50, __x2-__x1-90, te?te:"", 0, CYAN);
   T_Button _ok(__x1+120, __y2-25, "确定", DEF, CYAN);
   T_Button _no(__x1+170, __y2-25, "取消", DEF, CYAN);
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
//获取目录名
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
   print(__x1+5, __y1+5, "类型:");
   T_Select kind(__x1+5, __y1+40, 0, CYAN, WHITE);
   kind.add("编程题", Examflag==Program?Ax:0);
   kind.add("改错题", Examflag==Change?Ax:0);
   setcolor(WHITE);  print(__x1+85, __y1+5, "文件夹及文件:");
   T_List *dir = new T_List(__x1+90, __y1+30, __x2-10, __y2-60);
   setcolor(YELLOW);
   print(__x1+10, __y2-25, *head?head:"选择试题");
   setcolor(WHITE);  print(__x1+10, __y2-50, "文件名:");
   T_Text text(__x1+80, __y2-50, __x2-__x1-90, "", 0, CYAN);
   T_Button _ok(__x1+120, __y2-25, "确定", DEF, CYAN);
   T_Button _no(__x1+170, __y2-25, "取消", DEF, CYAN);
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
       { if(strcmp(kind.str, "编程题")==0)  Examflag = Program;
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
   xmenu->select(Examflag==Program?"编程题":"改错题");
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


//搜索试题组名
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
   T_Button _ok(__x2-120, __y2-25, "确定", DEF, CYAN);
   T_Button _no(__x2-60, __y2-25, "取消", DEF, CYAN);
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
   xmenu->select(Examflag==Program?"编程题":"改错题");
   return groupname;
 }

#define NUMERROR 0x8fff
//输入数字
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
   T_Button _ok (320-95, y2-25, " 确 定 ", 0, CYAN, BLUE);
   T_Button _no (320+30, y2-25, " 取 消 ", 0, CYAN, BLUE);
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
   T_Button _ok (320-95, y2-25, kind==0?" < 是 > ":kind==1?" 确 定 ":" 安 装 ", 0, CYAN, BLUE);
   T_Button _no (320+30, y2-25, kind==0?" < 否 > ":" 取 消 ", 0, CYAN, BLUE);
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
void askforsave()       //询问是否存盘
 { if(T_EditBox::ichange==true && Model==Editmodel)
     if(msgbox("当前试题的修改部分未保存，是否现在保存？")==true)  saveall();
 }

extern int backcolor, headcolor;

void renewxmenu()
 { xmenu->attrib("显示时间", _time.runenable==true?Ax:0);
   xmenu->attrib("声音开关", isound==true?Ax:0);
   xmenu->attrib("自动缩行", T_EditBox::Line==true?Ax:0);
   xmenu->select(T_EditBox::TIME==1?"快速":T_EditBox::TIME==2?"中速":"慢速");
   xmenu->attrib("显示标签提示", msgsee==true?Ax:0);
   xmenu->select(autoreset==0?"打开试题时询问是否初始化":autoreset==1?
	   "打开试题时自动初始化":"打开试题时不初始化");
   xmenu->attrib("显示信息框提示", msgboxsee==true?Ax:0);
   xmenu->select(autosave==true?"退出时自动保存":"退出时不自动保存");
   xmenu->select(waitclick==false?"立即返回":"等待按键或单击");
   xmenu->select(compilemodel==0?"小型":"微型");
   showhead(headcolor);
 }

void resetmsg();
void readmsg()      //读取信息
 { int code = 0;
   if(access("temp\\process\\set.msg", 0)==0)  code = 1;
   if(code==0)  msg("读取设置...");
   FILE *fp = fopen(code==0?"set.msg":"temp\\process\\set.msg", "rt");
   if(fp==NULL)
     { mouse_on();  msgboxsee = false;
       msgbox("软件配置文件未找到，将使用系统默认设置！", CYAN, WHITE, 1);
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

void resetmsg()    //恢复默认设置
 { if(msgboxsee==true &&
      msgbox("该项功能将把您的系统设置恢复为默认设置，它并不会带来不可挽回"
       "的信息，是否继续？")==false)  return;
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


void savemsg(int code)     //保存信息
 { if(code==0)  msg("保存设置...");
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

BOOL installexams()              //在开始时安装
 { void installexam();
   if(testexam()==true)  { xmenu->attrib("编辑模式");  return false; }
   if(msgbox("未发现试题，您可以从安装盘或其它文件中安装试题，如果您有安装盘"
     "，请马上插入，点击“安装”，选择Ａ盘，再选中安装文件，若无安装盘，也可"
     "以另选其它安装文件。您也可以点击“取消”自己来创建试题。想现在安装吗？"
       , CYAN, WHITE, 2)==false)  return true;
   installexam();  xmenu->attrib("编辑模式");  return true;
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
   print(__x1+15, __y1+10, "选择桌面主题");
   T_Button _ok(__x2-130, __y2-50, "确定", DEF, CYAN);
   T_Button _no(__x2-70, __y2-50, "取消", DEF, CYAN);
   T_Button _add(__x2-110, __y2-25, "新增", DEF, CYAN);
   T_Button _del(__x2-50, __y2-25, "删除", DEF, CYAN);
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

void desktopedit()        //桌面主题
 { if(msgboxsee==true &&
     msgbox("您可以选择一些有趣的歌词或者优美的散文放在桌面上，以代替枯燥"
     "的帮助信息。您可以从标准目录下选择，也可以选取一些别的文件放在标准"
     "目录下，要继续吗？")==false)  return;
   char file[30]; int code;   chdir("help\\desktop");
   while( (code=gethelpfiles(file))!=0)
    { if(code==1)
	{ sprintf(deskfile, "desktop\\%s", file);
	  showallbuttonsup();
	  deskbox.read(deskfile, "..");
	  xmenu->select("显示桌面");
	  deskbox.savetoimage("..\\..\\temp\\window\\desk.win");
	  break;
	}
      else if(code==2)
	{ char file[100];
	  if(getfilename(file, "\\", "选择文本文件")==NULL) continue;
	  if(access(file, 0)==-1)
	    { msgbox("错误：未找到该文件！");
	      continue;
	    }
	  char order[150];
	  sprintf(order, "copy %s . >NUL", file);
	  system(order);
	}
      else
	{ if(msgbox("确定要将其删除吗？")==true)  unlink(file); }
    }
   chdir("..\\..");
 }


char * getexamname(char *examname, char *head)   //获取试题名
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
   T_Button _ok(__x2-160, __y2-25, "确 定", DEF, CYAN);
   T_Button _no(__x2-80, __y2-25, "取 消", DEF, CYAN);
   T_Text text(__x1+10, __y2-50, __x2-__x1-20, "", 0, CYAN);
   T_Select kind(__x1+10, __y2-100, 0, CYAN, WHITE);
   kind.add("编程题", Examflag==Program?Ax:0);
   kind.add("改错题", Examflag==Change?Ax:0);
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
	 if(strcmp(kind.str, "编程题")==0)  Examflag = Program;
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
   xmenu->select(Examflag==Program?"编程题":"改错题");
   buildlist();   return examname;
 }


extern BOOL findall, unupsee;
extern int  findflag;
char * getstr(char *str, char *head, char *str2)   //获取寻找字符串
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-100, __x2=320+100,
       __y1=240-55-(str2?15:0), __y2=240+55+(str2?15:0);
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   setcolor(YELLOW);   print(__x1+10, __y1+5, head);
   T_Button _ok(__x2-110, __y2-25, "确定", DEF, CYAN);
   T_Button _no(__x2-50, __y2-25, "取消", DEF, CYAN);
   T_Button _clear(__x2-190, __y2-25, "清空", DEF, CYAN);
   T_Text *text2;
   T_Text text(__x1+10, __y1+30, __x2-__x1-20, str, 0, CYAN, WHITE, false);
   if(str2!=NULL) text2 = new T_Text(__x1+10, __y1+60, __x2-__x1-20, str2, 0, CYAN, WHITE, false);
   T_Select kind(__x1+5, __y2-50, Fx, CYAN, WHITE);
   kind.add("全程查找",   findall==false?0:Ax);
   kind.add("忽略大小写", unupsee==false?0:Ax, __x1+95, __y2-50);
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
       { if(strcmp("全程查找", kind.str)==0)  findall = (BOOL)(1-findall);
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




int getfontcolor(int color)     //获取颜色
 { GRAPHGETTING;  mouse_off();
   const int __x1=320-60, __x2=320+60, __y1=240-100, __y2=240+100;
   char *ptr = new char[imagesize(__x1, __y1, __x2+5, __y2+5)];
   getimage(__x1, __y1, __x2+5, __y2+5, ptr);
   setfillstyle(1, DARKGRAY);  bar(__x1+5, __y1+5, __x2+5, __y2+5);
   setfillstyle(1, CYAN);      bar(__x1, __y1, __x2, __y2);
   setcolor(WHITE);            rectangle(__x1, __y1, __x2, __y2);
   setcolor(YELLOW);   print(__x1+10, __y1+5, "选择颜色:");
   T_Button _ok(__x1+10, __y2-25, "确定", DEF, CYAN);
   T_Button _no(__x1+65, __y2-25, "取消", DEF, CYAN);
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
 { T_Label(0, 23, "☆", WHITE);
   xmenu = new T_xMenu;
   T_Menu  *ptr = xmenu->add("文件", 20, 23), *ptr0, *ptr1;
   ptr->add("创建...   (Ctrl+N)", Wx);
   ptr->add("打开...   (Ctrl+O|F3)", Wx);
   ptr->add("全部存入  (Ctrl+A|F2)", Wx);
   ptr->add("另存为...", Wx);
   ptr->add("从磁盘上移除", Wx);
   ptr->add("-");
   ptr->add("编辑模式 (Ctrl+F12)", Wx, 1);
   ptr->add("测试模式 (Ctrl+F12)", Ax, 1);
   ptr->add("-");
   ptr0 = ptr->add("特殊格式(标注与答案)", Mx|Wx);
   ptr0->add("标志(错行提示) (F6)");
   ptr0->add("隐藏(答案及详注) (F7)");
   ptr0->add("-");
   ptr0->add("取消特殊格式 (Esc)");
   ptr->add("-");
   ptr->add("退出程序 (Shift+Esc)");

   ptr = xmenu->add("编辑", 65, 23);
   ptr0 = ptr->add("块操作", Mx|Wx);
   ptr0->add("定义块首 (F4)");
   ptr0->add("定义块尾 (F5)");
   ptr0->add("-");
   ptr0->add("块复制");
   ptr0->add("块移动");
   ptr0->add("块删除");
   ptr0->add("块取消");
   ptr0 = ptr->add("读写文件", Mx|Wx);
   ptr0->add("读文件...");
   ptr0->add("块写文件...");
   ptr0 = ptr->add("光标移动", Mx|Wx);
   ptr0->add("行首 (Home)");
   ptr0->add("行尾 (End)");
   ptr0->add("-");
   ptr0->add("页首 (^Up)");
   ptr0->add("页尾 (^Down)");
   ptr0->add("-");
   ptr0->add("上页 (PgUp)");
   ptr0->add("下页 (PgDn)");
   ptr0->add("-");
   ptr0->add("篇首 (^Home)");
   ptr0->add("篇末 (^End)");
   ptr0->add("-");
   ptr0->add("指定行 (F10)");
   ptr0 = ptr->add("寻找与替换", Mx|Wx);
   ptr0->add("寻找... (Ctrl+F)");
   ptr0->add("替换... (Ctrl+R)");
   ptr0->add("-");
   ptr0->add("继续    (Ctrl+L)");
   ptr->add("-");
   ptr->add("剪切 (Ctrl+X)", Wx);
   ptr->add("复制 (Ctrl+C)", Wx);
   ptr->add("粘贴 (Ctrl+V)", Wx);
   ptr->add("删除 (Ctrl+D)", Wx);
   ptr->add("-");
   ptr->add("全选", Wx);
   ptr->add("整行删除(^Y)", Wx);
   ptr->add("-");
   ptr->add("TC 2.0 编辑器", Wx);

   ptr = xmenu->add("测试", 110, 23);
   ptr->add("编程题", Ax, 1);
   ptr->add("改错题", 1);
   ptr->add("-");
   ptr->add("上一题 (Ctrl+PgUp)", Wx);
   ptr->add("下一题 (Ctrl+PgDn)");
   ptr->add("依题号选择... (F8)");
   ptr->add("-");
   ptr->add("存入  (F2)", Wx);
   ptr->add("关闭", Wx);
   ptr->add("激活", Wx);
   ptr->add("-");
   ptr0 = ptr->add("重置(初始化)", Mx);
   ptr0->add("当前试题", Wx);
   ptr0->add("-");
   ptr0->add("所有编程题");
   ptr0->add("所有改错题");
   ptr0->add("-");
   ptr0->add("全部试题");

   ptr = xmenu->add("答案", 155, 23);
   ptr->add("标识信息行(改错行)", Wx);
   ptr->add("撤去标识", Wx);
   ptr->add("-");
   ptr->add("填入参考答案 (^F1)", Wx);
   ptr->add("撤去参考答案", Wx);
   ptr->add("-");
   ptr0 = ptr->add("标注答案(编辑中)", Mx|Wx);
   ptr0->add("标志(错行提示) (F6)");
   ptr0->add("隐藏(答案及详注) (F7)");
   ptr0->add("-");
   ptr0->add("取消特殊格式 (Esc)");
   ptr->add("-");
   ptr->add("试题分析 (F1)", Wx);

   ptr = xmenu->add("编译", 200, 23);
   ptr->add("Turbo C 2.0 编辑器方式 (F12)", Wx);
   ptr->add("-");
   ptr->add("编译...     (F9)", Wx);
   ptr->add("运行程序... (Ctrl+F9)", Wx);

   ptr = xmenu->add("设置", 245, 23);
   ptr->add("声音开关", Ax, -1);
   ptr->add("显示时间", Ax, -1);
   ptr->add("-");
   ptr0 = ptr->add("编辑器设置", Mx);
   ptr0->add("自动缩行", Ax, -1);
   ptr1 = ptr0->add("光标闪烁", Mx);
   ptr1->add("快速", 1);
   ptr1->add("中速", Ax, 1);
   ptr1->add("慢速", 1);
   ptr0->add("文字颜色...");
   ptr0 = ptr->add("信息框设置", Mx);
   ptr0->add("显示信息框提示", Ax, -1);
   ptr0->add("显示标签提示", Ax, -1);
   ptr0 = ptr->add("试题初始化设置", Mx);
   ptr0->add("打开试题时自动初始化", 1);
   ptr0->add("打开试题时询问是否初始化", Ax, 1);
   ptr0->add("打开试题时不初始化", 1);
   ptr0 = ptr->add("其它选项", Mx);
   ptr1=ptr0->add("当前设置", Mx);
   ptr1->add("退出时自动保存", Ax, 1);
   ptr1->add("退出时不自动保存", 1);
   ptr1->add("-");
   ptr1->add("现在保存");
   ptr1 = ptr0->add("运行后", Mx);
   ptr1->add("立即返回", 1);
   ptr1->add("等待按键或单击", Ax, 1);
   ptr1 = ptr0->add("编译模式", Mx);
   ptr1->add("小型", Ax, 1);
   ptr1->add("微型", 1);
   ptr0->add("-");
   ptr0->add("背景颜色");
   ptr0->add("标题颜色");
   ptr->add("桌面主题...");
   ptr->add("-");
   ptr->add("保存当前设置");
   ptr->add("恢复默认设置");

   ptr = xmenu->add("窗口", 290, 23);
   ptr->add("编辑窗口 (^F6)", Wx, 1);
   ptr->add("编译窗口 (^F7)", Wx, 1);
   ptr->add("运行窗口 (^F5)", Wx);
   ptr->add("-");
   ptr->add("试题说明窗口 (^F8)", Wx, 1);
   ptr->add("答案说明窗口 (F1)", Wx, 1);
   ptr->add("-");
   ptr->add("显示桌面(帮助信息)", Ax, 1);
   ptr->add("Turbo C 2.0 窗口", Wx);
   ptr->add("-");
   ptr->add("刷新屏幕 (F11)");

   ptr = xmenu->add("工具", 335, 23);
   ptr->add("软件重置");
   ptr->add("制作集成文件...");
   ptr->add("-");
   ptr->add("导出源文件...", Wx);
   ptr->add("导出程序文件...", Wx);
   ptr->add("-");
   ptr->add("安装试题组...");
   ptr->add("移除试题组...");
   ptr->add("导出试题组...");
   ptr->add("选择试题组...");
   ptr->add("-");
   ptr->add("新建试题组...");
   ptr->add("导入试题...");
   ptr->add("移除试题...");
   ptr->add("-");
   ptr->add("DOS命令状态");

   ptr = xmenu->add("帮助", 380, 23);
   ptr->add("关于...");
   ptr->add("软件说明");
   ptr->add("使用指南 (Shift+F1)");
   ptr->add("桌面主题...");
   ptr->add("-b");
   ptr->add("祝您成功");
 }


