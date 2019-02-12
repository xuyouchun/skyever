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
#include <mouse.h>
#include <key.h>
#include "tclass.h"

T_Tx  screen(DARKGRAY);

void interrupt far newdiskerror(...) { /* do nothing */ }
void interrupt far (*olddiskerror)(...);
void interrupt far newctrlbreak(...) { /* do nothing */ }
void interrupt far (*oldctrlbreak)(...);

int  backcolor = BLUE, headcolor = CYAN;
char exampath[20];   //试题路径

enum EXAMFLAG { Program, Change } Examflag=Program;
enum MODEL    { Testmodel, Editmodel } Model=Testmodel;
enum EXAM     { Closed, Opened } Exam=Closed;
#include "tools.h"

void  buildxmenu();
void  buildlist(), examchange();
void  savesourcefile(char *s);
BOOL  windowdeal(int i), Tcedit(), windowdeal(T_Button &);
BOOL  readin(T_Button *win);
int   xmenudeal(char *str);
BOOL  start=false;
BOOL  Compile(), runexefile();
int   readcompilemsg(char *filename);
void  editforlinenumber(char *str);
BOOL  saverunscreen(char *filename), readrunscreen(char *filename);
BOOL  showrunscreen();
BOOL  ucdosflag=false, bucdosflag=false;
BOOL  Haveseen=false,  Havechange=false;
BOOL  msgsee=true, msgboxsee=true;
int   autoreset = 0, compilemodel = 0,  process = 0;
BOOL  autosave = true, waitclick = true;
extern isound;
char  *blockptr = NULL;
char deskfile[30] = "aboutpro.msg";
T_Time  _time(480, 3, CYAN);
T_EditBox * getbox();
void  waitforstep();


T_xMenu  *xmenu;
T_Button _compile(450, 455, "编译", Wx),  _run(500, 455, "运行", Wx),
   _reset(570, 455, "初始化", Wx),
   _left(510, 23, "上一题", Wx),          _right(570, 23, "下一题");
   T_Button _showwindow = T_Button(59, 455, "题目", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN),
   _tcwindow = T_Button(105, 455, "Turbo C", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN),
   _editwindow = T_Button(175, 455, "编辑", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN),
   _compilewindow = T_Button(222, 455, "编译", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN),
   _runwindow = T_Button(269, 455, "运行", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN),
   _helpwindow = T_Button(316, 455, "分析", Fx|Tx|Wx|Cx, BLACK, LIGHTGREEN);


T_Button *winptr[] = { &_editwindow, &_compilewindow, &_runwindow, &_tcwindow, &_showwindow, &_helpwindow };
void  groupattrib_1(BOOL Fa), groupattrib_2(BOOL Fa), groupattrib_3(BOOL Fa);
T_Combo  *list;


int  Num = 1;
T_EditBox editbox(45, 450, BLACK, GREEN);
T_EditBox showbox(45, 450, BLACK, YELLOW, yes);
T_EditBox helpbox(45, 450, BLACK, WHITE, yes);
T_EditBox deskbox(45, 450, BLACK, CYAN, yes);
T_EditBox compilebox(45, 450, BLACK, LIGHTCYAN, yes);

T_Select select(10, compilebox.y1+25);
void waitforclick();
void showbuttons();
void showallbuttonsup();
void showbutton(T_Button &button, BOOL _A);
void _able(T_Button &button, BOOL _W);
void addgroups(T_List *list, char *str=NULL);
char * getgroupname(char *groupname, char *head="");
#define NUMERROR 0x8fff
int getnum(char *Head="请输入数字:", char *str=NULL);
int xstrcmp(char *ptr1, char *ptr2);
void msg(char *str="提示", char backcolor=CYAN, char forcolor=WHITE, int Ti=10);
void saveall();
void askforsave();       //询问是否存盘
void readmsg();      //读取信息
void savemsg(int code=0);      //保存信息
void resetmsg();     //恢复默认设置
BOOL installexams();              //在开始时安装
void desktopedit();        //桌面主题
char * getexamname(char *examname, char *head);   //获取试题名

void timedeal()
 { _time.run();
   if(getpixel(1, 478) && getpixel(638, 478))  ucdosflag = true;
   else  ucdosflag = false;
   if(ucdosflag!=bucdosflag)
     { bucdosflag = ucdosflag;  if(ucdosflag==false) showbuttons(); }
 }

void Unlink(char *filename)
 { _chmod(filename, 1, 0);
   unlink(filename);
 }

void readfiles()
 { FILE *fp = fopen("temp\\process\\editor.msg", "rt");
   char str[100];
   fscanf(fp, "%s\n", str);
   buildlist();
   editbox.read(str+3);
   fscanf(fp, "%s\n", str);
   list->changetext(str);
   groupattrib_3(true);
   groupattrib_1(true);

   _left.able((BOOL)(1-list->top()));  _right.able((BOOL)(1-list->bottom()));
   xmenu->attrib("上一题",  list->top()?Wx:0);
   xmenu->attrib("下一题",  list->bottom()?Wx:0);

   _editwindow.show(down);
   xmenu->select("编辑窗口");
   editbox.savetoimage("edit.win", "temp\\window");

   fscanf(fp, "EXAMFLAG: %d\n", &Examflag);
   fscanf(fp, "MODEL: %d\n", &Model);
   fscanf(fp, "EXAM: %d\n", &Exam);
   fscanf(fp, "START: %d\n", &start);
   fscanf(fp, "HAVESEEN: %d\n", &Haveseen);
   fclose(fp);
        
   unlink("temp\\process\\editor.msg");
   xmenu->select(Examflag==Program?"编程题":"改错题");
   if(Examflag==Change)
     { xmenu->attrib("标识信息行");  editbox.lineable(false); }
 }

void showhead(int color=CYAN)
 { T_Label(0, 0, 639,
   "  海阔天空(NCR2 1.1) -- Ｃ语言学习软件  为您二级尽心尽力!            Skyever",
    LEFT_TEXT, color, ~color);
 }

void testmousedriver()
 { void far *address;
   address = getvect(0x33);
   if(address==NULL || *(unsigned char *)address==0xcf)
    { msgbox("鼠标错误：尚未运行鼠标驱动程序，或驱动程序出现问题，您可以首先"
       "在DOS下运行\"mouse.com\"，然后再运行该软件，因为该软件若无鼠标无法正常"
       "工作，请按回车键或空格键退出！", 1);
      fcloseall();  exit(1);
    }
  }

////////////////////////////////主函数开始////////////////////////////

int main(int argc, char **argv)
 { showhead(DARKGRAY);
     
   T_Label(390, 455, "执行->", LIGHTCYAN);
   T_Label(6, 455, "窗口->", LIGHTCYAN);
   xline(0, 20, 639, 20);

   buildlist();    buildxmenu();

   testmousedriver();

   if(argc!=2 || strcmp(argv[1], "SKYEVER")!=0)
     { msg("正在加载引导系统，请稍候...", BROWN, LIGHTGREEN, -1);
       waitforstep();
       fcloseall(); closegraph();
       execl("tools\\process.exe", NULL);
     }

   showhead();

   disable();  olddiskerror = getvect(0x24);
   setvect(0x24, newdiskerror);
   oldctrlbreak = getvect(0x1b);
   setvect(0x1b, newctrlbreak);
   enable();
  
   clearscreenfile();
   readmsg();
   if(process==0)
    if(deskbox.read(deskfile, "help")==false)
     { deskbox.read("aboutpro.msg", "help");
       deskbox.savetoimage("desk.win", "temp\\desktop");
     }
   setmousexy(320, 260);  mouse_on();
   if(_time.runenable==true)  _time.show();
   
   if(installexams()==false)
    { char path[100];  void selectexam();
      sprintf(path, "exam\\%s", exampath);
      if(access(path, 0)==-1)
	if(msgbox("您所设定的当前试题组已经被移除，要选择新的试题组吗？")==true)
	   selectexam();
    }
   if(process==0) { buildlist();  msg("准备就绪！", BROWN); }
   else  readfiles();

   while(true)
    {  timedeal();
       
       if(list->ishow()==false && xmenu->testclick()==click &&
	  xmenudeal(xmenu->str)==0)  break;
       if(xmenu->ishow()==false && list->ishow()==false)
	{ if(_editwindow.idown())          editbox.edit();
	  else if(_showwindow.idown())     showbox.edit();
	  else if(_helpwindow.idown())     helpbox.edit();
	  else if(_compilewindow.idown())
	    { if(select.testclick())  editforlinenumber(select.str);
	      if(kbhit())  T_EditBox::key = getch();
	    }
	  else deskbox.edit();
	}
       if(ucdosflag==false)
	 { void resetcurexam(BOOL msg=false);
	   if(_compile.testclick())  Compile();
	   if(_run.testclick())      runexefile();
	   if(_reset.testclick())    resetcurexam();
	   for(int i=0; i<6; i++)  if(winptr[i]->testclick())  windowdeal(i);
	 }
       if(xmenu->ishow()==false)
	 if(list->testclick() || _left.testclick()&&list->movefirst()
	    || _right.testclick()&&list->movenext())   examchange();
       if(T_EditBox::key)
	 { void keydeal(int), askforsave();
	   int ch = T_EditBox::key;  T_EditBox::key = 0;
	   if(ch==ESC_KEY)
	     if(bioskey(2)&0x03) { askforsave(); break; } //退出程序
	   keydeal(ch);
	 }
       redo:;
    }
  _break:
   if(msgbox("您选择了退出，如果您真的要退出，请选择“是”，否则，请选择“否”"
	"继续您的练习。真的要退出吗？")==0)  goto redo;
   msg("退出程序..."); 
   clearscreenfile();  Unlink("temp\\desktop\\desk.win");
   Unlink("temp\\desktop\\desktop.win");
   if(autosave==true)  savemsg();
   savesourcefile(list->str);   fcloseall();
   disable();  setvect(0x24, olddiskerror);
   setvect(0x1b, olddiskerror);  enable();
   return 0;
 }

