#define  TRUE          1
#define  FALSE         0
#define  YES         TRUE
#define  NO          FALSE
#define  ON           YES
#define  OFF          NO
#define  LEFTBUTTON    0
#define  RIGHTBUTTON   1
#define  BOTHBUTTON    2
#define  NOBUTTON      3

extern  int  _mouse_x, _mouse_y;
extern  char _mouse_op;

#define  getmousebutton()  getmouse_xy(&_mouse_x, &_mouse_y)
#define  mouse_op()  _mouse_op

int   initmouse();
int   getmouse_xy(int *x, int *y);
int   getmouse_x();
int   getmouse_y();
void  mouse_on();
void  mouse_off();
void  setmouse_max_x(int left, int right);
void  setmouse_max_y(int top,  int bottom);
void  setmousexy(int x, int y);
int   imouse_inbox(int x1, int y1, int x2, int y2);