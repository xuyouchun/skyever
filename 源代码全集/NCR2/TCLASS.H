#define  DEF  -1
const char Tx = 0x01;
const char Fx = 0x02;
const char Ax = 0x04;
const char Wx = 0x08;
const char Hx = 0x10;
const char Cx = 0x20;
const char Sx = 0x40;
const char Lx = 0x80;


#define  NOSEL  0x7f
#define  CURSET -1
#define  DEFSET -1
#define print(x, y, str)  _pn.xshowstr(x, y, str)
#define qprint(x, y, str, color)  _pn.qshowstr(x, y, str, color, begin, end)

typedef enum
  { false, true,
    up=0, down, curset, def, cls, click, all,
    no=0, yes, off=0, on
  }  BOOL;

const char  COPY_MODEL=0, XOR_MODEL=1;

const int NOSIGHT = 1;
const int QUICK   = 2;

//显示汉字

class  T_ShowHz       //定义显示汉字的类
 { public:
     T_ShowHz(char *filename, int _dot=16, int _fx=0) //初始化构造函数
      { fp = fopen(filename, "rb");
	if(fp==NULL)  openflag = 0;
	else
	 { openflag = 1; dot = _dot; size = dot*(dot>>3);
	   zoomx = 1; zoomy = 1; fx = _fx; _speed = 0;
	 }
      }
     T_ShowHz(FILE *_fp, int _dot=16, int _fx=0)
      { fp = _fp; dot = _dot; fx = _fx;
	openflag = 1;  size = dot*(dot<<3);  zoomx = zoomy = 1;
      }
     T_ShowHz() { /* do nothing; */ }
     int isuccess() { return openflag; }  //检测是否打开成功
     int getdot() { return dot; }  //访问点阵数
     void showhz(int x, int y, long order, int color=-1);  //在指定位置显示汉字
     void xshowhz(int x, int y, long order, int color=-1, char model=COPY_MODEL);  //增强的在指定位置显示汉字
     void zoom(int zoomx=CURSET, int zoomy=CURSET);  //设置横向和纵向缩放比例
     void speed(int __speed) { _speed = __speed; }  //设置显示速度
     FILE *fp;      //汉字点阵字库
   private:
     int  openflag;  //打开成功标志 1-成功  2-失败
     int  dot, size;  //点阵数, 汉字字节数
     int  zoomx, zoomy, fx, _speed;  //横向和纵向缩放比例, 方向, 显示速度
 };

class  T_ShowAsc  //定义显示ASC字符的类
 { public:
     T_ShowAsc(char *filename);  //初始化构造函数
     T_ShowAsc(FILE *_fp)
      { fp = _fp; openflag = 1; zoomx = zoomy = 1; _speed = 0; }
     T_ShowAsc() { /* do nothing */ }
     int  isuccess() { return openflag; }  //检测是否打开字库成功
     void showasc(int x, int y, long order, int color=-1); //在指定位置显示ASC字符
     void xshowasc(int x, int y, long order, int color=-1, char model=COPY_MODEL);  //增强的在指定位置显示ASC字符
     void zoom(int zoomx, int zoomy);  //设置横向纵向的缩放比例
     void speed(int __speed) { _speed = __speed; }  //设置显示的速度
     void qshowasc(int x, int y, long order, int color);
     FILE *fp;    //汉字显示点阵字库
     unsigned char asck[16*(127-32)];
   private:
     int  openflag;  //打开字库成功标志
     int  zoomx, zoomy, _speed;  //横向缩放比例, 纵向缩放比例, 显示速度
 };

class T_ShowStr
 { public:
     T_ShowStr(char *hzstr="hzk16", char *ascstr="asc16");
     T_ShowStr(FILE *hzfp, FILE *ascfp);
     void showstr(int x, int y, char *str, int color=-1);
     void xshowstr(int x, int y, char *str, int color=-1, char model=COPY_MODEL);
     void hzfar(int _dxy) { dxy = _dxy; }
     void zoom(int _zoomx=CURSET, int _zoomy=CURSET)
      { if(_zoomx!=CURSET)  zoomx = _zoomx;
	if(_zoomy!=CURSET)  zoomy = _zoomy;
	Hz.zoom(_zoomx, _zoomy);  Asc.zoom(_zoomx, _zoomy); }
     void pn_xy(int x, int y=CURSET)
      { pn_x = x; if(y!=CURSET)  pn_y = y; }
     void qshowstr(int x, int y, char *str, int color, char begin=NOSEL, char end=NOSEL);
     T_ShowHz  Hz;
     T_ShowAsc Asc;
   private:
     int dxy;
     int pn_x, pn_y;
     int zoomx, zoomy;
 };
