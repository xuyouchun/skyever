#include <tx.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include "extern.h"

char order, Order;

void  freeall();
int   getmsg();
int   getstr();
double strtonum(char *str);
void  renewsel(int), resetsel(int);
void  cleardesktop(int);
void  enablebutton(BUTTON*), unablebutton(BUTTON*);
void  testsel(char *str, int *K),  testnum(char *str, int *K);
void  show_asktit(), show_about(), show_end();
void  unablex(int,char*), autox(int,char*), hidex(int,char*);
void  Delay(int), showsound(int), getdef();
int   getlabelname(char *ptr, char *f, char *str);
int   Fgetc(),   getline(long);
int   getlabel(char *label);
int   findlabel(char *label);
void  test_buttonenable(), reset_qk(), show_qk();
void  renewstr(char *str);
int  _getlabel(char *str, int *K, char *label);
int  getdnum(char *str, int *K, double *m);
int  getorder(int N, int model), getflag(char *str, char *label);
int  _fgetc(FILE *);
void  _rewind(FILE *), _fseek(FILE *, long seek, int where);
char  * gettimestr(char *), * getdatestr(char *);
char nameptr[50];
char mm[10];
char filelist[10][20]; int listnum, maxlistnum=10;
char dirfile[21] = "menuset", commandfile[21] = "command.def",
	 includefile[21] = "define.txt", helpfile[21]="help.txt";
void addfilelist(), renewfilelist(), readdirectseek(), readcommandseek();
long  *seek;
char  *readptr; int readflag;
int  point = 6;
char trueflag=1, orflag=-1;

typedef struct
 { char  dataname[21];
   double data;
   char  *str;
   char  attrib;
 } DATADEF;            //用于定义数据
DATADEF  *DATA=NULL;
int  dataptr, maxdata;

typedef  struct
 { char  definename[21];
   long  seek;
 } SEEK;              //用于定义位置
SEEK  *seekptr=NULL;
int   maxseek, defineorder;

typedef  struct
 { char  definename[21];
   char  definestr[31];
 } DIRECTSEEK;         //用于定义宏
DIRECTSEEK  *directseek=NULL;
int  directptr, maxdirect;

typedef  struct
 { char  definename[11];
   char  definestr[11];
 } COMMANDSEEK;      //用于定义命令
COMMANDSEEK  *commandseek=NULL;
int  commandptr, maxcommand;

typedef  struct
 { char  flagname[20];
   int   seek;
 } FLAG;            //用于定义标号
FLAG  *Flag=NULL;
int  flagptr, maxflag;

typedef  struct
 { char  funcname[20];
   char  *str;
 } FUNC;             //用于定义程序段
FUNC  *Func=NULL;
int   funcptr, maxfunc;

char  showflag, ifstep=1, spaceshow=1;
long  aT, aT0;
int   aTime = -1, sflag, remindtime, ifenterpause=1, ifsound=1;
char  *menuptr = NULL, *helpptr = NULL;
char  ispacein;
void  pushorder(), poporder();
double _DATA;
char  ifjm;

#define   _BOT    30
#define   inum(x)  (x>='0'&&x<='9'||x=='-')
int       ispacech(x) { if(x==' '||x==9||x=='\n'||x=='\r') return 1; return 0; }

#define   CLEARSTEP 				 \
 if(y>y2-Hd) 					 \
   { if(ifstep==0)  { ishow=0; break; } 	 \
     mouse_on();				 \
     while(!kbhit()&&getmousebutton()==NOBUTTON);\
     if(kbhit()&&getch()==ESC_KEY)		 \
       { but_end[2].attrib&=~BD; show_end(); return y2-Hd; }\
     while(kbhit())  getch(); 			 \
     while(getmousebutton()!=NOBUTTON);          \
     mouse_off(); 				 \
     bar(x1, y1, x2, y2);			 \
     x = x1;  y = y1; high = 0;  		 \
   }


