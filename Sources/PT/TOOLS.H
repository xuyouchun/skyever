#define  IN    1
#define  OUT   0

void  getpoints();
void  _getpoints(int code);
void  linkpoints(double *x, double *y, int num);
void  link(int code);
void  flood(int code);
int   idotin3(double *_X, double *_Y, double X0, double Y0);
int   idotin(int code, double X0, double Y0);
int   which_active(int X, int Y);
void  set_layer(int n);
void  middle_round(double*, double*, double);
double middle_rad(double, double, double X, double Y);

extern  int _color_;
extern  int _fillcolor_;
#define setlinkcolor(x) _color_ = x
#define getlinkcolor()  _color_
#define setfillcolor(x) _fillcolor_ = x
#define getfillcolor()  _fillcolor_