extern  T_ShowStr  _pn;

//时间
class T_Time
 { public:
      T_Time(int _x, int _y, char _backcolor=BLACK, char _forcolor = WHITE)
	{ x = _x;  y = _y;  backcolor = _backcolor;  forcolor = _forcolor;
	  gettime(&T0);  _T = T0;  runenable = true;
	}
      void run();
      void show(struct time *_T=NULL);
      void clear();
      BOOL runenable;
   private:
      struct time _T, T0;
      int x, y;
      char backcolor, forcolor;
 };


//按钮
class T_Button
 { friend class T_xM;
   public:
     T_Button(int x1, int y1, int x2, int y2, char *str="", char Kind=DEF, char _backcolor=GREEN, char _forcolor=RED);
     T_Button(int x, int y, char *str, char Kind=DEF, char _backcolor=GREEN, char _forcolor=RED);
     T_Button(){};
     void show(BOOL idown=curset);
     void attrib(char _attrib=DEF);
     void able(BOOL _W)  { W = (BOOL)(1-_W); show(curset); }
     void moveto(int x, int y);
     void hide(int color=DEF);
     BOOL testclick();
     BOOL idown() { return A; }
     BOOL inbox() { return (BOOL)imouse_inbox(x1, y1, x2, y2); }
     char *str;
     BOOL T, F, W, H, C, A;
   private:    
     int  x1, y1, x2, y2;
     char backcolor, forcolor;
     BOOL Aflag;
 };


//文本框
#define Lock 0x02
class T_Text
 { public:
     T_Text(int x, int y, int width=DEF, char *str="", char attrib=0, char backcolor=CYAN, char forcolor=LIGHTBLUE, BOOL ch=true);
     T_Text(int x, int y, char *str, int color=WHITE, BOOL ch=true)
       { T_Text(x, y, DEF, str, Lock, BLACK, color, ch); }
     T_Text(){};
     char *str(char *s=NULL);
     void clear(), show();
     BOOL testclick();
     BOOL edit();
     char ptr[50], bptr[50];
   private:
     int x, y, width;
     int  K, begin, Len;
     char backcolor, forcolor;
     BOOL L, W, T, ch;
     char shines();
     void showstr(char *ptr);
 };

//标签
class T_Label
 { public:
     T_Label(int x, int y, int width=DEF, char *str="", char attrib=LEFT_TEXT, char backcolor=BLACK, char forcolor=WHITE);
     T_Label(int x, int y, char *str, int color=WHITE, char attrib=LEFT_TEXT)
       { T_Label(x, y, DEF, str, attrib, BLACK, color); }
     char *str(char *s="");
     void clear(), show();
     char *ptr;
   private:
     int x, y, width;
     char backcolor, forcolor, attrib;
     void showstr(char *ptr);
 };

//单选/复选框
class T_Select
 { public:
     class T_Sel
       { public:
	   T_Sel(int x, int y, char *str="", char attrib=0, char backcolor=BLACK, char forcolor=LIGHTCYAN);
	   ~T_Sel()  { clear(); }
	   T_Sel *Tnext;
	   BOOL  select(BOOL AX=def);
	   BOOL  able(BOOL Y=def);
	   void  clear(), show(), showhead();
	   void  change(char *str="");
	   char  str[80];
	   int   x, y;
	   BOOL  A, W, F;
	 private:
	   char backcolor, forcolor;
       };
     T_Sel *Home;
     char  *str;
     T_Select(int x, int y, char _attrib=0, char backcolor=BLACK, char forcolor=LIGHTCYAN);
     void  add(char *str, char attrib=0, int x=DEF, int y=DEF);
     BOOL  del(char *str="");
     void  heigh(int _dy) { dy = _dy; }
     void  setpn(int _x, int _y)  { x = _x;  y = _y; }
     BOOL  moveptr(char *str="");
     BOOL  select(BOOL Ax=def, char *str="");
     BOOL  select(char *str) { return select(true, str); }
     BOOL  sel(char *str) { return select(def, str); }
     BOOL  able(BOOL Y=def, char *str="");
     BOOL  change(char *str, char *_str="");
     BOOL  testclick();
     void  reset(), showhead();
   private:
     int  x, y;
     int  dy, bx, by;
     BOOL F;
     int  pn, num;
     unsigned long SEL;
     char backcolor, forcolor;
     T_Sel *PTR;
 };