void showdesk()
 { if(deskbox.readinimage("desk.win", "temp\\desktop")==false)
       deskbox.read(deskfile, "help");
   xmenu->select("显示桌面");
   groupattrib_3(false);
 }

void renewdesktop()    //显示桌面
 { showallbuttonsup();
   showdesk();
 }

void savesourcefile(char *s)    //存入源文件
 { if(s==NULL || strcmp(s, "请选择")==0)  return;
   if(T_EditBox::ichange==false)  return;
   if(Model==Editmodel)  return;
   char str[80];
   if(access("temp\\window\\edit.win", 0)==-1 && _editwindow.idown()==up)  return;
   sprintf(str, "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", s);
   editbox.save(str);   msg("存入临时文件...", MAGENTA);  T_EditBox::ichange = false;
 }

void closeall()       //关闭所有打开的文件
 { if(Exam==Closed)  return;
   clearscreenfile();  savesourcefile(list->str);
   showdesk();   start = false;  groupattrib_1(false);
   showallbuttonsup();  xmenu->attrib("标识信息行", Wx);
   msg("已经关闭!", MAGENTA);
   Haveseen = false;
 }

void resetcurexam(BOOL Msg=false)  //重置当前试题
 { if(Msg==false&&msgbox("当前试题即将恢复为原始状态，删除您所做的任何修改，是否继续？")==false) return;
   char str[40];   clearscreenfile();
   sprintf(str, "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   Unlink(str);
   showallbuttonsup();
   if(readin(&_showwindow)==false)    showdesk();
   else
     { showbutton(_showwindow, down);
       xmenu->select("试题说明窗口");
       groupattrib_3(showbox.lock()?false:true);
     }
   msg("当前试题已经初始化!", BROWN);
   Haveseen = false;
 }

void resetallprograms()  //重置所有编程题
 { if(msgbox("所有改错类试题即将恢复为原始状态，删除您所做的任何修改，是否继续？")==false) return;
   if(start==true)  closeall();
   char path[50];  sprintf(path, "temp\\%s\\program", exampath);
   clearfile(path, "*.*");
   clearscreenfile();   msg("所有编程题已全部重置!", BROWN);
   Haveseen = false;
 }
void resetallchanges()  //重置所有改错题
 { if(msgbox("所有编程类试题即将恢复为原始状态， 删除您所做的任何修改， 是否继续？")==false) return;
   if(start==true)  closeall();
   char path[50];  sprintf(path, "temp\\%s\\change", exampath);
   clearfile(exampath, "*.*");
   clearscreenfile();  msg("所有改错题已全部重置!", BROWN);
   Haveseen = false;
 }
void resetallexams()    //重置全部试题
 { if(msgbox("所有试题即将恢复为原始状态，删除您所做的任何修改，是否继续？")==false) return;
   if(start==true)  closeall();
   char path[50];  sprintf(path, "temp\\%s\\program", exampath);
   clearfile(path, "*.*");
   sprintf(path, "temp\\%s\\change", exampath);
   clearfile(path, "*.*");
   clearscreenfile();   msg("试题已全部重置!", BROWN);
   Haveseen = false;
 }
void showcompilewindow()  //显示编译窗口
 { showallbuttonsup();
   showbutton(_compilewindow, down);  readin(&_compilewindow);
 }

void resetbuildlist()   //刷新列表
 { buildlist();
   closeall();
   _left.able(false);  _right.able(true);
   xmenu->attrib("上一题", Wx);  xmenu->attrib("下一题");
   xmenu->attrib("激活", Wx);
   msg(Examflag==Program?"设置为编程模块!":"设置为改错模块!");
 }

void writeout_C()      //导出源文件
 { char path[100];  getcurdir(0, path);
   char file1[100];  int flag = 0;
   sprintf(file1, *path?"%c:\\%s\\temp\\%s\\%s\\%s.c":"%c:%s\\temp\\%s\\%s\\%s.c",
      getdisk()+'A', path, exampath, Examflag==Program?"program":"change", list->str);
   if(access(file1, 0)==-1)
    { sprintf(file1, *path?"%c:\\%s\\exam\\%s\\%s\\%s\\exam.msg":"%c:%s\\exam\\%s\\%s\\%s\\exam.msg",
      getdisk()+'A', path, exampath, Examflag==Program?"program":"change", list->str);
      if(access(file1, 0)==-1)
	{ msgbox("导出源文件出错，没有找到源文件！", 1); return; }
      flag = 1;
    }
   char file2[100];
   if(getfilename(file2, "..", "输出文件名:", "C")==NULL)  return;
   if(access(file2, 0)==0)
     { if(msgbox("目标文件已经存在，是否覆盖掉它？")==false)  return;
       else Unlink(file2);
     }
   if(strcmp(strupr(file1), strupr(file2))==0)
     { msgbox("错误，源文件和目标文件不能相同！", 1); return; }
   char str[200];
   FILE *fp2 = fopen(file2, "wt");
   if(fp2==NULL)    { msgbox("错误，无法写入目标文件！");  return; }
   FILE *fp1 = fopen(file1, "rt");
   if(fp1==NULL)    { msgbox("错误，无法读取源文件！"); fclose(fp2); return; }
   while(fgets(str, 200, fp1))  fputs(str+flag, fp2);
   fclose(fp1);  fclose(fp2);
   msg("导出完毕!", MAGENTA);
 }

void writeout_EXE()    //导出程序文件
 { char path[100];  getcurdir(0, path);
   char file1[100];
   sprintf(file1, *path?"%c:\\%s\\temp\\exefile\\%s.exe":"%c:%s\\exefile\\%s.exe",
      getdisk()+'A', path, list->str);
   if(access(file1, 0)==-1)
      if(access(file1, 0)==-1)
	{ msgbox("导出程序文件出错，请先点击“编译”按钮进行编译！", 1); return; }
   char file2[100];
   if(getfilename(file2, "..", "输出文件名:", "EXE")==NULL)  return;
   if(access(file2, 0)==0)
     { if(msgbox("目标文件已经存在，是否覆盖掉它？")==false)  return;
       else Unlink(file2);
     }
   if(strcmp(strupr(file1), strupr(file2))==0)
     { msgbox("错误，源文件和目标文件不能相同！", 1); return; }
   char order[200];
   sprintf(order, "copy %s %s >NUL", file1, file2);
   system(order);  msg("导出完毕!", MAGENTA);
 }

void rundos()          //DOS命令状态
 { savescreen("temp\\desktop\\desktop.win");
   mouse_off();
   int save = getdisk();
   char path[200];  getcurdir(0, path);
   restorecrtmode();
   textattr((LIGHTGRAY<<4)|LIGHTBLUE|0x80);
   cprintf("\n\n\n Type Exit to return to Ncr2 ... ");
   system("command.com");
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   setdisk(save);  chdir("\\");  chdir(path);
   writescreen("temp\\desktop\\desktop.win");
   mouse_on();  
 }

void clearallfiles(char *path, char *filter="*.*", BOOL Y=false)
 { struct ffblk Ffblk;
   chdir(path);
   findfirst(filter, &Ffblk, FA_DIREC|FA_ARCH);
   do
    { char name[20];  strcpy(name, Ffblk.ff_name);
      if(Ffblk.ff_attrib&FA_DIREC)
	{ if(name[0]=='.')  continue;
	  clearallfiles(name, filter, Y);
	}
      else  if(Y==false)  unlink(name);
      else  Unlink(name);
    } while(findnext(&Ffblk)!=-1);
   chdir("..");  if(Y==true) rmdir(path);
 }

void resetprogram()   //软件重置
 { if(msgboxsee==true &&
      msgbox("该项功能将把软件恢复为原始状态，删除您所建立的一切信息文件，"
      "若执行该功能，将清空您做题过程中所记录的内容，但并不删除您所安装"
      "的试题，是否继续？")==false)  return;
   clearallfiles("temp");
   clearfile("exam");          clearfile("help");
   clearfile("compile");       clearfile("compile\\include");
   clearfile("compile\\lib");  clearfile("compile\\include\\sys");
   clearfile("tools");
   msgbox("完成，已经将软件恢复为原始状态！", 1);
 }

void makeaddfile()      //制作集成文件
 { if(msgboxsee==true &&
     msgbox("该项功能将把您现在状态的软件集成为一个文件，以便拷贝或网上发送，"
    "在开始之前，最好把软件重置，以便减少所占用的磁盘空间，是否继续？")==false)  return;
   char file[100];
   if(getfilename(file, "..", "集成文件名:", "ADD")==NULL)  return;
   if(access(file, 0)==0)
     if(msgbox("文件已经存在，覆盖掉吗？")==false)  return;
     else Unlink(file);
   char order[200];
   sprintf(order,  "ncr2\\tools\\add ncr2 /%s %s", file,
    msgbox("您可以将其集成为压缩格式的文件，以减少磁盘存储的空间，但这样集成速度比较慢。"
     "如果想快速集成，请点击“否”，但它不会给您压缩。要将其集成为压缩格式文件吗？")
     ?"/a":"" );
   savescreen("..\\desktop.win");
   mouse_off();
   int save = getdisk();
   char path[200];  getcurdir(0, path);
   restorecrtmode();
   chdir("..");  system(order);
   for(int k=strlen(file); k>=0; k--)  if(file[k]=='\\')  break;
   file[k+1] = '\0';
   strcat(file, "Install.exe");
   if(access(file, 0)==-1)
    { sprintf(order, "copy ncr2\\tools\\install.exe %s >NUL", file);
      system(order);
    }
   chdir("ncr2");
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   setdisk(save);  chdir("\\");  chdir(path);
   writescreen("..\\desktop.win");   unlink("..\\desktop.win");
   mouse_on();
   msgbox("完成，已经将软件集成为一个文件，并在相同目录下附有解集成文件Install.exe！", 1);
 }

void saveall()        //保存当前试题的全部文件(编辑模式中)
 { if(T_EditBox::ichange==false)  return;
   char path[60];  Havechange = true;
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   if(access("temp\\window\\edit.win", 0)==0)  editbox.save("Exam.msg", path, true);
   if(access("temp\\window\\show.win", 0)==0)  showbox.save("Show.msg", path);
   if(access("temp\\window\\help.win", 0)==0)  helpbox.save("Help.msg", path);
   if(Model==Testmodel)   xmenu->attrib("全部存入", Wx);
   T_EditBox::ichange = false;  msg("存入文件...", MAGENTA);
 }

void opennewfile(), addnewfile();
void editmodel()        //转入编辑模式
 { if(Model==Editmodel)  return;
   if(msgboxsee==true &&
     msgbox("您将要转入编辑模式中，在这里，您可以修改已有试题或创建新的试题，"
    "但希望您不要轻易修改已成型的试题，否则您无法将它恢复。详细编辑方法请见帮助信息。"
    "是否要继续？")==false)
     { xmenu->select("测试模式");  return; }
   if(Exam==Opened)
    { char name[100];
      sprintf(name, "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
      editbox.save(name);
    }
   T_LineEdit::shines(off);
   Model = Editmodel;
   groupattrib_1((BOOL)Exam);
   groupattrib_2(true);   showbox.lock(no);   helpbox.lock(no);
   Unlink("temp\\window\\edit.win");
   if(_editwindow.idown())  readin(&_editwindow);
   T_EditBox::ichange = false;
   msg("编辑模式中...");
   if(Exam==Closed)
     if(list->inull())  opennewfile();
     else               addnewfile();
   Havechange = false;
   T_LineEdit::Sf = true;
   groupattrib_3(getbox()==NULL?false:true);
 }


void testmodel()       //转入测试模式
 { if(Model==Testmodel)  return;
   T_LineEdit::shines(off);
   askforsave();
   Model = Testmodel;
   groupattrib_1((BOOL)Exam);
   groupattrib_2(false);   showbox.lock(yes);   helpbox.lock(yes);
   Unlink("temp\\window\\edit.win");
   if(_editwindow.idown())  readin(&_editwindow);

   msg("测试模式中...");
   if(Exam==Opened && Havechange==true&&
	msgbox("本试题原题已经改变，是否现在将其初始化？")==true)
     resetcurexam(true);
   T_EditBox::ichange = false;      T_LineEdit::Sf = false;
   Havechange = true;
   groupattrib_3(getbox()==&editbox?true:false);
 }

void saveas()         //换名另存当前试题的全部文件(编辑模式中)
 { char dirname[20], filename[100], order[200], path[60], path0[60];
   sprintf(path0, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   if(getdirname(dirname, "试题另存为:", Examflag)==NULL)  return;
   sprintf(filename, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", dirname);
   if(access(filename, 0)==0)
     { msgbox("您所起的试题名字已经存在，请另起一个试题的名字！", 1);  return; }
   if(mkdir(filename)==-1)    { msgbox("另存试题失败!", 1);  return; }
   buildlist();   
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", dirname);
   if(access("temp\\window\\edit.win", 0)==0) editbox.save("Exam.msg", path);
   else { sprintf(order, "copy %s\\Exam.msg %s\\Exam.msg >NUL", path0, path);
	  system(order);
	}
   if(access("temp\\window\\show.win", 0)==0) showbox.save("show.msg", path);
   else { sprintf(order, "copy %s\\Show.msg %s\\Show.msg >NUL", path0, path);
	  system(order);
	}
   if(access("temp\\window\\help.win", 0)==0) helpbox.save("help.msg", path);
   else { sprintf(order, "copy %s\\Help.msg %s\\Help.msg >NUL", path0, path);
	  system(order);
	}
   list->changetext(dirname);   msg("保存完毕", MAGENTA);
 }

void addnewfile();
void opennewfile()    //打开新文档
 { askforsave();
   char dirname[20], filename[100];
   if(getdirname(dirname, "打开试题:", Examflag)==NULL)  return;
   sprintf(filename, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", dirname);
   if(access(filename, 0)==-1)
     { if(msgbox("您想打开的试题不存在，要创建它吗？")==true)
	{ if(mkdir(filename)==-1)
	   { msgbox("建立试题失败！", 1);  return; }
	  chdir(filename);   
	  system("copy ..\\..\\..\\file Exam.msg >NUL");  _chmod("Exam.msg", 1, 0);
	  system("copy ..\\..\\..\\file Help.msg >NUL");  _chmod("Help.msg", 1, 0);
	  system("copy ..\\..\\..\\file Show.msg >NUL");  _chmod("Show.msg", 1, 0);
	  chdir("..\\..\\..\\..");   
	  resetbuildlist();  list->changetext(strupr(dirname));
          examchange();   msg("创建完毕!", BROWN);
	}
       return;
     }
   resetbuildlist();  list->changetext(strupr(dirname));
   examchange();   msg("打开完毕!", BROWN);
 }

void addnewfile()    //创建新文档
 { askforsave();
   char dirname[20], filename[100];
   if(getdirname(dirname, "创建试题:", Examflag)==NULL)  return;
   sprintf(filename, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", dirname);
   if(access(filename, 0)==0)
     { if(msgbox("您想建立的试题已经存在，是否将其打开？")==true)
	{ resetbuildlist();  list->changetext(strupr(dirname));
	  examchange();   msg("打开完毕!", BROWN);
	}
       return;
     }
   if(mkdir(filename)==-1)
     { msgbox("建立试题失败！", 1);  return; }
   chdir(filename);
   system("copy ..\\..\\..\\file Exam.msg >NUL");  _chmod("Exam.msg", 1, 0);
   system("copy ..\\..\\..\\file Help.msg >NUL");  _chmod("Help.msg", 1, 0);
   system("copy ..\\..\\..\\file Show.msg >NUL");  _chmod("Show.msg", 1, 0);
   chdir("..\\..\\..\\..");   
   buildlist();   list->changetext(strupr(dirname));
   examchange();   msg("创建完毕!", BROWN);
 }

void deletecurfile()    //从磁盘上删除当前文件
 { if(msgbox("将要从磁盘上删除当前试题，这将不能恢复，您确定吗？")==false)  return;
   char dir[20];  strcpy(dir, list->str);
   closeall();
   char path[100];
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", dir);
   chdir(path);  clearfile(".");  chdir("..");  rmdir(dir);
   chdir("..\\..\\..");  resetbuildlist();  msg("删除完毕!", BROWN);
 }

BOOL trueread(BOOL readf=true)
 { char path[50];
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   return editbox.read("exam.msg", path, readf);
 }

void readkey()         //填入参考答案
 { if(msgboxsee==true &&
    msgbox("您将要填入参考答案，这将清除您所做的一切修改，不过您还可以及时选择"
 "“撤去参考答案”来恢复，是否继续？")==false)  return;
   if(_editwindow.idown()==up)
    { showbutton(_editwindow, down);  windowdeal(_editwindow);  }
   editbox.save("temp\\source\\bsource1");
   trueread(true);
   xmenu->attrib("填入参考答案", Wx);   xmenu->attrib("撤去参考答案");
   if(Examflag==Change)
    { xmenu->attrib("标识信息行", Wx);     xmenu->attrib("撤去标识", Wx); }
   _able(_tcwindow,false);  msg("参考答案已经填入!", MAGENTA);
   xmenu->attrib("TC 2.0 编辑器", Wx);
   xmenu->attrib("Turbo C 2.0 编辑器方式", Wx);
   xmenu->attrib("Turbo C 2.0 窗口", Wx);
   T_LineEdit::Sf = true;
 }

void undoreadkey()          //撤去参考答案
 { if(msgboxsee==true &&
    msgbox("您将要撤去参考答案，恢复您先前所做的内容，是否继续？")==false)  return;
   if(_editwindow.idown()==up)
    { showbutton(_editwindow, down);  windowdeal(_editwindow);  }
   if(editbox.read("temp\\source\\bsource1")==false)
     msgbox("您已经做过太多的修改，无法撤消！", 1);
   else
    { _able(_tcwindow,true); msg("参考答案已撤去!", MAGENTA);
      xmenu->attrib("TC 2.0 编辑器");
      xmenu->attrib("Turbo C 2.0 编辑器方式");
      xmenu->attrib("Turbo C 2.0 窗口");
    }
   xmenu->attrib("填入参考答案");   xmenu->attrib("撤去参考答案", Wx);
   if(Examflag==Change)
    { xmenu->attrib("标识信息行");     xmenu->attrib("撤去标识", Wx); }
   Unlink("temp\\source\\bsource1");
   T_LineEdit::Sf = false;
 }

void lineflag()         //显示标志信息
 { if(msgboxsee==true &&
    msgbox("您将要显示标志信息，这将清除您所做的一切修改，不过您还可以及时选择"
      "“撤去标志”来恢复，是否继续？")==false)  return;
   if(_editwindow.idown()==up)
    { showbutton(_editwindow, down);  windowdeal(_editwindow);  }
   editbox.save("temp\\source\\bsource2");
   trueread(def);
   xmenu->attrib("标识信息行", Wx);
   xmenu->attrib("撤去标识");
   _able(_tcwindow,false);     msg("标志已显示!", MAGENTA);
   xmenu->attrib("TC 2.0 编辑器", Wx);
   xmenu->attrib("Turbo C 2.0 编辑器方式", Wx);
   xmenu->attrib("Turbo C 2.0 窗口", Wx);
   T_LineEdit::Sf = true;
 }

void undolineflag()          //撤去标志行
 { if(msgboxsee==true &&
     msgbox("您将要撤去标志，恢复您先前所做的内容，是否继续？")==false)  return;
   if(_editwindow.idown()==up)
    { showbutton(_editwindow, down);  windowdeal(_editwindow);  }
   if(editbox.read("temp\\source\\bsource2")==false)
     msgbox("您已经做过太多的修改，无法撤消！", 1);
   else
     { _able(_tcwindow,true);  msg("标志已撤去!", MAGENTA);
       xmenu->attrib("TC 2.0 编辑器");
       xmenu->attrib("Turbo C 2.0 编辑器方式");
       xmenu->attrib("Turbo C 2.0 窗口");
     }
   xmenu->attrib("标识信息行");
   xmenu->attrib("撤去标识", Wx);
   Unlink("temp\\source\\bsource2");
   T_LineEdit::Sf = false;
 }

void renewscreen()          //刷新屏幕
 { mouse_off();
   cleardevice();   xmenu->renew();
   T_Label(0, 0, 639,
     "  全国计算机等级考试(二级C语言) - 上机考试习题集(Ver:1.0)            Skyever",
       LEFT_TEXT, CYAN, RED);
   T_Label(0, 23, "☆", WHITE);
   T_Label(390, 455, "执行->", LIGHTCYAN);
   T_Label(6, 455, "窗口->", LIGHTCYAN);
   xline(0, 20, 639, 20);
   int flag = -1;
   for(int k=0; k<6; k++)
    { winptr[k]->show(); if(winptr[k]->idown()) flag = k; }
   if(flag==-1)  showdesk();     list->show();
   _left.show();  _right.show();  _compile.show();  _run.show();  _reset.show();
   if(flag!=-1)   windowdeal(*winptr[flag]);
   if(_showwindow.idown()==down)        showbox.show();
   else if(_editwindow.idown()==down)   editbox.show();
   else if(_helpwindow.idown()==down)   helpbox.show();
   mouse_on();   msg("刷新完毕!");
 }

void  selectfornum()   //依题号选择
 { int N = getnum("请输入题号:");
   if(N==NUMERROR)  return;
   char name[50];
   sprintf(name, "EXAM%d", N);
   name[13] = '\0';
   list->changetext(name);
   examchange();
 }

void  showhelpwindow()   //显示分析窗口
  { showbutton(_helpwindow,down);
    windowdeal(_helpwindow);
  }

T_EditBox * getbox()
 { if(_showwindow.idown())   { if(showbox.lock()==no)  return &showbox; }
   if(_editwindow.idown())   { if(editbox.lock()==no)  return &editbox; }
   if(_helpwindow.idown())   { if(helpbox.lock()==no)  return &helpbox; }
   return NULL;
 }

void selectbegin()    //定义块首
 { T_EditBox *ptr = getbox();
   if(ptr)  ptr->definebegin();
 }

void selectend()      //定义块尾
 { T_EditBox *ptr = getbox();
   ptr->defineend();
 }

void blockcopy()      //块复制
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)  return;
   char *str = ptr->selptr();
   if(str!=NULL)
    { ptr->addstr(str);
      delete str;
    }
 }

void blockdelete()   //块删除
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->delstr();
 }

void blockpaste()    //块移动
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   if(ptr->ishinesinblock())
     { msgbox("请注意：不能在块内进行块移动！", 1);
       return;
     }
   char *str = ptr->selptr();
   if(str!=NULL)
    { ptr->delstr();
      ptr->addstr(str);
      delete str;
      ptr->resetline();
      ptr->resetsel();
    }
 }

void blockreset()    //块取消
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->resetsel();
 }



void xcopy()   //复制
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   char *str = ptr->selptr();
   if(str==NULL)  return;
   if(blockptr!=NULL)  delete blockptr;
   blockptr = str;
 }