int  far _write_str(int x1, int y1, int x2, int y2, char *str, int x0, int y0)
 { int high = 0, Hd = 17, k, K=0, D, j;  char ch, c, label[21];
   double  f;
   static int x, y, N, _x, _y;
   int color = getcolor();
   static char ispaceshow, ishow, linecolor, iline, idot;
   list.attrib = 0;
   list._bar.bx1=list._bar.bx2=list._bar.by1=list._bar.by2 = 0;
   if(N==0)
     { x=x0; y=y0; _x=x; _y=y; ispaceshow=spaceshow; ishow=1; point=6;
       linecolor = YELLOW;  iline = 0;  idot = 0;
     }
   mouse_off();
   if(N==0)  { setfillstyle(0, 0);  bar(x1, y1, x2, y2); }
   while(str[K]!='\0')
    { if(str[K]<0)
       { if(x+17>x2)
	   { x = x1;  y += 17;  high+=17; CLEARSTEP }
	 if(ishow==1)  show_word(x, y, str+K);
	 if(iline==1)
	    { int color=getcolor();  setcolor(linecolor);
	      line(x, y+16, x+16, y+16); setcolor(color);
	    }
	   if(idot==1)
	    { setfillstyle(1, linecolor);  bar(x+7, y+14, x+9, y+16); }
	 K += 2;  x += 17;
       }
      else if(str[K]=='}'||str[K]=='{')  { K ++; continue; }
      else if(str[K]=='&')
       { K ++;
	 if(_getlabel(str, &K, label)==0)
	  { msg_box("运行错误：", "发现未允许的位置标识符定义形式！－错误状态：&(位置标签)");
	    return 0;
	  }
	 if(flagptr==maxflag)
	   Flag = (FLAG*)realloc(Flag, (maxflag=maxflag+20)*sizeof(FLAG));
	 strcpy(Flag[flagptr].flagname, label);
	 Flag[flagptr++].seek = K-1;
       }
      else if(str[K]=='%')
       { char  filename[35];
	 switch(ch=str[++K])
	  { case '[':
		  { int  flag = 0;
		    while(str[K] != '\0')
		     { if(str[K]=='%')  K++;
		       else if(str[K]=='{')  flag ++;
		       else if(str[K]=='}')
			      if(flag--<=0)  break;
		       K ++;
		     }
		  }  break;
	    case 'r': x=x1; y+=Hd; high+=Hd; Hd=17; CLEARSTEP  break;  //换行不空格
	    case 'R': x=x1+32; if(K-1) y+=Hd; high+=Hd; Hd=17; CLEARSTEP  break;  //换行空俩格
	    case 'p': if(ifenterpause==0) break;
		      mouse_on(); while(getch()!=ENTER_KEY); mouse_off(); break;  //暂停按回车键继续
	    case 'w': if(!inum(str[K+1]))  { x+=16; break; }  //横向空位
		      x += (int)strtonum(str+(++K));
		      if(str[K]=='-')  K ++;
		      while(str[K]<='9'&&str[K]>='0')  K ++;
		      K --;      break;
	    case 'h': if(!inum(str[K+1]))  { y+=16; break; }  //纵向空位
		      y += (k=(int)strtonum(str+(++K)));
		      if(str[K]=='-')  K ++;  high -= k;
		      while(str[K]<='9'&&str[K]>='0')  K ++;
		      K --;      break;
	    case 'N': testsel(str, &K); K--; break;    //依数目选择
	    case 'm': testnum(str, &K); K--; break;    //依分数选择
	    case 'C': if(str[K+1]<'0'||str[K+1]>'9')   //设定当前颜色
			{ setcolor(color); break; }
		      setcolor((int)strtonum(str+(++K)));
		      while(str[K]>='0'&&str[K]<='9')  K++;
		      K --;     break;
	    case 'c': if(str[K+1]<'0'||str[K+1]>'9')   //设定当前颜色
			{ setcolor(YELLOW); break; }
		      linecolor = (int)strtonum(str+(++K));
		      while(str[K]>='0'&&str[K]<='9')  K++;
		      K --;     break;
	    case 'L': switch(str[++K])
		       { case 'N': iline = 1; break;
			 case 'F': iline = 0; break;
			 default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%L...");
				   return 0;
		       } break;
	    case 'O': switch(str[++K])
		       { case 'N': idot = 1; break;
			 case 'F': idot = 0; break;
			 default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%O...");
				   return 0;
		       } break;
	    case '~': K ++;
		      if(_getlabel(str, &K, label)==0)
		       { msg_box("运行错误：", "发现未允许的位置标识符书写形式！错误状态－%~(位置标签)");
			 return 0;
		       }
		      if((K=getflag(str, label))<0)
		       { char str[100];
			 sprintf(str, "未发现标识符“%s”！", label);
			 msg_box("运行错误：", str);
			 return 0;
		       }  break;
	    case 'o': if(!inum(str[K+1]))  { point = 0; break; }  //横向空位
		      point = (int)strtonum(str+(++K));
		      while(str[K]<='9'&&str[K]>='0')  K ++;
		      K --;      break;
	    case '*': switch(ch=str[++K])
		       { case 'x': if(!inum(str[K+1]))  { x = _x; break; }
				   x = _x+(int)strtonum(str+(++K));
				   if(str[K]=='-')  K ++;
				   while(str[K]<='9'&&str[K]>='0')  K ++;
				   K --;      break;
			 case 'y': if(!inum(str[K+1]))  { y = _y; break; }
				   y = _y+(int)strtonum(str+(++K));
				   if(str[K]=='-')  K ++;
				   while(str[K]<='9'&&str[K]>='0')  K ++;
				   K --;      break;
			 default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%*...");
				   return 0;
		       } break;
	    case 'Y': case 'y':  case 'X': case 'x':   //与锁存内容的比较来决定执行的内容
		      { double a, b;
			if(ch=='Y'||ch=='y')
			 { K ++;
			   if(_getlabel(str, &K, label)==0)
			    { msg_box("运行错误：", "发现未允许的变量标识符定义形式！－错误状态：%Y/y/X/x...");
			      return 0;
			    }
			   D = getdata(label);   a = DATA[D].data;
			 }
			else
			 { while(str[++K]!='[')
			    { if(!ispacech(str[K]))
			       { msg_box("运行错误：", "发现未允许的参数形式！－错误状态：%Y/y/X/x...");
				 return 0;
			       }
			    }
			   a = strtonum(str+(++K));
			   while(str[++K]!=']');
			 }
			b = _DATA;
			if(ch=='x'||ch=='y') { a = (int)a;  b = (int)b; }
			K -= 2;  k = 0;
			while(str[++K]!=')'&&str[K]!=']');
			switch(str[++K])
			 { case 'D': if(a>b)   k = 1; break;
			   case 'd': if(a>=b)  k = 1; break;
			   case 'X': if(a<b)   k = 1; break;
			   case 'x': if(a<=b)  k = 1; break;
			   case 'E': if(a==b)  k = 1; break;
			   case 'N': if(a!=b)  k = 1; break;
			   default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%Y/y/X/x...");
				     return 0;
			 }
			if(orflag==1&&k==0)  trueflag = 0;
			if(orflag==0&&k==1)  trueflag = 1;
			if(orflag==-1)       trueflag = k;
			orflag = -1;
			break;
	    case '+': orflag = 1;  break;
	    case '-': orflag = 0;  break;
	    case '!': trueflag = 1-trueflag; break;
	    case '$': switch(str[++K])
		       { case '0': trueflag = 0; break;
			 case '1': trueflag = 1; break;
			 default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%$...");
				   return 0;
		       } break;
	    case 'g': case 'G': if(trueflag==1)
			 { while(str[++K]!='{')
			    if(!ispacech(str[K]))
			       { msg_box("运行错误：", "发现未能识别的控制格式！－错误状态：%Y/y/X/x...");
				 return 0;
			       }
			   break;
			 }
			else
			 { int flag = 0;
			   while(str[++K]!='\0')
			    { if(str[K]=='%')  K ++;
			      if(str[K]=='{')  flag++;
			      if(str[K]=='}') if(--flag<=0)  break;
			    }
			 }
		       }  break;
	    case 'e': case 'E':
		      if(trueflag==1)
		       { int flag=0;
			 while(str[++K])
			  { if(str[K]=='%')  K ++;
			    else if(str[K]=='{')  flag++;
			    else if(str[K]=='}')  if(--flag<=0)  break;
			  }
		       }
		      else
		       { while(str[++K])
			   { if(str[K]=='%') K ++;
			     if(str[K]=='{') break;
			   }
		       }
		      if(ch=='E')  trueflag = 1;
		      break;
	    case '(': if(_getlabel(str, &K, label)==0)
			{ msg_box("运行错误：", "发现未允许的过程标识符定义形式！－错误状态：%S/s(标识符)...");
			  return 0;
			}
		      for(k=0; k<funcptr; k++)
		       if(strcmp(label, Func[k].funcname)==0)  break;
		      if(k==funcptr)
		       { char str[100];
			 sprintf(str, "发现未定义的过程标识符“%s”！", label);
			 msg_box("错误：", str);
			 return 0;
		       }  K --;
		      N++; _write_str(x1, y1, x2, y2, Func[k].str, x, y); N--;
		      break;
	    case 'S': case 's': K ++;                  //字符串操作
		      if(_getlabel(str, &K, label)==0)
			{ msg_box("运行错误：", "发现未允许的变量标识符定义形式！－错误状态：%S/s(标识符)...");
			  return 0;
			}
		      D = getdata(label);
		      if(ch=='S')
		       { if(str[K]!='s'&&str[K]!='n')
			  { if(DATA[D].str!=NULL)
			      DATA[D].str = (char*)realloc(DATA[D].str, 1);
			    else  DATA[D].str = (char*)malloc(1);
			    DATA[D].str[0] = '\0';
			  }
		       }
		      else if(DATA[D].str==NULL)
			 { DATA[D].str = (char*)malloc(1);
			   DATA[D].str[0] = '\0';
			 }
		      switch(ch=str[K])
		       { case 'c': { char _str[1000]; int len; k=0;  //字符串赋值（连接）
				     while(str[++K]!='[')
				      if(!ispacech(str[K]))
				     re:{ msg_box("运行错误：", "给字符串赋值时出现不能识别的格式！－错误状态：%S/s(标识符)c[字符串]");
					  return 0;
					}
				     while(str[++K])
					{ if(str[K]==']')  break;
					  _str[k++] = str[K];
					  if(str[K]=='\0')  goto re;
					}
				     _str[k] = '\0';
				     DATA[D].str = (char*)realloc(DATA[D].str,
					    strlen(DATA[D].str)+strlen(_str)+2);
				     strcat(DATA[D].str, _str);
				   }  break;
			 case 'n': if(DATA[D].str==NULL||DATA[D].str[0]=='\0')  _DATA=0;  //检测字符串是否为空
				   else  _DATA=1;  break;
			 case 'e': if(DATA[D].str!=NULL)  free(DATA[D].str);//字符串清空
				   DATA[D].str = NULL; break;
			 case 'i': case 'f':  //字符串转化
				   sprintf(label, ch=='d'?"%d":"%f",
						   ch=='d'?(int)_DATA:_DATA);
				   if(ch=='f')  renewstr(label);
				   DATA[D].str = (char*)realloc(DATA[D].str,
					    strlen(DATA[D].str)+strlen(label)+2);
				   strcat(DATA[D].str, label);
				   break;
			 case 's': if(DATA[D].str==NULL)  break;
				   N++; _write_str(x1, y1, x2, y2, DATA[D].str, x, y); N--;
				   break;
			 case 'R': switch(ch=str[++K])  //字符串与数的转化
				    { case 'f': case 'i':
						if(ch=='f')
						  { sprintf(label, "%f", DATA[D].data); //同名数据以实型转化为同名字符串
						    renewstr(label);
						  }
						else sprintf(label, "%d", (int)DATA[D].data);  //同名数据以整型转化为同名字符串
						DATA[D].str = (char*)realloc(DATA[D].str,
						       strlen(label)+strlen(DATA[D].str)+2);
						strcat(DATA[D].str, label);  break;
				      case 's': case 'S'://同名字符串以实型转化为同名数据
						if(DATA[D].str==NULL)  DATA[D].data = 0;
						else  DATA[D].data = strtonum(DATA[D].str);
						break;
				     }  break;
			 case 'l': case 'F': case 'I': K ++;
				 { char _str[1000];
				   if(_getlabel(str, &K, label)==0)
				     { msg_box("运行错误：", "发现未允许的变量标识符定义形式！－错误状态：%S/s...");
				       return 0;
				     }
				    k = getdata(label);
				    if(ch=='F')
				      { sprintf(_str, "%f", DATA[k].data);
					renewstr(_str);
				      }
				    else if(ch=='I')
				       sprintf(_str, "%d", (int)DATA[k].data);
				    else { if(DATA[k].str==NULL) {K--; break;}
					   strcpy(_str, DATA[k].str);
					 }
				    DATA[D].str = (char*)realloc(DATA[D].str,
					    strlen(DATA[D].str)+strlen(_str)+2);
				    strcat(DATA[D].str, _str);
				  }  K--;  break;
			 case 'T':  gettimestr(label);
			 case 'D':  if(ch=='D')  getdatestr(label);
				    DATA[D].str = (char*)realloc(DATA[D].str,
					   strlen(DATA[D].str)+strlen(label)+2);
				    strcat(DATA[D].str, label);
				    break;
			 default:   msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%S(标识符)...");
				    return 0;
		       }  break;
	    case 'V': switch(str[++K])
		       { case 'N': ispaceshow = 1; break;
			 case 'F': ispaceshow = 0; break;
			 default:  msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%V...");
				   return 0;
		       } break;
	    case 't': switch(str[++K])
		       { case 'T': N++; _write_str(x1, y1, x2, y2, gettimestr(label), x, y); N--; break;
			 case 'D': N++; _write_str(x1, y1, x2, y2, getdatestr(label), x, y); N--; break;
			 case 'i': sprintf(label, "%d", (int)_DATA);
				   N++; _write_str(x1, y1, x2, y2, label, x, y); N--; break;
			 case 'f': sprintf(label, "%f", _DATA); renewstr(label);
				   N++; _write_str(x1, y1, x2, y2, label, x, y); N--; break;
			 default: msg_box("运行错误：", "发现不能识别的控制符！－错误状态：%t...");
				  return 0;
		       } break;
	    case 'i':  case 'f':
		       { char label[21];  int n;  K ++;
			 if(_getlabel(str, &K, label)==0)
			   { char str[100];
			     sprintf(str, "发现未允许的变量标识符定义形式！－出错状态：%%%c(标识符)]",ch);
			     msg_box("运行错误：", str);
			     return 0;
			   }
			 n = getdata(label);  K --;
			 if(ch=='i') sprintf(label, "%d", (int)DATA[n].data);
			 else       { sprintf(label, "%f", DATA[n].data); renewstr(label); }
			 N++; _write_str(x1, y1, x2, y2, label, x, y); N--;
		       } break;
	    case 'P': while(str[++K]!='(')            //显示图片
			if(str[K]=='\0')  break;
		      if(str[K]=='\0')  break;
		      k = 0;
		      while(str[++K]!=')')
		       { if(str[K]=='\0')  break;
			 filename[k++] = str[K];
			 if(k>30)  break;
		       }
		      if(k>30)  break;
		      filename[k] = '\0';
		      showimage(x, y, filename);
		      x += getimagewide();
		      if(getimagehigh()>Hd)  Hd = getimagehigh();
		      break;
	    case 'q': setfillstyle(0, 0);  bar(x1, y1, x2, y2); //清屏复位
		      x = _x; y = _y;  break;
	    case 'F': for(k=0; k<maxdata; k++)  DATA[k].attrib = 0;  //给数据分组
		      while(str[++K]!='[')
			if(!ispacech(str[K]))
			  { msg_box("错误：", "数据分组时出现格式错误！－出错状态：%F[(标识符)...]");
			    return 0;
			  }
		      do { int code;  K --;
			   while(str[++K]!='(')
			     if(str[K]==']'||str[K]=='\0') goto end;
			   code = _getlabel(str, &K, label);
			   if(code==0)
			    { msg_box("错误：", "发现未允许的变量标识符！－出错状态：%F[(标识符)...]");
			      return 0;
			    }
			   DATA[getdata(label)].attrib = 1;
			 } while(1);
		      end: break;
	    case 'J': case 'j':                                //计算数据
		      { int N, k, i;  K ++;
			if(_getlabel(str, &K, label)==0)
			  { msg_box("运行错误：", "发现未允许的变量标识符定义形式！－出错状态：%J/j(标识符)...");
			    return 0;
			  }
			N = getdata(label);
			while(ispacech(c=str[K++]));
			if(ch=='J' && c!='L'&&c!='M')  DATA[N].data = 0;
			switch(c)
			 { case 'A':  case 'D':  case 'S':   //计算总分/单数/双数
				      for(k=0; k<=titnum; k++)
				       { int flag=0;
					 if(c=='D'&&k%2!=0)  continue;
					 if(c=='S'&&k%2==0)  continue;
					 for(i=0; i<titptr[k].sel.num; i++)
					   if(titptr[k].sel.text[i].attrib&BD)
					      { DATA[N].data += titptr[k].sel.text[i].num;
						flag ++;
					      }
					  if(flag==0)  DATA[N].data += titptr[k].fs;
					}
				      break;
			   case 'I': case 'E':   //计算间隔/忽略
				     { int n, m; double f;
				       while(str[K]!='[')
					{ if(!ispacech(str[K++]))
					   { msg_box("运行错误：", "发现未允许的参数形式！－出错状态：%J/j(标识符)E[数字]");
					     return 0;
					   }
					}
					if((k=getdnum(str, &K, &f))<0)
					  { msg_box("运行错误：", "发现未允许的数学表达式！－出错状态：%J/j(标识符)E[数字]");
					    return 0;
					  }
					n = (int)f;
					if((k=getdnum(str, &K, &f))<0)
					  { msg_box("运行错误：", "发现未允许的数学表达式！－出错状态：%J/j(标识符)E[数字]");
					    return 0;
					  }
					m = (int)f;
					if(m==10&&n==10)  Exit(0);
					else if(k!=']')
					 { msg_box("运行错误：", "发现未允许的参数形式！－出错状态：%J/j(标识符)E[数字]");
					   return 0;
					 }
					for(k=0; k<=titnum; k++)
					  { int flag=0;
					    if(c=='I')
					      { if((k+1-m)%n==0)  flag = 1; }
					     else if(c=='E')
					      { if((k+1-m)%n!=0)  flag = 1; }
					     if(flag==1)
					      { int f, i;
						for(i=0; i<titptr[k].sel.num; i++)
						  if(titptr[k].sel.text[i].attrib&BD)
						    { DATA[N].data += titptr[k].sel.text[i].num;
							f ++;
						    }
						 if(f==0)  DATA[N].data += titptr[k].fs;
					      }
					   }
					 K ++;
				       }
				     break;
			   case 'T': { int k, i, code; double m;  //计算指定题号
				       while(str[K]!='[')
					{ if(ispacech(str[K++]))
					   { msg_box("运行错误：", "发现未允许的参数形式！－出错状态：%J/j(标识符)T[数字]");
					     return 0;
					   }
					}
				       do
					{ code = getdnum(str, &K, &m);
					  if(code<0)
					    { msg_box("运行错误：", "发现未允许的参数形式！－出错状态：%J/j(标识符)T[数字]");
					      return 0;
					    }
					  if(code==','||code==']')
					   { int flag, i;
					     if(--m<0 || m>titnum)
					       { msg_box("错误：", "标题号越界！－出错状态：%J/j(标识符)T[数字]");
						 return 0;
					       }
					     for(i=0; i<titptr[m].sel.num; i++)
					      if(titptr[m].sel.text[i].attrib&BD)
						{ DATA[N].data += titptr[m].sel.text[i].num;
						  flag ++;
						}
					     if(flag==0)  DATA[N].data += titptr[m].fs;
					    }
					  else if(code=='~')
					   { int begin=m, end, t; double f;
					     code=getdnum(str,&K,&f);
					     end = (int)f;
					     if(code!=',' && code!=']')
					       { msg_box("错误：", "发现未允许的参数形式！－出错状态：%J/j(标识符)T[数字]");
						 return 0;
					       }
					     if(begin>end) { t=begin; begin=end; end=t; }
					     begin--;  end--;
					     if(begin<0 || end>titnum)
					      { msg_box("错误：", "标题号越界！－出错状态：%J/j(标识符)T[数字]");
						return 0;
					      }
					     for(k=begin; k<=end; k++)
					      {  int flag=0;
						 for(i=0; i<titptr[k].sel.num; i++)
						   if(titptr[k].sel.text[i].attrib&BD)
						    { DATA[N].data += titptr[k].sel.text[i].num;
						      flag ++;
						    }
						 if(flag==0)  DATA[N].data += titptr[k].fs;
					      }
					   }
					}  while(code!=']'&&code>0);
				       K ++;
				     }  break;
			   case 'Z':  DATA[N].data = 0;  break;       //赋为零
			   case 'B':  while(str[K++]!='[')
					if(ispacech(str[K])) re0:
					  { msg_box("错误：", "发现未能识别的数字格式！－出错状态：%J/j(标识符)B[数字]");
					    return 0;
					  }
				      DATA[N].data += strtonum(str+K);
				      while(str[++K]!=']')
					if(str[K]=='\0')  goto re0;
				      K ++;  break;
			   case 'Y':  DATA[N].data += _DATA;  break;  //加上锁存内容
			   case '*':  case '$': case '+': case '-': case '|': case '@': case 'F': //加减乘除
				       while(str[K]!='[')
					{ if(!ispacech(str[K++]))
					   { char str[100];
					     sprintf(str, "发现未允许的参数形式！－出错状态：%%J/j(标识符)%c[数字]", c);
					     msg_box("运行错误：", str);
					     return 0;
					   }
					}
				       { double m;  int zs, xs;
					 int code = getdnum(str, &K, &m);
					 if(code<0)
					    { char str[100];
					      sprintf(str, "发现未允许的参数形式！－出错状态：%%J/j(标识符)%c[数字]", c);
					      msg_box("运行错误：", str);
					      return 0;
					    }
					  if(c=='*')  DATA[N].data *= m;
					  else if(c=='$') DATA[N].data /= m;
					  else if(c=='+') DATA[N].data += m;
					  else if(c=='-') DATA[N].data -= m;
					  else if(c=='|'||c=='@')
					    { zs = (int)m-1;
					      xs = (int)((m-(int)m)*1000)-1;
					      if(zs<0)  zs = 0;
					      if(xs<0)  xs = 0;
					      if(zs>=titnum)
					       { char str[100];
						 sprintf(str, "发现未允许的参数形式！－出错状态：%%J/j(标识符)%c[数字]", c);
						 msg_box("错误：", str); return 0;
						}
					      if(xs>titptr[zs].sel.num)
					       { char str[100];
						 sprintf(str, "发现未允许的参数形式！－出错状态：%%J/j(标识符)%c[数字]", c);
						 msg_box("错误：", "所指定的选项号越界！"); return 0;
					       }
					      if(c=='|')
					       { if(titptr[zs].sel.text[xs].attrib&BD) DATA[N].data = 1;
						 else     DATA[N].data = 0;
					       }
					      else  if(c=='@')  DATA[N].data = titptr[zs].sel.text[xs].num;
					      else  DATA[N].data = titptr[zs].sel.num;
					    }
					}
				       K ++;  break;
			   case 'd': DATA[N].data *= 2; break;       //加倍
			   case 'b': DATA[N].data /= 2; break;       //减半
			   case 'j': DATA[N].data += 1; break;
			   case 'q': DATA[N].data -= 1; break;
			   case 'X': switch(ch=str[K++])
				      { case 'e': DATA[N].data = exp(DATA[N].data); break;  //计算e的乘方
					case 'f': DATA[N].data = fabs(DATA[N].data); break; //计算绝对值
					case 'k': if(DATA[N].data<0)
						   { msg_box("错误：", "遇到对负数开平方问题！－出错状态：%J/j(标识符)Xk"); return 0; }
						  DATA[N].data = sqrt(DATA[N].data); break; //开平方
					case 'l': if(DATA[N].data<0)
						   { msg_box("错误：", "遇到对负数求常用对数的问题！"); return 0; }
						  DATA[N].data = log10(DATA[N].data); break; //计算常用对数
					case 's': DATA[N].data = sin(DATA[N].data); break;  //计算正弦值
					case 'c': DATA[N].data = cos(DATA[N].data); break; //计算余弦值
					case 't': DATA[N].data = tan(DATA[N].data); break; //计算正切值
					case 'p': DATA[N].data = DATA[N].data*DATA[N].data; break; //计算平方
					case 'a': DATA[N].data = pow(DATA[N].data, _DATA); break;
					case 'S': if(DATA[N].data<-1||DATA[N].data>1)      //计算反正弦值
						     { msg_box("错误：", "遇到对区间[-1,+1]之外的数求反正弦问题！－出错状态：%J/j(标识符)XS");
						       return 0;
						     }
						  DATA[N].data = asin(DATA[N].data); break;
					case 'C': if(DATA[N].data<-1||DATA[N].data>1)     //计算反余弦值
						     { msg_box("错误：", "遇到对区间[-1,+1]之外的数求反正弦问题！－出错状态：%J/j(标识符)XC");
						       return 0;
						     }
						  DATA[N].data = acos(DATA[N].data); break;
					case 'T': DATA[N].data = atan(DATA[N].data); break;    //计算反正切值
					case 'E': if(DATA[N].data<0)
						   { msg_box("错误：", "遇到对负数求自然对数的问题！"); return 0; }
						  DATA[N].data = log(DATA[N].data); break;
					default:  if(ch<='9'||ch>='0')
						    DATA[N].data = pow(DATA[N].data, (double)(ch-'0'));
							     //计算0~9次幂
						  else
						    { msg_box("计算错误：", "遇到不能识别的计算控制符！－出错状态：%J/j(标识符)X...");
						      return 0;
						    } break;
				      }  break;
			   case 'n': case 'y': case 'r':               //年月日
				   { struct date d;
				     getdate(&d);
				     if(c=='n')  DATA[N].data = d.da_year;
				     else if(c=='y') DATA[N].data = d.da_mon;
				     else DATA[N].data = d.da_day;
				     break;
				   }
			   case 's': case 'f': case 'm':              //时分秒
				   { struct time t;
				     gettime(&t);
				     if(c=='s')  DATA[N].data = t.ti_hour;
				     else if(c=='f') DATA[N].data = t.ti_min;
				     else if(c=='m') DATA[N].data = t.ti_sec;
				     break;
				   }
			   case 'L': _DATA = getorder(N, 0);  break;     //数据分组正序号存入_DATA
			   case 'M': _DATA = getorder(N, 1);  break;     //数据分组负序号存入_DATA
			   case 'N': while(str[K++]!='[')
				       if(!ispacech(str[K]))
					 { msg_box("错误：", "发现未允许的参数形式！"
					 "－出错状态：%J/j(标识符)N[分数或标号]");
					   return 0;
					 }
				      f = strtonum(str+K);
				      while(str[++K]!=']')
					if(str[K]=='\0')
					  { msg_box("错误：", "发现未允许的参数形式！"
					 "－出错状态：“%J/j(标识符)N[分数或标号]");
					    return 0;
					  }
				      K ++;	j = 0;
				      for(k=0; k<=titnum; k++)
				       { int i;
					 for(i=0; i<titptr[k].sel.num; i++)
					   if((titptr[k].sel.text[i].attrib&BD) &&
					     (double)titptr[k].sel.text[i].num==(double)f)  j ++;
				       }
				      DATA[N].data += j;
				      break;
			    case 'R': DATA[N].data += titnum+1; break;  //获取当前试题个数
			    default:  msg_box("运行错误：", "发现不能识别的控制符！－"
			    "出错状态：“%”后面的命令不能被识别！");
				      return 0;
			 }
			K --;
		      }
		     break;
	    case 'U': case 'M': case 'u':
		     { int N, k, i;  K ++;
			if(_getlabel(str, &K, label)==0)
			  { char str[100];
			    sprintf(str, "发现未允许的变量标识符定义形式！"
			  "－出错状态：“%%%c”", ch);
			    msg_box("运行错误：", str);
			    return 0;
			  }
			if(ch=='u')//检测变量是否已应用
			  { for(k=0; k<dataptr; k++)
			      if(strcmp(DATA[k].dataname, label)==0) break ;
			    if(k==dataptr)  _DATA = 0;
			    else            _DATA = 1;
			    break;
			  }
			N = getdata(label);
			_DATA = DATA[N].data; K --;
			if(ch=='M') { testnum(str, &K); K--; }
		      } break;
	    case 'B': while(str[K++]!='[')
			if(ispacech(str[K]))
			  re1:{ msg_box("错误：", "发现未能识别的数字格式！－出错状态：%B[数字]");
				return 0;
			      }
		      _DATA = strtonum(str+K);
		      while(str[++K]!=']')  if(str[K]=='\0')  goto re1;
		      break;
	    default : goto  show;
	  }
	 K ++;
       }
      else if(str[K]>32 || str[K]==32&&ispaceshow==1)
       { show:
	 if(x+8>x2)
	  { x = x1;  y += 17;  high++;  CLEARSTEP }
	   if(ishow==1)  show_char(x, y, str[K++]);
	   if(iline==1)
	    { int color=getcolor();  setcolor(linecolor);
	      line(x, y+16, x+8, y+16); setcolor(color);
	    }
	   if(idot==1)
	    { setfillstyle(1, linecolor);  bar(x+3, y+14, x+5, y+16); }
	   x += 8;
       }
      else  K ++;
    }
   mouse_on();  setcolor(color);
   return  high+Hd-17;
 }