//滚动条
class  T_Scroll
 { public:
     T_Scroll(int x1, int y1, int x2, int y2, int max=10, int flag=0, char backcolor=CYAN, char forcolor=LIGHTBLUE, BOOL W=true);
     T_Scroll() { /* do nothing */ }
     void setflag(int flag, int max=CURSET, int Flag=0);
     void show();
     void clear();
     BOOL able(BOOL Y=def);
     BOOL testclick();
     int  max, flag;
     char backcolor, forcolor;
     int  inbox() { return imouse_inbox(x1, y1, x2, y2); }
     double sd;
   private:
     int   x1, y1, x2, y2;
     int   lenth, width;
     double d;
     BOOL  H, Y;
 };

//菜单
const char Mx = 0x01;

class  T_Menu;
class  T_La
 { public:
     T_La(char *str, char attrib=DEF, char group=0);
     ~T_La() { delete str; if(menuptr!=NULL)  delete menuptr; }
     void show(int x, int y, char color, int wide);
     char *str;
     T_Menu  *menuptr;
     T_La    *Tnext;
     BOOL W, A, S;
     BOOL barflag;
     void setbar(int x, int y, int wide);
     void clearbar(int x, int y, int wide);
     char group;
 };

class  T_Menu
 { public:
    T_Menu(char _backcolor=GREEN, char _forcolor=RED)
      { headptr = NULL;  screenptr = NULL;  showflag = false;
	backcolor = _backcolor;  forcolor = _forcolor;
      }
    T_Menu *add(char *str, char *pstr=">", char attrib=DEF, char group=0);
    T_Menu *add(char *str, char attrib, int group=0)
       { return add(str, ">", attrib, group); }
    T_Menu *add(char *str, int group) { return add(str, ">", 0, group); }
    BOOL del(char *str);
    void show(int x=DEF, int y=DEF);
    void hide();
    BOOL attrib(char *str, char _attrib=0);
    BOOL askattrib(char *str, char _attrib=0, BOOL *ret=NULL );
    BOOL select(char *str);
    BOOL inbox() { return (BOOL)imouse_inbox(x1, y1+4, x2, y2-4); }
    BOOL testclick()
     { if(showflag==false)  return false;
       return _testclick()==true?true:false;
     }
    char *str()  { return ptr; }
    void clearbar();
    BOOL ishow()  { return showflag; }
   private:
    T_La *headptr;
    char far *screenptr;
    char backcolor,  forcolor;
    T_La *findptr(char *str);
    int  getwide(), gethigh();
    int  x1, y1, x2, y2;
    BOOL showflag;
    static char ptr[50];
    BOOL _testclick();
 };

class T_xMenu
 { class T_xM
     { public:
	 T_xM(char *str, int x, int y, int backcolor=GREEN, int forcolor=RED);
	 ~T_xM();
	 BOOL inbox() { return button.inbox(); }
	 T_xM  *Tnext;
         T_Menu   menu;
	 T_Button button;
	 BOOL  testclick()
	  { if(button.testclick()) return true;
	    else  return false;
	  }
	 void hidemenu() { menu.hide(); }
	 void showmenu() { menu.show(button.x1, button.y2+2); }
       private:
	 void show() { button.show(); }
     };
   public:
     T_xMenu(char _backcolor=GREEN, char _forcolor=RED)
      { backcolor = _backcolor;   forcolor = _forcolor;
	Head = NULL;   showflag = no;
      }
     T_xM *Head;
     T_Menu *add(char *str, int x, int y);
     BOOL del(char *str);
     BOOL testclick();
     BOOL attrib(char *str, char _attrib=0);
     BOOL askattrib(char *str, char _attrib=Wx);
     BOOL select(char *str);
     void hide();
     char *str;
     BOOL ishow() { return showflag; }
     void renew();
   private:
     char backcolor, forcolor;
     BOOL showflag;
     T_Menu  *PTR;
 };



