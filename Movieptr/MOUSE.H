#ifndef  MOUSE_H_FILE
#define  MOUSE_H_FILE

/**********************************************************************************************/
/*************�궨�岿��********************/
#define  OFF         0
#define  ON          1
#define  FALSE       0
#define  TRUE        1

#define  LEFTBUTTON  1
#define  RIGHTBUTTON 2
#define  NOBUTTON    0

#define  NOCLICK     0
#define  LEFTCLICK   1
#define  RIGHTCLICK  2
#define  DOUBLECLICK 3

#define  STATICPTR   0	 			/*ָ����ǰ�������Ϊ��̬ͼƬ*/
#define  MOVIEPTR    1 				/*ָ����ǰ�������Ϊ����*/

#define DEFAULT_STATICPTR  (&default_staticptr)	/*Ԥ���徲̬�������--ָ��*/
#define HAND_STATICPTR	   (&hand_staticptr)	/*Ԥ���徲̬�������--����*/
#define HELP_STATICPTR     (&help_staticptr)	/*Ԥ���徲̬�������--�ʺ�*/
#define MOVE_STATICPTR     (&move_staticptr)	/*Ԥ���徲̬�������--�ƶ�*/
#define TEXT_STATICPTR     (&text_staticptr)	/*Ԥ���徲̬�������--�ı�ѡ��*/
#define CROSS_STATICPTR    (&cross_staticptr)	/*Ԥ���徲̬�������--ʮ����*/
#define PEN_STATICPTR      (&pen_staticptr)	/*Ԥ���徲̬�������--Ǧ��*/
#define NOUSE_STATICPTR    (&nouse_staticptr)	/*Ԥ���徲̬�������--��Ч*/
#define CUR_STATICPTR      (getmouse_staticptr())/*��ǰ��̬�������*/

#define DEFAULT_MOVIEPTR   (&default_movieptr)	/*Ԥ���嶯̬�������--����ָ��*/
#define TRY_MOVIEPTR       (&try_movieptr)	/*Ԥ���嶯̬�������--��ײָ��*/
#define BREAK_MOVIEPTR     (&break_movieptr)	/*Ԥ���嶯̬�������--��ը*/
#define FIRE_MOVIEPTR      (&fire_movieptr)	/*Ԥ���嶯̬�������--����*/
#define WATER_MOVIEPTR     (&water_movieptr)	/*Ԥ���嶯̬�������--��Ȫ*/
#define CUR_MOVIEPTR	   (getmouse_movieptr())/*��ǰ��̬�������*/
/*********************************************************************************************/


/*************���ָ�붨�岿��************************/
typedef  struct				/*���徲ָ̬��*/
 { int focus_x;
   int focus_y;
   unsigned char static_ptr[4][16];
 } MOUSE_STATICPTR;

struct _PTR
 { MOUSE_STATICPTR *ptr;/*��ǰ֡*/
   struct _PTR  *next;	/*��һ֡*/
 };
typedef  struct
 { struct _PTR *home;	/*��һ֡*/
   struct _PTR *ptr;	/*��ǰ֡*/
   int num;		/*֡����*/
   int xch, mxch;	/*��¼��ǰ֡��ż�֡�ٶ�*/
 } MOUSE_MOVIEPTR;			/*���嶯̬���*/


/******************************************************************************************/
/***********�����״���ò���**************/
int  readmouse_staticptr(unsigned char (*ptr)[16], char *filename);   /*���ļ��ж�ȡ���ָ��ͼƬ*/
void setmouse_staticptr(MOUSE_STATICPTR *ptr);	 	/*����һ�����ָ����״*/
void start_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr);/*��ʼ������̬���*/
void add_makemovieptr(MOUSE_STATICPTR *mouse_staticptr);/*���Ӷ�̬���ͼ��*/
void end_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr);	/*������̬�������(��δ���κ�����)*/
void free_makemovieptr(MOUSE_MOVIEPTR *mouse_movieptr); /*�ͷŶ�̬���*/
void setmouse_movieptr(MOUSE_MOVIEPTR *mouse_movieptr);	/*����һ����̬���*/

