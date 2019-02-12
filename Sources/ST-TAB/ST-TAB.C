#include <stdio.h>
#include <graphics.h>
#include <dos.h>
#include <time.h>
#include <alloc.h>
#include <string.h>
#include <bios.h>
#include <dir.h>
#include "hz.h"
#include "mouse.h"
#include "tools.h"
#include "key.h"
#include "screen.h"
#include "extern.h"
#include "st-tab.h"
#include "set.h"

FILE  *hzk24fp, *hzk16fp;
int   DYnum, ZYnum, FJnum, STnum;
int   Begin = 0, BeginL = 0;
int   saveflag = 0;
unsigned char nowx = 0, nowy = 0;
struct _DY  *DY ;
struct _ZY  *ZY ;
struct _FJ  *FJ ;

char  msg = 1;
ST_MSG  *Home=NULL, *End=NULL;
char  filename[30] = "NONAME";
char  setfile[40] ;
char  Head[50] ="";
char  Man[7] ;
char  bkcolor = LIGHTGRAY;
char  kind = 0, order = 0, help=0, hzkf=0;
int   see = 0, watch = -2;
int   Beginx[3] ;
long  T0;
char  deskcolor = BLUE, papercolor = LIGHTGRAY;

void interrupt (*olddiskerror)();
void interrupt diskerror();

main(int argc, char **argv)
 { int graphdriver = VGA, graphmode=VGAHI, Ch, k;
   char altflag=0, bkaltflag=0, scrollflag=0, bkscrollflag=0, ctrlflag=0, bkctrlflag=0;
   registerbgidriver(EGAVGA_driver);
   registerbgifont(small_font);
   registerbgifont(triplex_font);
   initgraph(&graphdriver, &graphmode, "C:\\TC");
   cleardevice();   set_home();
   strcpy(setfile, argv[0]);
   for(k=strlen(setfile)-1; k>=0; k--)  if(setfile[k]=='\\')  break;
   setfile[k+1] = '\0';
   build();
   get_js();   get_sz();
   if(argc==2)
     { strcpy(filename, argv[1]);  format_name(filename);  get_msg(1); }
   else  get_zd();
   if(argc==1)  screen();
   write_screen();   show_msg();   change_name(0);   initmouse();
   T0 = clock();
BEGIN
   mouse_move(0);   show_time(0);   mouse_on();
   if(bioskey(2)&0x08)  altflag = 1;
   else                 altflag = 0;
   if(altflag != bkaltflag && kind==0)
     { line_button(kind); bkaltflag = altflag;
       barxy(nowx, nowy);
     }

   if(bioskey(2)&0x10)  scrollflag = 1;
   else                 scrollflag = 0;
   if(scrollflag != bkscrollflag && kind==0)
     { if(scrollflag==1)  clear_bar();
       else               barxy(nowx, nowy);
       bkscrollflag = scrollflag;
     }
   if(bioskey(2)&0x04)  ctrlflag = 1;
   else                 ctrlflag = 0;
   if(ctrlflag != bkctrlflag && kind==0)
     { if(ctrlflag==1)  clear_bar();
       else             barxy(nowx, nowy);
       bkctrlflag = ctrlflag;
     }
   if(getmousebutton()==NOBUTTON && !kbhit())  continue;
    Ch = mouse_click();
   if(getmousebutton()==LEFTBUTTON)
	{ leftbutton_click(Ch);  mouse_move(1); }
   else if(getmousebutton()==RIGHTBUTTON)
	{ rightbutton_click(Ch); mouse_move(1); }
   while(getmousebutton()!=NOBUTTON) show_time(0);
   if(kbhit())
     { char Ch;      show_date(1);
       key = Ch = getch();
       if(key==13||key==27||key==32)  key = 0;
       if(bioskey(2)&0x08)  key = 0;
       if(keyboard_click(Ch)==1)  key = 0;
       if(key&&((Ch>='0'&&Ch<='9')||Ch=='.'||Ch=='-')&&!(bioskey(2)&0x14))
	 { if(nowx==0)
	      { leftbutton_click(200+nowy);
		if(key)  keyboard_click(key);
		key = 0;
	      }
	   else if(nowx!=1)
	      { leftbutton_click(1000+nowy*100+nowx-2);
		if(key) keyboard_click(key);  key = 0;
	      }
	 }
       else if(key&&((Ch<0||Ch==BACKSPACE_KEY)&&nowx==1))
	    leftbutton_click(400+nowy);
       if(key)  keyboard_click(key);
       key = 0;
     }
WEND
 }

void  Exit(int code)
 { int i;  int bshowenter = showenter;
   showenter = 1;
   if(ask_save()==2)  { showenter = bshowenter;  return; }
   mouse_off();   cleardevice();   fcloseall();   closegraph();
   free_all(0);
   for(i=0; i<5; i++)   free(PTR[i].str);
   free(DY);  free(ZY);  free(FJ);
   if(autosave==1 && getdisk()!=0)  set_sz();
   disable();
   setvect(0x24, olddiskerror);
   enable();
   exit(code);
 }