//列表框
#ifndef Ff
#define Ff 0x01
#endif
#define Wl 0x80

class T_List
 { public:
     friend class T_Li
      { public:
	 T_Li(char *str="", char attrib=0, char backcolor=BROWN, char forcolor=CYAN,
	       int x=0, int y=0, int width=0, int begin=0, int buttom=0, int top=0, char flag=0,
		T_List *p=NULL);
	 void clear();
	 void show();
	 char str[15];
	 BOOL A, W;
	 T_Li  *Tnext;
	 BOOL select(BOOL A=def);
	 BOOL able(BOOL W=def);
	 static int  x;
	 int    y;
	 void operator =(T_Li &T)  { strcpy(str, T.str);  A = T.A;  W = T.W; }
	private:
	 static int  width, begin, buttom, top;
	 static char backcolor, forcolor;
      };
     T_List(int x1, int y1, int x2, int y2, char attrib=0, char backcolor=CYAN, char forcolor=BROWN, char flag=0);
     T_List() { Home = NULL;  PTR = NULL; }
     ~T_List() { reset(); }
     void reset();
     T_Scroll scroll;
     void add(char *str="", char attrib=0, char flag=0);
     void show(), showall(int flag=0);
     BOOL select(BOOL A=def, char *str="");
     BOOL select(char *str) { return select(true, str); }
     BOOL del(char *str="", char flag=0);
     BOOL testclick();
     BOOL move(int k);
     BOOL movenext()  { return move(1);  }
     BOOL movefirst() { return move(-1); }
     void setpn(int H) { y2 = y1+H; max = (y2-y1)/17; }
     void order(BOOL or=down, BOOL sight=true);
     char *str;
     T_Li *Home, *PTR;
     int x1, y1, x2, y2;
     int No;
   private:
     BOOL F, L;
     char backcolor, forcolor;
     int  num, max, begin;
     int  Yy;
     BOOL moveptr(char *str="");
 };

class  T_Combo
 { public:
      T_Combo(int x, int y, int width=100, int high=100, char *str="", char backcolor=CYAN, char forcolor=BROWN);
      T_Combo() {}
      void show() { button.show();  text.show(); }
      void add(char *str="", char attrib=0);
      BOOL del(char *str) { return list.del(str); }
      BOOL testclick();
      BOOL move(int k);
      BOOL movenext()  { return move(1);  }
      BOOL movefirst() { return move(-1); }
      char *str;
      char *bstr() { return text.bptr; }
      BOOL ishow() { return (BOOL)showflag; }
      BOOL top()    { return list.No<=0?true:false; }
      BOOL bottom() { return list.No>=max-1?true:false; }
      void changetext(char *_str)  { text.str(_str); str = text.str(); findorder(); }
      void order(BOOL ord=down)  { list.order(ord, false); }
      T_Li *inull() { return list.Home; }
   private:
      T_List    list;
      T_Text    text;
      T_Button  button;
      int x1, y1, x2, y2;
      char  far  * ptr;
      char      showflag;
      int       maxhigh, max;
      void  findorder();
 };


enum SHOWKIND { ALL, LINE, HIDE };
//编辑框
class  T_EditBox;
class  T_LineEdit
 { public:
     T_LineEdit(char *_str="", char _backcolor=BLACK, char _forcolor=YELLOW,
	     int ishow=0, SHOWKIND kind=ALL, char _begin=NOSEL, char _end=NOSEL)
       { strcpy(str, _str);  showkind = kind;
	 backcolor = _backcolor;  forcolor = _forcolor;
	 begin = _begin;  end = _end;
	 if(ishow!=NOSIGHT)  show();
       }
     char  str[80];
     static int K;
     T_LineEdit *Tnext;
     void  changestr(char *_str, int ishow=0);
     void  show();
     static void  setpn(int _y=DEF)  { if(_y!=DEF) y = _y; }
     void  beginedit() { ptr = str; }
     int   edit();
     static void shines(BOOL kind=def);
     void  showattrib(SHOWKIND kind=ALL);
     SHOWKIND  showkind;
     static BOOL Sf;
     char  begin, end;
     void del(int k);
     void add(char ch, int k);
     void addstr(char *s, int k);
     int  findstr(char *s, int k=0, BOOL unupsee=false);
   private:
     static backcolor, forcolor;
     static y;
     static char *ptr;
     void moveimage(int k1, int k2, int k);
     static void clear();
 };


