#include <alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <bios.h>
#include <dir.h>
#include "extern.h"
#include "set.h"
#include "mouse.h"
#include "hz.h"
#include "tools.h"
#include "key.h"
#include "click.h"
#include "st-tab.h"

double  get_xf(ST_MSG*);
int   bcode = -1;
char  INFLAG = -1;
char  bname[10][30];
int   namenum;
char  autosave = 1;
char  colorfa = 0;

void  leftbutton_click(int Ch)
 { int  flag = 0, N=0, k, code;  char str[80] = "";
   ST_MSG  *ptr = Home->Next, *ptr0;  float M;
   char bfilename[30];   FILE *fp;
   strcpy(bfilename, filename);
   if(Ch+1&&Ch<=800||Ch>=900&&Ch<1000)
      while(getmousebutton()!=NOBUTTON) show_time(0);
   mouse_on();
   switch(Ch)
    { case -1: if(((bioskey(2)&0x14)||kind==1)&&imouse_inbox(X1+91, Y1+48, X2-10, Y2-9))
		   move_win(); break;
      case  5: if(namenum==0)  break;
	       PX = 5;  PY = 50;
	       code = show_menu(namenum, bname[0], bname[1], bname[2], bname[3],
		 bname[4], bname[5], bname[6], bname[7], bname[8], bname[9] );
	       PX = PY = 0;
	       if(code==-1)  break;
	       if(ask_save()==2)  break;
	       strcpy( filename, bname[code] );
	       if(strcmp(filename, "NONAME")==0)  clear_str(filename);
	       if(strlen(filename)==0)
		 { strcpy(filename, bfilename);    break; }
	       if((fp=fopen(filename, "r"))==NULL)
		 { msg_box(320,240,"打开文件错误：","无法打开指定文件！",1);
		   strcpy(filename, bfilename);	   break;
		 }
	       if(fgetc(fp)-'S'||fgetc(fp)-'X')
		 { msg_box(320,240,"读取文件错误：","该文件的格式不符！",1);
		   strcpy(filename, bfilename);    break;
		 }
	       fclose(fp);
	       free_all(0);  Beginx[0]=Beginx[1]=Beginx[2]=0;
	       saveflag = 0;  jsflag = 0;  Begin = 0;  BeginL = 0;
	       Home->Next = NULL; STnum = 0; see = 0; watch = -2;  End = Home;
	       free(DY);  free(ZY);  free(FJ);
	       if(get_msg(0)!=1)  strcpy(filename, bfilename);  deal_kind(kind);
	       sethz_color(LIGHTGREEN);
	       mouse_off();
	       setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
	       show_hzs(10, 18, Head);
	       bar(50, 80, 96, 100);
	       T0 = clock();
	       show_hzs(50, 100, Man); change_name(0); mouse_on(); break;
      case 22: if(strcmp(filename, "NONAME")==0)
		  { clear_str(filename); leftbutton_click(23); }
	       else  save(1);  T0 = clock(); break;
      case 20: if(ask_save()==2)  break;    free_all(1);
	       Home -> Next = NULL; STnum = 0;  End = Home;
	       watch = -2; see = 0;  saveflag = 0;  jsflag = 0;  bcode = -1;
	       Beginx[0] = Beginx[1] = Beginx[2] = 0;  Begin = 0; BeginL = 0;
	       deal_kind(kind);  strcpy(filename, "NONAME");
	       clear_str(Head);  clear_str(Man);
	       mouse_off();
               sethz_color(LIGHTGREEN);
	       setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
	       show_hzs(10, 18, Head);
	       bar(50, 80, 96, 100);   T0 = clock();
	       show_hzs(50, 100, Man); change_name(0); mouse_on(); break;
      case 21: if(ask_save()==2)  break;
	       if(strcmp(filename, "NONAME")==0)  clear_str(filename);
	       if(input_boxstr(320, 240, "请输入要打开文件的文件名：", filename, 29)==-1) break;
	       if(strlen(filename)==0)
		 { strcpy(filename, bfilename);    break; }
	       format_name(filename);
	       if(strcmp(filename, bfilename)==0)  break;
	       if((fp=fopen(filename, "r"))==NULL)
		 { msg_box(320,240,"打开文件错误：","无法打开指定文件！",1);
		   strcpy(filename, bfilename);	   break;
		 }
	       if(fgetc(fp)-'S'||fgetc(fp)-'X')
		 { msg_box(320,240,"读取文件错误：","该文件的格式不符！",1);
		   strcpy(filename, bfilename);    break;
		 }
	       fclose(fp);
	       free_all(0);  Beginx[0]=Beginx[1]=Beginx[2]=0;
	       saveflag = 0;  jsflag = 0;  Begin = 0;  BeginL = 0;
	       Home->Next = NULL; STnum = 0; see = 0; watch = -2;  End = Home;
	       free(DY);  free(ZY);  free(FJ);
	       if(get_msg(0)!=1)  strcpy(filename, bfilename);  deal_kind(kind);
	       mouse_off();
               sethz_color(LIGHTGREEN);
	       setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
	       show_hzs(10, 18, Head);
	       bar(50, 80, 96, 100);
	       T0 = clock();
	       show_hzs(50, 100, Man); change_name(0); mouse_on(); break;
      case 23: if(strcmp(filename, "NONAME")==0)  clear_str(filename);
	       if(input_boxstr(320, 240, "请输入保存文件的文件名：", filename, 29)==-1) break;
	       if(strlen(filename)==0)  { strcpy(filename, "NONAME"); break; }
               format_name(filename);
	       if(save(0)!=1)  strcpy(filename, bfilename); change_name(0);
	       T0 = clock();  break;
      case 24: PX = 5;  PY = 210;
	       code = show_menu(6, "姓名－学号－学分","姓名－学号－德育成绩",
	       "姓名－学号－智育成绩","姓名－学号－附加分","姓名－学号－各科成绩",
	       "姓名－学号－各科成绩－学分");
	       PX = PY = 0;
	       if(code==-1)  break;
	       if(input_boxstr(320, 240, "请输入保存文本的文件名：", str, 29)==-1) break;
	       if(strlen(str)==0)   break;
	       for(k=0; k<strlen(str); k++)
		 if(str[k]=='.')  break;
	       if(k==strlen(str))  strcat(str, ".txt");
	       if((fp=fopen(str,"r"))!=NULL)
		 { fclose(fp);
		   if(msg_box(320,240,"发现重名文件：", "是否覆盖掉该同名文件？", 0)!=1)
		     break;
		 }
	       fp = fopen(str, "wt");
	       if(fp==NULL)  { msg_box(320, 240, "错误：", "打开文件失败！", 1); break; }
	       if(strlen(Head)!=0) fprintf(fp, "%s\n", Head);
	       if(strlen(Man)!=0)  fprintf(fp, "负责人：%s\n", Man);
	       while(ptr!=NULL)
		{ fprintf(fp, "学号：%3d  姓名：%-6s  ",(int)ptr->Xh, ptr->Name );
		  if(code==1||code==4||code==5)
		    for(k=0;k<DYnum;k++)
		      fprintf(fp, "%8s：%8.4f ", DY[k].Name, ptr->Dy[k]);
		  if(code==2||code==4||code==5)
		    for(k=0;k<ZYnum;k++)
		      fprintf(fp, "%8s：%4.1f ", ZY[k].Name, ptr->Zy[k]);
		  if(code==3||code==4||code==5)
		    for(k=0;k<FJnum;k++)
		      fprintf(fp, "%8s：%8.4f ", FJ[k].Name, ptr->Fj[k]);
		  if(code==0||code==5)
		    fprintf(fp, "学分：%10.6f ", get_xf(ptr));
		  fputc('\n', fp);
		  ptr = ptr->Next;
		}
	       fclose(fp);  msg_box(320, 240, "保存完毕","已经将该数据库信息保存至"
	       "文本形式，您可以将其排版后打印出来！", 1);  break;
      case 0: case 1: case 2:  if(msg==Ch)  break;
	     msg = Ch;  show_msg();       break;
      case 10: if(input_box(250, 113, "请输入主题：", Head, 20)==-1)  break;
	    sethz_color(LIGHTGREEN);
	    setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
	    show_hzs(10, 18, Head); break;
      case 11: clear_bar(); bkcolor = deskcolor;  sethz_color(LIGHTGREEN);
	    get_hzs(50, 100, Man, 3);  break;
      case 40: UP = 0;  PX = 80;
	       switch(show_menu(11, "刷新屏幕", "执行ＤＯＳ命令", "改变当前目录",
			  "新建文件夹", "恢复－磁盘数据", order?"按学分降序排列":
			  "按学分升序排序", "编写计算公式", "调整系统时间",
			  "页面打印", "信息查询", "全部清除"))
		 { case 0: write_screen();  deal_kind(kind); change_name(0); break;
		   case 1: restorecrtmode();  printf("\n\n               ");
			   textcolor(RED);  textbackground(deskcolor);
			   cprintf("Remind: Type \"EXIT\" to return to st-tab system...\n");
			   system(searchpath("command.com"));
			   setgraphmode(VGAHI);
			   write_screen();  deal_kind(kind); change_name(0); break;
		   case 2: { char bstr[50];   getcurdir(0, bstr);
			     str[0] = getdisk()+'A'; str[1] = ':';
			     str[2] = '\\';  str[3] = '\0';
			     strcat(str, bstr);
			     strcpy(bstr, str);
			     if(input_boxstr(320, 240, "请输入想要设置的目录：",
				     str, 29)==-1) break;
			     if(strlen(str)==0)  break;
			     if(strcmp(str,bstr)==0)  break;
			     msg_box(320, 240, "完毕：", chdir(str)==0?
			       "改变当前目录成功！":"改变当前目录失败，未找到所输入的路径!", 1);
			     if(str[1]==':'&&str[2]=='\\')
			       setdisk(str[0]-'a'<0?str[0]-'A':str[0]-'a');
			   }
			   break;
		   case 3: clear_str(str);
			   if(input_boxstr(320, 240, "请输入要建立的文件夹名称",
				    str, 13)==-1) break;
			   if(strlen(str)==0)  break;
			   mkdir(str);
			   msg_box(320, 240, "完毕：", mkdir(str)==-1?
			       "新建文件夹成功！":"新建文件夹失败，可能是所输入的名称有误！", 1);
			   break;
		   case 4: if(strcmp(filename, "NONAME")==0)  break;
			   if(saveflag==0&&jsflag==0)
			    { msg_box(320, 240, "错误：",
				"当前显示内容和磁盘文件内容相同，不必恢复！", 1);
			      break;
			    }
			   if(msg_box(320, 240, "确认恢复数据：", "是否真的要"
			   "放弃修改的内容，恢复为当前磁盘的数据？", 0)-1)  break;
			   fclose(fp);    free_all(0);
			   saveflag = 0;  jsflag = 0;
			   Home->Next = NULL; STnum = 0; see = 0; watch = -2;  End = Home;
			   free(DY);  free(ZY);  free(FJ);
			   get_msg(0);
			   if(Beginx[0]+7>DYnum)  Beginx[0] = DYnum-7;
			   if(Beginx[1]+11>ZYnum) Beginx[1] = ZYnum-11;
			   if(Beginx[2]+5>FJnum)  Beginx[2] = FJnum-5;
			   if(Begin+15>STnum)     Begin = STnum - 15;
			   if(Begin<0)  Begin = 0;
			   for(k=0; k<3; k++)
			     if(Beginx[k]<0)   Beginx[k] = 0;
			   deal_kind(kind);
			   mouse_off();
			   sethz_color(LIGHTGREEN);
			   setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
			   show_hzs(10, 18, Head);
			   bar(50, 80, 96, 100);
			   show_hzs(50, 100, Man);
			   saveflag = 0;  T0 = clock();
			   mouse_on();
			   break;
		   case 5: bcode = -2;  mk_order(bcode);  break;
		   case 6: input_js(); break;
		   case 7: set_time(); break;
		   case 8: print_scr(X1+10, Y1+10, X2-10, Y2-9); break;
		   case 9: show_msgnum();  break;
		   case 10: if(msg_box(320, 240, "确认清空全部：",
			       "是否真的要清空全部数据？", 0)-1)  break;
			   free_all(0);
			   Home -> Next = NULL; STnum = 0;  End = Home;
			   watch = -2; see = 0;  Beginx[0]=Beginx[1]=Beginx[2]=0;
			   saveflag = 1;
			   DYnum=ZYnum=FJnum=0;
			   deal_kind(kind);
			   clear_str(Head); clear_str(Man);
			   mouse_off();
			   sethz_color(LIGHTGREEN);
			   setfillstyle(1, deskcolor);  bar(0, 0, 360, 20);
			   show_hzs(10, 18, Head);
			   bar(50, 80, 96, 100);
			   show_hzs(50, 100, Man); mouse_on(); break;
		  }   UP = 1; PX = 0; break;
      case 42: help = 1;  UP = 0;  PX = 80;
	       switch(code=show_menu(6,"关于……", "济南大学综合测评实施办法",
				    "软件使用说明","快捷功能键汇集", "系统要求","编程思想交流"))
		{ case 0: msg_box(320, 240, "济南大学学生综合测评评分系统",
			  "编程：徐友春  二零零一年十月一日于济南  以此献给"
			  "济南大学所有热爱生活的老师和同学们！", 1); break;
		  case 1: moth_msg(); break;
		  case 2: help_msg(); break;
		  case 3: key_msg();  break;
		  case 4: file_msg(); break;
		  case 5: sys_demo(); break;
		}
		help = 0;  UP = 1; PX = 0;
		if(code==0||code==-1)  break;
		deal_kind(kind); line_button(kind);  break;
      case 41:  UP = 0;  PX = 80;
	      { char scolorfa[15] = "颜色方案～Ⅰ";
		scolorfa[11] += colorfa;
		code = show_menu(14, "设为默认字段", "获取默认字段",
		     "编写计算公式","设为默认计算公式","载入为当前计算公式",
		     "平均值选项", hzkf==0?"应用繁体字体":"应用简体字体",
		     T?"自动保存－√已启用":"自动保存－×未启用",
		     "保留文件名个数", autosave==0?"√打开自动保存设置":"×关闭自动保存设置",
		     "保存内部设置", scolorfa, showenter?"×关闭确认对话框":"√打开确认对话框",
		     SOUND==0?"√打开声音":"×关闭声音");
	      }
		UP = 1;  PX = 0;
	       switch(code)
		{ case 0: set_zd(); break;
		  case 1: leftbutton_click(20); get_zd();
			  deal_kind(kind);  break;
		  case 2: input_js();  break;
		  case 3: set_js();   break;
		  case 4: get_js();
			  if(kind==3||kind==3&&watch==-2)  deal_kind(kind);
			  break;
		  case 5: CLEAR = 1;  fcolor = WHITE;
			  M = (float)B; input_boxnum(320, 240, "一、请输入求平均值时最大项的舍弃个数",
				&M, 3, 0);   B = (int)M;
			  M = (float)E; input_boxnum(320, 240, "二、请输入求平均值时最小项的舍弃个数",
				&M, 3, 0);   E = (int)M;
			  CLEAR = 0;  fcolor = RED; break;
		  case 6: if(hzkf==1)
			   { if((fp=fopen("c:\\ucdos\\hzk16", "rb"))==NULL)
			     if((fp=fopen("d:\\ucdos\\hzk16", "rb"))==NULL)
			     if((fp=fopen("e:\\ucdos\\hzk16", "rb"))==NULL)
			     if((fp=fopen("a:\\ucdos\\hzk16", "rb"))==NULL)
			     fp = fopen(find_name(str,"main.dat"), "rb");
			   }
			  else
			    { if((fp=fopen("c:\\ucdos\\hzk16f", "rb"))==NULL)
			      if((fp=fopen("d:\\ucdos\\hzk16f", "rb"))==NULL)
			      if((fp=fopen("e:\\ucdos\\hzk16f", "rb"))==NULL)
			      if((fp=fopen("a:\\ucdos\\hzk16f", "rb"))==NULL)
			      fp = fopen(find_name(str, "mainf.dat"), "rb");
			    }
			  if(fp == NULL)
			    { msg_box(320, 240, "错误：", hzkf==1?"未发现简体字库！":"未发现繁体字库！", 1);
			      break;
			    }
			  setbuf(fp, NULL);
			  fclose(hzk16fp);  hzk16fp = fp;  hzkf = 1-hzkf;
			  write_screen();  deal_kind(kind); change_name(0); break;
		  case 7: CLEAR = 1;  fcolor = WHITE;
			  M = (float)T;
			  input_boxnum(320, 240, "请输入时间间隔（分钟）输入０表示不启用：",
				&M, 3, 0);   T = (int)M;
			  if(T<0)  T = 0;
			  T0 = clock();
			  CLEAR = 0;  fcolor = RED; break;
		  case 8: CLEAR = 1;  fcolor = WHITE;
			  M = (float)maxfilenum;
			  input_boxnum(320, 240, "请输入保留文件名的个数（０－１０）",
				&M, 3, 0);   maxfilenum = (int)M;
			  if(maxfilenum<0)  maxfilenum = 0;
			  if(maxfilenum>10) maxfilenum = 10;
			  if(namenum>maxfilenum)  namenum = maxfilenum;
			  CLEAR = 0;  fcolor = RED; break;
		  case 9: autosave = 1-autosave; break;
		  case 10: set_sz(); break;
		  case 11: k = 0;
			   switch( code = show_menu(6, "颜色方案Ⅰ－白纸黑字", "颜色方案Ⅱ－萍水相逢",
			   "颜色方案Ⅲ－秋高气爽", "颜色方案Ⅳ－云开日出",  "颜色方案Ⅴ－夕阳初下", "颜色方案Ⅵ－雨过天晴"))
			    { case 0: papercolor = LIGHTGRAY; deskcolor = BLUE; break;
			      case 1: papercolor = WHITE;     deskcolor = LIGHTBLUE;  break;
			      case 2: papercolor = CYAN;      deskcolor = BLUE; break;
			      case 3: papercolor = BROWN;     deskcolor = LIGHTBLUE; break;
			      case 4: papercolor = BROWN;     deskcolor = BLUE; break;
			      case 5: papercolor = GREEN;     deskcolor = BLUE; break;
			      default: k = 1;
			    }
			   if(k==1)  break;
			   colorfa = code;
			   write_screen();  deal_kind(kind); change_name(0);
			   break;
		  case 12: showenter = 1-showenter; break;
		  case 13: SOUND = 1-SOUND;  break;
		} break;
      case 43: Exit(0);  break;
      case 30: case 31: case 32: case 33:
	       kind = Ch-30;  show_ptr();  INFLAG = -1;
	       deal_kind(Ch-30);    break;
      case 50: keyboard_click((bioskey(2)&0x08)?ALT_HOME:HOME_KEY); break;
      case 51: keyboard_click((bioskey(2)&0x08)?ALT_PAGEUP:PAGEUP_KEY); break;
      case 52: keyboard_click((bioskey(2)&0x08)?ALT_LEFT:ALT_UP); break;
      case 53: keyboard_click((bioskey(2)&0x08)?ALT_RIGHT:ALT_DOWN); break;
      case 54: keyboard_click((bioskey(2)&0x08)?ALT_PAGEDOWN:PAGEDOWN_KEY); break;
      case 55: keyboard_click((bioskey(2)&0x08)?ALT_END:END_KEY); break;
      case 60: order = 0;  show_ptr(); mk_order(bcode); break;
      case 61: order = 1;  show_ptr(); mk_order(bcode); break;
      case 7:  PX = 100;  PY = 340;
	       code = order==0?
		   show_menu(4, "以学号为依据按升序排列", "以姓名为依据按升序排列",
				   "以学分为依据按升序排列", "随机打乱排列顺序"):
		   show_menu(4, "以学号为依据按降序排列", "以姓名为依据按降序排列",
				   "以学分为依据按降序排列", "随机打乱排列顺序");
	       PX = PY = 0;
	       switch(code)
		{ case 0: case 1:  mk_order(bcode=code); break;
		  case 2: mk_order(bcode=-2); break;
		  case 3: bcode = -1; mk_order(-4); break;
		} break;
      case 70: BeginL -= 14*3;  if(BeginL<0)  BeginL=0; show_watch(watch); break;
      case 71: BeginL += 14*3;  if(BeginL>STnum) BeginL-=14*3; show_watch(watch); break;
      case 81: if(see!=STnum-1)  { see++;  show_see(see); } break;
      case 80: if(see!=0)        { see--;  show_see(see); } break;
      case 90: if(input_boxnum(320, 240, "请输入学号：", &M, 3, 0)==-1)  break;
               while(ptr!=NULL)
		{ if((int)M==ptr->Xh)  break;
		  N++;  ptr = ptr->Next;
		}
	       if(ptr==NULL)  msg_box(320, 240, "错误：", "所输入的学号未找到！", 1);
	       else show_see(see=N);   break;
      case 91: if(select_stu(str)==0)  break;
	       while(ptr!=NULL)
		{ if(strcmp(str,ptr->Name)==0)  break;
		  N++;  ptr = ptr->Next;
		}
	       if(ptr==NULL)  msg_box(320, 240, "错误：", "所选择的姓名未找到！", 1);
	       else show_see(see=N);   break;
      case 95: show_watch(watch=-2);  break;
      case 96: if(select_name(str)==0) break;
	       for(k=0; k<DYnum; k++)
		 if(strcmp(str, DY[k].Name)==0)
		   { show_watch(watch=2+k); goto end; }
	       for(k=0; k<ZYnum; k++)
		 if(strcmp(str, ZY[k].Name)==0)
		   { show_watch(watch=1002+k); goto end; }
	       for(k=0; k<FJnum; k++)
		 if(strcmp(str, FJ[k].Name)==0)
		   { show_watch(watch=2002+k); goto end; }
		msg_box(320, 240, "错误：", "所选择的科目未找到！", 1);
		end: break;
      case 802: mouse_off();  clear_bar();  mouse_on();
		k = Beginx[msg];
		Beginx[msg] = scroll(X1+70,Y1+3,X2-70,Y1+8,MAXnum-MAX+1,Beginx[msg]);
		if(k-Beginx[msg])  show_msg();
		break;
      case 801: mouse_off();  clear_bar();  mouse_on();
		k = Begin;
		Begin = scroll(X2-8,Y1+70,X2-3,Y2-70,STnum-15+1,Begin);
		if(k-Begin)	show_msg();
		break;
      case 830: keyboard_click(ALT_LEFT);  break;
      case 831: keyboard_click(ALT_RIGHT); break;
      case 832: keyboard_click(ALT_UP);    break;
      case 833: keyboard_click(ALT_DOWN);  break;
      case 805: Begin-=3; if(Begin<0)  Begin=0;  show_msg(); break;
      case 806: Begin+=3; if(Begin+15>STnum)  Begin = STnum-15; show_msg(); break;
      case 807: Beginx[msg]-=3; if(Beginx[msg]<0)  Beginx[msg]=0; show_msg(); break;
      case 808: Beginx[msg]+=3;
		if(Beginx[msg]+MAX>MAXnum) Beginx[msg]=MAXnum-MAX; show_msg(); break;
      case 850: case 851: case 852: case 853: case 854:
		{ int k = Ch-850, M = k==4?5:8, snum=bsee[k];
		  int x1 = X1+20+38, x2 = X2-21-80-(k==4?60:40),
		       num=0, i, y1, H=0, b;
		  for(i=0;i<5;i++)  if(exist[i])  num++;
		  for(i=0;i<k;i++)  if(exist[i])  H++;
		  y1 = (Y2-Y1-num*60)/2+Y1+15;
		  mouse_off(); clear_bar(); mouse_on();	  mouse_on();
		  b = bsee[k];
		  bsee[k]=scroll(x1,y1+H*60+30,x2,y1+H*60+40,exist[k]-M+1,bsee[k]);
		  if(snum==bsee[k])   break;
		  if(b==bsee[k])  break;
		  clear_scroll();  show_see(see);
		  scroll_see(x1,y1+H*60+30,x2,y1+H*60+40,1,exist[k]-M+1, bsee[k]);
		 }	break;
      case 860: case 861: case 862: case 863: case 864:
		{ int k = Ch-860, M = k==4?5:8;
		  int x1 = X1+20+38, x2 = X2-21-80-(k==4?60:40),
		       num=0, i, y1, H=0;
		  for(i=0;i<5;i++)  if(exist[i])  num++;
		  for(i=0;i<k;i++)  if(exist[i])  H++;
		  y1 = (Y2-Y1-num*60)/2+Y1+15;
		  mouse_off(); clear_bar(); mouse_on();
		  bsee[k] -= 3;  if(bsee[k]<0)  bsee[k] = 0;
		  clear_scroll();  show_see(see);
		  scroll_see(x1,y1+H*60+30,x2,y1+H*60+40,1,exist[k]-M+1, bsee[k]);
		  mouse_on();
		} break;
      case 870: case 871: case 872: case 873: case 874:
		{ int k = Ch-870, M = k==4?5:8;
		  int x1 = X1+20+38, x2 = X2-21-80-(k==4?60:40),
		       num=0, i, y1, H=0;
		  for(i=0;i<5;i++)  if(exist[i])  num++;
		  for(i=0;i<k;i++)  if(exist[i])  H++;
		  y1 = (Y2-Y1-num*60)/2+Y1+15;
		  mouse_off(); clear_bar(); mouse_on();
		  bsee[k] += 3;  if(bsee[k]+M>exist[k]) bsee[k] = exist[k]-M;
		  clear_scroll();  show_see(see);
		  scroll_see(x1,y1+H*60+30,x2,y1+H*60+40,1,exist[k]-M+1, bsee[k]);
		  mouse_on();
		} break;
      default: flag = 1;
    }
   if(flag==0)  return;

  /*输入名称*/
   if(Ch>=110&&Ch<=130)
     { int ch = Ch+Beginx[msg], x;
       int KD = msg==0?61:msg==1?39:86;
       x = X1+93+KD*(Ch-110-Beginx[msg])+KD/2;
       if(x+100>630)  x = 630-100;
      if(kind==0)
	switch(msg)
	 { case 1: if(ZYnum==ch-110)  insert_l(ZYnum-Beginx[msg]);
		   else if(log_box(320, 160, &ZY[ch-110])!=-1)
		      show_linemsgy(Ch-110+Beginx[msg]);
		   break;
	   case 0: if(DYnum==ch-110)  insert_l(DYnum-Beginx[msg]);
		   else if(input_box(320, 150, "请输入名称：",
			 DY[ch-110].Name, 4)!=-1)
			   { saveflag = 1; show_linemsgy(Ch-110+Beginx[msg]); }
		   break;
	   case 2: if(FJnum==ch-110)  insert_l(FJnum-Beginx[msg]);
		   else if(input_box(320, 160, "请输入名称：",
		      FJ[ch-110].Name, 4)!=-1)
			{ saveflag = 1; show_linemsgy(Ch-110+Beginx[msg]); }
		   break;
	 }
	else if(kind==1)   mk_order(bcode=ch-110+2+msg*1000);
     }
   else if(kind==1&&(Ch==102||Ch==101))  mk_order(bcode=Ch-101);
   /*输入姓名*/
   else if(Ch>=400&&Ch<=600)
     { ST_MSG  *ptr, *ptr0;  int N = Ch-400;
       int flag = 0;  clear_bar();
       if(kind!=0)  return;
	N += Begin;
       if(STnum==N)
	 { if((ptr=add_point(End))==NULL)  return;
	   flag = 1; ptr0 = End; End = ptr;
	 }
       else  ptr = find_point(N) ;
       sethz_color(BLACK);  bkcolor=papercolor;
       if(get_hzs(X1+42, Y1+66+(Ch-400)*21, ptr->Name, 3)==-1)
	 { if(flag==1)  { End = ptr0;  End->Next=NULL; free(ptr);}  }
       else
	{ saveflag = 1;
	  if(flag==1)  { STnum++;  if(Ch-400==14) { Begin++; move_text(0); } }
	}
       show_linemsg(N);
       barxy(nowx, nowy);
     }
   /*输入学号*/
   else if(Ch>=200&&Ch<=400)
     { ST_MSG  *ptr, *ptr0;  int N = Ch-200;  float num;
       int flag = 0;  clear_bar();
       if(kind!=0)  return;
	N += Begin;
       if(STnum==N)
        { if((ptr=add_point(End))==NULL)  return;
	   flag = 1; ptr0 = End; End = ptr;
	 }
       else  ptr = find_point(N) ;  num = ptr->Xh;
       setcolor(BLACK);  bkcolor=papercolor;
       if(input_num(X1+12, Y1+50+(Ch-200)*21, &num, 3, 0)==-1)
	 { if(flag==1) { End = ptr0; End->Next=NULL; free(ptr); }  }
	else
	 { saveflag = 1;
	   if(flag==1)
	     {  STnum++;  if(Ch-200==14)  { Begin++; move_text(0); } }
	 }
	ptr->Xh = (int)num;
	show_linemsg(N);
	barxy(nowx, nowy);
     }
   /*输入成绩*/
   else if(Ch>=1000)
     { ST_MSG *ptr, *ptr0;  int N = (Ch-1000)/100, M = (Ch-1000)%100;
       int flag = 0;
       int KD = msg==1?39:msg==0?61:86;
       M += Beginx[msg]; N += Begin;
       if(bioskey(2)&0x14||kind==1) { move_win();  return; }
       if(kind!=0)  return;
       if(M >= MAXnum )
	 {  while(kbhit())  getch();  return; }
       clear_bar();
       if(STnum==N)
	 { if((ptr=add_point(End))==NULL)  return ;
	   flag = 1; ptr0 = End; End = ptr;
	 }
       else  ptr = find_point(N);
       setcolor(BLACK);  bkcolor=papercolor;
       if(input_num(X1+(msg==0?102:msg==1?93:105)+KD*(M-Beginx[msg]), Y1+50+(Ch-1000)/100*21,
	  msg==0?(&(ptr->Dy[M])):msg==1?(&(ptr->Zy[M])):(&(ptr->Fj[M])),
	     msg==0?4:msg==1?4:6, msg==0?1:msg==1?1:3)==-1)
	 { if(flag==1)  { End=ptr0; End->Next=NULL; free(ptr); } }
       else
	 { saveflag = 1;
	   if(flag==1)
	     {  STnum++;  if( (Ch-1000)/100==14)  { Begin++; move_text(0); } }
	 }
       show_linemsg(N);
       barxy(nowx, nowy);
     }
  }