void xcut()    //剪切
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   xcopy();
   ptr->delstr();
 }

void xpaste()   //粘贴
 { if(blockptr==NULL)  return;
   T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->addstr(blockptr);
 }

void xdelete()  //删除
 { blockdelete(); }

void linebegin()  //行首
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->linebegin();
 }

void lineend()    //行尾
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->lineend();
 }

void pageup()    //上页
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->page(up);
 }
void pagedown()    //下页
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->page(down);
 }

void pagebegin()    //页首
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->pagebegin();
 }
void pageend()    //页尾
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->pageend();
 }

void allbegin()    //篇首
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->allbegin();
 }
void allend()    //篇末
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->allend();
 }

void gotolinenum()  //到指定行
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   int num = getnum("请输入行数");
   if(num==NUMERROR)  return;
   if(num<=0)  num = 1;
   if(num-1>ptr->maxline())
     { char str[100];
       sprintf(str, "该文件的最大行数是[%d]，是移光标至结尾处？", ptr->maxline()+1);
       if(msgbox(str)==false)  return;
       num = ptr->maxline()+1;
     }
   ptr->gotolinenum(num-1);
 }

void readfile()    //读文件
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   char name[200];
   if(getfilename(name, ".", "读文件：")==NULL)  return;
   FILE *fp = fopen(name, "rt");
   if(fp==NULL)  { msgbox("未找到文件！", 1);  return; }
   char *str = new char[filelength(fileno(fp))+100];
   if(str==NULL)
     { msgbox("文件太长，无法读入！");  return; }
   char *str0 = str;
   while( (*str0=fgetc(fp)) != EOF)
     { if(*str0>0&&*str0<32 && *str0!=9 && *str0!='\n')
	{ msgbox("该文件不是文本文件，无法读入！");
	  delete str;  fclose(fp);  return;
	}
       str0 ++;
     }
   *str0 = '\0';  fclose(fp);
   ptr->addstr(str); delete str;
 }

