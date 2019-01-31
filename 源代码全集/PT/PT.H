#define   BEGIN  while(1) {
#define   WEND    }

int  X, Y, X0, Y0;
int    r = 100;
unsigned  int   txmsg[20];
int   txnum = 0, txnum0=37, txnumflag=0;
int   timeflag = 0;
char  S[10];
long  CLOCK0, CLOCK, BKC;
long  Test = 0;
long  *T;

#define   START   27
#define   Wide    120
int       High =  100 ;
int       ishowscreen = 1;
int       ireadonly   = 1;


FILE  *hzk24fp, *hzk16fp, *txkfp;
char  far *ptr;

struct _MSG MSG[4] =
 {
   { 180, 160, {0}, {0}, 0, 3, 0, 0, 0 , {2, 1, 1}, 0.5},
   { 420, 180, {0}, {0}, 0, 5, 0, 1, 0 , {6, 1, 3, 1, 1},
	    1.914213562  /* M_SQRT2+0.5 */  },
   { 170, 325, {0}, {5}, 0, 4, 0, 2, 0 , {3, 2, 2, 1},
	    0.914213562  /* M_SQRT2-0.5 */  },
   { 420, 340, {0}, {0}, 0, 4, 0, 3, 0 , {3, 1, 2, 2},
	    2.328427125  /* 2*M_SQRT2-0.5 */   }
 };
struct  _TXMSG  TXMSG;
int  door, passed = 0;
unsigned  size;
int     active = 0, MENU = 0;
double  middlex, middley;
double  middlex0, middley0;

int     inrect(int, int, int, int, int, int);
double  line_far(double *_X, double *_Y, double *getfarx, double *getfary);
double  find_nearpoint(int, int, double *, double *);
double  line_far(double *_X, double *_Y, double *getfarx, double *getfary);
double  find_nearline(int , int , double *, double *);
int     pointnum(double*, double*),  gettx(int n);
void    showtx(int x, int y), Exit(int, int);
void    resettx(), show_ok();
int     itrue(),  show_desk(int doors);
int     show_menu(),  rightbutton_click(),  inputbox();
void    save(), show_str(int code, char*), test_image(), clear_str(int code);
void    tx_click(int n), init_tx(int, char**), test_rad(), show_help();
void    test_point(), test_line(), round_tx(double _rad), select_tx();
void    move_tx(),  yround_tx(int), true_deal(), allselect_tx();
void    gettxpoints(int, int), gettxmiddlexy(int*,int*,int,int);
int     show_select();

unsigned char key=0;
void  far *address;

FILE  *hzk24fp, *hzk16fp, *txfp, *savefp;

int   X, Y, KEY=0;