int keyboard_click(int Ch)
 { int M = msg==0?DYnum:msg==1?ZYnum:FJnum, flag=1;
   mouse_on();
   switch((unsigned char)Ch)
    { case (unsigned char)ALT_UP:   if(kind<2)
		       { if(!Begin)  break;  Begin--; move_text(1); }
		     break;
      case (unsigned char)ALT_DOWN: if(kind<2)
		       { if(Begin<STnum-1)  { Begin++; move_text(0);} }
		     break;
      case ALT_LEFT: if(kind<2)
		       { if(Beginx[msg]--==0)  Beginx[msg]=0;
			 else move_text(3);
		       }
		     break;
      case ALT_RIGHT:if(kind<2)
		       { if(Beginx[msg]++>=MAXnum-MAX+1)
			    { Beginx[msg]=MAXnum-MAX+1; break; }
			 if(Beginx[msg]<0)
			    { Beginx[msg] = 0; break; }
			 else move_text(2);
		       }
		     break;
      case (unsigned char)ALT_HOME: if(kind<2)
		       { Beginx[msg] = 0;   show_msg(); }
		     break;
      case (unsigned char)ALT_END:  if(kind<2)
		       { Beginx[msg] = MAXnum-MAX;  show_msg(); }
		     break;
      case (unsigned char)ALT_PAGEUP:if(kind<2)
		       { Beginx[msg] -= MAX;  if(Beginx[msg]<0)  Beginx[msg]=0;
			 show_msg();
		       } break;
      case (unsigned char)ALT_PAGEDOWN: if(kind<2)
		       { Beginx[msg] += MAX;
			 if(Beginx[msg]>=MAXnum-MAX+1)  Beginx[msg]=MAXnum-MAX+1;
			 if(Beginx[msg]<0)  Beginx[msg] = 0;
			 show_msg();
		       } break;
      case PAGEUP_KEY:if(STnum<14)  break;
		      if(kind<2)
		       { Begin-=14; if(Begin<0)  Begin = 0; show_msg();}
		       else if(kind==3)
			{ if(see!=0)  { see--;  show_see(see); } }
		       else if(kind==2)
			{ BeginL -= 13*3;  if(BeginL<0)  BeginL=0;
			  show_watch(watch);
			}
		       break;
      case PAGEDOWN_KEY:if(STnum<14) break;
			if(kind<2)
			{ Begin+=14; if(Begin>=STnum-1)  Begin=STnum-1; show_msg(); }
			else if(kind==3)
			 { if(see!=STnum-1)  { see++;  show_see(see); } }
			else if(kind==2)
			 { BeginL += 13*3;  if(BeginL>STnum) BeginL-=13*3;
			   show_watch(watch);
			 }
			break;
      case HOME_KEY: if(kind<2) { Begin = 0; show_msg(); }
		     else if(kind==3)  show_see(see=0);
		     break;
      case END_KEY:  if(kind<2)
		       { Begin = STnum-14; if(Begin<0) Begin=0; show_msg(); }
		      else if(kind==3)  show_see(see=STnum-1);
		      break;
      case UP_KEY:   if(kind==0&&!(bioskey(2)&0x14))
		       { if(bioskey(2)&0x03)  { nowy = 0; break; }
			 else
			   { if(nowy==0)
			       { if(Begin==0) break;  Begin--; move_text(1);}
			     else nowy--;
			   }
			}
		      else if(kind==1||(bioskey(2)&0x14))
			   keyboard_click(ALT_UP); break;
      case DOWN_KEY: if(kind==0&&!(bioskey(2)&0x14))
		       { if(bioskey(2)&0x03)
			    { nowy=STnum-Begin<15?STnum-Begin-1:14; break; }
			 else if(nowy==14)
				{ if(Begin+14==STnum)  break;
				  Begin++; move_text(0);
				}
			 else nowy++;
			}
		     else if(kind==1||(bioskey(2)&0x14))
			keyboard_click(ALT_DOWN); break;
      case LEFT_KEY: if(kind==0&&!(bioskey(2)&0x14))
		      { if(bioskey(2)&0x03)  { nowx = 2; break; }
			else  { if(nowx==0)
				  { if(Beginx[msg]==0)  break;
				    Beginx[msg]--; move_text(3);
				  }
				else nowx--;
			       }
		      }
		     else if(kind==1||(bioskey(2)&0x14))
			  keyboard_click(ALT_LEFT);
		     break;
      case RIGHT_KEY: if(kind==0&&!(bioskey(2)&0x14))
			{ if(bioskey(2)&0x02)
			    { nowx = 2+M-1>MAX+1?MAX+1:2+M-1;  break; }
			  else
			    { if(nowx>=MAX+1)
				{ nowx = MAX+1; Beginx[msg]++; move_text(2);}
			      else nowx++;
			    }
			}
		      else if(kind==1||(bioskey(2)&0x14))
			  keyboard_click(ALT_RIGHT);
		      break;
      case (unsigned char)CTRL_INSERT:  key = 0;
		      if(kind==0)
			 { Beginx[msg]=MAXnum-MAX;
			   if(Beginx[msg])  Beginx[msg]++;
			   if(Beginx[msg]<0)  Beginx[msg]=0;  show_msg();
			   mouse_on();  mouse_on();
			   insert_l(MAXnum-Beginx[msg]);
			   nowx = MAXnum-Beginx[msg]+1;   nowy = 0;
			 }   break;
      case ENTER_KEY:if(kind!=0)      break;
		     if(bioskey(2)&0x14)  break;
		     if(nowx==0)      leftbutton_click(200+nowy);
		     else if(nowx==1) leftbutton_click(400+nowy);
		     else leftbutton_click(1000+nowy*100+nowx-2);
		     break;
      case CTRL_DELETE: if(nowx<2)  break;
		       if(msg_box(320, 240, "确认删除列", "是否真的要删除该列？", 0)==1)
			   deal_l(nowx-2);  break;
      case F9_KEY:
      case CTRL_O:   key = 0; leftbutton_click(21); break;
      case CTRL_Q:   key = 0; Exit(0);  break;
      case CTRL_N:   key = 0; leftbutton_click(20); break;
      case F2_KEY:   key = 0; leftbutton_click(22); break;
      case F12_KEY:  key = 0; leftbutton_click(23); break;
      case F5_KEY:   input_js(); break;
      case TAB_KEY:  line_button(kind=kind+1==4?0:kind+1);
		     show_ptr();    deal_kind(kind);
		     INFLAG = -1;  break ;
      case CTRL_TAB: line_button(kind=kind-1==-1?3:kind-1);
		     show_ptr();    deal_kind(kind);
		     INFLAG = -1;  break ;
      case F1_KEY:   help = 1;   help_msg();  help = 0;
		     deal_kind(kind); line_button(kind);  break;
      case SPACE_KEY: if(kind>=2)  break;
		     kind = 1-kind;    line_button(kind);
		     INFLAG = -1;  break;
      case F7_KEY:   if(kind==2)       leftbutton_click(95);
		     else if(kind==3)  leftbutton_click(90);
		     break;
      case F8_KEY:   if(kind==2)       leftbutton_click(96);
		     else if(kind==3)  leftbutton_click(91);
		     break;
      case CTRL_F9:  print_scr(X1+10, Y1+10, X2-10, Y2-9); break;
      default :      flag = 0;
    }
   if(nowy+Begin>STnum)  nowy = STnum-Begin;
   barxy(nowx, nowy); if(kind>1) clear_bar(); return flag;
 }