void  line_msg(char *str)
 { setfillstyle(0, 0);  bar(0, 460, 639, 479);
   setcolor(LIGHTGREEN);
   show_str(20, 462, str);
 }

int  openfile(char *filename)
 { int flag = 1;
   freeall();
   strcpy(nameptr, filename);
   fp = fopen(filename, "rb");
   if(fp==NULL)  return 0;
   readptr = NULL;
   defineorder = 0;  seekptr = (SEEK *)malloc((maxseek=50)*sizeof(SEEK));
   order = 0;  seek = (long*)malloc(20*sizeof(long));
   DATA = (DATADEF*)malloc((maxdata=50)*sizeof(DATADEF));
   directseek = (DIRECTSEEK*)malloc((maxdirect=20)*sizeof(DIRECTSEEK));
   directptr = 0;
   Flag = (FLAG*)malloc((maxflag=20)*sizeof(FLAG));  flagptr = 0;
   commandseek = (COMMANDSEEK*)malloc((maxcommand=50)*sizeof(COMMANDSEEK));
   commandptr = 0;
   Func = (FUNC*)malloc((maxfunc=10)*sizeof(FUNC));  funcptr = 0;
   readdirectseek();  readcommandseek();

   flag = getmsg();   //从文件中读取数据

   fclose(fp);  fp = NULL;
   free(seekptr);  free(seek);   free(directseek);   free(Flag); free(commandseek);
   nowselect = 0;   ifend = 0;
   unablebutton(button2);  unablebutton(button2+1);  unablebutton(button2+3);
   enablebutton(&button2[2]);
   autox(1, "XYZ");
   autoxmenu(xmenu, 3, "BB");  autoxmenu(xmenu, 3, "BC");
   iffileopen = 1;  line_msg("");  addfilelist();
   if(flag==1)  autox(3, "AB");
   return flag;
 }

#define FREESTR(x)  if((x)!=NULL) free((x)); (x)=NULL
void  freeall()
 { int k, i;
   FREESTR(PA);  FREESTR(PG);  FREESTR(PQ);  FREESTR(PU);
   FREESTR(PE);  FREESTR(PR);  FREESTR(Func);
   if(DATA != NULL)
     for(k=0; k<dataptr; k++)
       if(DATA[k].str!=NULL)  free(DATA[k].str);
   FREESTR(DATA);
   dataptr = maxdata = 0;
   unablebutton(button2+2);
   FREESTR(list.caption);
   list.attrib = 0;
   list._bar.bx1=list._bar.bx2=list._bar.by1=list._bar.by2 = 0;
   sel = 0;   unablex(3, "AB");
   ifstart = ifend = nowselect = 0;
   but_begin.caption = "点击此处开始测试";
   if(titptr==NULL)  return;
   for(k=0; k<=titnum; k++)
    { for(i=0; i<titptr[k].sel.num; i++)
       { FREESTR(titptr[k].sel.text[i].caption);
	 FREESTR(titptr[k].sel.text[i].msg);
       }
      FREESTR(titptr[k].ST);  FREESTR(titptr[k].SZ);
      FREESTR(titptr[k].sel.text);
    }
  free(titptr);  titptr = NULL;
  titnum = selnum = -1;  aTime = -1;
 }

