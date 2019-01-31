extern  FILE  *hzk16fp, *hzk24fp;
#define  X1   80
#define  Y1   115
#define  X2   560
#define  Y2   354

typedef struct
 {  double    X, Y ;
    int       Xn, Yn;
    double    Rad ;
    double    Speed ;
    unsigned  Life   :4;
    unsigned  Color  :4;
    unsigned  Speedflag :2;
    unsigned  Doub   :5;
    double    Bkspeed;
    double    Bkrad;
 }  BALL;

typedef struct
 {  int  Xn, Yn;
 }  LASTBALL;

extern struct  play
 {  unsigned  player    : 2;
    unsigned  color     : 5;
    unsigned  leaves    : 4;
 } player[2];

extern struct _FA
 { double  Speed;
   double  Rad;
 } FA;

extern struct _BALLIN
 { unsigned  WHITE  : 1;
   unsigned  BLACK  : 1;
   unsigned  RED    : 4;
   unsigned  BLUE   : 4;
   unsigned  REDFLAG  : 2;
   unsigned  BLUEFLAG   : 2;
 } ballin;

#define  INLIFE      1
#define  OUTLIFE     0
#define  R           8

extern   BALL  ball[16];
extern   LASTBALL lastball[16];
extern   char  far  *ptr;
extern   unsigned  B[16];
extern   double  a;
extern   int    Len;
extern   double  _Rad, _Rad0, _Speed, _Speed0;
extern   char pass, meet;
extern   char Ro, Rr, RR, PLAY;
extern   char far *SET;
extern   long  Time;
extern   char ifsound, ifwhile;
extern   int  Redn, Bluen;