void  rightbutton_click(int Ch)
 { int code, k, N, M, flag = 0;
   ST_MSG   *ptr;
   M = msg==0?DYnum:msg==1?ZYnum:FJnum;
   mouse_on();
   while(getmousebutton()!=NOBUTTON) show_time(0);
   switch(Ch)
    { case 880: if(INFLAG!=-1)
		{ code = show_menu(2, INFLAG==0?"返回录入状态":"返回排序状态",
				   "编写计算公式");
		  switch(code)
		   { case 0: line_button(INFLAG); kind = INFLAG;
			     show_ptr();  deal_kind(INFLAG);  INFLAG = -1; break;
		     case 1: input_js();  break;
		   }
		}
		else
		 { code = show_menu(1, "编写计算公式");
		   if(code!=-1)   input_js();
		  }
		break;
      case 881: if(INFLAG!=-1)
		 { code = show_menu(4, INFLAG==0?"返回录入状态":"返回排序状态",
			"升序排列", "降序排列", "编写计算公式");
		   switch(code)
		     { case 0: line_button(INFLAG); kind=INFLAG; show_ptr();
			       deal_kind(INFLAG);  INFLAG = -1; break;
		       case 1:
		       case 2: order = code-1; mk_order(bcode=watch+Beginx[msg]); break;
		       case 3: input_js(); break;
		     }
		 }
		else
		  { code = show_menu(3, "升序排列", "降序排列", "编写计算公式");
		    switch(code)
		     { case 0:  case 1:
			       order = code;  mk_order(bcode=watch); break;
		       case 2:  input_js(); break;
		     }
		  }
		break;
      case 835: PY = 450;
		if(show_menu(1, "调整系统时间")==0)  set_time();
		PY = 0; break;
      default: flag = 1;
    }
   if(flag==0)  return;
   if(Ch>=110&&Ch<130||Ch==101||Ch==102)
     { int MC = msg==1?39:msg==0?61:86;
       int x=(Ch-110)*MC+X1+93+M/2, y=Y1+49+50;
       if(x+170>639)  x=639-170;  if(y+50>479)  y=479-150;
       if(kind==0&&Ch>=110)
	{ N = Ch-110;
	  if(N+Beginx[msg]>=MAXnum)  return;
	  code = show_menu(5, "浏览该列", "删除当前列", "当前列左移", "当前列右移",
		  "插入一列");
	  if(code==-1)  return;
	  switch(code)
	    { case 1:  if(N+Beginx[msg]!=M && msg_box(x, y, "确认删除列", "是否真的要删除该列？", 0)==1)
			   deal_l(N);  break;
	      case 2:  if(N+Beginx[msg]-1<0)  break;  move_l(N-1, N);  break;
	      case 3:  if(N+Beginx[msg]+1>=M) break;  move_l(N, N+1);  break;
	      case 4:  insert_l(N); break;
	      case 0:  line_button(2); kind=2;  INFLAG = 0;
		       show_watch(watch=msg*1000+2+N+Beginx[msg]); break;
	    }
	}
       else if(kind==1)
	 { N = Ch-110+2; if(N<0)  N+=7;
	   code = N<2?show_menu(2, "以该列升序排列","以该列降序排列")
	    : show_menu(3, "以该列升序排列", "以该列降序排列", "浏览该列");
	   switch(code)
	    { case 0: order = 0; mk_order(bcode=N<2?N:N+msg*1000+Beginx[msg]); show_ptr(); break;
	      case 1: order = 1; mk_order(bcode=N<2?N:N+msg*1000+Beginx[msg]); show_ptr(); break;
	      case 2: line_button(2); kind=2; INFLAG = 1;
		      show_watch(watch=msg*1000+N+Beginx[msg]); break;
	    }
	}
     }
   else if(Ch>=400&&Ch<600)
     { int x=X1+93, y=Y1+49+21*(Ch-400)+10+50;
       if(x+170>639)  x=639-170;  if(y+50>479)  y=479-150;
       N = Ch-400 + Begin;
       if(N>=STnum)  return;
       if(kind==0)
	 { code = show_menu(5, "查询该行", "删除当前行", "当前行上移", "当前行下移",
			   "插入一行");
	   if(code==-1)  return;
	   switch(code)
	     { case 1: if(N!=STnum && msg_box(x, y, "确认删除行","是否真的要删除该行？",0)==1)
			  { bcode = -1; deal_h(N); } break;
	       case 2: if(N-1<0)  break;   bcode = -1;   move_h(N-1, N);   break;
	       case 3: if(N+1>=STnum) break; bcode = -1; move_h(N, N+1);   break;
	       case 4: if(N>=STnum)   break; bcode = -1; insert_h(N);      break;
	       case 0: line_button(3);  kind = 3;  show_ptr();
		       show_see(see=N); INFLAG = 0;  break;
	     }
	 }
       else if(kind==1)
	 { code = show_menu(1, "查询该行");
	   switch(code)
	    { case 0: line_button(3); kind = 3; show_ptr(); show_see(see=N);
		      INFLAG = 1; break;
	    }
	 }
     }
   else if(Ch>=1000)
      { int Ny = (Ch-1000)/100+Begin, Nx = (Ch-1000)%100;
	int MM = msg==1?39:msg==0?61:86;
	int x=Nx*MM+X1+93+M/2, y=Y1+49+21*Ny+10+50;
	if(x+170>639)  x=639-170;  if(y+50>479)  y=479-150;
	if(kind!=0)  return;
	if(bioskey(2)&0x14)  { rightbutton_click(-1); return; }
	code = show_menu(4, "清空该单元格", "清空该行",
			    "清空该列", "以平均值方式输入");
	if(code==-1)  return;
	switch(code)
	 { case 1:  if(Ny!=STnum && msg_box(x, y, "确定清空行",
		      "是否真的要清空该行数据？",0)==1)  clear_h(Ny); break;
	   case 2:  if(Nx!=M && msg_box(x, y, "确定清空列",
		      "是否真的要清空该列数据？",0)==1)  clear_l(Nx); break;
	   case 0:  if(Ny!=STnum&&Nx!=M)  clear_g(Ny, Nx);  break;
	   case 3:  if(Nx!=M)      arg_in(Ny, Nx);   break;
	 }
      }
   else { code = show_menu(5, "保存当前文件", "打开现有文件",
		     "执行ＤＯＳ命令", "编写计算公式", "使用说明" );
	  switch(code)
	   { case 0: leftbutton_click(22); break;
	     case 1: leftbutton_click(21); break;
	     case 2: restorecrtmode();  printf("\n\n               ");
		     textcolor(RED);  textbackground(deskcolor);
		     cprintf("Remind: Type \"EXIT\" to return to st-tab system...\n");
		     system(searchpath("command.com"));
		     setgraphmode(VGAHI);
		     write_screen();  deal_kind(kind); change_name(0); break;
	     case 3: input_js(); break;
	     case 4: help = 1;   help_msg();  help = 0;
		     deal_kind(kind); line_button(kind);  break;
	   }
	}
 }