void  build()
 { void  far*  address;  char *find_name(char *, char *);
   char  str[80];
   olddiskerror = getvect(0x24);
   disable();
   setvect(0x24, diskerror);
   enable();
   setcolor(deskcolor);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   settextstyle(1, 0, 3);
   address = getvect(0x33);
   if(address==NULL || *(unsigned char *)address==0xcf)
     { outtextxy(320, 200, "Mouse driver not installed, please install it!");
       outtextxy(320, 230, "(Run \"mouse.com\")");
       outtextxy(320, 260, "Press any key to continue . . .");
       getch();
       Exit(1);
     }
   hzk24fp = fopen(find_name(str, "st-tab.hzk"), "rb");
   if(hzk24fp==NULL)
     { outtextxy(320, 200, "Can't open file \"ST-TAB.HZK\" !");
       outtextxy(320, 280, "Press any key to continue . . .");
       getch();  Exit(1);
     }
   if((hzk16fp=fopen(find_name(str, "main.dat"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"c:\\ucdos\\hzk16"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"d:\\ucdos\\hzk16"),"rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"e:\\ucdos\\hzk16"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"a:\\ucdos\\hzk16"), "rb"))==NULL&&
      (hzk16fp=fopen(find_name(str, "mainf.dat"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"c:\\ucdos\\hzk16f"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"d:\\ucdos\\hzk16f"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"e:\\ucdos\\hzk16f"), "rb"))==NULL&&
      (hzk16fp=fopen(strcpy(str,"a:\\ucdos\\hzk16f"), "rb"))==NULL)
      { outtextxy(320, 200, "Can't open HZK file \"MAIN.DAT\" or \"HZK16\"!");
	outtextxy(320, 280, "Press any key to continue . . .");
	getch();  Exit(1);
      }
   setbuf(hzk16fp, NULL);
   if(str[strlen(str)-1]=='f')  hzkf = 1;
 }

char *find_name(char *str, char *S)
 { strcpy(str, setfile);
   strcat(str, S);
   return str;
 }

void  write_screen()
 { int k;
   char S[][8] = { "文件名", "主 题", "负责人", "新 建", "打 开", "保 存",
     "另 存", "存文本", "录 入", "排 序","浏 览","查 询","工 具","设 置",
     "帮 助", "退 出" };
   mouse_off();
   setfillstyle(1, deskcolor);
   bar(0, 0, 639, 450);
   setfillstyle(1, LIGHTGRAY);
   bar(0, 460, 639, 479);
   setcolor(LIGHTGRAY);
   line(0, 455, 639, 455);  line(0, 453, 639, 453);
   sethz_color(BLACK);
   show_hzs(100, 478, "注意：在ＵＣＤＯＳ环境下才能录入汉字（请先阅读帮助信息）");
   setcolor(LIGHTCYAN);
   setlinestyle(0, 0, 3);
   line(0, 22, 639, 22);
   sethz_color(RED);
   show_hzs(369, 17, "济南大学学生综合测评评分系统");
   sethz_color(YELLOW);
   show_hzs(370, 18, "济南大学学生综合测评评分系统");
   sethz_color(LIGHTGREEN);
   show_hzs(10, 18, Head);
   setcolor(LIGHTGREEN);
   rectangle(X1, Y1, X2, Y2);
   show_paper(X1+10, Y1+10, X2-10, Y2-9);

   if(kind<2)
    { sethz_color(LIGHTGREEN);
      show_hzs(160, 44, "德 育");
      show_hzs(340, 44, "智 育");
      show_hzs(520, 44, "附加分");
    }
   setcolor(LIGHTGREEN);
   sethz_color(RED);
   show_hzs(2, 50, "文件名");
   setlinestyle(1, 0, 3);
   line(2, 55, 90, 55);
   sethz_color(LIGHTRED);
   show_hzs(2, 80, S[1]);
   show_hzs(2, 100, S[2]);
   sethz_color(LIGHTGREEN);
   show_hzs(50, 100, Man);
   line(2, 105, 90, 105);
   sethz_color(GREEN);
   for(k=0; k<5; k++)
     show_hzs(22, 130+k*20, S[3+k]);
   line(2, 217, 90, 217);
   sethz_color(LIGHTCYAN);
   for(k=0; k<4; k++)
     show_hzs(22, 242+k*20, S[8+k]);
   line(2, 310, 90, 310);
   sethz_color(YELLOW);
   for(k=0; k<4; k++)
     show_hzs(22, 336+k*20, S[12+k]);
   settextstyle(1, 0, 3);
   settextjustify(CENTER_TEXT, CENTER_TEXT);
   outtextxy(50, 420, "Skyever");
   setcolor(RED);
   outtextxy(52, 422, "Skyever");
   settextstyle(0, 0, 1);
   setcolor(LIGHTGREEN);
   outtextxy(67, 445, "Ver:3.51");
   line_button(kind);
   mouse_on();
 }

void  show_paper(int x1, int y1, int x2, int y2)
 { setfillstyle(1, deskcolor);  bar(X1+3, Y1+3, X2-3, Y2-3);
   setfillstyle(1, DARKGRAY);   bar(x1-3, y1+5, x2-3, y2+5);
   setfillstyle(1, papercolor); bar(x1, y1, x2, y2);
 }

void  help_msg()
 { help_new();
   if(show_str("欢迎使用济南大学学生综合测评评分系统，该系统通过巧妙的设计"
   "完美的构思，使您的分数统计工作简单化、趣味化，该系统有直观的界面，"
   "其中大多数功能都可以从界面上了解到。该系统需要鼠标的支持，当您需要输"
   "入汉字时，您要启动ＵＣＤＯＳ，否则也能查看和输入数字，但无法输入汉字。"
   "该系统提供了录入、排序、浏览、查询四大模块，其中“录入”为输入原始的"
   "数据，您可以在表格中的最上方一行上点鼠标右键，从中可以弹出许多功能，"
   "其含义一目了然，也可以在姓名栏上点右键来选取功能，其中包括表格行列的"
   "插入、删除、移动等，若在表格的最末插入行列，直接点击该行列即可。"
   "在“录入”状态下，您可以用箭头键来移动活动单元格按回车键可以击活活动单"
   "元格，以便输入。另外还提供了"
   "平均值输入方式，在多数求平均值时应用十分方便。在“排序”状态下，可以在底"
   "部选择升序还是降序，点击表格最上方一行可以以该行为标准进行排序。“浏览”"
   "功能提供了查看某个项目的多人成绩的方法，您可以先以某列排序后再进行浏览。"
   "“查询”功能提供了查看一个人的多项成绩的方法，以方便您对某位同学的了解。"
   "该软件运用了高精度的运算，一般不会出偏差，希望学校各位老师和同学们能够喜"
   "欢。在使用的过程当中不要忘记使用鼠标右键弹出菜单，这样可以获取多项功能。"
   "该软件还有着许多有趣有用的功能，等待着您的发现！编程者徐友春热诚期待着与"
   "您的共同交流！" , 0))
   if(show_str("一、原始数据的录入：当左边的手形标志停留在“录入”上时，即为"
   "录入状态，此时，在主界面上显示的是一张表格。鼠标移到上面的其中的格上时，"
   "若此格高亮显示，则表明此格有待于激活，激活的方法是按鼠标左键或按ＥＮＴＥ"
   "Ｒ键您也可以选择用箭头键来选择有待于激活的表格。注意：有待于激活的格只能"
   "是有数据的格或是紧贴有数据的单元格。也就是说，您只能在表格中连续输入数据"
   "，而不能这里一个那里一个地输入。在“录入”状态下，表格的上方显示三个选择"
   "按钮：“德育”、“智育”、“附加分”您需要先选定这些大标题，再添加好字段"
   "以后才能输入数据。（添加一列即为添加字段。）其中表格的最上方一行为字段的"
   "名称，您可以在该行上按鼠标左键来修改它的名称或属性，列可以移动、删除、修"
   "改等，但“姓名”和“学号”不能移动、删除、修改。若要在列的左方添加一列，"
   "在该列名称上点击右键，选择添加一列，此时提示输入名称，若想在最右方添加一"
   "列，直接点击该列即可。当在字段（最上方一行）上点击鼠标右键时，您还会发现"
   "弹出一个“浏览该列”的选项，选中它后将自动切换为“浏览”状态，此时将会出"
   "现以该列为主题的“浏览”内容。行与列的操作有着相似之处，当在“姓名”上点"
   "击右键时，也会弹出相", 0))
   if(show_str("似的菜单。其中“查询该行”即为自动切换为“查询”状态，出现以"
   "该行为主题的“查询”内容。在数据区内点右键也将会弹出菜单，其中“清空该行"
   "”即为将该行除姓名和学号的全部内容赋值为零，“清空该列”和“清空该单元格"
   "”类似。另外，按ＡＬＴ＋上箭头可以上翻一行，按ＡＬＴ＋下箭头可以下翻一行"
   "，按ＰＡＧＥＵＰ可以上翻一页，按ＰＡＧＥＤＯＷＮ可以下翻一页，按ＨＯＭＥ"
   "移到最首，按ＥＮＤ移到最尾。如果您不想动用键盘，拿鼠标点击最下方的相应按"
   "钮即可。新版本的该软件提供了更加简便的方法：只要您按住ＣＴＲＬ键（或将ＳＣ"
   "ＲＯＬＬＬＯＣＫ键打开，在排序状态下不需按下任何键），再在页面内拖动，即"
   "可出现一个虚框，拖移虚框"
   "可以模拟其页面的移动，应用此方法，您将更多地提高工作效率。另外，该软件的"
   "升级版本又提供了滚动条的功能，您只要拖住滚动块移动即可，还有，按ＡＬＴ＋"
   "左箭头为左移一列，按ＡＬＴ＋右箭头为右移一列，按ＡＬＴ＋ＨＯＭＥ可以移至"
   "首列，按ＡＬＴ＋ＥＮＤ为移至最末一列。还有光条的移动：按ＳＨＩＦＴ＋上下"
   "箭头分别为移至当前屏幕的最上、最下、最左、最右，按ＣＴＲＬ＋ＩＮＳＥＲＴ"
   "键也可添加一列。", 1))
   if(show_str("二、平均值方式录入法：当您拿着一大堆数据，为要求输入它的平均值"
   "而发愁的时候，这会儿您该消气了，您在这以前该会是拿着计算器一个一个地加起"
   "来，再输入吧！这样，如果您输错一个数据，该数便一去不回，您只好再重新来输"
   "入。这还不说，如果您要先去掉几个最大值和几个最小值再输入，这时您可来了麻"
   "烦了，您只好一个一个地比较半天再计算了。现在好了，所有的工作您都可以交给"
   "计算机来完成，您要做的只是把数据一个一个地输入即可。并且如果您输错了，您"
   "还可以选中它以后再删除，大大地提高了工作速度和准确性。方法是：首先在要输"
   "入数据的单元格上点右键，此时弹出菜单，选中“以平均值方式输入”，此时显示"
   "一个对话框，其中左边为所输入数据的记录区，右边显示总分、平均分、个数和输"
   "入数据区（黑条）。首先在黑条上单击，或直接按回车键，激活黑条，输入数据后"
   "再按ＥＮＴＥＲ键（或单击，若想取消输入可按ＥＳＣ键），此时所输入的数据便"
   "记录在左键的的记录区中，若想继续输入可再按ＥＮＴＥＲ键（或单击，也可直接"
   "输入）。所输入的数据从下往上依次记录在记录区，记录区中的数据自动上滚，若"
   "想删除哪个数据可单击该数据，此时该数据显示在黑条上，单击“删除”按钮即可"
   "将其删除，若想要删", 0))
   if(show_str("除的数据已经滚至上方的不可见区域，可用上下箭头键来滚回。您可以修改所"
   "舍弃的最大数的最小数的个数，单击主菜单中的“设置”下的相应选项即可选中该"
   "功能。在用平均值方式输入时千万要注意所舍弃的个数，在刚出现“平均值输入”"
   "对话框时有提示。若所输入的还不够舍弃的，平均值将为零。当输入完毕后，点“"
   "确定”即可将平均值输入到相应的数据表格中。另外，记录区的翻页有许多技巧，"
   "按上下箭头键可以上下翻动一行，按ＰＡＧＥＵＰ、ＰＡＧＥＤＯＷＮ可以翻一屏"
   "，按ＨＯＭＥ、ＥＮＤ可以翻至最首和最尾。如果您试一下平均值输入方式，您会"
   "发现其中的许多奥妙！", 1))
   if(show_str("三、数据的排序：当数据输入完毕以后，您可以用此功能进行排序，该项"
   "功能已经达到可以随心所欲地排序的地步。首先切换到该模式下（单击主菜单中的"
   "“排序”）。但在该模式下，您不能输入数据。您可以选中某列，以该列为基准进行"
   "排序，单击该列的标题（最上方一行）即可，您还可以选择是升序排列还是降序排列"
   "。单击最下方的相应项即可，在最下方一行上有提示当前排序的信息。即所见的“排"
   "序依据：”，在“升序”和“降序”菜单上也有相应的手形标志来指示。当排好顺序"
   "后，以后的显示，翻页都将以该顺序为准。", 0))
   if(show_str("四、浏览和查询：若您可以查看该班级中的某项的成绩，可以使用“浏览”"
   "功能，此时，该项的数据将以列简表的形式显示出来，您可以先切换到该模式下，（"
   "点击主菜单中的“浏览”。）再选择上方的“按学分浏览”或“按科目浏览”即可"
   "其中“按科目浏览”将要求您输入某一科目的名称，该名称必须是该数据库中的一个"
   "字段，否则将提示找不到。您还可以在“录入”状态下，在字段（最上方一行）上点"
   "右键，选择“浏览该列”。“查询”功能和“浏览”类似，“查询”是以某人为单位"
   "显示所有的相关数据，让您可以清楚地了解到某位同学的信息。其操作和“浏览”相"
   "似，您也可以在“录入”状态下，在“姓名”上点右键，选择“查询该行”来进行查"
   "询，此时也将自动切换到查询方式。在查询方式下，所有的数据，以及总分和学分。"
   "都将以列表的形式列出来，该表采用动态的变化效果，例如当附加分一栏中没有任何"
   "数据时，它将不显示。还有，该表格有自动显示和隐藏滚动条的功能，您只要看到表"
   "格一栏的两侧（或一侧）有箭头时，表明该栏未入下所有的数据，移动鼠标至该栏内"
   "，将会显示滚动条，拖移滑块可以查看其它数据。当鼠标移开时，滑块自动消失。", 0))
   if(show_str("五、计算公式的编写：计算公式为本软件该版本新添的一项内容，它为"
   "实现通用性迈进了一大步，不然的话，若要每年计算学分的公式不同，那只有每年新"
   "编一个软件，或者再费尽心思重新修改，使得每年的学分只有用不同的软件才能查到"
   "。计算公式的编写很简单：只要您按照通常运算的规则，用简练的表达式表示出来即"
   "可，所用的原始数据可用字母ｘ来表示，该科的规定学分可用字母ｆ来表示，另外还"
   "有一些数据可以从自带的表中查到，如ＳＤ表示德育成绩的总分，ＭＢ表示必修课的"
   "规定分的总分等等。几乎所有的数据都可以由这些规定数据计算得来，如要用到德育"
   "平均分，可以这样计算：（ＳＤ／ＮＤ）要注意语法的正确性，否则不能计算出正确"
   "结果，在计算时会给出提示。您可以无限制地添加多个条件性执行计算公式，如ｘ值"
   "在某个范围内，用这样的公式来计算，在另一个范围内用另外一个公式来计算等，点"
   "击界面上的“新添”和“删除”按钮可以实现相应的操作。如您可以这样来编写计算"
   "必修课学分的计算公式，ｘ＊ｆ／（ＭＢ＋ＭＸ）＊１．２＊７０％，在条件栏上保"
   "留空白。大小写字母没有严格的区分。要特别注意：所写的公式要注意括号的"
   "添加，因为软件只是机械地按照公式的顺序进行运算，并不考虑优先级问题。再"
   "如：您可以这样来编写任选课的计算公式：在条件栏上添上区间，如０到５９分，再"
   "在计算公式栏内填写０．３；再点击“新添”按钮，则上面的三个框变为空白，再填"
   "上６０到７９分，再在计算公式栏内填", 0))
   if(show_str("写０．４。依次类推，所填写的计算公式不仅"
   "可以是数，而且可以是具体的计算公式，如（ｘ＋０．５）＊ｆ等。这样就可以根据"
   "具体的区间来计算学分了。您可以用“新添”按钮来添加一项，用“删除”按钮来“"
   "删除”当前项，用“上一项”或“下一项”来浏览各个区间，当您按住ＣＴＲＬ再点"
   "“新添”按钮时将在当前项前面添加一项，否则将在后面添加一项。当按住ＣＴＲＬ"
   "再点击“上一项”和“下一项”时，将会每次跳跃五个区间。如果您不需要区"
   "间，只要将区间栏留空即可。按左箭头可以激活左限输入框，按右箭头可以激活右限"
   "输入框，按上下箭头可以激活计算公式输入框，按ＴＡＢ可以在三个输入框之间切换"
   "。计算公式将和数据文件一并保存，也就是说，今年的成绩"
   "在明年照样可以打开来看，不会因为明年计算公式的改变影响了其正确性。不过特别"
   "需要提醒的是：您所看到的计算结果并不是在文件中存在的，而是每次查看都需要再"
   "计算一次，因此特别要注意当前计算公式的正确性。还有一点，在书写计算公式时，"
   "大小写字母并无严格的区分，即ＳＤ和ｓｄ视为两个相同的变量，Ｘ和ｘ也等价，括"
   "号可以嵌套使用，但不要使用其它类型的括号，否则将引起混乱。", 1))
   if(show_str("六、系统设定：新版本的本软件提供了自动保存文件的功能，当您打开"
   "自动保存功能，您就可以不必担心忘存盘而丢失了数据，如果您好忘事，还是最好打"
   "开该功能，您可以设定为多少分钟保存一次数据，输入零表示要关闭该功能，还要注"
   "意：当到了保存时间时，如果当前文件还未被改变，则不执行该次的保存。如果您习"
   "惯于应用繁体字体，可以选择“设置”菜单下的“应用繁体字体”，不过要求您的磁"
   "盘上存在繁体字库ＨＺＫ１６Ｆ，或在软件目录下存在文件ＭＡＩＮＦ．ＨＺＫ。否"
   "则该功能无效，若想再转回简体字体，可以再次打开设置菜单，您会发现原来的“应"
   "用繁体字体”菜单变为“应用简体字体”，单击它即可。“自动保存设置”为在关闭"
   "应用程序时自动将当前的设置存盘，包括屏幕颜色、声音开关、平均值选项、保留文"
   "件名等。自动保留文件名即为最近几次打开或编辑的文件，您可以在“设置”菜单中"
   "设定要保留的个数，若以后要用到这些文件，可以单击左上角的“文件名”即可弹出"
   "菜单。“关闭／打开声音”为关闭或打开在弹出对话框时出现的声音。“全部清除”"
   "为清除全部的数据，包括字段在内。“恢复磁盘数据”为恢复至该文件最近一次保存"
   "的数据。“设定系统时间”为设定当前的系统时间和日期，记住：按住ＣＴＲＬ再单"
   "击箭头可以每次改变１０个单位，否则每次改变一个单位。", 0))
   show_str("七、其它：“主题”为该数据库的标志，让其它人打开该数据库便可清楚地"
   "了解到该数据库的作用。主题显示在最上方的标题栏上，若想更改，可以选择主菜单"
   "中的“主题”一项，在建立数据库时特别建议输入主题。“负责人”即为对该数据库"
   "负责的人，选择主菜单中的“负责人”一项即可更改，该选项也可以为空。主菜单上"
   "还有“新建”、“打开”、“保存”、“另存”、“存文本”的选项，其中“存文本"
   "”即为把数据按“学号－姓名－学分”的形式存成文件，以便打印之用，其它的功能"
   "一目了然。还有，在“设置”当中有“设为默认字段”和“获取默认字段”的选项，"
   "它将把当前所用的字段保存为默认字段，以便下将新建文件时应用。另外，以“ＳＴ"
   "＿ＴＡＢ”方式启动软件时，将出现起始的广告画面，之后新建一个命名为“ＮＯＮＡ"
   "ＭＥ”的文件。若在命令行之后带一个文件名，将省略起始的广告画面，之后，若该"
   "该存在，则打开它，否则，以该文件名新建一个文件。文件名将显示在左上方，您也"
   "可以以完整路径或相对路径来输入文件名，若显示的文件名前有一个反斜线，表明该"
   "文件不在当前目录下。还有，若要退出该帮助信息，可以按鼠标右键，或者按ＥＳＣ键"
   "。编程者徐友春再次感谢学校老师和同学们的信任和支持。并祝愿中国的软件事业蓬"
   "勃健康地发展！－－二零零一年九月六日。", 0);
 }

void  key_msg()
 {  help_new();
    show_str("１、ＣＴＲＬ＋Ｏ／Ｆ９：打开文件；ＣＴＲＬ＋Ｎ：新建文件；Ｆ２："
    "保存；Ｆ１２：另存；Ｆ１：帮助；ＴＡＢ／ＣＴＲＬ＋ＴＡＢ：在四个模式之间切"
    "换；空格键：在“录入”和“排序”模式之间切换。２、在“录入”模式下：ＣＴＲ"
    "Ｌ＋ＩＮＳＥＲＴ：插入一列；ＨＯＭＥ：移至"
    "首行；ＥＮＤ：移至末行；ＰＡＧＥＵＰ：上翻一页；ＰＡＧＥＤＯＷＮ：下翻一页"
    "ＡＬＴ＋上箭头：上翻一格；ＡＬＴ＋下箭头：下翻一格；ＡＬＴ＋ＨＯＭＥ：移到"
    "首列；ＡＬＴ＋ＥＮＤ：移至末列；ＡＬＴ＋ＰＡＧＥＵＰ：左移一屏；ＡＬＴ＋Ｐ"
    "ＡＧＥＤＯＷＮ：右移一屏；ＡＬＴ＋左箭头：左翻一列；ＡＬＴ＋右箭头：右翻一"
    "列；上下左右箭头：移动活动单元格。ＳＨＩＦＴ＋上下左右箭头键：移活动单元格"
    "至当前屏最上、最下、最左、最右；ＥＮＴＥＲ：激活单元格（在输入数据时可不"
    "必激活单元格，直接输入即可，输完后直接按箭头键也可起确定作用。）３、在排序"
    "模式下，翻页功能同“录入”模式。上下左右箭头键起翻格的作用。不能插入，修改"
    "删除数据。单击字段行，以该行按升序或降序排列。４、在“浏览”和“查询”状态"
    "下无特殊的快捷键，但要注意应用鼠标的右键弹出菜单中的功能。５、Ｆ５：编写计"
    "算公式；Ｆ７：在“浏览”状态下为按学号浏览，在“查询”状态下为“按学分查询"
    "”；Ｆ８：在“浏览”状态下为按姓名浏览，在“查询”状态下为按科目查询；６、"
    "ＣＴＲＬ＋Ｆ９：页面打印；７、ＣＴＲＬ＋Ｑ：退出程序。", 0);
  }

int  show_str(char *str, int code)
 { char S[61];  int num = code==0?56:60, begin=code==0?2:0, flag=LEFTBUTTON;
   int k=0, K=0, L=0, high = code==0?strlen(str)/60*16:Y2-Y1-80;
   mouse_off(); clear_bar(); show_paper(X1+10, Y1+10, X2-10, Y2-9);
   while(1)
    { S[k++] = str[K++];
      if(k==num || str[K-1]==0)
	{ S[num] = 0;
	  show_hzs(X1+25+begin*16, Y1+(Y2-Y1-high)/2+10+16*L++, S);
	  begin=0; num=60;
	  if(str[K-1] == 0)  break;
	  k = 0;
	}
    }
   mouse_on();
   while(!kbhit()&&(flag=getmousebutton())==NOBUTTON)
     { mouse_move(0);  show_time(0); }
   while(getmousebutton()!=NOBUTTON) show_time(0);
   if(kbhit()) { while(kbhit()) if(getch()==27)  return 0; }
   else { if(flag!=LEFTBUTTON)  return 0;
	  else if(mouse_click()==0)  return 0;
	}
   return 1;
 }
void moth_msg()
 {  help_new();
    if(show_str("为定量地反映一名学生的综合素质状况，建立对学生教育与管理的激"
    "励机制，培育“四有人才”，参照《高等学校学生行为准则》，结合我校具体情况"
    "特制定学生综合测评实施办法。（学生工作方面的规章制度于二○○一年五月十"
    "八日由校长办公会批准，于２００１～２００２学年第一学期在全校施行）",0))
    if(show_str("一、德育测评：（一）德育测评的内容：１、政治品质：政治品质的"
    "内容主要包括对党的基本路线的态度，在是非面前的态度和表现，以及开展批评和"
    "自我批评参加政治活动，社会现任责任感，工作责任心等方面的情况。２、道德品"
    "质：道德品质主要是为人处事的态度及种种表现。包括是否遵守社会公德，爱护公"
    "共和他人财物，不损人利已，为人诚实守信，团结帮助他人，讲文明礼貌等方面的"
    "情况。３、集体观念：集体观念主要指对待集体与个人关系的态度、表现，集体荣"
    "誉感等。包括能否正确自理集体利益与个人利益的关系，是否具有较强的集体荣誉"
    "感，是否积极参加集体组织的各项活动和劳动，能否自觉主动地维护宿舍、教室等"
    "公共场所良好的卫生环境等方面。４、学习态度：学习态度主要指对学业的态度和"
    "表现，勤奋刻苦物程度等。包括是否立志成才，注意自己多方面能力的培养学业是"
    "否勤奋刻苦、扎实用功。能否按时独立地完成作业。全年有无迟到、早退、旷课、"
    "缺交作业等不良现象。５、法纪观念：法纪观念主要是指遵纪守法的情况，包括是"
    "否自觉地遵守宪法、法律和校规校纪，参与和组织违法违纪行为，是否有违反校规"
    "校纪受过纪律处分。",0))
    if(show_str("（二）德育测评的方法：学生德育五项满分为１００分，每项满分为"
    "２０分，各项分Ａ（１８－２０）、Ｂ（１６－１７）、Ｃ（１４－１５）、Ｄ（"
    "１３分以下）。每班由班主任和全体同学组成测评组，测评组和每个成员按德育测"
    "评的五项内容，根据每个同学的现实表现同时独立地给全班每位同学打分，汇总时"
    "去掉五个最高分和五个最低分。所得平均分即为德育中某项得分，将德育五项得分"
    "累计，即为德育五项分。学生德育分Ｐ＝Ｍ×３０％  Ｍ代表德育五项分之和。",0))
    if(show_str("二、智育测评：（一）智育测评的内容：包括所有考试、考查、限选"
    "、任选课程。（二）智育测评方法：Ｓ＝【∑（各门必修课成绩×学分×１．２＋"
    "∑（各门限选课成绩×学分）】÷∑各门课学分＋Ｂ。说明：（１）各门课成绩均"
    "按每一次考试成绩计入；（２）公共外语课第一学年、第二学年上学期以正考成绩"
    "计；第二学年下学期外语成绩等于四级统考成绩加２０分。（３）Ｂ（任选课）＝"
    "Ｂ１＋Ｂ２＋Ｂ３（只限三门任选课）１、２、３、为任选课排列数。凡任选课成"
    "绩不及格者不加分；成绩在６０～７９分之间，Ｂ＝０．３；８０～８９分之间，"
    "Ｂ＝０．４；９０分以上，Ｂ＝０．５。（４）任选课以１６．５个学时为一个学"
    "分。",0))
    if(show_str("三、综合测评成绩计算方法。Ｓ＝德育×３０％＋智育×７０％＋附"
    "加（减）分。四、关于附加（减）分的内容和标准：（一）德育和表现附加（减）"
    "分的范围和标准。总的原则：优加劣减、合理适度。１、优秀学生干部的加分额为"
    "１．５分（班级按学生人数的７％评定；校（院、系）学生会干部按学生会干部人"
    "数的２０％评定）。各院（系）评选的优秀学生干部由院（系）学生工作领导小组"
    "审核，报校学生工作处审定。２、优秀团员的加分额１．０分。３、有其他突出事"
    "迹而受到学校表彰者，视情况附加０．３～１．０分；受到省级表彰者（省优干、"
    "省三好、省级优秀团员等分别加３分、２．５分、２分；省级青春立功、省级社会"
    "实践调查报告按一、二、三等分别加３分２．５分、２分；受到国家级表彰者加５"
    "分。４、超额完成学校规定的公益劳动，每超一次附加０．１分。〖说明：德育附"
    "加，不采取重复相加的办法，如一个同学在几项上者应加分，则采取最高的得分，"
    "然后每多一项，附加０．１分（低于０．２分加原分）〗５、受院系以及通报批评"
    "者，每次减０．５分。６、受行政处分警告者每次减１．０分；严重警告者每次减"
    "１．５分；记过者每次减２．０分；留校察看者每次减３．０分；",0))
    if(show_str("７、有其他不良"
    "行为并造成恶劣影响者，视情况减０．５～２．０分。８、学年内完不成学校规定"
    "的公益劳动定额者，每缺一次减０．１分。（说明：每一年内德育附减分可重复累"
    "计附减）（二）智育的附加分范围和标准：１、发表科研论文（以篇计）：国家级"
    "刊物，附加３～４分（由科研处审定，以下同）；省级刊物，加２～３分；市、校"
    "级刊物，加１～２分；２、发表文学作品、新闻报道按国家级、省市级、校级杂志"
    "、报刊分别加１分、０．５分、０．２分。（每学年累加不超过１分）３、发明创"
    "造获国家专利者，每项加１～２分（科研处审定）；有科研成果并对教学、科研、"
    "生产有积极作用者，视情况附加１～３分。４、学术性竞赛（参赛未获奖者不计）"
    "全国性，加２～５分；（根据获奖等级、各次审定具体加分额）省级，加１～３分"
    "（同上）市、校级，加０．１分；（同上）校级大赛附加分参赛人数的１０％计取"
    "，１～３名，加０．５分；４～６名，加０．３分；７～１０名，加０．２分。获"
    "大赛团体第一名的成员，每人附加０．１分；团体第二名附加０．０８分；团体第"
    "三名附加０．０５分。院系级学习竞赛获奖者的加分额掌握在０．１～０．５分之"
    "间（由院系核审）。",0))
    if(show_str("（三）文体方面的加分内容及标准。１、全国大学生体育竞赛获奖者"
    "，视情况加２～５分；凡参赛者加１分。２、省市大学生运动会上，每得一分加０"
    "．２分；集体基础上上每得一分加０．１分。３、校运动会每得一分附加０．１分"
    "集体基础上每得一分加０．０５分。４、破校记录者每项加０．８分；破省市大学"
    "生运动会记录者，每中１．５分。说明：以上竞赛得分，一学年内同项目参赛多次"
    "，只取最高分附加，××同学校运动会百米第一名，应加０．１×９＝０．９分，"
    "同时又破校记录，应加０．８分，在省大学生运动会上百米赛每一名，应０．２×"
    "９＝１．８分，这种民政部只加最高分１．８分。一学年内有不同参赛项目，可以"
    "重复相加。５、非田径类体育比赛获奖者附加分：（１）个人项目：省市第一名加"
    "２．０分，第二名加１．５分，第三名加１．０分，第四名到第六名加０．５分，"
    "其余名次加０．１～０．３分（前１０名以内）；校级第一名加０．５分，第二名"
    "加０．３分，第三名加０．２分，第四到第六名加０．１分。（２）团体项目：省"
    "市第一名每人加１．５分，第二至第三名加１．０分，第四到第六名加０．５分。"
    "校级第一名每人加０．３分，第二名加０．２分，第三名加０．１分。说明：非田"
    "径类比赛同一项目参赛多次，只取一次最高分，不同项目参赛多次可重复相加，以"
    "上均指正式比赛。",0))
    show_str("６、院系级文体类竞赛加额掌握在０．１～０．５之间，审定权在院"
    "系学生工作办公室。７、参加省市级大学生文化艺术类竞赛并获奖者，一等奖加１"
    "．５分，二等奖加０．１分。只限于全校性大型文化艺术类活动。９、凡积极参加"
    "社会实践、社会调查，其社会实践调查报告，一等奖附加０．６～１．０分，按参"
    "加活动学生数的３％计取；二等奖附加０．３～０．５分，按参加活动学生数的１"
    "５％计取；三等奖附加０．１～０．３分，具体加分额由学工部审定。此项附加分"
    "属德育附加分范畴，不重复相加。五、综合测证每年９月份进行，毕业班级６月份"
    "进行。六、本办法学校授权学生工作处负责解释和修改。",0);
    help = 0;
 }

void file_msg()
 {  mouse_off();
    sethz_color(LIGHTGREEN);
    clear_bar();
    setfillstyle(1, deskcolor);
    bar(100, 424, 639, 449); bar(120, 27, 619, 46);  show_time(1);
    show_hzs(260, 44, "济南大学学生综合测评实施办法");
    sethz_color(LIGHTCYAN);   show_hzs(270, 445, "退出帮助系统");
    mouse_on();
    sethz_color(BLACK);
    show_str("应用本系统需要具备以下设置：一、鼠标及鼠标驱动程序，若在ＷＩＮ"
    "ＤＯＷＳ下运行，驱动程序已经启动，若在ＤＯＳ下运行，需首先运行ＭＯＵＳＥ"
    "．ＣＯＭ。二、汉字操作系统ＵＣＤＯＳ，若不需要输入汉字，则不必运行ＵＣＤ"
    "ＯＳ，在不启动ＵＣＤＯＳ的情况下也可以显示汉字，也可以输入数字及字符。三"
    "、在本系统所在目录下存在文件ＳＴ－ＴＡＢ．ＨＺＫ、ＪＳ．ＳＥＴ、ＴＡＢ．"
    "ＳＥＴ、ＳＺ．ＳＥＴ分别用于存放２４点阵式汉字库、默认计算公式、默认字段"
    "和内部设置。四、在系统"
    "所在目录下存在ＭＡＩＮ．ＨＺＫ或在硬盘的ＵＣＤＯＳ目录下存在ＨＺＫ１６或"
    "ＨＺＫ１６Ｆ。您可以将ＵＣＤＯＳ装入硬盘的根目录下，然后将本系统的有关文"
    "件装入硬盘根目录下的ＳＴ－ＴＡＢ目录中。若存在ＵＣＤＯＳ，则不必再在系统"
    "所在目录下装入ＭＡＩＮ．ＤＡＴ文件，以节省硬盘空间。直接运行安装盘根目录"
    "下的安装程序ＩＮＳＴＡＬＬ．ＢＡＴ（＋盘符）即可安装。", 0);
    help = 0;
 }

void  set_zd()
 { FILE  *fp; int k; char str[60];
   fp = fopen(find_name(str, "tab.set"), "wt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "不能打开字段文件！", 1) ;
       return;
     }
   fprintf(fp, "%d\n", DYnum);
   for(k=0; k<DYnum; k++)
     fprintf(fp, "%s\n", DY[k].Name);
   fprintf(fp, "%d\n", ZYnum);
   for(k=0; k<ZYnum; k++)
     fprintf(fp, "%s %f %d\n", ZY[k].Name, ZY[k].Xf, ZY[k].Attrib);
   fprintf(fp, "%d\n", FJnum);
   for(k=0; k<FJnum; k++)
     fprintf(fp, "%s\n", FJ[k].Name);
   fclose(fp);
 }

void  set_sz()
 { FILE  *fp;  char str[60];  int k;
   fp = fopen(find_name(str, "sz.set"), "wt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "不能打开内部设置文件！", 1) ;
       return;
     }
   fprintf(fp, "%d,%d\n", B, E);
   fprintf(fp, "%d,%d\n", T, SOUND);
   fprintf(fp, "%d\n", autosave);
   fprintf(fp, "%d\n", maxfilenum);
   fprintf(fp, "%d\n", namenum);
   for(k=0; k<namenum; k++)  fprintf(fp, "%s\n", bname[k]);
   fprintf(fp, "%d,%d,%d\n", colorfa, deskcolor, papercolor);
   fprintf(fp, "%d\n", showenter);
   fclose(fp);
 }

void  get_zd()
 { FILE  *fp; int k;  char str[60];
   fp = fopen(find_name(str,"tab.set"), "rt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "不能打开字段文件！", 1);
       return;
     }
   fscanf(fp, "%d\n", &DYnum);
   DY = (struct _DY*)malloc(DYnum*sizeof(struct _DY));
   for(k=0; k<DYnum; k++)
     fscanf(fp, "%s\n", DY[k].Name);
   fscanf(fp, "%d\n", &ZYnum);
   ZY = (struct _ZY*)malloc(ZYnum*sizeof(struct _ZY));
   for(k=0; k<ZYnum; k++)
     fscanf(fp, "%s %f %d\n", ZY[k].Name, &ZY[k].Xf, &ZY[k].Attrib);
   fscanf(fp, "%d\n", &FJnum);
   FJ = (struct _FJ*)malloc(FJnum*sizeof(struct _FJ));
   for(k=0; k<FJnum; k++)
     fscanf(fp, "%s\n", FJ[k].Name);
   fclose(fp);
 }

void  get_sz()
 { FILE  *fp;  char str[60];  int k;
   fp = fopen(find_name(str, "sz.set"), "rt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "不能打开内部设置文件！", 1) ;
       return;
     }
   fscanf(fp, "%d,%d\n", &B, &E);
   fscanf(fp, "%d,%d\n", &T, &SOUND);
   fscanf(fp, "%d\n", &autosave);
   fscanf(fp, "%d\n", &maxfilenum);
   fscanf(fp, "%d\n", &namenum);
   for(k=0; k<namenum; k++)  fscanf(fp, "%s\n", bname[k]);
   fscanf(fp, "%d,%d,%d\n", &colorfa, &deskcolor, &papercolor);
   fscanf(fp, "%d\n", &showenter);
   fclose(fp);
 }

void  set_js()
 { FILE *fp; char str[80];
   fp = fopen(find_name(str, "js.set"), "wt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "设置默认计算公式失败！", 1);  return; }
   fprintf(fp, "%d,%d,%d,%d,%d\n",
	    PTR[0].num, PTR[1].num, PTR[2].num, PTR[3].num, PTR[4].num );
   fprintf(fp, "%s\n%s\n%s\n%s\n%s\n",
	    PTR[0].str, PTR[1].str, PTR[2].str, PTR[3].str, PTR[4].str );
   fclose(fp);
 }
void  get_js()
 { FILE *fp; char str[80];
   fp = fopen(find_name(str, "js.set"), "rt");
   if(fp==NULL)
     { msg_box(320, 240, "错误：", "获取默认计算公式失败！", 1);  return; }
   fscanf(fp, "%d,%d,%d,%d,%d\n",
	&PTR[0].num, &PTR[1].num, &PTR[2].num, &PTR[3].num, &PTR[4].num);
   PTR[0].str = (char*)malloc(PTR[0].num);
   PTR[1].str = (char*)malloc(PTR[1].num);
   PTR[2].str = (char*)malloc(PTR[2].num);
   PTR[3].str = (char*)malloc(PTR[3].num);
   PTR[4].str = (char*)malloc(PTR[4].num);
   fscanf(fp, "%s\n%s\n%s\n%s\n%s\n",
       PTR[0].str, PTR[1].str, PTR[2].str, PTR[3].str, PTR[4].str);
   fclose(fp);
 }
void  sys_demo()
 { int flag = 1;
   help_new();
   while(flag==1)
  { flag = 0;
   if(show_demo("您曾经为繁忙的学分统计而大伤脑筋-您曾经为繁琐的学分统计而费"
   "尽心思-您曾经为计算多个数的平均值而按红了手指-您曾经为检验结果的正确性而忙得废寝忘食"
   "-您也曾领略过ＥＸＣＥＬ的风采-不过您还是期望有一个针对性强的软件来处理问"
   "题--现在好了……--一个针对性极强的“济南大学学生综合测评评分系统”出现在您"
   "的面前，您所做的所有的工作，仅是准确无误地将所有原始数据输入，然后，您就"
   "可以随心所欲地查看任何数据。它在学分统计这一方面比ＥＸＣＥＬ更灵活、更方"
   "便。它运行于ＤＯＳ操作系统之上，避免了系统的庞大而带来的运行不稳定，并且"
   "采用模拟ＷＩＮＤＯＷＳ界面，支持鼠标操作，更增加了其灵活性。还可以让您自"
   "己来编写计算公式，避免了因每年计算公式的变化而不得不重新编写计算软件。"))
   if(show_demo("本软件采用Ｃ语言编程，因为编程环境并非运行于ＷＩＮＤＯＷＳ之"
   "上，所以编程工作相当复杂，深藏了相当复杂的逻辑关系，在该升级版本中解决了"
   "更多的实际问题，希望学校各位老师和同学们能够重视和喜欢，并在以后的学分统"
   "计中多加应用，多多提出宝贵的建议和意见，深表感谢！--－－谨以此软件献给济"
   "南大学所有热爱生活的老师和同学们！--－－感谢学校各位老师和同学们的大力支"
   "持！--－－祝愿中国软件事业蓬勃健康地发展！"))
   if(show_demo("在ＤＯＳ下的编程，其编程环境都未提供可视化，所以在编程过程中"
   "的所有工作只能靠代码的输入来完成，尤其在于界面的设计上，其编程过程全是代码"
   "的输入和数据的计算。通常程序员在编写一个程序时，仅用于界面设计的精力便占了"
   "８０％以上，这和用ＶＢ相比，简直是天渊之别。不仅如此，ＴＣ还未提供鼠标函"
   "数库，所以所有的鼠标函数，只能靠中断调用来自己编写。还有汉字的显示、按钮"
   "、输入框、滚动条、信息框、输入框、菜单、表格，包括鼠标移入按钮时亮条的显示，移"
   "开时亮条的消失，程序运行过程中的每一个动作，都必须自己来设计，Ｃ语言中没"
   "有现成的函数可供调用，所以编程的难度大大增加。不过它运行于ＤＯＳ之上，并且"
   "系统较小，所以系统运行"
   "比较稳定，通常不会发生死机现象和不合理的现象，经常多日的试用，基本上"
   "解决了该程序的众多缺陷，达到可以应用的标准。-希望学校各位老师和同学们能够"
   "有兴趣与编者交流，取长补短，在计算机编程这一学科上携手共进，共同领略这一"
   "学科的许多奥妙！"))
   if(show_demo("随着该软件的不断升级，许多功能得到了大大的增强，在速度方面远"
   "远超过了以前，提供了更多个性化的支持：如声音开关、自动保存功能、界面颜色方"
   "案、简体字和繁体体的选择、文件名的自动保留、对计算公式的错误检查、快捷键功"
   "能的增强、帮助文件的完善等。在这个版本中，基本上满足了济南大学学分统计的要"
   "求。--希望这个版本的学分统计系统能够减轻老师和同学们在学分统计方面的负担，"
   "并衷心济南大学的明天更加辉煌灿烂！--作者：徐友春-二零零一年十月一日于济南"))
     flag = 1;
  }
 }
void  help_new()
 { mouse_off();
   sethz_color(LIGHTGREEN);
   clear_bar();
   setfillstyle(1, deskcolor);
   bar(100, 424, 639, 449); bar(120, 27, 619, 46);  show_time(1);
   show_hzs(208, 44, "济南大学学生综合测评评分系统使用说明");
   sethz_color(LIGHTCYAN);   show_hzs(270, 445, "退出帮助系统");
   sethz_color(BLACK);
   mouse_on();
 }

int  show_demo(char *str)
 { int Nx=2, Ny=0, K=0, flag;
   char CH[3];  long T;
   mouse_off(); clear_bar(); show_paper(X1+10, Y1+10, X2-10, Y2-9);  mouse_on();
   while(str[K]!=0)
    { if(str[K]>0)
       { if(Nx) Ny++; Nx=2; K++;  T = clock();
	 while(clock()-T<10L)
	   { mouse_move(0);
	     if(kbhit()||(flag=getmousebutton())!=NOBUTTON)  goto out;
	   }
       }
      else
       { CH[0] = str[K]; CH[1] = str[K+1]; CH[2] = '\0';
	 show_hz(X1+25+Nx*16, Y1+30+Ny*20, CH);
	 Nx++;
	 T = clock();
	 while(clock()-T<2L)
	   { mouse_move(0);
	     if(kbhit()||(flag=getmousebutton())!=NOBUTTON)     goto out;
	   }
	 if(Nx==30)  { Nx = 0; Ny++; }
	 K += 2;
       }
    }
   mouse_on();
   T = clock();
   while(clock()-T<70L)
     { mouse_move(0);  show_time(0); }
   out:while(getmousebutton()!=NOBUTTON) show_time(0);
   if(kbhit()) { while(kbhit()) if(getch()==27)  return 0; }
   else { if(flag==RIGHTBUTTON)  return 0;
	  else if(mouse_click()==0)  return 0;
	}
   return 1;
 }

void interrupt diskerror()
 { /* Do Nothing */
 }