class  T_EditBox
 { public:
     T_EditBox(int y1=0, int y2=479, int backcolor=BLACK, int forcolor=YELLOW, BOOL lock=no);
     void show();
     void addline(char *str, BOOL pn=down, int k=DEF, int ishow=0, SHOWKIND kind=ALL, BOOL selflag=false);
     void addline(char *str, int ishow, SHOWKIND kind=ALL) { addline(str, down, DEF, ishow, kind, false); }
     void delline(int num);
     void setline(int num);
     void gotolinenum(int num);
     void beginedit() { PTR->beginedit(); }
     void edit(int KEY=-1);
     void page(BOOL p=down);
     void moveline(int k, BOOL kind=down, int num=DEF);
     void move(BOOL kind=up);
     void reset(int color=DEF);
     void free();
     void linebegin()  { T_LineEdit::K = 0; }      //光标移至行首
     void lineend()    { T_LineEdit::K = strlen(PTR->str); }//  光标移至行尾
     void pagebegin()  { gotolinenum(begin); }  //光标移至页首
     void pageend()    { gotolinenum((begin+high-1)>=max?(max-1):(begin+high-1)); }  //光标移至页尾
     void allbegin()   { gotolinenum(0); }     //光标移至篇首
     void allend()     { gotolinenum(max-1); } //光标移至篇末
     void definebegin();       //定义块首
     void defineend();         //定义块尾
     void addstr(char *ptr="ABCDEFG");   //添加字符串
     void delstr();            //删除选定内容
     char *selptr();           //选定字符指针
     BOOL ishinesinblock();    //判断光标是否在块内
     void resetline();         //刷新显示行
     void resetsel();          //块取消
     void selectall();         //全选
     void delcurline() { delline(now); }    //整列删除
     BOOL read(char *file, char *path=".", BOOL flag=false);
     BOOL read(char *file, BOOL flag) { return read(file, ".", flag); }
     BOOL save(char *file, char *path=".", BOOL flag=false);
     BOOL save(char *file, BOOL flag) { return save(file, ".", flag); }
     BOOL savetoimage(char *file, char *path=".");
     BOOL readinimage(char *file, char *path=".");
     BOOL lineable(BOOL able=curset) { if(able==curset) return la;  la = able; }
     int  shines_k;   //保存光标位置
     char *ptr_k;     //保存当前编辑指针
     static key;
     BOOL lock(BOOL l=def)  { if(l==def)  return L;  L = l; }
     static int y1, y2;
     void showattrib(SHOWKIND kind)
       { if(PTR==NULL)  return;  PTR->showattrib(kind); }
     static BOOL ichange;  //曾经被改变
     BOOL Isel;
     int maxline() { return max-1; }
     static long TIME;  //光标闪烁速度
     static BOOL Line;  //是否自动缩行
     BOOL  findstr(char *s, BOOL unupsee=false);  //寻找字符串
     void  replacestr(char *_str, int len);    //替换字符串
     int   backcolor, forcolor;  //前景色, 背景色
   private:
     void showsel();
     T_LineEdit *go(int num=0);   //指针移动到指定行
     int begin, now, max;
     int high;
     BOOL L, la;   //能否编辑, 能否加减行, 是否显示格式
     int  charnum();  //选中字符个数
     T_LineEdit  *Head, *PTR;
     T_Scroll  scroll;
 };


//图形初始化
class T_Tx
 { public:
    T_Tx(int backcolor=BLACK, int frontcolor=WHITE);
    ~T_Tx();
   private:
    int graphdriver, graphmode;
 };