void writeblock()   //块写文件
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   char *str = ptr->selptr();
   if(str==NULL)
     { msgbox("请先定义块！", 1);   return; }
   char name[200];
   if(getfilename(name, ".", "块写文件：")==NULL)  return;
   if(access(name, 0)==0)
     if(msgbox("指定文件已经存在，是否覆盖？")==false)  return;
   FILE *fp = fopen(name, "wt");
   if(fp==NULL)
     { msgbox("文件名有误，无法创建文件！", 1); delete str; return; }
   char *str0 = str;
   while(*str0)  fputc(*str0++, fp);
   delete str;
   fclose(fp);
 }

void installexam()    //安装试题组
 { if(msgboxsee==true &&
      msgbox("您将要安装新的试题组，一个试题组中可以有多题试题，试题组一般"
     "存在于软盘上或“EXM”文件中。若您有试题安装盘，请首先插入。是否继续？") ==false)
       return;
   char file[200], dirname[13], str[40];
   if(getfilename(file, "..", "安装试题组：", "EXM")==NULL)  return;
   if(access(file, 0)==-1)     { msgbox("未找到文件！", 1);  return; }
   for(int k=strlen(file)-1; k>=0; k--)
     if(file[k]=='\\' || file[k]==':')  break;
   strcpy(dirname, file+k+1);  dirname[strlen(dirname)-4] = '\0';
   sprintf(str, "exam\\%s", dirname);
   if(access(str, 0)==0)
     if(msgbox("发现已存在有相同名称的试题组，建议您不要再导入该试题组，如果"
	"强行导入，将可能出现覆盖，是否继续？")==false)  return;
   char order[120];
   sprintf(order,  "..\\tools\\install %s", file);
   savescreen("temp\\desktop\\desktop.win");
   mouse_off();
   restorecrtmode();
   chdir("exam");   system(order);   chdir("..");
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   writescreen("temp\\desktop\\desktop.win");
   mouse_on();
   chdir("temp");  mkdir(dirname);  chdir(dirname);
   mkdir("Program");  mkdir("Change");  chdir("..\\..");
   if(msgbox("已经将试题组安装到相应路径下，是否将其转化为当前试题组？")==true)
    { closeall();      strcpy(exampath, dirname);
      buildlist();     msg("试题组已经准备就绪", BROWN);
      xmenu->attrib("编辑模式");
    }
 }

void deleteexam()   //移除试题组
 { void selectexam();  BOOL testexam();
   if(msgboxsee==true &&
       msgbox("您选择了移除试题组，如果您不是为了节约磁盘空间或其它重要目的，"
      "建议您不要移除它，否则可以会造成大量的难以挽回的损失，因为一个试题"
      "组中可以包括几百套试题。要继续吗？")==false)  return;
   char group[20];
   if(getgroupname(group, "移除试题组")==NULL)  return;
   if(Model==Editmodel && strcmp(group, exampath)==0)
     { msgbox("您不能在编辑模式下移除当前试题组，如果要移除它，请先切换到别的"
	"试题组中，或转入测试模式下。", 1);
       return;
     }
   if(msgboxsee==true &&
     msgbox("您将要移除选中的试题组，请您在点击“是”之前一定要考虑好，"
      "否则移除后，您无法恢复它，除非从安装盘中再次安装。要继续吗？")==false)
	return;
   msg("试题移除过程中，请稍候...", RED, YELLOW, 20);
   chdir("exam");   clearallfiles(group, "*.*", true);   chdir("..");
   chdir("temp");   clearallfiles(group, "*.*", true);   chdir("..");
   buildlist();
   msgbox("移除完毕，如果要恢复它，您只有从安装盘中再次安装了！", 1);
   xmenu->attrib("编辑模式", Wx);
   if(testexam())
     { if(strcmp(group, exampath)==0 &&
	 msgbox("您移除了当前试题组，现在试题组已空，您要选择新的试题组吗？")==true)
	selectexam();
     }
   else
     { if(msgbox("您移除了当前试题组，现在已无可选试题组，您可以安装新的试题组，"
	   "想要现在安装试题组吗？")==false)  return;
       installexam();
     }
 }

void writeoutexam()  //导出试题组
 { if(msgboxsee==true &&
       msgbox("您将要把当前的试题组导出，试题组导出后为一个后缀为“EXM”的文件，"
     "您可以将其保存于软盘上以制成试题安装盘。导出试题组并不会将它删除，"
     "是否继续？") ==false)
       return;
   char file[200], dirname[13];
   if(getfilename(file, "..", "导出试题组：", "EXM", exampath)==NULL)  return;
   if(access(file, 0)==0)
     if(msgbox("文件已经存在，要覆盖掉吗？")==false)  return;
     else Unlink(file);
   char order[120];
   sprintf(order, "..\\tools\\add  %s /%s", exampath, file);
   savescreen("temp\\desktop\\desktop.win");
   mouse_off();
   restorecrtmode();
   chdir("exam");   system(order);   chdir("..");
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   writescreen("temp\\desktop\\desktop.win");
   mouse_on();
   msgbox("已经将试题组导出为相应的文件，您可以将其拷贝到软盘上制成安装盘。", 1);
 }

