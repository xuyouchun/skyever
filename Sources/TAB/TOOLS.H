void  resetball();
void  move_ball();
void  showall();
void  far  _outtextxy(int x, int y, char *format, ...);
int   ball_click(int n1, int n2);
void  get_points();
void  get_ball(unsigned *B);
void  show_ball();
int   ball_border(int n);
void  ball_in(int k);
void  start_move();
int   test_speed(), test_ballin(), test_life(int) ;
int   test_line(double,double,double,double,double,double);
double  _get_rad(double,double,double,double);
int   get_sr(int n, int n0, int code);
void  get_psn(int), reset_speedflag();
int   get_sin(int n, int code);
int   get_roads(int color, double endspeed);
int   leave_speed(), reset_black(int color);
void  write_line(double, double, double, double);
void  reset_inball(int code), reset_ballin();
char  *_mktime(long time);
void  show_str(int,int,int,char*);
void  show_in(int), line_msg(char *);
void  Sound(int code), speed_msg(double speed, double rad);
int   test_L(int n, int code);

#define  NOTEND   1
#define  HAVEEND  0