void  arg_in(int Ny, int Nx)
 { ST_MSG *ptr, *ptr0;
   int x, y, y0;
   int flag = 0;
   int KD = msg==1?39:msg==0?61:86;
   Nx += Beginx[msg];
   clear_bar();
   if(STnum==Ny)
     { if((ptr=add_point(End))==NULL)  return;
       flag = 1; ptr0 = End; End = ptr;
     }
   else  ptr = find_point(Ny);
   bkcolor=papercolor;
   x = X1+91+KD*(Nx-Beginx[msg]);
   y = Y1+47+(Ny-Begin)*21 ;
   y0 = y;
   mouse_off();
   setlinestyle(2, 0, 3);   setcolor(YELLOW^papercolor);  setwritemode(XOR_PUT);
   rectangle(x, y0, x+KD, y0+21);
   setwritemode(COPY_PUT);  setlinestyle(0, 0, 1);
   setcolor(BLACK);  mouse_on();
   if(y>240)  y -= 155;
   else       y += 25;
   if(avr_input(x, y, msg==0?(&(ptr->Dy[Nx])):msg==1?
	(&(ptr->Zy[Nx])):(&(ptr->Fj[Nx])),
	    msg==0?4:msg==1?4:6, msg==0?1:msg==1?1:3)==-1)
      { if(flag==1)  { End=ptr0; End->Next=NULL; free(ptr); } }
   else
     { saveflag = 1;
       if(flag==1)
	{  STnum++;  if( Ny-Begin==14)  { Begin++; move_text(0); } }
     }
   mouse_off();
   setlinestyle(2, 0, 3);  setcolor(YELLOW^papercolor);  setwritemode(XOR_PUT);
   rectangle(x, y0, x+KD, y0+21);
   setwritemode(COPY_PUT); setlinestyle(0, 0, 1);
   mouse_on();
   show_linemsg(Ny);
 }