void selectexam()   //选择试题组
 { if(msgboxsee==true &&
      msgbox("您将要重新选择试题组，原有试题组中您做过的试题内容将会保存，不会"
     "带来什么损失。一个试题组中可以有几百套试题，软件将为您记住您现在设定的"
     "试题组，在下次打开的时候将会自动切换到您选择的试题组中。要继续吗？")==false)
       return;
   char group[20];
   if(getgroupname(group, "选择试题组")==NULL)  return;
   closeall();
   strcpy(exampath, group);
   buildlist();
   _left.able(false);  _right.able(true);
   xmenu->attrib("上一题", Wx);   xmenu->attrib("下一题");
   xmenu->attrib("编辑模式");
   msg("试题组已经准备就绪", BROWN);
 }

void newexam()     //新建试题组
 { if(msgboxsee==true &&
      msgbox("一般情况下，您不要自己建立那么多试题组，除非您要保存您感到有用"
     "的试题，或者自己要建立多套试题。要继续吗？")==false)  return;
   char groupname[20];
   if(getnum("请输入试题组名称：", groupname)==NUMERROR)  return;
   chdir("exam");  mkdir(groupname);  chdir(groupname);
   mkdir("Program");  mkdir("Change");  chdir("..\\..");
   chdir("temp");  mkdir(groupname);  chdir(groupname);
   mkdir("Program");  mkdir("Change");  chdir("..\\..");
   if(msgbox("建立试题组完毕，您想要把它设定为当前试题组吗？")==true)
     { closeall();   strcpy(exampath, groupname);
       buildlist();  msg("试题组已经准备就绪", BROWN);
       xmenu->attrib("编辑模式");
     }
 }

void selectall()       //全选
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->selectall();
 }

void delline()        //整列删除
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   ptr->delcurline();
 }

void showtimeable()  //显示时间
 { _time.runenable = (BOOL)(1-_time.runenable);
   if(_time.runenable==false)  _time.clear();
 }

void about()      //显示关于信息框
 { msgbox("全国计算机等级考试（二级Ｃ语言）模拟测试软件“海阔天空”，由济南大学"
	  "材料系学办开发。本软件是一个免费软件，可由济南大学的老师和同学们"
	  "任意下载，自由拷贝。现在版本号为1.1，请您随时关注Email:Skyever_y"
	  "ouer@sohu.com，询问有无升级信息及新试题信息。", 1);
 }

void showhelp(char *name)   //显示桌面文件
 { strcpy(deskfile, name);
   showallbuttonsup();
   deskbox.read(deskfile, "help");
   deskbox.savetoimage("desk.win", "temp\\desktop");
   xmenu->select("显示桌面");
   groupattrib_3(false);
 }

void addexamfile()     //导入试题
 { char examfile[100], nowfile[50];
   if(msgboxsee==true &&
     msgbox("您可以将现有的试题导入到当前试题组中，但您必须注意，编程题只能导"
     "入到编程题组中，改错题只能导到到改错题组中。是否继续？")==false)  return;
   if(getexamname(examfile, "导入试题")==NULL)  return;
   for(int k=strlen(examfile)-1; k>=0; k--)
	 if(examfile[k]=='\\')  break;
   strcpy(nowfile, examfile+k+1);
   chdir("exam");  chdir(exampath);  chdir(Examflag==Program?"program":"change");
   while(access(nowfile, 0)==0)
     { if(msgbox("发现有重名的试题，请另取一个试题的名字！", 1)==false)
	      { chdir("..\\..\\..");  return; }
       if(getnum("请输入试题名称：", nowfile)==NUMERROR)
	   { chdir("..\\..\\.."); return; }
     }

   char order[150];   mkdir(nowfile);
   sprintf(order, "copy ..\\..\\%s\\*.* %s >NUL", examfile, nowfile);
   system(order);  chdir("..\\..\\..");   buildlist();
   if(msgbox("该试题已经成功导入到当前试题组中，要打开它吗？")==true)
     { list->changetext(nowfile);  closeall();
       examchange();
     }
 }

void delexamfile()    //移除试题
 { if(msgboxsee==true &&
     msgbox("您选择了移除试题，在这个功能中，您可以把现有的试题从磁盘上移除"
     "但是您要注意，这个操作是不能被恢复的。是否继续？")==false)  return;
   char examfile[20];
   while(getdirname(examfile, "移除试题:", Examflag, false)!=NULL)
    { if(msgbox("确定要移除选中的试题吗？")==false)  return;
      char path[100];
      sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", examfile);
      chdir(path);  clearfile(".");  chdir("..");  rmdir(examfile);
      chdir("..\\..\\..");   msg("移除完毕!", BROWN);
      resetbuildlist();
    }
 }

char * getstr(char *str, char *head, char *str2=NULL);   //获取寻找字符串
char strforfound[30]="", strforreplace[30]="";
void findcontinue();
BOOL findall=false, unupsee=false;
int  findflag;

void findstr()          //寻找
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   if(getstr(strforfound, "寻找指定内容", NULL)==NULL)  return;
   if(findall==true)  { ptr->gotolinenum(0);  T_LineEdit::K = -1; }
   findflag = 0;  findcontinue();
 }

void findcontinue()  //继续寻找
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   if(findflag==0)
    { while(ptr->findstr(strforfound, unupsee)==false)
	{ if(msgbox("寻找完毕，但没有找到。是否从文件开头再寻找？")==false)  return;
	  ptr->gotolinenum(0);  T_LineEdit::K = -1;
	}
    }
   else
    { while(ptr->findstr(strforfound, unupsee)==false)
	{ if(msgbox("寻找完毕，但没有找到。是否从文件开头再寻找？")==false)  return;
	  ptr->gotolinenum(0);  T_LineEdit::K = -1;
	}
      ptr->replacestr(strforreplace, strlen(strforfound));
    }
 }

void replacestr()      //替换
 { T_EditBox *ptr = getbox();
   if(ptr==NULL)   return;
   if(getstr(strforfound, "寻找与替换指定内容", strforreplace)==NULL) return;
   findflag = 1;  findcontinue();
 }

int getfontcolor(int color);     //获取颜色
void editfontcolor()     //改变编辑器颜色
 { T_EditBox  *ptr;  T_Button *p;
   if(_showwindow.idown())         {  ptr = &showbox;    p = &_showwindow; }
   else if(_editwindow.idown())    {  ptr = &editbox;    p = &_editwindow; }
   else if(_compilewindow.idown()) {  ptr = &compilebox; p = &_compilewindow; }
   else if(_helpwindow.idown())    {  ptr = &helpbox;    p = &_helpwindow; }
   else                            {  ptr = &deskbox;    p = NULL; }
   int color = getfontcolor(ptr->forcolor);
   if(color==-1)  return;
   ptr->forcolor = color;
   readin(p);
 }

void deskbackcolor()     //改变背景颜色
 { int color = getfontcolor(backcolor);
   if(color!=-1)   setbkcolor(backcolor = color);
 }

void headbackcolor()     //改变背景颜色
 { int color = getfontcolor(headcolor);
   if(color!=-1)  showhead(headcolor=color);
 }

void compilemodelset(int model)
 { char str[400];
   sprintf(str, "您已经把编译模式转化为%s模式，编译模式包括微型、小型、中型、"
     "紧凑、大型、巨型六种，它们的区别是代码段和数据段的存放方式，如果您确保"
     "您的数据段和代码段的和不会超过64K，则最好使用微型模式，它在运行时的内"
     "存开销较小。小型模式是较常用的模式，不过在模拟题中提供的程序绝大多数可以"
     "使用微型模式。而对于其它模式，因为较少使用而未提供！",
	model==0?"小型":"微型");
   compilemodel = model;
   if(msgboxsee==true)  msgbox(str, 1);
 }