#define  GETPS(x)  if(x!=NULL) free(x); getstr(); x=ptr
#define  SELNULL   if(selnum==-1)  { getstr(); free(ptr); break; }
#define  TITNULL   if(titnum==-1)  { getstr(); free(ptr); break; }
#define  tit       (titptr[titnum])
#define  titsel    (titptr[titnum].sel)
#define  tittext   (titptr[titnum].sel.text[selnum])
int  getmsg()
 { int c, flag=0, titN = -1, selN = -1;
   int x=gethzleft(), y=0, len, k, bselN = -1, ch;
   long seek;  int  W=0, H=20;  char label[21];
   sflag = 0;
   sethzdx(0);   ifenablepause = 1;
   dataptr = 0;
   if(fgetc(fp)=='&')  { ifjm = 1; autoxmenu(xmenu, 3, "AB"); unablexmenu(xmenu, 3, "AA"); }
   else                { ifjm = 0; autoxmenu(xmenu, 3, "AA"); unablexmenu(xmenu, 3, "AB"); }
   if(ifjm==1)
    { mm[0] = '\0';
      if(input_box("请输入密码：", mm, 8)<=0)  return -1;
      if(strlen(mm)==0)  strcpy(mm, "Skyever");
      line_msg("文件解密过程中，请稍候...");
    }
   _rewind(fp);
   while((c=Fgetc())!=EOF)
    { if(c==-2)  return -1;
      if(c=='/' && Fgetc()=='S')  titN ++;
    }
   titnum = -1;  titptr = (SEL *)malloc((titN+1)*sizeof(SEL));
   aTime = -1;   remindtime = 300;  ispacein = 1;

   _rewind(fp);
   while((c=Fgetc())!=EOF)
    { if(c=='<')                               //新选项开始
       { if(flag==0)
	  { flag = 1;  selnum ++;
	    tittext.caption = tittext.msg = NULL;
	    tittext.x = x;   tittext.y = y;   tittext.attrib = BY;
	    titsel.num = selnum+1;
	  }
       }
      else if(c=='>')  flag = 0;                //新选项结束
      else if(c=='{')  sflag = 1;
      else if(c=='}')  sflag = 0;
      else if(c=='/')
       { switch(c=Fgetc())
	  { case 'A': GETPS(PA);  break;        //标题
	    case 'G': GETPS(PG);  break;        //测试说明
	    case 'U': GETPS(PU);  break;        //评分细则
	    case 'Q': GETPS(PQ);  break;        //评价
	    case 'E': GETPS(PE);  break;        //总结
	    case 'R': GETPS(PR);  break;
	      /*********************************/
	    case '*': ch=Fgetc();
		      getstr();
		      if(!inum(ptr[0]))   k = 0;
		      else k = (int)strtonum(ptr);
		      free(ptr);
		      switch(ch)
		       { case 'x': x = LEFT+k; break;
			 case 'y': y = k; break;
			 default:  msg_box("读入错误：", "发现不能识别的控制符！－错误状态：/*...");
				   return 0;
		       }  break;
	    case 'V': if(Fgetc()=='N')  ispacein = 1;
		      else              ispacein = 0;
		      break;
	    case 'D': getstr();
		      if(!inum(ptr[0]))  remindtime = 0;
		      else remindtime = (int)strtonum(ptr)*60;
		      free(ptr);  break;
	    case 'W': getstr();                 //缺省宽度
		      if(!inum(ptr[0]))   W = 0;
		      else W = (int)strtonum(ptr);
		      free(ptr);  break;
	    case 'H': getstr();                 //缺省高度
		      if(!inum(ptr[0]))   H = 20;
		      else H = (int)strtonum(ptr);
		      free(ptr);  break;
	    case 'S': titnum ++;                    //新选题开始
		      tit.ST = tit.SZ = NULL;
		      tit.time=tit.time0 = -1;   tit.format = 0;
		      tit.passtime = 0;
		      titsel.forcolor = CYAN;    titsel.bkcolor  = BLACK;
		      titsel.attrib   = 0;       titsel.num      = 0;
		      titsel.maxselect = 1000;   tit.fs = 0;
		      x = gethzleft();  y = 0;
		      pushorder();  selN = bselN = -1;
		      while((c=Fgetc())!='}')
		       { if(c=='/') { Fgetc();  continue; }
			 if(c=='<')  selN ++;
			 else if(c=='>')  bselN ++;
			 else if(c==EOF)  break;
		       }
		      titsel.text = (SELTEXT *)malloc((selN+1)*sizeof(SELTEXT));
		      selnum = -1;      poporder();
		      if(selN!=bselN)
		       { char str[100];
			 sprintf(str, "请检查文件中第%d题的\"<>\"是否配对!", titnum+1);
			 msg_box("读入错误：", str);
			 return -1;
		       }
		      break;
	    case 'T': TITNULL		         //当前选题标题
		      GETPS(tit.ST);  break;
	    case 'f': TITNULL      		 //当前选题为复选
		      titsel.attrib = XF;
		      getstr();
		      if(ptr[0]<'0'||ptr[0]>'9')  titsel.maxselect = 1000;
		      else titsel.maxselect = (int)strtonum(ptr);
		      if(titsel.maxselect<0)  titsel.maxselect = 1000;
		      free(ptr);  break;
	    case 'd': TITNULL		        //当前选题为单选(缺省)
		      titsel.attrib = 0;
		      break;
	    case 't': if(sflag==0)              //当前套题的指定时间
		       { getstr();
			 if(ptr[0]<'0'||ptr[0]>'9')  aTime = -1;
			 else  aTime += (int)strtonum(ptr)+(aTime<0);
			 free(ptr);
		       }
		      else
		       { TITNULL               //当前选题的指定时间
			 getstr();
			 if(ptr[0]<'0'||ptr[0]>'9')  tit.time=tit.time0 = -1;
			 else  tit.time=tit.time0 += (int)strtonum(ptr)+(tit.time<0);
			 free(ptr);
		       } break;
	    case 'm': if(sflag==0)           //当前套题的指定时间
		       { getstr();
			 if(ptr[0]<'0'||ptr[0]>'9')  aTime = -1;
			 else aTime += (int)strtonum(ptr)*60+(aTime<0);
			 free(ptr);
		       }
		      else
		       { TITNULL             //当前选题的指定时间
			 getstr();
			 if(ptr[0]<'0'||ptr[0]>'9')  tit.time=tit.time0 = -1;
			 else  tit.time=tit.time0 += (int)strtonum(ptr)*60+(tit.time<0);
			 free(ptr);
		       }  break;
	    case 'e': getstr();              //指定左空距离
		      if((ptr[0]<'0'||ptr[0]>'9')&&ptr[0]!='-') sethzdx(0);
		      else  sethzdx((int)strtonum(ptr));
		      free(ptr);  break;
	    case 'r': y += H;  x = LEFT; break;    //新选项从下一行开始
	    case 'w': getstr();
		      if((ptr[0]<'0'||ptr[0]>'9')&&ptr[0]!='-')
			   x += 16;  //新选项空余象素数
		      else  x += (int)strtonum(ptr);
		      free(ptr);  break;
	    case 'h': getstr();
		      if((ptr[0]<'0'||ptr[0]>'9')&&ptr[0]!='-')
			   y += 16;  //新选项空余象素数
		      else  y += (int)strtonum(ptr);
		      free(ptr);  break;
	     /**************************************/
	    case 'b': SELNULL                //当前选项分数或标号
		      getstr();
		      tittext.num = strtonum(ptr);
		      free(ptr);   break;
	    case 's': SELNULL                //当前选项内容
		      GETPS(tittext.caption);
		      len = strlen(ptr);
		      for(k=0; k<len; k++)
		       { if(ptr[k]=='%' && ptr[k+1]=='r')
			   { ptr[k] = '\n'; ptr[k+1] = ' '; }
		       }
		      x += 20+W;
		      if(x>gethzright())
			{ y += 16;  x = gethzleft(); }
		      testhzhigh(x, y, ptr);
		      x = gethzopx();  y = gethzopy();
		      break;
	    case 'z': if(flag==1)
		       { if(selnum==-1)  break;           //当前选项注解
			 getstr();
			 if(tittext.msg)   free(tittext.msg);
			 tittext.msg = ptr; break;
		       }
		      if(titnum==-1)  break;        //当前选题注解
		      getstr();   if(tit.SZ)  free(tit.SZ);
		      tit.SZ = ptr;  break;
	    case 'u': ifenablepause = 0;  break;   //禁止暂停
	    case '~': if(getlabel(label)==0)  return 0;
		      for(k=0; k<funcptr; k++)
			if(strcmp(label, Func[k].funcname)==0)
			  { char str[100];
			    sprintf(str, "发现有重复的过程定义：%s", label);
			    msg_box("错误：", str);
			    return 0;
			  }
		      if(funcptr>=maxfunc)
			Func = (FUNC*)realloc(Func, (maxfunc=maxfunc+10)*sizeof(FUNC));
		      strcpy(Func[funcptr].funcname, label);
		      getstr();
		      Func[funcptr++].str = ptr;
		      break;
	  }
       }
    }
   return 1;
 }

int  getstr()
 { int  len=0, k, c, flag=0;
   pushorder();
   while((c=Fgetc())!=EOF)
    {      if(c=='^'||c=='/')  break;
      else if(c==' ')
	    { if(ispacein==0)  continue;
	      else len ++;
	    }
      else if(c>=0&&c<32) continue;
      else if(c=='%')
       { c = Fgetc();	 len += 2;
	 if(c=='[')  flag = 1;
	 if(c==']')  flag = 0;
       }
      else if(c=='[')  { flag = 1;  len ++; }
      else if(c==']')  { flag = 0;  len ++; }
      else if(c=='<'||c=='>')
	{ if(flag==1)  len ++;
	  else  break;
	}
      else len ++;
    }
   poporder();
   ptr = (char *)malloc(len+1);
   if(ptr==NULL)
    { closegraph();  fcloseall();      freeall();
      printf("Sorry: No Enough Memory!\n");
      Exit(0);
    }
   for(k=0; k<len; k++)
    { while((c=Fgetc())>=0&&c<32 || c==32&&ispacein==0);
      ptr[k] = c;
    }
   ptr[k] = '\0';
   return  len;
 }

double  strtonum(char *str)
 { int K=-1, f=0, n=-1, d=-1, k;
   double  N = 0;
   while(ispacech(str[++K]));
	if(str[K]=='-')  { f = 1; K++; }
   else if(str[K]=='+')  { f = 0; K++; }
   else if(str[K]>'9'||str[K]<'0')  return str[K];
   while(str[K]<='9' && str[K]>='0' || str[K]=='.')
     { if(str[K]=='.')
	 { if(d>=0)
	    { char _str[100];  sprintf(_str, "遇见非法数字形式“%d”!", str);
	      msg_box("错误：", _str);  return 0;
	    }
	   d = 0;
	 }
       else  N = N*10 + (str[K]-'0');
       K ++;   if(d>=0)   n ++;
     }
   if(f==1)   N = -N;
   if(n!=-1)
     for(k=0; k<n; k++)  N /= 10;
   return N;
 }