/**********�����Ʋ���*******************/
int  start_mouse();				/*��ʼ������*/
void mouse_off(), mouse_on();     		/*���Ĺرպʹ�*/
void setmouse_xy(int x, int y);  		/*�������λ��*/
void setmouse_xy(int x, int y);			/*��������λ��*/
void setmouse_max_x(int left, int right);	/*������������ƶ�����*/
void setmouse_max_y(int top, int bottom);	/*�������������ƶ�����*/
void exit_mouse();				/*�˳�������*/
#define setmouse_movespeed(movieptr,x) movieptr.mxch=x; /*����һ����̬���ı仯Ƶ��*/
void setmouse_model(int model);			/*��������ָ��ģʽ(��̬��̬)*/
void doubleclick_on();				/*�򿪼��˫��ģʽ(Ĭ��)*/
void doubleclick_off();				/*�رռ��˫��ģʽ*/
void setdbclick_time(long newtime);		/*����˫���ٶ�*/
void setfocus_xy(int x, int y);			/*���õ�ǰ���ĺ�������򽹵�*/
void movieptr_mirrorx(MOUSE_MOVIEPTR *ptr);	/*����ת��̬���*/
void movieptr_mirrory(MOUSE_MOVIEPTR *ptr);	/*����ת��̬���*/
void staticptr_mirrory(MOUSE_STATICPTR *ptr);		/*����ת��̬���*/
void staticptr_mirrorx(MOUSE_STATICPTR *ptr);		/*����ת��̬���*/
void curptr_mirrorx();				/*����ת��ǰ���*/
void curptr_mirrory();				/*�ݺͷ�ת��ǰ���*/

/**********����ⲿ��*******************/
void getmouse_xy(int *x, int *y);   		/*��ȡ��ǰ���λ��*/
int  getmouse_x();				/*��ȡ��ǰ��������*/
int  getmouse_y();				/*��ȡ��ǰ���������*/
int  getmouse_button();				/*�������İ���״̬*/
int  imouse_inbox(int x1, int y1, int x2, int y2);	/*�жϵ�ǰ��꽹���Ƿ���ָ��������*/
int  imouse();					/*��ѯ��ǰ����Ƿ��*/
#define getmouse_movespeed(movieptr) (movieptr.mxch)    /*��ѯһ����̬���ı仯Ƶ��*/
int  getmouse_model();				/*��ѯ����ָ��ģʽ(��̬��̬)*/
MOUSE_STATICPTR  *getmouse_staticptr();		/*��ѯ��̬������״*/
MOUSE_MOVIEPTR  *getmouse_movieptr();		/*��ѯ��̬������״*/
int  idoubleclick();				/*���˫��ģʽ�Ĵ���ر�*/
int  iclick();					/*��ⵥ����˫���¼�*/
#define mousehit() _click			/*����Ƿ��е�����˫���¼�(�����)*/
long getdbclick_time();				/*���˫���ٶ�(Ĭ��Ϊ4)*/
#define   getclick_x()	_nowX_			/*��ѯ������˫���¼�ʱ�ĺ�����*/
#define   getclick_y()  _nowY_			/*��ѯ������˫���¼�ʱ��������*/
int   getfocus_x();				/*��ȡ��ǰ���ĺ��򽹵�*/
int   getfocus_y();				/*��ȡ��ǰ�������򽹵�*/


/*************ȫ�ֱ�������***************/
/********************************************************************************************/
extern MOUSE_STATICPTR default_staticptr, hand_staticptr, help_staticptr, move_staticptr, 
		text_staticptr, cross_staticptr, pen_staticptr, nouse_staticptr;
extern MOUSE_MOVIEPTR default_movieptr, tryer_movieptr, break_movieptr,
                        fire_movieptr, water_movieptr;
extern int _nowX_, _nowY_;
extern int _click;
/********************************************************************************************/

#endif