#define cmp(S)  xstrcmp(str, S)
int  xmenudeal(char *str)
 { if(cmp("退出程序"))    { askforsave(); msg("退出程序...");  return 0; }
   else if(cmp("编程题")) { Examflag = Program;  resetbuildlist(); }
   else if(cmp("改错题")) { Examflag = Change;   resetbuildlist(); }
   else if(cmp("TC 2.0 编辑器"))     Tcedit();
   else if(cmp("Turbo C 2.0 编辑器方式"))  Tcedit();
   else if(cmp("Turbo C 2.0 窗口"))  Tcedit();
   else if(cmp("上一题"))   { if(list->movefirst()) examchange(); }
   else if(cmp("下一题"))   { if(list->movenext())  examchange(); }
   else if(cmp("显示桌面")) renewdesktop();
   else if(cmp("存入  (F2)"))     savesourcefile(list->str);
   else if(cmp("编辑窗口") && _editwindow.idown()==up)
	   { showbutton(_editwindow,down); windowdeal(_editwindow); }
   else if(cmp("试题说明窗口")&&_showwindow.idown()==up)
	   { showbutton(_showwindow,down); windowdeal(_showwindow); }
   else if((cmp("答案说明窗口")||cmp("试题分析")) && _helpwindow.idown()==up)
	     showhelpwindow();
   else if(cmp("编译窗口") && _compile.idown()==up)     showcompilewindow();
   else if(cmp("关闭"))         closeall();
   else if(cmp("激活"))         examchange();
   else if(cmp("当前试题"))     resetcurexam();
   else if(cmp("所有编程题"))   resetallprograms();
   else if(cmp("所有改错题"))   resetallchanges();
   else if(cmp("全部试题"))     resetallexams();
   else if(cmp("运行窗口"))     showrunscreen();
   else if(cmp("编译..."))      Compile();
   else if(cmp("运行程序..."))  runexefile();
   else if(cmp("导出源文件"))   writeout_C();
   else if(cmp("导出程序文件")) writeout_EXE();
   else if(cmp("DOS命令状态"))  rundos();
   else if(cmp("软件重置"))     resetprogram();
   else if(cmp("制作集成文件")) makeaddfile();
   else if(cmp("编辑模式"))     editmodel();
   else if(cmp("测试模式"))     testmodel();
   else if(cmp("全部存入"))     saveall();
   else if(cmp("创建"))         addnewfile();
   else if(cmp("打开..."))         opennewfile();
   else if(cmp("从磁盘上移除")) deletecurfile();
   else if(cmp("另存为"))       saveas();
   else if(cmp("标志"))         editbox.showattrib(LINE);
   else if(cmp("隐藏"))         editbox.showattrib(HIDE);
   else if(cmp("取消特殊格式")) editbox.showattrib(ALL);
   else if(cmp("填入参考答案")) readkey();
   else if(cmp("撤去参考答案")) undoreadkey();
   else if(cmp("标识信息行"))   lineflag();
   else if(cmp("撤去标识"))     undolineflag();
   else if(cmp("刷新屏幕"))     renewscreen();
   else if(cmp("依题号选择"))   selectfornum();
   else if(cmp("定义块首"))     selectbegin();
   else if(cmp("定义块尾"))     selectend();
   else if(cmp("块复制"))       blockcopy();
   else if(cmp("块删除"))       blockdelete();
   else if(cmp("块移动"))       blockpaste();
   else if(cmp("块取消"))       blockreset();
   else if(cmp("剪切"))         xcut();
   else if(cmp("复制"))         xcopy();
   else if(cmp("粘贴"))         xpaste();
   else if(cmp("删除"))         xdelete();
   else if(cmp("行首"))         linebegin();
   else if(cmp("行尾"))         lineend();
   else if(cmp("上页"))         pageup();
   else if(cmp("下页"))         pagedown();
   else if(cmp("页首"))         pagebegin();
   else if(cmp("页尾"))         pageend();
   else if(cmp("篇首"))         allbegin();
   else if(cmp("篇末"))         allend();
   else if(cmp("到指定行"))     gotolinenum();
   else if(cmp("读文件"))       readfile();
   else if(cmp("块写文件"))     writeblock();
   else if(cmp("安装试题组"))   installexam();
   else if(cmp("移除试题组"))   deleteexam();
   else if(cmp("导出试题组"))   writeoutexam();
   else if(cmp("选择试题组"))   selectexam();
   else if(cmp("新建试题组"))   newexam();
   else if(cmp("全选"))         selectall();
   else if(cmp("整行删除"))     delline();
   else if(cmp("快速"))         T_EditBox::TIME = 1;
   else if(cmp("中速"))         T_EditBox::TIME = 2;
   else if(cmp("慢速"))         T_EditBox::TIME = 3;
   else if(cmp("自动缩行"))     T_EditBox::Line = (BOOL)(1-T_EditBox::Line);
   else if(cmp("显示信息框提示"))  msgboxsee = (BOOL)(1-msgboxsee);
   else if(cmp("显示标签提示"))    msgsee = (BOOL)(1-msgsee);
   else if(cmp("打开试题时自动初始化"))  autoreset = 1;
   else if(cmp("打开试题时不初始化"))    autoreset = -1;
   else if(cmp("打开试题时询问是否初始化"))  autoreset = 0;
   else if(cmp("退出时自动保存"))   autosave = true;
   else if(cmp("退出时不自动保存")) autosave = false;
   else if(cmp("现在保存"))         savemsg();
   else if(cmp("保存当前设置"))     savemsg();
   else if(cmp("恢复默认设置"))     resetmsg();
   else if(cmp("声音开关"))         isound = (BOOL)(1-isound);
   else if(cmp("显示时间"))         showtimeable();
   else if(cmp("关于"))             about();
   else if(cmp("软件说明"))         showhelp("aboutpro.msg");
   else if(cmp("使用指南"))         showhelp("helpyou.msg");
   else if(cmp("祝您成功"))         showhelp("wishyou.msg");
   else if(cmp("桌面主题"))         desktopedit();
   else if(cmp("导入试题"))         addexamfile();
   else if(cmp("移除试题"))         delexamfile();
   else if(cmp("寻找"))             findstr();
   else if(cmp("替换"))             replacestr();
   else if(cmp("继续"))             findcontinue();
   else if(cmp("文字颜色"))         editfontcolor();
   else if(cmp("背景颜色"))         deskbackcolor();
   else if(cmp("标题颜色"))         headbackcolor();
   else if(cmp("立即返回"))         waitclick = false;
   else if(cmp("等待按键或单击"))   waitclick = true;
   else if(cmp("微型"))             compilemodelset(1);
   else if(cmp("小型"))             compilemodelset(0);
   return 1;
 }

void keydeal(int ch)
 { switch(ch)
    { case F11_KEY: renewscreen();  break;  //刷新屏幕
      case F12_KEY: if(!_tcwindow.W) Tcedit();   break;  //Turbo C 2.0 编辑器方式
      case F9_KEY:  if(!_compilewindow.W) Compile();  break;  //编辑
      case CTRL_F9: if(!_runwindow.W) runexefile();   break;  //运行
      case CTRL_N:  if(!xmenu->askattrib("创建"))  addnewfile();    break;  //创建
      case CTRL_O:  case F3_KEY: if(!xmenu->askattrib("打开"))  opennewfile();   break;  //打开
      case CTRL_A:  if(!xmenu->askattrib("全部存入")) saveall(); break;  //全部存入
      case CTRL_PAGEUP: if(!xmenu->askattrib("上一题"))
		    if(list->movefirst()) examchange();  break;  //上一题
      case CTRL_PAGEDOWN: if(!xmenu->askattrib("下一题"))
		    if(list->movenext()) examchange();  break;  //下一题
      case CTRL_UP:   pagebegin();  break;    //页首
      case CTRL_DOWN: pageend();    break;    //页尾
      case CTRL_HOME: allbegin();   break;    //篇首
      case CTRL_END:  allend();     break;    //篇尾
      case F6_KEY:  if(!xmenu->askattrib("特殊格式"))  editbox.showattrib(LINE); break; //标志
      case F7_KEY:  if(!xmenu->askattrib("特殊格式"))  editbox.showattrib(HIDE); break; //隐藏
      case ESC_KEY: if(!xmenu->askattrib("特殊格式"))  editbox.showattrib(ALL);  break; //无格式
      case F2_KEY:  if(!xmenu->askattrib("存入"))      savesourcefile(list->str); //保存当前文件(测试中)
		    else if(!xmenu->askattrib("全部存入"))  saveall();  //全部存入(编辑中)
		    break;
      case CTRL_X:  if(!xmenu->askattrib("剪切"))  xcut();    break;
      case CTRL_C:  if(!xmenu->askattrib("复制"))  xcopy();   break;
      case CTRL_V:  if(!xmenu->askattrib("粘贴"))  xpaste();  break;
      case CTRL_D:  if(!xmenu->askattrib("删除"))  xdelete(); break;
      case F8_KEY:  selectfornum();  break;
      case CTRL_F6: if(_editwindow.idown()==down)  break;  __editshow:
		    if(!xmenu->askattrib("编辑窗口"))   //显示编辑窗口
			{ showbutton(_editwindow,down); windowdeal(_editwindow);
			  xmenu->select("编辑窗口");    groupattrib_3(true);
			}     break;
      case F10_KEY: if(_editwindow.idown()==down)  goto  __showshow;  //切换编辑与题目窗口
		      if(_showwindow.idown()==down)  goto  __editshow;  break;
      case CTRL_F7: if(_compile.idown()==down)  break;
		    if(!xmenu->askattrib("编译窗口"))   //显示编译窗口
			 { showcompilewindow();  xmenu->select("编译窗口"); groupattrib_3(false); } break;
      case CTRL_F5: if(!xmenu->askattrib("运行窗口"))   //显示运行窗口
			 { showrunscreen(); xmenu->select("运行窗口"); } break;
      case F1_KEY:  if(_helpwindow.idown()==down)  break;
		    if(!xmenu->askattrib("试题分析"))   //显示分析窗口
			   showhelpwindow();  break;
      case CTRL_F1: if(!xmenu->askattrib("填入参考答案"))  //填入参考答案
			   readkey();  break;
      case CTRL_F8: if(_showwindow.idown()==down)  break;  __showshow:
		    if(!xmenu->askattrib("试题说明"))   //显示试题说明窗口
			 { showbutton(_showwindow,down); windowdeal(_showwindow);
			   xmenu->select("试题说明");  groupattrib_3(showbox.lock()?false:true);
			 }    break;
      case CTRL_F12: if(xmenu->askattrib("编辑模式"))  break;
		     if(Model==Testmodel) { editmodel(); xmenu->select("编辑模式"); } //在编辑和测试模式之间转换
		     else                 { testmodel(); xmenu->select("测试模式"); }
		     break;
      case F4_KEY:   selectbegin();  break;     //定义块首
      case F5_KEY:   selectend();    break;     //定义块尾
      case CTRL_Y:   delline();      break;     //整行删除
      case CTRL_F:   findstr();      break;     //寻找
      case CTRL_L:   findcontinue(); break;     //继续寻找或替换
      case CTRL_R:   replacestr();   break;     //替换
      case SHIFT_F1: showhelp("helpyou.msg");  break;  //使用指南
    }
 }