int  showtit(int num)
 { int  high, k;
   reset_qk();
   if(num>titnum)  return 0;
   setcolor(LIGHTCYAN);
   if(ifend==1)  ifenterpause = 0;
   high = write_str(LEFT, TOP, RIGHT, BOTTOM, titptr[num].ST);
   ifenterpause = 1;
   Y = TOP + high + 25;
   renewsel(num);
   show_select(&titptr[num].sel);
   if(ifend==1 && titptr[num].SZ!=NULL)
    { setlinestyle(0, 0, 3);  setcolor(LIGHTBLUE);
      line(LEFT-7, gethzopy()+23, LEFT+100, gethzopy()+23);
      setcolor(LIGHTGRAY);
      write_str(LEFT, gethzopy()+30, RIGHT, BOTTOM, titptr[num].SZ);
      setlinestyle(0, 0, 1);
    }
   renew.y1 = gethzopy()+30;  renew.y2 = gethzopy()+50;
   complete.y1 = gethzopy()+30; complete.y2 = gethzopy()+50;
   allend.y1 = gethzopy()+30; allend.y2 = gethzopy()+50;
   if(num==titnum) { complete.attrib &= ~BY; allend.attrib |= BY; }
   else            { complete.attrib |=  BY; allend.attrib &= ~BY; }
   if(ifstart==1)
     { if(titptr[num].time)  renew.attrib|=BY;
       else   renew.attrib&=~BY;
       showbutton(&renew);  showbutton(&complete);
     }
   if(num==titnum&&ifstart==1)
    { allend.caption = aTime>0?"提前交卷":"现在交卷";
      showbutton(&allend);
    }
   ifcontinue = 1;
   replacexmenu(xmenu, 0, "E", "E暂停答题|");
   return 1;
 }

void  top_title(char *S)
 { int color = getcolor();
   int len = strlen(S)*8;
   setfillstyle(0, 0);
   bar(320-len/2-10, 80-12+10, 320+len/2+10, 80+12+10);
   setcolor(YELLOW);
   mouse_off();
   setfillstyle(1, DARKGRAY);
   bar(320-len/2-8+3, 80-12+8+3, 320+len/2+8+3, 80+12+8+3);
   setfillstyle(1, LIGHTBLUE);
   bar(320-len/2-8, 80-12+8, 320+len/2+8, 80+12+8);
   setlinestyle(0, 0, 1);
   setcolor(YELLOW);
   rectangle(320-len/2-8+2, 80-12+8+2, 320+len/2+8-2, 80+12+8-2);
   setcolor(WHITE);
   show_str(320-len/2, 80, S);
   mouse_on();
   setcolor(color);
 }

void  showPG()
 { int k;
   reset_qk();
   cleardesktop(_BOT);
   top_title("本题测试说明");
   setcolor(LIGHTCYAN);
   write_str(LEFT, TOP, RIGHT, BOTTOM, PG?PG:"（本套测试题无特别说明！）");
   if(ifend==0)
    { but_begin.attrib |= BY;
      _showbutton(&but_begin, 2);  //showbutton(&but_begin);
    }
   else but_begin.attrib &= ~BY;
   if(ifopen==0)
    { addxmenu(xmenu, 0, "X<",
	"S开始答题|^E暂停答题|L关闭该套试题|K查看测试说明|-1|");
      ifopen = 1;
    }
   for(k=0; k<4; k++)    unablebutton(button2+k);
   unablex(0, "Klnm");
   if(ifend==1)
     for(k=0; k<4; k++)
       { but_end[k].attrib &= ~BD;
	 showbutton(but_end+k);
       }
 }

void  showPQ()
 { int k;
   reset_qk();
   if(PQ==NULL)  return;
   enablebutton(button2+2);  unablebutton(button2+3);
   unablebutton(button2+1);  unablebutton(button2);
   autoxmenu(xmenu, 0, "K");
   if(but_end[0].attrib & BD)
    { cleardesktop(_BOT);
      top_title("对您的评价");
      setcolor(LIGHTCYAN);
      write_str(LEFT, TOP, RIGHT, BOTTOM, PQ);
      for(k=0; k<4; k++)  but_end[k].attrib &= ~BD;
      but_end[0].attrib |= BD;
      for(k=0; k<4; k++)  showbutton(but_end+k);
    }
   else   show_end();
 }

void  showPP()
 { int k;
   reset_qk();
   if(PU==NULL)  return;
   enablebutton(button2+2);  unablebutton(button2+3);
   unablebutton(button2+1);  unablebutton(button2);
   autoxmenu(xmenu, 0, "K");
   if(but_end[1].attrib & BD)
    { cleardesktop(_BOT);
      top_title("详细评分细则");
      setcolor(LIGHTCYAN);
      write_str(LEFT, TOP, RIGHT, BOTTOM, PU);
      for(k=0; k<4; k++)  but_end[k].attrib &= ~BD;
      but_end[1].attrib |= BD;
      for(k=0; k<4; k++)  showbutton(but_end+k);
    }
   else  show_end();
 }

void  showPE()
 { int k;
   reset_qk();
   if(PE==NULL)  return;
   enablebutton(button2+2); unablebutton(button2+3);
   unablebutton(button2+1);  unablebutton(button2);
   autoxmenu(xmenu, 0, "K");
   if(but_end[2].attrib & BD)
    { cleardesktop(_BOT);
      top_title("大总结");
      setcolor(LIGHTCYAN);
      for(k=0; k<4; k++)  but_end[k].attrib &= ~BD;
      but_end[2].attrib |= BD;
      for(k=0; k<4; k++)  showbutton(but_end+k);
      write_str(LEFT, TOP, RIGHT, BOTTOM, PE);
      showbutton(but_end+2);
    }
   else  show_end();
 }

void  renewsel(num)
 { int k;
   if(titptr[num].format==1)  return;
   for(k=0; k<titptr[num].sel.num; k++)
      titptr[num].sel.text[k].y += Y;
   titptr[num].format = 1;
 }
void  resetsel(num)
 { int k;
   if(titptr[num].format==0)  return;
   for(k=0; k<titptr[num].sel.num; k++)
      titptr[num].sel.text[k].y -= Y;
   titptr[num].format = 0;
 }

void  delx(int num, char *S)
 { char s[2] = "0\0";
   int  K = 0;
   while(S[K])
    { s[0] = S[K++];
      delxmenu(xmenu, num, s);
    }
 }

void  hidex(int num, char *S)
 { char s[2] = "0\0";
   int  K = 0;
   while(S[K])
    { s[0] = S[K++];
      hidexmenu(xmenu, num, s);
    }
 }

void  autox(int num, char *S)
 { char s[2] = "0\0";
   int  K = 0;
   while(S[K])
    { s[0] = S[K++];
      autoxmenu(xmenu, num, s);
    }
 }

void  unablex(int num, char *S)
 { char s[2] = "0\0";
   int  K = 0;
   while(S[K])
    { s[0] = S[K++];
      unablexmenu(xmenu, num, s);
    }
 }

void  cleardesktop(int d)
 { list.attrib = 0;
   list._bar.bx1=list._bar.bx2=list._bar.by1=list._bar.by2 = 0;
   setfillstyle(0, 0);
   mouse_off();
   bar(15+2, 65+2, 624-2, 453-d);
   mouse_on();
   write_hb();
 }

void  unablebutton(BUTTON *button)
 { button->attrib &= ~BY;
   showbutton(button);
 }

void  enablebutton(BUTTON *button)
 { button->attrib |= BY;
   showbutton(button);
 }
#define H  (*K)
void  testsel(char *str, int *K)
 { int len=0, num=0, k, i, j, N;
   char selstr[20], op='!';
BEGIN
   while(str[H]!='\0')
    { if(str[H]=='%')
	if(str[++H]=='[')  break;
	else H ++;
      else if(str[H]=='}')  return;
      else H ++;
    }
   H ++;
   while(str[H]!=']')
    { if(str[H]<='9'&&str[H]>='0' || str[H]=='-')
       { num = (int)strtonum(str+H);
	 while(str[H]<='9'&&str[H]>='0')  H ++;
       }
      else if(str[H]>='A'&&str[H]<='Z' || str[H]>='a'&&str[H]<='z')
       { if(len<20)  selstr[len++] = str[H++]; }
      else if(str[H]=='=' || str[H]=='!')
	op = str[H++];
      else if(str[H]=='<')
       { if(str[H+1]=='=')  { op = '(';  H += 2; }
	 else               { op = '<';  H ++; }
       }
      else if(str[H]=='>')
       { if(str[H+1]=='=')  { op = ')';  H += 2; }
	 else               { op = '>';  H ++; }
       }
      else if(str[H]=='\0')  return;
      else if(str[H]=='/')   H += 2;
      else if(str[H]=='[')
	{ while(str[H]!='\0')  H ++;  return; }
      else if(str[H]=='}')  { H ++;  return; }
      else H++;
    }
   H ++;  N = 0;
   if(len==0)  return;
   for(k=0; k<len; k++)
    { for(j=0; j<=titnum; j++)
       { for(i=0; i<titptr[j].sel.num; i++)
	   if((titptr[j].sel.text[i].attrib&BD) &&
	       (titptr[j].sel.text[i].num==selstr[k]) )
	     N ++;
       }
    }
   switch(op)
    { case '=': if(N==num)  return; break;
      case '>': if(N> num)  return; break;
      case '<': if(N< num)  return; break;
      case ')': if(N>=num)  return; break;
      case '(': if(N<=num)  return; break;
      case '!': if(N!=num)  return; break;
    }
   len = num = N = 0;  op = '!';
WEND
 }

void  testnum(char *str, int *K)
 { double  left, right, N=_DATA;
   char leftflag = 0, rightflag = 0, flag = 0;
BEGIN
   while(str[H]!='\0')
    { if(str[H]=='%')
	if(str[++H]=='[')  break;
	else H ++;
      else if(str[H]=='}')  return;
      else H ++;
    }
   while(ispacech(str[++H]));
   while(str[H]!=']')
    { if(str[H]<='9'&&str[H]>='0' || str[H]=='-')
       { if(flag==0)
	  { left = strtonum(str+H);  leftflag = 1;   }
	 else
	  { right = strtonum(str+H);  rightflag = 1; }
	 while(str[H]<='9'&&str[H]>='0')  H ++;
       }
      else if(str[H]=='\0')  return;
      else if(str[H]=='/')   H += 2;
      else if(str[H]=='[')
	{ while(str[H]!='\0')  H ++;  return; }
      else if(str[H]=='}')  { H ++;  return; }
      else if(str[H]==' ')  H ++;
      else  { H++; flag = 1; }
    }
   H ++;
   if(flag==0 && leftflag==1)
     { if(N==left)  return; }
   else if( (!leftflag||N>=left) && (!rightflag||N<=right) )  return;
   leftflag = rightflag = flag = 0;
WEND
 }
#undef  H

void  asktit()
 { int k;
   if(list.caption)  { free(list.caption);  list.caption = NULL; }
   if(but_end[3].attrib&BD)
    { for(k=0; k<4; k++)  but_end[k].attrib &= ~BD;
      but_end[3].attrib |= BD;
      for(k=0; k<4; k++)  showbutton(but_end+k);
      show_asktit();
      enablebutton(button2+3);
    }
   else  show_end();
 }

void  show_asktit()
 { char  str[30];
   top_title("查询答题情况");
   test_buttonenable();
   setfillstyle(0, 0);
   bar(LEFT-10, TOP-20, RIGHT, BOTTOM);
   bar(30, 80, 30+20*8, 80+16);
   sprintf(str, "第%d题(共%d题)", nowselect+1, titnum+1);
   setcolor(YELLOW);
   mouse_off();
   bar(450, 80, 620, 96);
   show_str(30, 80, str);
   show_str(450, 80, "约定时间:");
   setcolor(LIGHTGREEN);
   if(titptr[nowselect].time0<0)
     show_str(450+75, 80, "无限制");
   else
     { sprintf(str, "%d分%d秒",
       titptr[nowselect].time0/60, titptr[nowselect].time0%60);
       show_str(450+75, 80, str);
     }
   bar(450, 100, 620, 116);
   show_str(450, 100, "答题时间:");
   setcolor(GREEN);
   sprintf(str, "%d分%d秒",
      titptr[nowselect].passtime/60, titptr[nowselect].passtime%60);
   show_str(450+75, 100, str);
   mouse_on();
   showtit(nowselect);
   enablebutton(button2+2);  autox(0, "K");
 }

void  show_about()
  { mouse_off();
    setcolor(LIGHTGREEN);
    show_str(320-30*8/2, 230, "编程者：徐友春(济南大学材料系)");
    setcolor(LIGHTCYAN);
    show_str(320-16*8/2, 250, "天恒软件开发中心");
    setcolor(LIGHTGREEN);
    show_str(320-26*8/2, 270, "二○○一年十二月六日于济南");
    setcolor(YELLOW);
    show_str(320-11*8/2, 290, "版本号：1.0");
    settextstyle(1, 0, 5);  settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(RED);
    outtextxy(320, 180, "Skyever");
    setcolor(YELLOW);
    outtextxy(322, 182, "Skyever");
    mouse_on();
  }