void  clear_g(int Ny, int Nx)
 { ST_MSG *ptr = find_point(Ny);
   switch(msg)
    { case 0: ptr->Dy[Nx] = 0; break;
      case 1: ptr->Zy[Nx] = 0; break;
      case 2: ptr->Fj[Nx] = 0; break;
    }
   saveflag = 1;
   show_linemsg(Ny);
 }

void  clear_h(int N)
 { ST_MSG *ptr;  int k;
   ptr = find_point(N);
   switch(msg)
    { case 0: for(k=0; k<DYnum; k++)   ptr->Dy[k] = 0; break;
      case 1: for(k=0; k<ZYnum; k++)   ptr->Zy[k] = 0; break;
      case 2: for(k=0; k<FJnum; k++)   ptr->Fj[k] = 0; break;
    }
   saveflag = 1;
   show_linemsg(N);
 }

void  clear_l(int N)
 { ST_MSG *ptr = Home;
   N += Beginx[msg];
   while(ptr!=NULL)
     { switch(msg)
	{ case 0: ptr->Dy[N] = 0; break;
	  case 1: ptr->Zy[N] = 0; break;
	  case 2: ptr->Fj[N] = 0; break;
	}
       ptr = ptr->Next;
     }
   saveflag = 1;
   show_msg();
 }

