ST_MSG *add_point(ST_MSG far *);
void  xshow_hzs(int,int,char*), show_line();
ST_MSG  *find_point(int n);
void not_bar(int, int, int, int), free_all(int);
int _test_in(int, int, int, int, int);
int mouse_move(int), mouse_click(), _mouse_move();
int save(int), get_msg(int);
void  show_linemsgy(int), add_name();

#define test_in(x,y,z,a)     _test_in(x, y, z, a, 0)
#define test_inbox(x,y,z,a)  if(_test_in(x,y,z,a,0)==1)  return 1
#define  clear_bar()  _test_in(0,0,0,0,1)

void  renew_name(), renew_name(int);
void  renew_xms(), set_home(), renew_xhs(), renew_data(), show_linemsg();
void  move_text(int);
int   avr_input(int, int, float *, int, int);
#define  MAX     (msg==0?7:msg==1?11:5)
#define  MAXnum  (msg==0?DYnum:msg==1?ZYnum:FJnum)