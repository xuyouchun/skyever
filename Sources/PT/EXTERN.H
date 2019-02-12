#define  writeflag(n)  (txmsg[(n-1)/16] |= ((unsigned)0x01<<((n-1)%16)))
#define  readflag(n)   (txmsg[(n-1)/16] & ((unsigned)0x01<<((n-1)%16)))

struct  _MSG
 { double  XX, YY;
   double  X[5], Y[5];
   double  Rad;
   int     Points;
   int     Round;
   int     Layer;
   int     Active;
   int     Rct[5];
   double  S;
 } ;


struct _TXMSG
 { int     N;
   int     Begin;
   int     End;
   int     Points;
   double  Dxy;
   double  R[4];
   double  Rad[4];
   double  Txrad[4];
   int     Round[4];
   double  Farxy[16];
 };

extern int r;
extern struct _MSG MSG[4];
extern int    active;
extern double  middlex, middley;
extern double  middlex0, middley0;
extern FILE  *hzk24fp, *hzk16fp, *txfp;
extern struct  _TXMSG   TXMSG;
extern int  door;
extern unsigned int  txmsg[20];
extern int  txnum;
extern char *ptr;
extern int  timeflag;
extern char S[10];
extern long CLOCK, CLOCK0, BKC;
extern long Test;
extern long *T;
extern int  passed, txnum0;
extern int  MENU;
extern int  KEY;