void  insert_h(int N)
 { ST_MSG *ptr, *ptr0;
   int k;
   if(N==0)  ptr0 = Home;
   else      ptr0 = find_point(N-1);
   ptr = (ST_MSG*)malloc(sizeof(ST_MSG));
   if(ptr==NULL||ptr0==NULL)  return;
   ptr->Dy = (float*)malloc(DYnum*sizeof(float));
   ptr->Zy = (float*)malloc(ZYnum*sizeof(float));
   ptr->Fj = (float*)malloc(FJnum*sizeof(float));
   if(!ptr->Dy||!ptr->Zy||!ptr->Fj)   return;
   for(k=0; k<DYnum; k++)   ptr->Dy[k] = 0.0;
   for(k=0; k<ZYnum; k++)   ptr->Zy[k] = 0.0;
   for(k=0; k<FJnum; k++)   ptr->Fj[k] = 0.0;
   ptr->Xh = 0;
   ptr->Name[0] = '\0';
   ptr->Next = ptr0->Next;
   ptr0->Next = ptr;
   STnum++;
   saveflag = 1;
   show_msg();
 }

void  deal_h(int N)
 { ST_MSG  *ptr, *ptr0;
   if(N==0)  ptr0 = Home;
   else      ptr0 = find_point(N-1);
   ptr = find_point(N);
   ptr0->Next = ptr->Next;
   free(ptr);  STnum--;  show_msg();
   if(N==STnum)  End = ptr0;
   saveflag = 1;
 }

void  move_h(int N1, int N2)
 { ST_MSG  *ptr1, *ptr2 ;
   int k;  float t;  char S[20];
   ptr1 = find_point(N1);
   ptr2 = find_point(N2);
   data_chang(ptr1, ptr2);
   saveflag = 1;
   show_msg();
 }

void  deal_l(int N)
 { int k; ST_MSG *ptr;
   char str[10];
   bk_str(str);
   N += Beginx[msg];
   switch(msg)
    { case 0: for(k=N; k<DYnum-1; k++)
		strcpy(DY[k].Name, DY[k+1].Name);
	      DY = (struct _DY*)realloc(DY, (DYnum-1)*sizeof(struct _DY));
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ for(k=N; k<DYnum-1; k++)
		     ptr->Dy[k] = ptr->Dy[k+1];
		  ptr->Dy = (float*)realloc(ptr->Dy, (DYnum-1)*sizeof(float));
		  ptr = ptr->Next;
		}
	      DYnum--;  saveflag = 1;
	      format_msg();  show_msg();
	      if((watch-2)%1000>=DYnum)  watch = -2;
	      break;
     case 1: for(k=N; k<ZYnum-1; k++)
		{ strcpy(ZY[k].Name, ZY[k+1].Name);
		  ZY[k].Attrib = ZY[k+1].Attrib;
		  ZY[k].Xf = ZY[k+1].Xf;
		}
	      ZY = (struct _ZY*)realloc(ZY, (ZYnum-1)*sizeof(struct _ZY));
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ for(k=N; k<ZYnum-1; k++)
		     ptr->Zy[k] = ptr->Zy[k+1];
		  ptr->Zy = (float*)realloc(ptr->Zy, (ZYnum-1)*sizeof(float));
		  ptr = ptr->Next;
		}
	      ZYnum--;
	      format_msg();  show_msg();
	      if((watch-2)%1000>=ZYnum)  watch = -2;
	      saveflag = 1;
	      break;
     case 2: for(k=N; k<FJnum-1; k++)
		strcpy(FJ[k].Name, FJ[k+1].Name);
	      FJ = (struct _FJ*)realloc(FJ, (FJnum-1)*sizeof(struct _FJ));
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ for(k=N; k<FJnum-1; k++)
		     ptr->Fj[k] = ptr->Fj[k+1];
		  ptr->Fj = (float*)realloc(ptr->Fj, (FJnum-1)*sizeof(float));
		  ptr = ptr->Next;
		}
	      FJnum--;
	      format_msg();  show_msg();
	      if((watch-2)%1000>=FJnum)  watch = -2;
	      saveflag = 1;
	      break;
   }
   bcode = find_order(str);
 }

