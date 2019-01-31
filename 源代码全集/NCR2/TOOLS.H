void xline(int,int,int,int);
void clearfile(char *path, char *arc="*.*");

void tcedit();
void savescreen(char *filename),  writescreen(char *filename);
int  xstrcmp(char *str1, char *str2);

#define clearscreenfile()   clearfile("temp\\window", "*.win")

BOOL msgbox(char *str, char backcolor=CYAN, char forcolor=YELLOW, int kind=0);
BOOL msgbox(char *str, int kind) { return msgbox(str, (char)CYAN, (char)YELLOW, kind); }
char * getfilename(char *filename, char *searchpath=".", char *head="", char *hz="*", char *te=NULL);
char * getdirname(char *dirname, char *head="", EXAMFLAG &Examflag=Program, BOOL iedit=true);
void  bar3ds(int x1, int y1, int x2, int y2, BOOL flag=down, int color=-1);