void  show_end()
 { int k;
   cleardesktop(_BOT);
   for(k=0; k<4; k++)  showbutton(but_end+k);
   show_about();
   top_title("答题完毕!");
   setcolor(WHITE);
   mouse_off();
   show_str(320-28*8/2, 340, "请从下面选择您想要了解的内容");
   mouse_on();
   list.attrib = 0;
   list._bar.bx1=list._bar.bx2=list._bar.by1=list._bar.by2 = 0;
   unablebutton(button2); unablebutton(button2+1);  unablebutton(button2+3);
   autoxmenu(xmenu, 3, "BA");  autoxmenu(xmenu, 3, "BD");
   autoxmenu(xmenu, 3, "BC");  autoxmenu(xmenu, 3, "BB");
   reset_qk();
 }


void  selectfortitle()
 { int LEN = 0, high;
   if(titnum<0)  return;
   if(list.caption==NULL)
    { int k;
      list.caption = (char *)malloc(1);
      list.caption[0] = '\0';
      for(k=0; k<=titnum; k++)
       { char *p = titptr[k].ST;
	 int  len = strlen(p), i;
	 if(len>20)  len = 20;
	 list.caption = (char*)realloc(list.caption, LEN+len+10);
	 if(LEN!=0)  list.caption[LEN++] = '|';
	 for(i=0; i<len; i++)
	   list.caption[LEN++] = p[i];
	 list.caption[LEN] = '\0';
	 list.caption = (char*)realloc(list.caption, LEN+1);
       }
    }
   list.attrib = BY;
   high = 16*(titnum+1>16?16:titnum+1);
   list.y1 = (480-high)/2 + 30 ;
   list.y2 = high + list.y1 + 5;
   setfillstyle(1, DARKGRAY);
   bar(list.x1-10, list.y1-10-15, list.x2+10, list.y2+10);
   setfillstyle(1, LIGHTGRAY);
   bar(list.x1-5, list.y1-5-15, list.x2+5, list.y2+5);
   setcolor(BLACK);
   show_str(list.x1+5, list.y1-18, " 请您选择：");
   showlist(&list);
 }
#define  titp    (titptr[nowselect].sel.text)
void  asktest()
 { int k, code;   char *ptr;  int _maxselect;
   if(list.attrib&BY)  return;
   if(!(but_end[3].attrib&BD))  return;
   _maxselect = titptr[nowselect].sel.maxselect;
   titptr[nowselect].sel.maxselect = 1000;
   ptr = (char *)malloc(titptr[nowselect].sel.num);
   for(k=0; k<titptr[nowselect].sel.num; k++)
      ptr[k] = titptr[nowselect].sel.text[k].attrib;
   if(!ixmenuopen(xmenu) && (code=select_click(&titptr[nowselect].sel))>=0)
     { if(titp[code].msg)
	 { showsound(2); msg_box("该选项标注：", titp[code].msg); }
       else showsound(3);
       for(k=0; k<titptr[nowselect].sel.num; k++)
	{ if((ptr[k]&BD) != (titptr[nowselect].sel.text[k].attrib&BD))
	    { titptr[nowselect].sel.text[k].attrib = ptr[k];
	      test_select( &titptr[nowselect].sel, k );
	    }
	}
     }
   free(ptr);  titptr[nowselect].sel.maxselect = _maxselect;
 }

void  showsound(int code)
 { if(ifsound==0)  return;
   switch(code)
    { case 0: sound(800);  Delay(2); break;
      case 1: sound(1500); Delay(4); break;
      case 2: sound(600);  Delay(1); sound(800);
	      Delay(2); sound(700); Delay(1); break;
      case 3: sound(400);  Delay(3); break;
      case 4: sound(1000); Delay(5); break;
    }
   nosound();
 }

static void Delay(int T)
 { long T0 = clock();
   while(clock()-T0<(long)T);
 }

int  readinmenu(char *filename)
 { int k, c, dir=0, len = 0;
   char menustr[10] = "@", bifjm=ifjm;
   char str[50];  ifjm = 0;
   fp = fopen(filename, "rb");
   if(fp==NULL)  return 0;
   ispacein = 0;  readptr=NULL;
   if(menuptr)  free(menuptr);
   while((c=Fgetc())!=EOF)
    { if(c!='/')  continue;
      if(Fgetc()!='F')  continue;
      getstr(); len += strlen(ptr)+10; free(ptr);
    }
   menuptr = (char *)malloc(len);  menuptr[0] = '\0';
   _rewind(fp);    delx(1, "ABCDEFGHIJKLMN@");
   addxmenu(xmenu, 1, "1<", "^@(无任何测试题)|");
   while((c=Fgetc())!=EOF)
    { if(c=='%')  { Fgetc();  continue; }
      if(c=='/')
       { switch(c=Fgetc())
	  { case '%': Fgetc(); break;
	    case 'T': GETPS(TITLE);  break;
	    case 'X': getstr();
		      if(strlen(ptr)>40)  ptr[40] = '\0';
		      strcpy(str, menustr+dir);
		      str[0] ++;     strcat(str, ptr);
		      free(ptr);     strcat(str, "|");
		      strcat(menustr, ">");
		      addxmenu(xmenu, 1, menustr, str);
		      menustr[dir+1] = '\0';
		      if(menustr[dir]=='@')  delxmenu(xmenu, 1, menustr);
		      menustr[dir] ++;
		      break;
	    case '>': addxmenudir(xmenu, 1, menustr);
		      strcat(menustr, ">");
		      addinxmenu(xmenu, 1, menustr, "^@(无任何测试题)|");
		      menustr[++dir] = '@';
		      menustr[dir+1] = '\0';
		      break;
	    case '<': menustr[dir--] = '\0';  break;
	    case 'F': getstr();
		      if(strlen(ptr)>45)  ptr[45] = '\0';
		      len = strlen(menuptr);
		      sprintf(menuptr+len, "[%s]%s", menustr, ptr);
		      free(ptr);   break;
	    case '!': goto  end;
	  }
       }
    }
   end: menuptr = (char *)realloc(menuptr, strlen(menuptr)+1);
   fclose(fp);  fp = NULL;  ifjm = bifjm;
   ispacein = 1;
   unablexmenu(xmenu, 3, "E");
   autoxmenu(xmenu, 3, "D");
   return 1;
 }

int  readinhelp(char *filename)
 { int k, c, dir=0, len = 0;
   char helpstr[10] = "@", bifjm=ifjm;
   char str[50];  ifjm = 0;
   fp = fopen(filename, "rb");
   if(fp==NULL)  return 0;
   ispacein = 0;  readptr=NULL;
   if(helpptr)  free(helpptr);
   while((c=Fgetc())!=EOF)
    { if(c!='/')  continue;
      if(Fgetc()!='L')  continue;
      getstr(); len += strlen(ptr)+10; free(ptr);
    }
   helpptr = (char *)malloc(len);  helpptr[0] = '\0';
   _rewind(fp);
   while((c=Fgetc())!=EOF)
    { if(c=='%')  { Fgetc(); continue; }
      if(c=='/')
       { switch(c=Fgetc())
	  { case 'X': getstr();
		      if(strlen(ptr)>40)  ptr[40] = '\0';
		      strcpy(str, helpstr+dir);
		      str[0] ++;     strcat(str, ptr);
		      free(ptr);     strcat(str, "|");
		      strcat(helpstr, ">");
		      addxmenu(xmenu, 2, helpstr, str);
		      helpstr[dir+1] = '\0';
		      if(helpstr[dir]=='@')  delxmenu(xmenu, 2, helpstr);
		      helpstr[dir] ++;
		      break;
	    case '>': addxmenudir(xmenu, 2, helpstr);
		      strcat(helpstr, ">");
		      addinxmenu(xmenu, 2, helpstr, "^@(无任何帮助内容)|");
		      helpstr[++dir] = '@';
		      helpstr[dir+1] = '\0';
		      break;
	    case '<': helpstr[dir--] = '\0';  break;
	    case 'L': getstr();
		      if(strlen(ptr)>45)  ptr[45] = '\0';
		      len = strlen(helpptr);
		      sprintf(helpptr+len, "[%s]%s", helpstr, ptr);
		      free(ptr);  break;
	    case '!': goto  end;
	  }
       }
    }
   end: helpptr = (char *)realloc(helpptr, strlen(helpptr)+1);
   fclose(fp);  fp = NULL;  ifjm = bifjm;
   ispacein = 1;
   return 1;
 }



int  getlabelname(char *mptr, char *f, char *str)
 { int K=0, k; char sname[10];
   Order = -1;
   while(mptr[K])
    { if(mptr[K]=='[')
	{ k = 0;  Order ++;
	  while(mptr[++K]!=']')
	    { if(mptr[K]=='\0')  return 0;
	      sname[k++] = mptr[K];
	      if(k>9)  return 0;
	    }
	  sname[k] = '\0';
	  if(strcmp(sname, f)!=0)  { K ++; continue; }
	  k = 0;
	  while(mptr[++K]!='[')
	   { if(mptr[K]=='\0')  break;
	     str[k++] = mptr[K];
	     if(k>45)  return 0;
	   }
	  str[k] = '\0';  return 1;
	}
       else K++;
    }
   return 0;
 }

int  _getfilename(int order, char *str)
 { int K=0, k; char sname[10], n=0;
   while(menuptr[K])
    { if(menuptr[K]==']')
	if(n++!=order)  { K ++; continue; }
	else
	 { k = 0;
	   while(menuptr[++K]!='[')
	    { if(menuptr[K]=='\0')  break;
	      str[k++] = menuptr[K];
	      if(k>45)  return 0;
	    }
	   str[k] = '\0';   return 1;
	 }
      K++;
    }
   return 0;
 }

void  setreadptr(char *S)
 { readptr = S; readflag = 0; }

int  Fgetc()
 { int   flag, k;   char  label[21];
   int   ch;        char  str[100];
   if(readptr!=NULL)
    { if(readptr[readflag]!='\0')      ch = readptr[readflag++];
      else  { readptr = NULL; ch = _fgetc(fp); }
    }
   else ch = _fgetc(fp);
   if(ch==EOF)  return  EOF;
   else if(ch=='#')
    { switch(ch=_fgetc(fp))
       { case '~': if(getlabel(label)==0)  return -2;
		   for(k=0; k<defineorder; k++)
		     if(strcmp(seekptr[k].definename, label)==0)  goto jump;
		   while((ch=_fgetc(fp))!='{')
		     if(!ispacech(ch))
		       { char str[100];
			 sprintf(str, "请检查宏“%s”的格式是否正确！", label);
			 msg_box("宏应用出错：", str);
			 return  -2;
		       }
		   if(k>=maxseek)
		     seekptr = (SEEK*)realloc(seekptr, (maxseek=maxseek+50)*sizeof(SEEK));
		   strcpy(seekptr[defineorder].definename, label);
		   seekptr[defineorder++].seek = ftell(fp);
	 case '{': jump:
		   while((ch=_fgetc(fp))!=EOF)
		     if(ch=='#'&&_fgetc(fp)=='}')  break;
		   if(ch==EOF)  return EOF;
		   return  Fgetc();
	 case '!': return  EOF;
	 case '*': while((ch=_fgetc(fp))!='\n' && ch!=EOF);  return Fgetc();
	 case '}': if(--order<0)
		     { msg_box("宏定义错误：",
			"请检查“#(label){”和“}”是否配对！");
		       return -2;
		     }
		   fseek(fp, seek[order], SEEK_SET);
		   return  Fgetc();
	 case '#': return  '#';
	 default:  fseek(fp, -1, SEEK_CUR);
		   if(getlabel(label)==0)  return -2;
		   if(order>19)
		    { msg_box("宏嵌套溢出",
			"宏的嵌套次数不得超过20层，请检查是否有宏递归嵌套的错误！");
			return -2;
		     }
		   seek[order++] = ftell(fp);
		   if(findlabel(label)==0)  return -2;
		   return  Fgetc();
       }
    }
   else if(ch=='{')  sflag++;
   else if(ch=='}')  sflag--;
   return ch;
 }