void  examchange()               //转换试题
 { if(Model==Editmodel && T_EditBox::ichange==true &&
      msgbox("当前试题的修改部分未保存，是否现在保存？")==true)
    { char path[60];  Havechange = false;
      sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->bstr());
      if(access("temp\\window\\edit.win", 0)==0)  editbox.save("Exam.msg", path, true);
      if(access("temp\\window\\show.win", 0)==0)  showbox.save("Show.msg", path);
      if(access("temp\\window\\help.win", 0)==0)  helpbox.save("Help.msg", path);
    }
   char file[60];
   savesourcefile(list->bstr());
   _left.able((BOOL)(1-list->top()));  _right.able((BOOL)(1-list->bottom()));
   xmenu->attrib("上一题",  list->top()?Wx:0);
   xmenu->attrib("下一题",  list->bottom()?Wx:0);
   clearscreenfile();
   showallbuttonsup();
   showbutton(_showwindow, down);
   if(readin(&_showwindow)==false)
     { groupattrib_1(false);  showbutton(_showwindow,up);  showdesk(); start = false;
       msgbox("错误：不能打开指定文件，请检查文件名是否正确！", 1);
       showdesk();
     }
   else
     { groupattrib_1(true);  showbox.savetoimage("show.win", "temp\\window");
       start = true;   xmenu->select("试题说明窗口");  groupattrib_3(showbox.lock()?false:true);
       sprintf(file, "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
       if(access(file, 0)==0 && Model==Testmodel)
       switch(autoreset)
	{ case 0: if(msgbox("本套试题未初始化，还保留着以前所做的内容，您想现在初始化吗？")==true)
		     resetcurexam(true);  break;
	  case 1:  resetcurexam(true);  break;
	}
       xmenu->attrib("撤去参考答案", Wx);
       xmenu->attrib("撤去标识", Wx);
       msg("试题准备就绪！", MAGENTA);
     }
   Unlink("temp\\source\\bsource1");  Unlink("temp\\source\\bsource2");
   T_EditBox::ichange = false;    Haveseen = false;
   if(Model==Testmodel)  T_LineEdit::Sf = false;
   clearallfiles("temp\\exefile");  chdir("..");
 }

void xmenuselect(T_Button *button)
 { if(button==&_showwindow)          xmenu->select("试题说明窗口");
   else if(button==&_compilewindow)  xmenu->select("编译窗口");
   else if(button==&_editwindow)     xmenu->select("编辑窗口");
   else if(button==&_helpwindow)     xmenu->select("答案说明窗口");
 }


BOOL  windowdeal(int i) { return windowdeal(*winptr[i]); }
BOOL  windowdeal(T_Button &win)      //按钮处理
 { void savetoimage(T_Button *win);
   BOOL ret;  T_Button *button = NULL;
   if(&win==&_editwindow&&Model==Editmodel&&Exam==Opened)
      { xmenu->attrib("特殊格式", win.idown()?0:Wx);
	xmenu->attrib("标注答案", win.idown()?0:Wx);
      }
   if(win.idown()==down)
     { for(int k=0; k<6; k++)
	{ if(winptr[k]==&win)  continue;
	  if(winptr[k]->idown()==down)
	    { showbutton(*winptr[k],up);  savetoimage(winptr[k]);
	      button = winptr[k];  break;
	    }
	}
       if(&win==&_editwindow)
	 { ret = readin(&win);  xmenu->select("编辑窗口");
	   groupattrib_3(true);
	 }
       else if(&win==&_showwindow)
	 { ret = readin(&win);  xmenu->select("试题说明窗口");
	   groupattrib_3(showbox.lock()?false:true);
	 }
       else if(&win==&_helpwindow)
	 { if(Model==Testmodel && Haveseen==false && msgboxsee==true &&
	     msgbox("在试题分析窗口中，您可以看到对该题的详细解释，但您不要"
	      "轻易查看试题分析窗口，除非是在百思不解之后，或程序现在运行已经正确，"
	      "否则您达不到练习的目的。是否要查看试题分析窗口？")==false)  ret = false;
	   else  ret = true;
	   if(ret==false)
	     { showbutton(_helpwindow, up);
	       if(button) { showbutton(*button, down); xmenuselect(button); }
	     }
	   else
	     { ret = readin(&win);   Haveseen = true;
	       xmenu->select("答案说明窗口");
	       groupattrib_3(helpbox.lock()?false:true);
	     }
	 }
       else if(&win==&_tcwindow)     ret = Tcedit();
       else if(&win==&_compilewindow)
	 { ret = readin(&win); xmenu->select("编译窗口");
	   groupattrib_3(false);
	 }
       else if(&win==&_runwindow)
	 { ret = showrunscreen(); showbutton(win,up);
	   if(button&&ret==curset)  showbutton(*button,down);
	 }
     }
   else
     { savetoimage(&win);
       if(deskbox.readinimage("desk.win", "temp\\desktop")==false)
	 ret = deskbox.read(deskfile, "help");
       xmenu->select("显示桌面");  groupattrib_3(false);
     }
   return ret;
 }

void savetoimage(T_Button *win)      //以图象方式保存文件
 { if(win==&_editwindow)      editbox.savetoimage("edit.win", "temp\\window");
   else if(win==&_showwindow) showbox.savetoimage("show.win", "temp\\window");
   else if(win==&_helpwindow) helpbox.savetoimage("help.win", "temp\\window");
   else if(win==&_compilewindow) compilebox.savetoimage("compile.win", "temp\\window");
 }