void  move_l(int N1, int N2)
 { int k;  ST_MSG *ptr;
   float t; char n;  char S[20];
   char str[10];
   bk_str(str);
   N1 += Beginx[msg];  N2 += Beginx[msg];
   switch(msg)
    { case 0: strcpy(S, DY[N1].Name);
	      strcpy(DY[N1].Name, DY[N2].Name);
	      strcpy(DY[N2].Name, S);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ t = ptr->Dy[N1];
		  ptr->Dy[N1] = ptr->Dy[N2];
		  ptr->Dy[N2] = t;
		  ptr = ptr->Next;
		}
	      saveflag = 1;
	      show_msg();  break;
     case 1:  strcpy(S, ZY[N1].Name);
	      strcpy(ZY[N1].Name, ZY[N2].Name);
	      strcpy(ZY[N2].Name, S);
	      n = ZY[N1].Attrib;
	      ZY[N1].Attrib = ZY[N2].Attrib;
	      ZY[N2].Attrib = n;
	      t =  ZY[N1].Xf;
	      ZY[N1].Xf = ZY[N2].Xf;
	      ZY[N2].Xf = t;
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ t = ptr->Zy[N1];
		  ptr->Zy[N1] = ptr->Zy[N2];
		  ptr->Zy[N2] = t;
		  ptr = ptr->Next;
		}
	      saveflag = 1;
	      show_msg();  break;
     case 2:  strcpy(S, FJ[N1].Name);
	      strcpy(FJ[N1].Name, FJ[N2].Name);
	      strcpy(FJ[N2].Name, S);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ t = ptr->Fj[N1];
		  ptr->Fj[N1] = ptr->Fj[N2];
		  ptr->Fj[N2] = t;
		  ptr = ptr->Next;
		}
	      saveflag = 1;
	      show_msg();  break;
   }
   bcode = find_order(str);
 }

void  insert_l(int N)
 { int k, x, KD;
   char S[20] = "";
   ST_MSG  *ptr;
   struct _ZY  A = { "", 0, 0 };
   char str[10];
   bk_str(str);
   KD = msg==0?61:msg==1?39:86;
   x = X1+93+KD*N+KD/2;
   if(x+(msg==1?100:200)>630)  x = 630-(msg==1?100:200);
   N += Beginx[msg];
   switch(msg)
    { case 0: if(input_box(x, 150, "请输入名称：", S, 4)==-1)  break;
	      DY = (struct _DY*)realloc(DY, (DYnum+1)*sizeof(struct _DY));
	      for(k=DYnum; k>N; k--)
		strcpy(DY[k].Name, DY[k-1].Name);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Dy = (float*)realloc(ptr->Dy, (DYnum+1)*sizeof(float));
		  for(k=DYnum; k>N; k--)
		     ptr->Dy[k] = ptr->Dy[k-1];
		  ptr = ptr->Next;
		}
	      strcpy(DY[N].Name, S);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Dy[N] = 0;  ptr = ptr->Next; }
	      if(Beginx[msg]+MAX==DYnum)  Beginx[msg]++;
	      DYnum++;  saveflag = 1; show_msg();  break;
     case 1: if(log_box(x, 150, &A)==-1)  break;
	     ZY = (struct _ZY*)realloc(ZY, (ZYnum+1)*sizeof(struct _ZY));
	     for(k=ZYnum; k>N; k--)
		{ strcpy(ZY[k].Name, ZY[k-1].Name);
		  ZY[k].Attrib = ZY[k-1].Attrib;
		  ZY[k].Xf = ZY[k-1].Xf;
		}
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Zy = (float*)realloc(ptr->Zy, (ZYnum+1)*sizeof(float));
		  for(k=ZYnum; k>N; k--)
		     ptr->Zy[k] = ptr->Zy[k-1];
		  ptr = ptr->Next;
		}
              ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Zy[N] = 0;  ptr = ptr->Next; }
	      strcpy(ZY[N].Name, A.Name);
	      ZY[N].Attrib = A.Attrib;  ZY[N].Xf = A.Xf;
	      if(Beginx[msg]+MAX==ZYnum)  Beginx[msg]++;
	      ZYnum++;  saveflag = 1; show_msg();  break;
     case 2: if(input_box(x, 150, "请输入名称：", S, 4)==-1)  break;
	     FJ = (struct _FJ*)realloc(FJ, (FJnum+1)*sizeof(struct _FJ));
	      for(k=FJnum; k>N; k--)
		strcpy(FJ[k].Name, FJ[k-1].Name);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Fj = (float*)realloc(ptr->Fj, (FJnum+1)*sizeof(float));
		  for(k=FJnum; k>N; k--)
		     ptr->Fj[k] = ptr->Fj[k-1];
		  ptr = ptr->Next;
		}
	      strcpy(FJ[N].Name, S);
	      ptr = Home->Next;
	      while(ptr!=NULL)
		{ ptr->Fj[N] = 0;  ptr = ptr->Next; }
	      if(Beginx[msg]+MAX==FJnum)  Beginx[msg]++;
	      FJnum++;  saveflag = 1; show_msg();  break;
   }
   bcode = find_order(str);
 }

void  barxy(int x, int y)
 {  int M = msg==1?39:msg==0?61:86;
    int N = MAXnum;
    if(kind!=0)  return;
    if(bioskey(2)&0x14)  return;
    if(Beginx[msg]+x>=N+1)    nowx = x = N+1-Beginx[msg];
    if(Begin+y>=STnum)        nowy = y = STnum-Begin;
    not_bar(bx1, by1, bx2, by2);
    if(x==0) { bx1=X1+11; by1=Y1+49+y*21; bx2=X1+38; by2=Y1+45+(y+1)*21; }
    else if(x==1) { bx1=X1+42; by1=Y1+49+y*21; bx2=X1+89; by2=Y1+45+(y+1)*21; }
    else { bx1=X1+93+M*(x-2);      by1=Y1+49+21*y;
	   bx2=X1+89+M*((x-2)+1);  by2=Y1+45+21*(y+1); }
    not_bar(bx1, by1, bx2, by2);
 }

void  mk_order(int code)
 { ST_MSG *ptr1, *ptr2;   int Or;
   if(code == -1)  return;
   ptr1 = Home->Next;
   while(ptr1!=NULL)
    { ptr2 = ptr1->Next;
      while(ptr2!=NULL)
       { Or = data_cmp(ptr1, ptr2, code);
	 if(Or==-3)  return;
	      if(order==0 && Or==1)   data_chang(ptr1, ptr2);
	 else if(order==1 && Or==-1)  data_chang(ptr1, ptr2);
	 ptr2 = ptr2->Next;
       }
      ptr1 = ptr1->Next;
    }
   deal_kind(kind);
 }
int  data_cmp(ST_MSG *ptr1, ST_MSG *ptr2, int code)
 { int cmd, Nx = (code-2)%1000;  float A, B;
   if(code == -4)  return  random(2);
   else if(code==-2)
     { A = (float)get_xf(ptr1);
       if(ERR==FALSE)  { start[inexist] = bstart; input_js(); return -3; }
       B = (float)get_xf(ptr2);
       if(ERR==FALSE)  { start[inexist] = bstart; input_js(); return -3; }
       return A>B?1:A<B?-1:0;
     }
   else if(code==0) return ptr1->Xh>ptr2->Xh?1:ptr1->Xh<ptr2->Xh?-1:0;
   else if(code==1)
     { cmd = strcmp(ptr1->Name, ptr2->Name) ;
       return cmd>0?1:cmd<0?-1:0;
     }
   else
     switch((code-2)/1000)
      { case 0: A = ptr1->Dy[Nx];  B = ptr2->Dy[Nx]; break;
	case 1: A = ptr1->Zy[Nx];  B = ptr2->Zy[Nx]; break;
	case 2: A = ptr1->Fj[Nx];  B = ptr2->Fj[Nx]; break;
      }
   return A>B?1:A<B?-1:0;
 }
void  data_chang(ST_MSG *ptr1, ST_MSG *ptr2)
 { int k;
   char xh; float t; char S[20];
   xh = ptr1->Xh;  ptr1->Xh = ptr2->Xh;  ptr2->Xh=xh;
   strcpy(S, ptr1->Name); strcpy(ptr1->Name, ptr2->Name); strcpy(ptr2->Name, S);
   for(k=0; k<DYnum; k++)
    { t = ptr1->Dy[k];  ptr1->Dy[k] = ptr2->Dy[k]; ptr2->Dy[k] = t; }
   for(k=0; k<ZYnum; k++)
    { t = ptr1->Zy[k];  ptr1->Zy[k] = ptr2->Zy[k]; ptr2->Zy[k] = t; }
   for(k=0; k<FJnum; k++)
    { t = ptr1->Fj[k];  ptr1->Fj[k] = ptr2->Fj[k]; ptr2->Fj[k] = t; }
 }

