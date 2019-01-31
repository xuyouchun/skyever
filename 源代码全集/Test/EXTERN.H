extern BUTTON  button[4], button2[4];
extern XMENU   xmenu;
extern int     select;
extern FILE  * fp;
extern int     titnum, selnum;
extern char  * PA;
extern char  * PG;
extern char  * PQ;
extern char  * PU;
extern char  * PE;
extern char  * PR;
extern char  * TITLE;
extern int     sel;
extern char  dirfile[21], includefile[21], commandfile[21], helpfile[21];

extern char  * ptr, * menuptr, nameptr[50], *helpptr;

typedef struct _SEL
 { SELECT    sel;
   char    * ST;
   char    * SZ;
   int       time, time0, passtime;
   char      format;
   int       fs;
 } SEL;

extern  SEL  *titptr;
extern  char  ifcontinue, ifopen, ifstart, ifend,
		  ifenablepause, iffileopen, ifft, ifjm, *readptr;

extern  int  X, Y;
extern  BUTTON  but_begin, renew, complete, allend;
extern  BUTTON  but_end[4], *blist;
extern  int  nowselect;
extern  char showflag, spaceshow, Order, ifsave, ishowqk;
extern  long  aT, aT0, T, T0;
extern  int   aTime, remindtime;
extern  int   select, dataptr, maxdata;
extern  int   listnum, maxlistnum;
extern  char  filelist[10][20];
extern  int   bkcolor;
extern  double _DATA;

extern  LIST  list;

#define  LEFT    28
#define  TOP     130
#define  RIGHT   612
#define  BOTTOM  422

#define  write_str(x1, y1, x2, y2, str) _write_str(x1, y1, x2, y2, str, x1, y1)