int  getlabel(char *label)
 { int k = 0, c, khflag = 0, writeflag = 0;
   while((c=_fgetc(fp))!=EOF)
    { switch(c)
       { case ' ': case 9: case '\n': case '\r': case '}': case '{':
		   case '<': case '>':
		   if(writeflag==1)
		     { label[k] = '\0'; fseek(fp,-1,SEEK_CUR); return 1; }
		   break;
	 case '(': if(writeflag==1)
		    { fseek(fp,-1,SEEK_CUR); label[k]='\0'; return 1; }
		   khflag = 1; break;
	 case ')': if(khflag==0) goto end; label[k] = '\0'; return 1;
	 case '%': case '/': case '#': if(writeflag==0)  goto end;
		   fseek(fp, -1, SEEK_CUR); label[k]='\0'; return 1;
	 default:  label[k++] = (char)c; writeflag = 1;
       }
    }
   end:
   msg_box("宏名或变量名或标签名错误：","发现未允许的宏名或变量名或文件名或标签名形式，"
    "请仔细检查文件书写是否正确！");
   return 0;
 }

int  findlabel(char *label)
 { int c, i, k;  long  seek, seek0 = ftell(fp);
   for(k=0; k<commandptr; k++)
    if(strcmp(commandseek[k].definename, label)==0)
      { setreadptr(commandseek[k].definestr); return 1; }
   for(k=0; k<directptr; k++)
    if(strcmp(directseek[k].definename, label)==0)
      { setreadptr(directseek[k].definestr); return 1; }
   for(k=0; k<defineorder; k++)
    { if(strcmp(seekptr[k].definename, label)==0)
       { fseek(fp, seekptr[k].seek, SEEK_SET);
	 return 1;
       }
    }
   while((c=_fgetc(fp))!=EOF)
    { if(c=='%')  { _fgetc(fp);  continue; }
      if(c!='#')  continue;
      if(_fgetc(fp)!='~')  continue;
      seek = ftell(fp);
      while((c=_fgetc(fp))!=EOF)   if(c!=' '&&c!='\n'&&c!=9)  break;
      if(c!='(')  { fseek(fp, seek, SEEK_SET);  continue; }
      while((c=_fgetc(fp))==' ' || c=='\n' || c==9);
      fseek(fp, -1, SEEK_CUR);
      k = -1;
      while(label[++k] != '\0')  if((char)_fgetc(fp)!=label[k])  break;
      if(label[k]=='\0' && ((c=_fgetc(fp))==' '||c==')'||c=='\n'||c==9))
	while((c=_fgetc(fp))!=EOF)
	  if(c=='{')
	    { if(defineorder>=maxseek)
		seekptr = (SEEK*)realloc(seekptr, (maxseek=maxseek+50)*sizeof(SEEK));
	      strcpy(seekptr[defineorder].definename, label);
	      seekptr[defineorder++].seek = ftell(fp);
	      return 1;
	    }
      fseek(fp, seek, SEEK_SET);
    }
   { char str[100];
     sprintf(str, "未发现宏名或变量“%s”！  『错误行：%d』", label, getline(seek0));
     msg_box("宏名或变量名错误：", str);
   }
   return 0;
 }

int  getline(long seek)
 { long  k;
   int  linenum = 0;
   _rewind(fp);
   for(k=0; k<seek; k++)
     if(_fgetc(fp)=='\n')  linenum ++;
   return linenum+1;
 }

char  border;
long  _seek;
char  *bkstr;
int   bkreadflag;
void  pushorder()
 { border = order;
   _seek = ftell(fp);
   bkstr = readptr;
   bkreadflag = readflag;
 }

void  poporder()
 { order = border;
   fseek(fp, _seek, SEEK_SET);
   readptr = bkstr;
   readflag = bkreadflag;
 }

void  test_buttonenable()
 { if(titnum>0)
    { if(nowselect>0)
	{ enablebutton(button2);  autoxmenu(xmenu, 0, "l"); }
      else
	{ nowselect=0; unablebutton(button2); unablexmenu(xmenu, 0, "l"); }
      if(nowselect<titnum)
	{ enablebutton(button2+1); autoxmenu(xmenu, 0, "n"); }
      else
	{ nowselect=titnum; unablebutton(button2+1); unablexmenu(xmenu, 0, "n"); }
      autoxmenu(xmenu, 0, "m");
    }
   else  { unablexmenu(xmenu, 0, "m");  return; }
 }

int  getdata(char *str)
 { int k;
   for(k=0; k<dataptr; k++)
    if(strcmp(DATA[k].dataname, str)==0)  return k;
   if(k>=maxdata)
     DATA = (DATADEF*)realloc(DATA, (maxdata=maxdata+50)*sizeof(DATADEF));
   strcpy(DATA[k].dataname, str);
   DATA[k].data = 0;  DATA[k].attrib = 0;  DATA[k].str = NULL;
   return dataptr++;
 }

int  _getlabel(char *str, int *K, char *label)
 { int k = 0, khflag = 0, writeflag = 0;
   char c;
   while((c=str[(*K)++])!='\0')
    { switch(c)
       { case ' ': case 9: case '\n': case '\r': case '}': case '{':
		   if(writeflag==1)
		     { label[k] = '\0'; (*K)--; return 1; }
		   break;
	 case '(': if(writeflag==1)
		    { (*K)--; label[k]='\0'; return 1; }
		   khflag = 1; break;
	 case ')': if(khflag==0) goto end; label[k] = '\0'; return 1;
	 case '%': case '/': case '#': if(writeflag==0)  goto end;
		   (*K)--; label[k]='\0'; return 1;
	 default:  label[k++] = (char)c; writeflag = 1;
       }
    }
   end:
   msg_box("宏名或变量名错误：","发现未允许的宏名或变量名或文件名形式，"
    "请仔细检查文件书写是否正确！");
   return 0;
 }

int  getdnum(char *str, int *K, double *m)
 { while(str[*K])
    { if(inum(str[*K]))  break;
      if(str[(*K)++]==']')   return -2;
    }
   if(str[*K]==0)  return -1;
   *m = strtonum(str+(*K));
   while(str[(*K)])
    { if((str[*K]<'0'||str[*K]>'9') && str[*K]!=' ' && str[*K]!='.'
	      && str[*K]!=9 && str[*K]!='\n')  break;
      (*K) ++;
    }
   return str[*K];
 }

int  getorder(int N, int model)
 { int k, order=0;
   double d = DATA[N].data;
   if(DATA[N].attrib==0)  return 0;
   for(k=0; k<dataptr; k++)
    { if(DATA[N].attrib==0)  continue;
      if(k==N)  continue;
      if(model==0)
       { if(DATA[N].data>d)  order++; }
      else
       { if(DATA[N].data<d)  order++; }
    }
   return order+1;
 }

char *gettimestr(char *str)
 { struct time t;
   gettime(&t);
   sprintf(str, "%d:%d:%d", t.ti_hour, t.ti_min, t.ti_sec);
   return str;
 }

char *getdatestr(char *str)
 { struct date d;
   getdate(&d);
   sprintf(str, "%d.%d.%d", d.da_year, d.da_mon, d.da_day);
   return str;
 }

int  saverel(int code)   //保存答题结果
 { char filename[50]="";  int rel[12], k;
   FILE  *fp;
   if(code==0)
     { if(input_box("请输入保存答案的文件：", filename, 45)==0) return 0; }
   else
     { int len = strlen(nameptr);
       strcpy(filename, nameptr);
       for(k=0; k<len; k++)
	 if(filename[k]=='.')  break;
       if(k==len)  strcat(filename, ".rel");
       else  strcpy(filename+k, ".rel");
     }
   fp = fopen(filename, "rt");
   if(fp!=NULL)
     { char str[100];      fclose(fp);
       sprintf(str, "文件%s已经存在，要替换吗？", filename);
       if(ask_box("发现重名文件", str)!=1)  return 0;
     }
   fp = fopen(filename, "wt");
   if(fp==NULL)
    { char  str[100];  sprintf(str, "无法建立文件%s！", filename);
      msg_box("建立文件错误：", str);
      return 0;
    }
   fprintf(fp, "%s\n", PA==NULL?"null":PA);
   for(k=0; k<=titnum; k++)
    { int i;
      for(i=0; i<12; i++)  rel[i] = 0;
      for(i=0; i<titptr[k].sel.num; i++)
	if(titptr[k].sel.text[i].attrib & BD)
	  rel[i/16] |= (int)0x01<<(i%16);
      for(i=0; i<12; i++) fprintf(fp, "%d ", rel[i]);
      fputc('\n', fp);
    }
   fprintf(fp, "The End of File %s", nameptr);
   fclose(fp);  msg_box("保存完毕：", "您以后可以通过该文件载入该答案！");
   return 1;
 }

int  userel(int code)   //载入答题结果
 { char filename[50]="";  int rel[12], k;
   FILE  *fp;
   if(code==0)
     { if(input_box("请输入载入答案的文件：", filename, 45)<=0) return 0; }
   else
     { int len = strlen(nameptr);
       strcpy(filename, nameptr);
       for(k=0; k<len; k++)
	 if(filename[k]=='.')  break;
       if(k==len)  strcat(filename, ".rel");
       else  strcpy(filename+k, ".rel");
     }
   if(strlen(filename)==0)  return 0;
   fp = fopen(filename, "rt");
   if(fp==NULL)
    { char  str[100];
      if(code==0) sprintf(str, "无法打开文件%s！", filename);
      else        sprintf(str, "该套试题的标准答案未发现！");
      msg_box("打开文件错误：", str);
      return 0;
    }
{  char str[100]; int k;
   fscanf(fp, "%s\n", str);
   if(strcmp(str, PA==NULL?"null":PA)!=0)
    { msg_box("读入答案时发生错误：", "该文件不是该套试题的答案！");
      return 0;
    }
}
   for(k=0; k<=titnum; k++)
    { int i; char str[100];
      fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d \n",
	&rel[0], &rel[1], &rel[2], &rel[3], &rel[4], &rel[5], &rel[6], &rel[7],
	 &rel[8], &rel[9], &rel[10], &rel[11] );
      for(i=0; i<titptr[k].sel.num; i++)
	if(rel[i/16] & ((int)0x01<<(i%16)) )
	  titptr[k].sel.text[i].attrib |= BD;
	else titptr[k].sel.text[i].attrib &= ~BD;
    }
   fclose(fp);  msg_box("载入完毕：", "您所指定的答案已经载入该套试题中！");
   return 1;
 }


int  filejm()
 { int k, c;  FILE *fp, *fp0;  char filename[50] = ""; char seek;
   mm[0] = '\0';
   if(input_box("请输入加密以后的文件名：", filename, 45)<=0)  return 0;
   if(strlen(filename)==0)  return 0;
   if(strcmp(filename, nameptr)==0)
    { msg_box("错误：", "加密以后的文件名不能和当前文件名相同！");
      return 0;
    }
   fp = fopen(filename, "rb");
   if(fp!=NULL)
    { char str[100];
      fclose(fp);  sprintf(str, "是否覆盖掉文件%s？", filename);
      if(ask_box("发现重名文件：", str)!=1)  return 0;
    }
   if(input_box("请输入密码", mm, 8)<=0)   return 0;
   if(strlen(mm)==0)  strcpy(mm, "Skyever");
   fp = fopen(filename, "wb");
   if(fp==NULL)
    { char str[100];
      sprintf(str, "无法建立文件%s！", filename);
      msg_box("建立文件时出错：", str); return 0;
    }
   fp0 = fopen(nameptr, "rb");
   if(fp0==NULL)
    { char str[100];
      sprintf(str, "无法打开源文件%s！", filename);
      msg_box("打开文件出错：", str);
      fclose(fp);  return 0;
    }
   line_msg(ifjm==0?"加密过程中，请稍候...":"解密过程中，请稍候...");
   if(ifjm==1)  fgetc(fp0);
   else         fputc('&', fp);
   while((c=fgetc(fp0))!=EOF)
    { c = ~c;  
      seek = (char)(ftell(fp)+ifjm);
      c ^= seek;  c ^= mm[seek%strlen(mm)];
      c &= (int)0x00ff;
      fputc(c, fp);
    }
   fclose(fp); fclose(fp0);
   if(ifjm==0) msg_box("加密完毕", "加密完毕，请牢记密码以便日后打开！");
   else        msg_box("解密完毕", "解密完毕，您现在可以用正常方法打开该文件！");
   strcpy(nameptr, filename);
   line_msg("");    return 1;
 }