void  write_fl(int code)
 { char S[20];
	if(code==0)  strcpy(S, "学号");
   else if(code==-2) strcpy(S, "学分");
   else if(code==1)  strcpy(S, "姓名");
   else if(code==-1) strcpy(S, "（未排）");
   else
     switch((code-2)/1000)
      { case 0: strcpy(S, DY[(code-2)%1000].Name); break;
	case 1: strcpy(S, ZY[(code-2)%1000].Name); break;
	case 2: strcpy(S, FJ[(code-2)%1000].Name); break;
      }
   clear_bar();
   setfillstyle(1, deskcolor);  bar(100, 424, 244, 449);
   sethz_color(LIGHTGREEN);
   show_hzs(100, 445, "排序依据：");
   show_hzs(100+16*5, 445, S);
 }

void  deal_kind(int N)
 { mouse_off();
   switch(N)
    { case 0:
      case 1: show_msg();  break;
      case 2: show_watch(watch); break;
      case 3: show_see(see); break;
    }
   show_time(1);  line_button(kind);
   mouse_on();
 }

int  ask_save()
 { int ch;
   if(saveflag==0&&jsflag==0)   return -1;
   if(saveflag&&(ch=msg_box(320, 240, "询问是否存盘",
	 "当前文件数据被修改，是否现在存盘？",0))==1)  save(1);
   else if(jsflag&&(ch=msg_box(320, 240, "询问是否存盘",
	 "当前文件计算公式被修改，是否现在存盘？",0))==1)  save(1);
   return ch;
 }

void  write_fb()
 { int k;
   setfillstyle(1, RED);
   bar(X1+12, Y1+2, X1+60, Y1+9);
   bar(X2-60, Y1+2, X2-12, Y1+9);
   bar(X2-2, Y1+12, X2-9, Y1+60);
   bar(X2-2, Y2-12, X2-9, Y2-60);
   sethz_color(WHITE);
   if(Beginx[msg]!=0)          show_hzs(X1+15, Y1+15, "←…");
   if(Beginx[msg]+MAX<MAXnum)  show_hzs(X2-50, Y1+15, "…→");
   if(Begin!=0)
     { show_hzs(X2-14, Y1+30, "↑");
       for(k=0; k<3; k++)  show_hzs(X2-13, Y1+43+5*k, "·");
     }
   if(Begin+15<STnum)
     { show_hzs(X2-14, Y1+359, "↓");
       for(k=0; k<3; k++)  show_hzs(X2-13, Y1+336+5*k, "·");
     }
 }

int  find_order(char *S)
 { int k;
   if(strcmp(S, "学分")==0)      return -2;
   if(strcmp(S, "（未排）")==0)  return -1;
   if(strcmp(S, "学号")==0)      return  0;
   if(strcmp(S, "姓名")==0)      return  1;
   for(k=0; k<DYnum; k++)
    if(strcmp(S, DY[k].Name)==0)  return 2+k;
   for(k=0; k<ZYnum; k++)
    if(strcmp(S, ZY[k].Name)==0)  return 1002+k;
   for(k=0; k<FJnum; k++)
    if(strcmp(S, FJ[k].Name)==0)  return 2002+k;
   return -1;
  }

void  bk_str(char *S)
 { int Nx, Ny;
	if(bcode == -2)  strcpy(S, "学分");
   else if(bcode == -1)  strcpy(S, "（未排）");
   else if(bcode == 0)   strcpy(S, "学号");
   else if(bcode == 1)   strcpy(S, "姓名");
   else
    { Ny = (bcode-2)/1000;  Nx = (bcode-2)%1000;
      switch(Ny)
       { case 0: strcpy(S, DY[Nx].Name); break;
	 case 1: strcpy(S, ZY[Nx].Name); break;
	 case 2: strcpy(S, FJ[Nx].Name); break;
       }
    }
 }

void  move_win()
{ int KD = msg==1?39:msg==0?61:86;
  int MX, MY, X0, Y0, _X, _Y, dx, dy;
  int x1 = X1+10, y1 = Y1+10, x2 = X2-10, y2 = Y2-9;
  getmouse_xy(&MX, &MY);
  _X = MX;  _Y = MY;
  setwritemode(XOR_PUT);
  setlinestyle(3, 0, 3);
  setcolor(YELLOW);
  mouse_off(); rectangle(x1, y1, x2, y2); mouse_on();
  while(getmousebutton()==LEFTBUTTON)
   { getmouse_xy(&X0, &Y0);
     setwritemode(COPY_PUT);
     show_time(0);
     setwritemode(XOR_PUT);  setcolor(YELLOW);
     if(X0==MX && Y0==MY)  continue;
     mouse_off();
     rectangle(x1, y1, x2, y2);
     x1 -= MX-X0;  y1 -= MY-Y0;
     x2 -= MX-X0;  y2 -= MY-Y0;
     rectangle(x1, y1, x2, y2);
     mouse_on();
     MX = X0;  MY = Y0;
   }
  mouse_off();
  rectangle(x1, y1, x2, y2);
  mouse_on();
  setlinestyle(0, 0, 1);
  setwritemode(COPY_PUT);
  dx = (_X-X0<0?_X-X0-KD/2:_X-X0+KD/2)/KD;
  dy = (_Y-Y0<0?_Y-Y0-10:_Y-Y0+10)/21;
  if(_X==X0&&_Y==Y0&&kind==0)
    { msg_box(320, 240, "提示：", "若想输入数据，请关闭ＳＣＲＯＬＬＬＯＣＫ开关，并松开"
			"ＣＴＲＬ键！", 1);
      return;
    }
  if(dx==0&&dy==0)  return;
  Beginx[msg] += dx;
  if(Beginx[msg]>MAXnum-MAX)  Beginx[msg] = MAXnum-MAX;
  if(Beginx[msg]<0)  Beginx[msg] = 0;
  Begin += dy;
  if(Begin>STnum-15)  Begin = STnum-15;
  if(Begin < 0)       Begin = 0;
  clear_bar();  deal_kind(kind);
 }

void  format_msg()
 { if(Beginx[msg]+MAX>MAXnum)  Beginx[msg] = MAXnum-MAX;
   if(Beginx[msg]<0)  Beginx[msg] = 0;
 }

void  show_msgnum()
 { int x1 = 320-150,       x2 = 320+150,
       y1 = 240-88,        y2 = 240+88;
   int num[3], k;
   char far *ptr;
   ptr = (char far*)farmalloc(imagesize(x1, y1, x2+5, y2+5));
   if(ptr==NULL)  return;
   num[0] = num[1] = num[2] = 0;
   for(k=0; k<ZYnum; k++)  num[ZY[k].Attrib] ++;
   mouse_off();  clear_bar();
   getimage(x1, y1, x2+5, y2+5, ptr);
   setfillstyle(1, DARKGRAY);
   bar(x1+5, y1+5, x2+5, y2+5);
   setfillstyle(1, GREEN);
   bar(x1, y1, x2, y2);
   setcolor(WHITE);
   rectangle(x1+3, y1+3, x2-3, y2-3);
   show_hzs(x1+10, y1+25, "文件名：");
   show_hzs(x1+10, y1+45, "德育项目总数：");
   show_hzs(x1+10, y1+65, "智育项目总数：");
   sethz_color(LIGHTRED);
   show_hzs(x1+10, y1+85,  "  必修：");
   show_hzs(x1+10, y1+105, "  限选：");
   show_hzs(x1+10, y1+125, "  任选：");
   sethz_color(RED);
   show_hzs(x1+10, y1+145, "附加项目总数：");
   show_hzs(x1+10, y1+165, "人员总数：");
   setcolor(BLUE);
   settextstyle(0, 0, 1);
   settextjustify(LEFT_TEXT, TOP_TEXT);
   outtextxy( x1+80, y1+12, filename );
   settextstyle(2, 0, 6);
   show_nums(x1+130, y1+30, DYnum, 5, 0);
   show_nums(x1+130, y1+50, ZYnum, 5, 0);
   show_nums(x1+130, y1+130, FJnum, 5, 0);
   show_nums(x1+130, y1+150, STnum, 5, 0);
   setcolor(LIGHTBLUE);
   for(k=0; k<3; k++)
     show_nums(x1+130, y1+70+k*20, num[k], 5, 0);
   sethz_color(LIGHTGREEN);
   show_hzs(x1+180, y1+80, "按任意键或");
   show_hzs(x1+180, y1+100,"单击鼠标时");
   show_hzs(x1+180, y1+120, "将关闭该窗");
   show_hzs(x1+180, y1+140, "口……");
   mouse_on();
   while(!kbhit()&&getmousebutton()==NOBUTTON)  show_time(0);
   while(getmousebutton()!=NOBUTTON)   show_time(0);
   while(kbhit())  getch();
   mouse_off();  clear_bar();
   putimage(x1, y1, ptr, COPY_PUT);
   farfree(ptr);
   mouse_on();
 }