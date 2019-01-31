#ifndef  MOUSE_H_FILE
#define  MOUSE_H_FILE

/**********************************************************************************************/
/*************宏定义部分********************/
#define  OFF         0
#define  ON          1
#define  FALSE       0
#define  TRUE        1

#define  LEFTBUTTON  1
#define  RIGHTBUTTON 2
#define  NOBUTTON    0

#define  NOCLICK     0
#define  LEFTCLICK   1
#define  RIGHTCLICK  2
#define  DOUBLECLICK 3

#define  STATICPTR   0	 			/*指明当前鼠标类型为静态图片*/
#define  MOVIEPTR    1 				/*指明当前鼠标类型为动画*/

#define DEFAULT_STATICPTR  (&default_staticptr)	/*预定义静态鼠标类型--指针*/
#define HAND_STATICPTR	   (&hand_staticptr)	/*预定义静态鼠标类型--手形*/
#define HELP_STATICPTR     (&help_staticptr)	/*预定义静态鼠标类型--问号*/
#define MOVE_STATICPTR     (&move_staticptr)	/*预定义静态鼠标类型--移动*/
#define TEXT_STATICPTR     (&text_staticptr)	/*预定义静态鼠标类型--文本选择*/
#define CROSS_STATICPTR    (&cross_staticptr)	/*预定义静态鼠标类型--十字形*/
#define PEN_STATICPTR      (&pen_staticptr)	/*预定义静态鼠标类型--铅笔*/
#define NOUSE_STATICPTR    (&nouse_staticptr)	/*预定义静态鼠标类型--无效*/
#define CUR_STATICPTR      (getmouse_staticptr())/*当前静态鼠标类型*/

#define DEFAULT_MOVIEPTR   (&default_movieptr)	/*预定义动态鼠标类型--亮边指针*/
#define TRY_MOVIEPTR       (&try_movieptr)	/*预定义动态鼠标类型--碰撞指针*/
#define BREAK_MOVIEPTR     (&break_movieptr)	/*预定义动态鼠标类型--爆炸*/
#define FIRE_MOVIEPTR      (&fire_movieptr)	/*预定义动态鼠标类型--火焰*/
#define WATER_MOVIEPTR     (&water_movieptr)	/*预定义动态鼠标类型--喷泉*/
#define CUR_MOVIEPTR	   (getmouse_movieptr())/*当前动态鼠标类型*/
/*********************************************************************************************/


/*************鼠标指针定义部分************************/
typedef  struct				/*定义静态指针*/
 { int focus_x;
   int focus_y;
   unsigned char static_ptr[4][16];
 } MOUSE_STATICPTR;

struct _PTR
 { MOUSE_STATICPTR *ptr;/*当前帧*/
   struct _PTR  *next;	/*下一帧*/
 };
typedef  struct
 { struct _PTR *home;	/*第一帧*/
   struct _PTR *ptr;	/*当前帧*/
   int num;		/*帧总数*/
   int xch, mxch;	/*记录当前帧序号及帧速度*/
 } MOUSE_MOVIEPTR;			/*定义动态鼠标*/


/******************************************************************************************/
/***********鼠标形状设置部分**************/
int  readmouse_staticptr(unsigned char (*ptr)[16], char *filename);   /*从文件中读取鼠标指针图片*/
void setmouse_staticptr(MOUSE_STATICPTR *ptr);	 	/*设置一个鼠标指针形状*/
void start_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr);/*开始制作动态鼠标*/
void add_makemovieptr(MOUSE_STATICPTR *mouse_staticptr);/*添加动态鼠标图形*/
void end_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr);	/*结束动态鼠标制作(并未做任何事情)*/
void free_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr); /*释放动态鼠标*/
void setmouse_movieptr(MOUSE_MOVIEPTR *mouse_movieptr);	/*设置一个动态光标*/

/**********鼠标控制部分*******************/
int  start_mouse();				/*开始鼠标控制*/
void mouse_off(), mouse_on();     		/*鼠标的关闭和打开*/
void setmouse_xy(int x, int y);  		/*设置鼠标位置*/
void setmouse_xy(int x, int y);			/*设置鼠标的位置*/
void setmouse_max_x(int left, int right);	/*设置鼠标横向可移动区域*/
void setmouse_max_y(int top, int bottom);	/*设置鼠标纵向可移动区域*/
void exit_mouse();				/*退出鼠标控制*/
#define setmouse_movespeed(movieptr,x) movieptr.mxch=x; /*设置一个动态光标的变化频率*/
void setmouse_model(int model);			/*设置鼠标的指针模式(动态或静态)*/
void doubleclick_on();				/*打开检测双击模式(默认)*/
void doubleclick_off();				/*关闭检测双击模式*/
void setdbclick_time(long newtime);		/*设置双击速度*/
void setfocus_xy(int x, int y);			/*设置当前鼠标的横向和纵向焦点*/
void movieptr_mirrorx(MOUSE_MOVIEPTR *ptr);	/*横向翻转静态鼠标*/
void movieptr_mirrory(MOUSE_MOVIEPTR *ptr);	/*纵向翻转静态鼠标*/
void staticptr_mirrory(MOUSE_STATICPTR *ptr);		/*横向翻转动态鼠标*/
void staticptr_mirrorx(MOUSE_STATICPTR *ptr);		/*纵向翻转动态鼠标*/
void curptr_mirrorx();				/*横向翻转当前鼠标*/
void curptr_mirrory();				/*纵和翻转当前鼠标*/

/**********鼠标检测部分*******************/
void getmouse_xy(int *x, int *y);   		/*获取当前鼠标位置*/
int  getmouse_x();				/*获取当前鼠标横坐标*/
int  getmouse_y();				/*获取当前鼠标纵坐标*/
int  getmouse_button();				/*访问鼠标的按键状态*/
int  imouse_inbox(int x1, int y1, int x2, int y2);	/*判断当前鼠标焦点是否在指定区域内*/
int  imouse();					/*查询当前鼠标是否打开*/
#define getmouse_movespeed(movieptr) (movieptr.mxch)    /*查询一个动态光标的变化频率*/
int  getmouse_model();				/*查询鼠标的指针模式(动态或静态)*/
MOUSE_STATICPTR  *getmouse_staticptr();		/*查询静态鼠标的形状*/
MOUSE_MOVIEPTR  *getmouse_movieptr();		/*查询动态鼠标的形状*/
int  idoubleclick();				/*检测双击模式的打开与关闭*/
int  iclick();					/*检测单击或双击事件*/
#define mousehit() _click			/*检测是否有单击或双击事件(不清空)*/
long getdbclick_time();				/*检测双击速度(默认为4)*/
#define   getclick_x()	_nowX_			/*查询单击或双击事件时的横坐标*/
#define   getclick_y()  _nowY_			/*查询单击或双击事件时的纵坐标*/
int   getfocus_x();				/*获取当前鼠标的横向焦点*/
int   getfocus_y();				/*获取当前鼠标的纵向焦点*/


/*************全局变量部分***************/
/********************************************************************************************/
extern MOUSE_STATICPTR default_staticptr, hand_staticptr, help_staticptr, move_staticptr, 
		text_staticptr, cross_staticptr, pen_staticptr, nouse_staticptr;
extern MOUSE_MOVIEPTR default_movieptr, tryer_movieptr, break_movieptr,
                        fire_movieptr, water_movieptr;
extern int _nowX_, _nowY_;
extern int _click;
/********************************************************************************************/

#endif