extern  FILE  *hzk16fp, *hzk24fp;
#define   X1  100
#define   Y1  50
#define   X2  630
#define   Y2  420
typedef  struct  _MSG
 { unsigned char Xh;
   char Name[7];
   float *Dy;
   float *Zy;
   float *Fj;
   struct _MSG  *Next;
 } ST_MSG;

extern struct _DY
 { char Name[10];
 } *DY ;

extern struct _ZY
 { char Name[10];
   char Attrib;
   float Xf;
 } *ZY ;

extern struct _FJ
 { char Name[10];
 } *FJ ;
extern char  bkcolor;
extern char  INPUT;
extern char  msg;
extern int DYnum, ZYnum, FJnum, STnum;
extern int  Begin, BeginL;
extern ST_MSG *Home, *End;
extern char filename[30], Man[7], Head[50];
extern char bname[10][30];
extern int  namenum;
extern unsigned char nowx, nowy;
extern int bx1, bx2, by1, by2;
extern char kind, order, help, fcolor, AT, autosave, showenter;
extern int  see, bcode, watch, saveflag, jsflag, maxfilenum;
extern int  B, E;
extern char setfile[40] ;
extern int  Beginx[3];
extern char key, hzkf, deskcolor, papercolor, colorfa;
extern int  exist[5], bsee[5];
extern int  K, T;
extern long T0;
extern struct JS
 { int num;
   char *str;
 } PTR[5];
extern float SD, SB, SX, SR, SF;
extern float  ND, NB, NX, NR, NF;
extern float  MB, MX, MR;
extern char ERR, CLEAR, SOUND, INFLAG, UP;
extern int  PX, PY;
extern int  start[5], inexist, bstart;
#define  TRUE  1
#define  FALSE 0