int  _fgetc(FILE *fp)
 { int ch;  char seek;
   ch = fgetc(fp);
   if(ch==EOF)  return EOF;
   if(ifjm==1)
    { ch = ~ch;
      seek = (char)(ftell(fp)-1);
      ch ^= seek;  ch ^= mm[seek%strlen(mm)];
      ch &= (int)0x00ff;
      return ch;
    }
   else return ch;
 }

void  _rewind(FILE *fp)
 { if(ifjm==0) { rewind(fp); return; }
   else fseek(fp, 1, SEEK_SET);
 }

int  openforfile()
 { char filename[50] = "";
   if(input_box("请输入文件名：", filename, 45)<=0)  return -2;
   if(strlen(filename)==0)  return -2;
   return  openfile(filename);
 }

#define inull(x)  (x[0]=='\0'?1:0)
int  saveset()
 { FILE *fp = fopen("Exam.set", "wt");
   int k;
   if(fp==NULL)
     { msg_box("错误：", "无法建立文件“Exam.set”，请检查磁盘是否写保护！");
       return 0;
     }
   fprintf(fp, "bkcolor = %d\n", bkcolor);
   fprintf(fp, "dirfile = %s\n", inull(dirfile)?"null":dirfile);
   fprintf(fp, "filelistnum = %d\n", listnum);
   fprintf(fp, "maxfilelistnum = %d\n", maxlistnum);
   for(k=0; k<listnum; k++)
     fprintf(fp, "file %d = %s\n", k+1, filelist[k]);
   fprintf(fp, "ifautosave = %d\n", ifsave);
   fprintf(fp, "ifft = %d\n", ifft);
   fprintf(fp, "ifsound = %d\n", ifsound);
   fprintf(fp, "commandfile = %s\n", inull(commandfile)?"null":commandfile);
   fprintf(fp, "includefile = %s\n", inull(includefile)?"null":includefile);
   fprintf(fp, "helpfile = %s\n", inull(helpfile)?"null":helpfile);
   fclose(fp);
   return 1;
 }

int  getset()
 { FILE *fp = fopen("Exam.set", "rt");
   int k, n;
   if(fp==NULL)
     { msg_box("错误：", "无法打开文件“Exam.set”，请检查是否存在该文件！");
       return 0;
     }
   fscanf(fp, "bkcolor = %d\n", &bkcolor);
   fscanf(fp, "dirfile = %s\n", dirfile);
   if(strcmp(dirfile,"null")==0)  dirfile[0] = '\0';
   fscanf(fp, "filelistnum = %d\n", &listnum);
   fscanf(fp, "maxfilelistnum = %d\n", &maxlistnum);
   for(k=0; k<listnum; k++)
     fscanf(fp, "file %d = %s\n", &n, filelist[k]);
   fscanf(fp, "ifautosave = %d\n", &ifsave);
   
   renewfilelist();
   replacexmenu(xmenu, 3, "GB", ifsave==0?"B退出时自动保存设置(OFF)|":
	       "B退出时自动保存设置(ON)|");
   fscanf(fp, "ifft = %d\n", &ifft);
   fscanf(fp, "ifsound = %d\n", &ifsound);
   replacexmenu(xmenu, 3, "GC", ifsound==1?"C声音开关(已打开)":"C声音开关(已关闭)");
   fscanf(fp, "commandfile = %s\n", commandfile);
   if(strcmp(commandfile,"null")==0)  commandfile[0] = '\0';
   fscanf(fp, "includefile = %s\n", includefile);
   if(strcmp(includefile,"null")==0)  includefile[0] = '\0';
   fscanf(fp, "helpfile = %s\n", helpfile);
   if(strcmp(helpfile,"null")==0)  helpfile[0] = '\0';
   fclose(fp);
   return 1;
 }

int  opendir()
 { FILE *fp;
   char filename[20] = "";
   if(input_box("请输入目录文件名：", filename, 19)<=0)  return 0;
   if(strlen(filename)==0)  return 0;
   fp = fopen(filename, "rt");
   if(fp==NULL)
    { char str[100]; sprintf(str, "未发现文件%s！", filename);
      msg_box("打开文件时出错：", str);
      return 0;
    }
   fclose(fp);
   if(readinmenu(filename)==1)
    { strcpy(dirfile, filename);
      unablexmenu(xmenu, 3, "E");
      autoxmenu(xmenu, 3, "D");
      showTITLE();
      msg_box("成功：", "已经成功地替换了目录列表！");
      return 1;
    }
   else  return 0;
 }

int  closedir()
 { delx(1, "ABCDEFGHIJKLMN");
   addxmenu(xmenu, 1, "1<", "^@(无任何测试题)|");
   unablexmenu(xmenu, 3, "D");
   autoxmenu(xmenu, 3, "E");
   free(TITLE);   TITLE = NULL;
   showTITLE();   dirfile[0] = '\0';
   msg_box("成功：", "已经成功地关闭了目录列表！");
   return 1;
 }

void  addfilelist()
 { int k;
   for(k=0; k<listnum; k++)  if(strcmp(filelist[k], nameptr)==0)  return;
   if(listnum>=maxlistnum)
     { for(k=0; k<listnum-1; k++)  strcpy(filelist[k], filelist[k+1]);
       strcpy(filelist[k], nameptr);
       listnum = maxlistnum;
     }
   else  strcpy(filelist[listnum++], nameptr);
   renewfilelist();
 }

void  renewfilelist()
 { int k; char road[10], list[50];
   delxmenu(xmenu, 1, "j");
   addxmenu(xmenu, 1, "f>", "j近期文件{^L清除文件列表}");
   if(listnum==0 || maxlistnum==0)  return;
   autoxmenu(xmenu, 1, "jL");
   addxmenu(xmenu, 1, "jL>", "|-A");
   strcpy(road, "jA>");
   for(k=0; k<listnum; k++)
    { sprintf(list, "|%d%s", k, filelist[k]);
      addxmenu(xmenu, 1, road, list);
      sprintf(road, "j%d>", k);
    }
 }

void  renew_screen(int code)
 { int k, flag = (button2[3].attrib&BD)?1:0;
   if(code==0)
    { writescreen();
      showxmenu(&xmenu);
      for(k=0; k<4; k++)  showbutton(button2+k);
    }
   else  cleardesktop(0);
   if(ifstart==0)
      if(ifend==1)
	{ if(but_end[0].attrib&BD)  showPQ();
	  else if(but_end[1].attrib&BD)  showPP();
	  else if(but_end[2].attrib&BD)  showPE();
	  else if(but_end[3].attrib&BD)  asktit();
	  else if(!(button2[2].attrib&BY)) showPG();
	  else  show_end();
	}
      else { if(!iffileopen) show_about();
	     else            showPG();
	   }
   else  { starttest(nowselect);  top_title("测试进行中"); }
   if(flag)  { button2[3].attrib|=BD;  show_qk(); return; }
 }


void  show_qk()           //显示答题情况
 { int i, j, k;
   char str[40] = "０\0１\0２\0３\0４\0５\0６\0７\0８\0９";
   if(!(button2[3].attrib&BD))
    { if(blist==NULL)  return;
      for(k=0; k<=titnum; k++)   free(blist[k].caption);
      free(blist);  blist=NULL;  ishowqk = 0;
      T += clock()-T0;  ifcontinue = 1;
      starttest(nowselect);      return;
    }
   if(blist==NULL)
     blist = (BUTTON*)malloc((titnum+1>119?119:titnum+1)*sizeof(BUTTON));
   cleardesktop(_BOT);  top_title("答题情况");
   setlinestyle(0, 0, 1);  setcolor(LIGHTBLUE);
   for(j=0; j<14; j++)  line(29, 110+j*24, 611, 110+j*24);
   for(i=0; i<12; i++)  line(29+53*i, 110, 29+53*i, 422);
   setcolor(GREEN);
   for(i=0; i<10; i++)  show_str(29+53*i+53+17, 114, str+i*3);
   for(j=0; j<10; j++)  show_str(29+17, 114+24*(j+1), str+j*3);
   show_str(29+9, 114+24*(j+++1), "１０");
   show_str(29+9, 114+24*(j+++1), "１１");
   for(k=0; k<=titnum; k++)
    { if(k==119)  break;
      blist[k].x1 = ((k+1)%10+1)*53+29+1;
      blist[k].y1 = ((k+1)/10+1)*24+110+1;
      blist[k].x2 = blist[k].x1+50;
      blist[k].y2 = blist[k].y1+10;
      blist[k].caption = (char *)malloc(20);
      for(i=0; i<titptr[k].sel.num; i++)
	if(titptr[k].sel.text[i].attrib&BD)  break;
      if(i!=titptr[k].sel.num)
	{ strcpy(blist[k].caption, "已答");  blist[k].forcolor = GREEN;}
      else
	{ strcpy(blist[k].caption, "未答");  blist[k].forcolor = RED;}
      blist[k].bkcolor = BLACK;
      blist[k].attrib = BY|BT|BF;
    }
   for(k=0; k<=titnum; k++)  showbutton(blist+k);
   ishowqk = 1;  T0 = clock();  ifcontinue = 0;
 }


void  reset_qk()
 { int k;  if(blist==NULL)  return;
   button2[3].attrib &= ~BD;   showbutton(button2+3);
   for(k=0; k<=titnum; k++)   free(blist[k].caption);
   free(blist);  blist = NULL;
   T += clock()-T0;  ifcontinue = 1;
   ishowqk = 0;
 }

void  readdirectseek()
 { char str[100], name[100]; int k=0;
   FILE *fp = fopen(includefile, "rt");
   directptr = 0;
   if(fp==NULL)  return;
   while(fscanf(fp, "%s %s\n", name, str)!=EOF)
    { if(strlen(name)>20 || strlen(str)>30)  continue;
      if(k>=maxdirect)
	directseek = (DIRECTSEEK*)realloc(directseek,
	   (maxdirect=maxdirect+20)*sizeof(DIRECTSEEK) );
      strcpy(directseek[k].definename, name);
      strcpy(directseek[k++].definestr, str);
      directptr ++;
    }
   fclose(fp);
 }
void  readcommandseek()
 { char str[100], name[100]; int k=0;
   FILE *fp = fopen(commandfile, "rt");
   commandptr = 0;
   if(fp==NULL)  Exit(0);
   while(fscanf(fp, "%s %s\n", name, str)!=EOF)
    { if(strlen(name)>10 || strlen(str)>10)  continue;
      if(k>=maxcommand)
	commandseek = (COMMANDSEEK*)realloc(commandseek,
	   (maxcommand=maxcommand+20)*sizeof(DIRECTSEEK) );
      strcpy(commandseek[k].definename, name);
      strcpy(commandseek[k++].definestr, str);
      commandptr ++;
    }
   fclose(fp);
 }

void  renewstr(char *str)
 { int k=-1, i;
   while(str[++k]!='.')
     if(str[k]=='\0')  return;
   for(i=0; i<=point; i++)
    if(str[++k]=='\0')  return;
  str[k] = '\0';
 }

int  getflag(char *str, char *label)
 { int k, K=-1; char _label[20];
   for(k=0; k<flagptr; k++)
    if(strcmp(label, Flag[k].flagname)==0)  return Flag[k].seek;
   while(str[++K]!='\0')
    { if(str[K]=='&')
       { K ++;
	 if(_getlabel(str, &K, _label)==0)
	  { msg_box("错误：", "发现未允许的位置标号！");
	    return -1;
	  }
	 if(strcmp(_label, label)==0)
	  { if(flagptr>=maxflag)
	      Flag = (FLAG*)realloc(Flag, (maxflag=maxflag+20)*sizeof(FLAG));
	    strcpy(Flag[flagptr].flagname, label);
	    Flag[flagptr++].seek = K-1;
	    return K-1;
	  }
       }
    }
   return -1;
 }


