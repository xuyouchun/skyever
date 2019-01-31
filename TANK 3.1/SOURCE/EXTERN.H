extern  FILE *hzk24fp, *hzk16fp;
#define   YES  1
#define   NO   0

typedef struct        //Tank option
 { unsigned   ienemy   : 1;
   unsigned   life     : 3;  //0,1,2,3    0-death;
   unsigned   blife    : 3;
   unsigned   state    : 3;  //7-4 appear;  3 life;  2-0; death;
   unsigned   statetime: 5;  //state time
//****************************************
   unsigned   eats     : 3;  //0,1,2,3    0-NONE;
   unsigned   flashflag: 4;  //0,1-flash; 2,3-quit;
   unsigned   addjust  : 12;
   unsigned   spendflag: 1;
//****************************************
   unsigned   round    : 2;  //0-left; 1-right; 2-up; 3-down;
   unsigned   speed    : 2;  //0,1,2,3;
//****************************************
   unsigned   istop    : 1;  //0,1
   unsigned   ibstop   : 1;  //0,1
   unsigned   iship    : 1;  //in sea
   unsigned   ibship   : 1;  //in sea
   unsigned   ipause   : 1;  //pause flag 
   unsigned   shotspeed: 2;  //0,1,2
   unsigned   shottime : 6;  //shottime flag;
   unsigned char stop     ;  //flag for stop;
//****************************************
   char          X        ;  //X
   char          Y        ;  //Y
   char          dxy      ;  //dxy
 }  TANKDEF;

#define  MAXSHOTNUM   20
extern TANKDEF  Tank[12] ;

typedef struct
 { unsigned  ienemy    : 1;
   unsigned  speed     : 2; //0,1,2,3;
   unsigned  round     : 2; //0-left; 1-right; 2-up; 3-down;
   unsigned  life      : 1; //0-death;  1-life;
   unsigned  state     : 3;
   unsigned  statetime : 3;
   unsigned  which     : 4;
   unsigned  treebreak : 1;
   unsigned  istep     : 1;
   char X                 ; //X
   char Y                 ; //Y
   char      dxy          ; //dxy
 }  SHOTDEF;
extern SHOTDEF  Shot[MAXSHOTNUM];

typedef  unsigned char   ROADDEF;
extern   ROADDEF  Road[28][36], Roadtank[28][36];

extern   unsigned  char  Roadptr[8][4][16*2];
typedef  struct
 { unsigned   istoptank : 1;  //ifstoptank  1-Yes,  0-No
   unsigned   ihard     : 1;  //ifhard      1-Yes,  0-No
   unsigned   istopshot : 1;  //ifstopshot  1-Yes,  0-No
   unsigned   icoverd   : 1;  //ifcoverd tank and shot  1-Yes,  0-No
   unsigned   ihua      : 1;  //ifstop tank at once   1-YES,  0-NO
   unsigned   iend      : 1;  //ifgame end on break   1-YES,  0-NO
 } ROADATTRIBDEF;
extern  ROADATTRIBDEF  Roadattrib[7];

extern  unsigned  char   Tankptr[5][4][4][32*4];
extern  unsigned  char   Tankbit[5][4][32*4];
extern  unsigned  char   Breakptr[5][4][32*4];
extern  unsigned  char   Breakbit[5][32*4];
extern  unsigned  char   Exptr[9][4][32*6];
extern  unsigned  char   Exbit[9][32*6];
extern  unsigned  char   Headptr[4][32*4];
extern  unsigned  char   Picptr[4][32][36*2];
extern  unsigned  char   Shotptr[3][4][16];
extern  unsigned  char   Shotbit[3][16];
extern  unsigned  char   Kindptr[8][32*4];

extern  int  nowtankappear, hadtankappear;
extern  int  nowshotappear, fire, firekind;

#define  NOEAT  100
extern  struct _EAT { char  kind, X, Y; }  Eat;

typedef struct
 { char num;
   unsigned char Hz[5];
   unsigned char Ti[5];
 } SOUNDDEF;

typedef struct
 { unsigned  mynum:       7;
   unsigned  ennum:       7;
   unsigned  myshotkind:  2;
   unsigned  myshotspeed: 2;
   unsigned  iappear:     1;
   unsigned  isea:        1;
   unsigned  tankspeed:   2;
   unsigned  enshotspeed: 2;
   unsigned  ieneat:      1;

   unsigned  shothz:      3;
   unsigned  pausetime:   3;  
   unsigned  firetime:    3;
   unsigned  irontime:    3;
   unsigned  addjusttime: 3;
   unsigned  inbreak:     1;
   unsigned  insea:       1;
   unsigned  fs:          2;
   unsigned  maxtank:     4;
 } KINDDEF;
extern KINDDEF  playkind[26];
#define  P    (playkind[kindflag])

struct _KEY
 { unsigned  left    : 1;
   unsigned  right   : 1;
   unsigned  up      : 1;
   unsigned  down    : 1;
   unsigned  akey    : 1;
   unsigned  skey    : 1;
   unsigned  dkey    : 1;
   unsigned  fkey    : 1;
   unsigned  space   : 1;
   unsigned  esc     : 1;
   unsigned  enter   : 1;
   unsigned  insert  : 1;
 } ;


typedef struct
 { char x, y, dx, dy, dxy ;
   char fs[25];
 } SHOWFS;

extern SHOWFS _showfs[13];
extern int    _shownum;

#define  NOTANK   100

#define  VER  "Tank 3.1"
#define  KINDSTR "==>> KIND %c <<=="
