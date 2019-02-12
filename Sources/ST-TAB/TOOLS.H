int   far get_hzs(int x, int y, char *S, int num);
char  shines(int x, int y);
int   show_nums(int x, int y, double N, int, int);
int   msg_box(int x, int y, char *, char *, int);
int   input_num(int x, int y, float *, int, int);
int   input_box(int x, int y, char *, char *, int);
int   log_box(int x, int y, struct _ZY *);
int   show_menu(int n, ...);
void  line_button(int), show_ptr(), show_ptr1(int), show_ptr2(int);
int   input_boxnum(int x, int y, char *, float *, int n, int point);
void  show_watch(int), change_name(int);
int   Inword(int x, int y, char *S, int Num);
int   input_boxstr(int x, int y, char *, char *, int);
void  show_time(int), input_js(), show_inline(int,int,char*, char*, char*);
int   scroll(int,int,int,int,int,int);
void  show_scroll(int,int,int,int,int,int), show_sound(), show_date(int);
int   scroll_move(), chnum(char *);
int   inline_str(int, char *, char *, char *, char *, int *, int *);
void  renew_sdata(struct _MSG *), renew_ptrdata(), set_time();
double js_xf(char *S, float data, float xf);
double get_DATA(char *S, float data, float xf);
double get_data(char *S, float data, float xf);
void  scroll_see(int x1, int y1, int x2, int y2, int code, int max, int n);
void  format_name(char *S), print_scr(int,int,int,int);
int   select_str(int n, int size, char *head, char *str, int *begin);
int   select_name(char *), select_stu(char *);

#define  clear_str(str)  str[0]='\0'
#define  clear_scroll()  scroll_see(0,0,0,0,-1,0,0);