BOOL readin(T_Button *win)         //读取文件
 { char path[60], file[60];  BOOL ret;
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   sprintf(file, "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   if(win==&_editwindow)
     { if((ret=editbox.readinimage("edit.win", "temp\\window"))==false)
	 { if(Model==Editmodel)  ret = editbox.read("exam.msg", path, all);
	   else if((ret=editbox.read(file))==false)
	      ret = editbox.read("exam.msg", path, Model==Editmodel?all:curset);
	   savetoimage(&_editwindow);
	 }
       editbox.lineable(Examflag==Program||Model==Editmodel?true:false);
     }
   else if(win==&_helpwindow)
     { if((ret=helpbox.readinimage("help.win", "temp\\window"))==false)
	{ ret = helpbox.read("help.msg", path);  savetoimage(&_helpwindow); }
     }
   else if(win==&_showwindow)
     { if((ret=showbox.readinimage("show.win", "temp\\window"))==false)
	{ ret = showbox.read("show.msg", path);  savetoimage(&_showwindow); }
     }
   else if(win==&_compilewindow)
     { if((ret=compilebox.readinimage("compile.win", "temp\\window"))==false)
	 { compilebox.reset();
	   compilebox.addline(" 请先单击\"编辑\"按钮进行编译!");
	   if(msgbox("目前尚未进行编译，是否现在编译？")==true)  Compile();
	   else select.reset();
	 }
       else select.showhead();
       savetoimage(&_compilewindow);
     }
   else
     { if((ret=showbox.readinimage("desk.win", "temp\\window"))==false)
	  ret = deskbox.read(deskfile, "help");
       deskbox.savetoimage("desk.win", "temp\\desktop");
     }
   return ret;
 }


BOOL Tcedit()                  //转入TC编辑器
 { char str[100], path[50];
   sprintf(str,  "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   if(access("temp\\window\\edit.win", 0)==-1)
     if(editbox.read("exam.msg", path, curset)==false)  return false;
   editbox.save(str);
   sprintf(str, "..\\temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   mouse_off();
   FILE *fp = fopen("temp\\process\\editor.msg", "wt");
   fprintf(fp, "%s\n", str);
   fprintf(fp, "%s\n", list->str);
   fprintf(fp, "EXAMFLAG: %d\n", Examflag);
   fprintf(fp, "MODEL: %d\n", Model);
   fprintf(fp, "EXAM: %d\n", Exam);
   fprintf(fp, "START: %d\n", start);
   fprintf(fp, "HAVESEEN: %d\n", Haveseen);
   fcloseall();   savemsg(1);   clearscreenfile();
   disable();  setvect(0x24, olddiskerror);
   setvect(0x1b, olddiskerror);  enable();
   closegraph();  exit(1);
 }

BOOL Compile()       //编译程序
 { char str[150], path[50];
   sprintf(str,  "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   sprintf(path, "exam\\%s\\%s\\%s", exampath, Examflag==Program?"program":"change", list->str);
   if(access("temp\\window\\edit.win", 0)==-1)
     if(readin(&_editwindow)==false)  return false;
   editbox.save(str);
   if(_editwindow.idown()==down) editbox.savetoimage("edit.win", "temp\\window");
   sprintf(str, "tcc -Iinclude -Llib -g12 -j12 %s -n..\\temp\\exefile"
     " ..\\temp\\%s\\%s\\%s.c >..\\temp\\exefile\\compile.msg",
     compilemodel==0?"-ml":"-mt", exampath, Examflag==Program?"program":"change", list->str);
   chdir("compile");   system(str);   chdir("..");
   showallbuttonsup();
   showbutton(_compilewindow,down);
   xmenu->select("编译窗口");  groupattrib_3(false);
   readcompilemsg("temp\\exefile\\compile.msg");
   savetoimage(&_compilewindow);
   return true;
 }

BOOL  runexefile()
 { char str[50], order[150], *path=order;
   if(access("temp\\window\\edit.win", 0)==-1)
      if(readin(&_editwindow)==false)  return false;
   sprintf(str, "temp\\exefile\\%s.exe", list->str);
   Unlink(str);
   sprintf(path,  "temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   editbox.save(path);
   if(_editwindow.idown()==down) editbox.savetoimage("edit.win", "temp\\window");
   sprintf(order, "tcc -Iinclude -Llib -g12 -j12 -n..\\temp\\exefile"
	 " ..\\temp\\%s\\%s\\%s.c >..\\temp\\exefile\\compile.msg",
	     exampath, Examflag==Program?"program":"change", list->str);
   chdir("compile");  system(order);  chdir("..");
   showallbuttonsup();
   showbutton(_compilewindow, down);
   xmenu->select("编译窗口");  groupattrib_3(false);
   int N = readcompilemsg("temp\\exefile\\compile.msg");
   savetoimage(&_compilewindow);
   if(access(str, 0)==-1)  return false;
   if(N)  return false;
   savescreen("temp\\desktop\\desktop.win");
   mouse_off();
   restorecrtmode();
   disable();   setvect(0x1b, oldctrlbreak);   enable();
   chdir("temp\\exefile");   system(list->str);   chdir("..\\..");
   disable();   setvect(0x1b, newctrlbreak);   enable();
   saverunscreen("temp\\window\\run.win");
   mouse_on();   if(waitclick)  waitforclick();   mouse_off();
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   writescreen("temp\\desktop\\desktop.win");
   mouse_on();  Haveseen = true;  return true;
 }

BOOL  showrunscreen()
 { if(access("temp\\window\\run.win", 0)==-1)
     { BOOL RES = msgbox("目前尚未运行该程序，是否现在运行？");
       if(RES == false)  return curset;
       return  runexefile();
     }
   savescreen("temp\\desktop\\desktop.win");
   mouse_off();
   restorecrtmode();
   readrunscreen("temp\\window\\run.win");
   mouse_on();  waitforclick();  mouse_off();
   setgraphmode(VGAHI);   setbkcolor(backcolor);
   writescreen("temp\\desktop\\desktop.win");
   mouse_on();  return true;
 }


void  buildlist()
 { struct ffblk Ffblk;
   if(list!=NULL)  delete list;
   list = new T_Combo(430, 23, 75, 17*20, "请选择", GREEN, WHITE);
   char path[100];
   sprintf(path, "exam\\%s\\%s\\*.*", exampath, Examflag==Program?"program":"change");
   findfirst(path, &Ffblk, FA_DIREC);
   findnext(&Ffblk);
   if(findnext(&Ffblk)==-1)  return;
   do
    { list->add(Ffblk.ff_name);
    } while(findnext(&Ffblk)!=-1);
   list->order();
 }

void  groupattrib_1(BOOL Fa)
 { int k;  char W = Fa==false?Wx:0;
   for(k=0; k<6; k++)  _able(*winptr[k],Fa);
   xmenu->attrib("TC 2.0 编辑器", W);  xmenu->attrib("Turbo C 2.0 窗口", W);
   xmenu->attrib("Turbo C 2.0 编辑器方式", W);
   xmenu->attrib("存入  (F2)", W);  xmenu->attrib("关闭", W);
   xmenu->attrib("编辑窗口", W);  xmenu->attrib("编译窗口", W);
   xmenu->attrib("运行窗口", W);  xmenu->attrib("试题说明窗口", W);
   xmenu->attrib("答案说明窗口", W);  xmenu->attrib("试题分析", W);
   xmenu->attrib("编译", W);  xmenu->attrib("运行程序", W);
   if(Model==Testmodel)  xmenu->attrib("填入参考答案", W); 
   xmenu->attrib("当前试题", W);
   xmenu->attrib("激活", Fa==false?0:Wx);
   xmenu->attrib("导出源文件", W);  xmenu->attrib("导出程序文件", W);
   xmenu->attrib("软件重置", Fa==false?0:Wx);
   xmenu->attrib("制作集成文件", Fa==false?0:Wx);
   if(Examflag==Change&&Model==Testmodel)   xmenu->attrib("标识信息行", W);
   _able(_compile,Fa);  _able(_run,Fa); _able(_reset,Fa);
   if(Fa==false)  Exam = Closed;    else Exam = Opened;
   char w = Exam==Closed?Wx:0;
   if(Model==Editmodel)
    { xmenu->attrib("存入  (F2)", Wx);  xmenu->attrib("关闭  (Alt+Esc)", Wx);
      xmenu->attrib("激活", Wx);        xmenu->attrib("重置(初始化)", Wx);
      xmenu->attrib("标识信息行", Wx);
      xmenu->attrib("撤去标识", Wx);    xmenu->attrib("填入参考答案", Wx);
      xmenu->attrib("撤去参考答案", Wx); xmenu->attrib("Turbo C 2.0 编辑器方式 (F12)", Wx);
      xmenu->attrib("编译...", Wx);     xmenu->attrib("运行程序", Wx);
      xmenu->attrib("TC 2.0 编辑器", Wx);  xmenu->attrib("Turbo C 2.0 窗口", Wx);
      xmenu->attrib("编译窗口", Wx);    xmenu->attrib("运行窗口", Wx);
      _able(_run,false);  _able(_compile,false);  _able(_reset,false); _able(_tcwindow,false);
      _able(_runwindow,false);  _able(_compilewindow,false);
      if(_compilewindow.idown()) { showbutton(_showwindow,down);  windowdeal(_showwindow); }
      xmenu->attrib("全部存入", w); xmenu->attrib("另存为", w);
      xmenu->attrib("从磁盘上移除", w);  xmenu->attrib("特殊格式", w);
      xmenu->attrib("标识答案", w);
      if(Exam==Opened&&_editwindow.idown()==up)
	{ xmenu->attrib("特殊格式", Wx);
	  xmenu->attrib("标识答案", Wx);
	}
    }
   if(Exam==Closed)
    { xmenu->attrib("撤去参考答案", Wx);  xmenu->attrib("撤去标识", Wx); }
 }

void groupattrib_2(BOOL Fa)
 { char W = Fa==false?Wx:0;   
   xmenu->attrib("创建", W);    xmenu->attrib("打开", W);
   xmenu->attrib("重置", Fa==false?0:Wx);
   xmenu->attrib("近期文件", W);
   if(Fa==false)  Model = Testmodel;  else Model = Editmodel;
   char w = Exam==Closed||Model==Testmodel?Wx:0;
   xmenu->attrib("全部存入", w); xmenu->attrib("另存为", w);
   xmenu->attrib("从磁盘上移除", w);  xmenu->attrib("特殊格式", w);
   xmenu->attrib("标注答案", w);
   if(Exam==Opened&&_editwindow.idown()==up)
     { xmenu->attrib("特殊格式", Wx); xmenu->attrib("标注答案", Wx); }
 }

int readcompilemsg(char *filename)
 { FILE *fp = fopen(filename, "rt");
   if(fp==NULL)  return -1;
   char str[202];
   off();  sprintf(str, " 编译 %s.c ...", list->str);  on();
   char tcname[100];
   sprintf(tcname, "..\\temp\\%s\\%s\\%s.c", exampath, Examflag==Program?"program":"change", list->str);
   strlwr(tcname);
   compilebox.reset();
   setcolor(LIGHTCYAN);  print(8, compilebox.y1+2, str);
   select.reset();
   int N = 0, mucherrorflag = 0, errors = 0, warnings = 0;
   while(fgets(str, 200, fp)!=NULL)
    { if(str[0]=='E' || str[0]=='U')  errors ++;
      else if(str[0]=='W')            warnings ++;
      else continue;
      char kind[20], filename[100], errormsg[150];  int errorline;
      sscanf(str, "%s %s %d: %s\n", kind, filename, &errorline, errormsg);
      int i=0, k;
      for(k=0; k<3; k++)   while(str[i++]!=' ');
      strcpy(errormsg, str+i);   errormsg[strlen(errormsg)-1] = '\0';
      if(errormsg[0]=='T'&&errormsg[1]=='o')  mucherrorflag = 1;
      if(str[0]=='E' || str[0]=='W')
	{ if(strcmp(str, "Warning: no stack\n")==0)  { warnings --; continue; }
	  else if(strcmp(filename, tcname)==0)
	    sprintf(str, "%s %d: %s", kind, errorline, errormsg);
	  else sprintf(str, "%s %d: %s (%s)", kind, errorline, errormsg, filename);
	}
      else str[strlen(str)-1] = '\0';
      str[75] = '\0';
      select.add(str);      if(++N>15)  break;
    }
   fclose(fp);
   if(errors)
     { if(mucherrorflag==0)
	 { if(warnings) sprintf(str, " 编译失败: 在编译和连接过程中共发现 %d 处错误和 %d 处警告!", errors, warnings);
	   else         sprintf(str, " 编译失败: 在编译和连接过程中共发现 %d 处错误!", errors);
	 }
       else
	 { if(warnings) sprintf(str, " 编译失败: 目前共发现 %d 处错误和 %d 处警告, 编译过程因错误太多而中断!", errors, warnings);
	   else         sprintf(str, " 编译失败: 目前共发现 %d 处错误, 编译过程因错误太多而中断!", errors);
	 }
     }
   else
     { if(warnings==0)  strcpy(str, " 编译成功: 在编译和连接过程中没有发现任何错误!");
       else  sprintf(str, " 编译成功: 但发现 %d 处警告信息, 请认真审查!", warnings);
     }
   setcolor(LIGHTCYAN);
   mouse_off();  print(8, compilebox.y1+10+(N+1)*20, str);  mouse_on();
   return errors;
 }

void  editforlinenumber(char *str)
 { if(str[strlen(str)-1]==')')
     { msgbox("您只能修改当前文件，不能转至其它文件中！", 1);  return; }
   int linenum = 1;
   if(str[0]=='E' || str[0]=='W')   sscanf(str, "%*s %d: %*s", &linenum);
   showbutton(_editwindow,down);   windowdeal(_editwindow);
   editbox.gotolinenum(linenum-1);  T_LineEdit::K = 0;
 }

BOOL  saverunscreen(char *filename)
 { FILE *fp = fopen(filename, "wt");
   if(fp==NULL)  return false;
   char far *ptr = (char far *)0xb8000000L;
   register int k;
   for(k=0; k<4000; k++)   fputc(*ptr++, fp);
   fputc((char)wherey(), fp);  fputc((char)wherex(), fp);
   fclose(fp);  return true;
 }

BOOL readrunscreen(char *filename)
 { FILE *fp = fopen(filename, "rt");
   if(fp==NULL)  return false;
   char far *ptr = (char far *)0xb8000000L;
   register int k;
   for(k=0; k<4000; k++)   *ptr++ = fgetc(fp);
   gotoxy(fgetc(fp), fgetc(fp));
   fclose(fp);  return true;
 }

void groupattrib_3(BOOL Fa)
 { char W = Fa==false?Wx:0;
   xmenu->attrib("块操作", W);
   xmenu->attrib("光标移动", W);
   xmenu->attrib("读写文件", W);
   xmenu->attrib("寻找与替换", W);
   xmenu->attrib("全选", W);
   xmenu->attrib("整行删除", W);
   xmenu->attrib("剪切", W);
   xmenu->attrib("复制", W);
   xmenu->attrib("粘贴", W);
   xmenu->attrib("删除", W);
 }
