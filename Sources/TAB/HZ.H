#define  LEFT_HZ      0
#define  RIGHT_HZ     1
#define  MIDDLE_HZ    2
#define  TOP_HZ       0
#define  BOTTOM_HZ    1
#define  X_ROUND      0
#define  Y_ROUND      1
#define  COPY_MODEL   0
#define  XOR_MODEL    1

#define  sethz_round(x)    HZ.text_round = x
#define  sethz_text(x,y)   ( HZ.text_x = x , HZ.text_y = y )
#define  sethz_mode(x)     HZ.text_mode = x
#define  sethz_color(x)    HZ.text_color = x
#define  sethz_zoomxy(x,y) ( HZ.text_zoomx = x , HZ.text_zoomy = y )
#define  sethz_file(x)     hz_fp = x
#define  sethz_dxy(x)      HZ.text_dxy = x
#define  sethz_dot(x)      HZ.text_dot = x
#define  sethz_fx(x)       HZ.text_hz_round = x
#define  sethz_speed(x)    hz_speed = x


struct  _HZ
 { unsigned  text_round    : 1;
   unsigned  text_x        : 2;
   unsigned  text_y        : 2;
   unsigned  text_mode     : 1;
   unsigned  text_color    : 4;
   unsigned  text_zoomx    : 3;
   unsigned  text_zoomy    : 3;
   unsigned  text_dxy      : 8;
   unsigned  text_dot      : 7;
   unsigned  text_hz_round : 1;
 };

extern  struct _HZ   HZ;
extern  FILE   *hz_fp;
extern  int    hz_speed;

void  writehz(int x, int y, int begin);
void  writehzs(int x, int y, int begin, int end);
void  x_writehzs(int x, int y, ...);
int   gethz_wide(int begin, int end);
int   gethz_high(int begin, int end);