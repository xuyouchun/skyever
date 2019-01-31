#include <tx.h>
#include <dir.h>

#define  NUMERROR  0x7FFF

struct
 { char str[2];
   unsigned char attrib;
   unsigned char index;
 } HZ[12000];

char  source[50],  object[50], _source[50];
FILE  *sourcefp,  *objectfp;
char  putstr[130];
char  dotstr[5], offsetstr[10] = "0";
char  hzkstr[1000], fontstr[50][13];
int   dot = 24, Line=10;
int   num, Num;
static int index=-1, n0;
static FILE *fp;
#define sLine  (tscroll.now)
#define sMax   (tscroll.max)
char  exepath[100];
int   exeend;  char saveflag;

void  write_barline();
void  show_allhz(), write_screen();
void  test_click(), renew_desktop(), change_dot(unsigned char ptr[][12], unsigned char);
int   show_fontdot(int x, int y, int code);
int   find_hzkfile();
int   strtonum(char *str);
int   savetext(), readtext(), saveobj(), setmsg();
#define clear_select()  select(-1)
void  select(int code), renew_desk(), reset();
int   begin=-1, end=-1;
#define  FARX  500
#define  FARY  420
#define  LX  10
#define  LY  10
#define  dT  ((FARY-LY)/dot)

BUTTON    renew = { FARX+5, FARY+5, 0, 0, "刷新汉字显示区", -1, -1, BT|BY };
INPUTBOX  sourcehz = { 10, FARY+10, putstr, 60, DARKGRAY, WHITE };
TSCROLL   tscroll = { FARX+5, 10, FARX+15, FARY-5, GREEN, RED, 1, 0 };
INPUTBOX  sourcename = { FARX+40, 30, _source, 14, DARKGRAY, WHITE };
INPUTBOX  objectname = { FARX+40, 70, object, 14, DARKGRAY, WHITE };

#define   SELY  110
SELTEXT   seldot[3] =
 {{FARX+40,SELY,"16点阵式",BY},{FARX+40,SELY+20,"24点阵式",BY|BD},{FARX+40,SELY+40,"其它:",BY}};
SELECT    dotsel = { seldot, 3, YELLOW, BLUE };
INPUTBOX  elsedot = { FARX+103, SELY+40, dotstr, 3, BLUE, YELLOW };
INPUTBOX  offsetbox = { FARX+65, SELY+157, offsetstr, 9, DARKGRAY, YELLOW };

DLIST  fontlist =           //显示汉字库列表
   { FARX+40, 190, 90,
       { 0, 0, 0, 0, hzkstr,
	   { 0, 0, 0, 0, DARKGRAY, RED, 0, 0 },
	 BLUE, YELLOW, 0, {0}, 1
       },
      0, BLUE, YELLOW, 1
   };

BUTTON  mirro_0 = { FARX+60, 215, 0, 0, "水平", -1, -1, BY };
BUTTON  mirro_1 = { FARX+106, 215, 0, 0, "垂直", -1, -1, BY };
BUTTON  round_0 = { FARX+60, 240, 0, 0, "顺向", -1, -1, BY };
BUTTON  round_1 = { FARX+106, 240, 0, 0, "逆向", -1, -1, BY };

BUTTON  cut  = { FARX+20, 312, 0, 0, "剪切", -1, -1, BY|BT };
BUTTON  copy = { FARX+60, 312, 0, 0, "复制", -1, -1, BY|BT };
BUTTON  paste= { FARX+100, 312, 0, 0, "粘贴", -1, -1, BY|BT };

BUTTON  new  = { FARX+20, 360, 0, 0, "导入", -1, -1, BY|BT };
BUTTON  save = { FARX+60, 360, 0, 0, "保存", -1, -1, BY|BT };
BUTTON  aclose={ FARX+100, 360, 0, 0, "退出", -1, -1, BY|BT };

struct
 { char  fontname[13];
   char  index;
   int   offset;
 } FONT[50];
int maxfont;

char *pa(char *filename)  
 { strcpy(exepath+exeend, filename);
   return  exepath;
 }

void  renewscroll()
 { Line = (FARX-LX)/dot; 
   sMax = (num/Line)-dT+2;  if(sMax<1)  sMax = 1;  
   showscroll(&tscroll);  
 }

int  main(int argc, char **argv)
 { int i=0, j, k, ch;
   strcpy(exepath, argv[0]);
   for(k=strlen(exepath)-1; k>=0; k--)  
     if(exepath[k]==':'||exepath[k]=='\\') break;
   exeend = k+1;
   if(find_hzkfile()==0)
     { printf("Error: Can't find any font file!\n");  return 1; }
  { char hzk[100];   strcpy(hzk, pa("fnt\\hzk16"));
    if(open_hzk(pa("fnt\\asc16"), hzk)==-1)
     { printf("Error: Can't open system font file !\n");  return 1; }
  }
   registerbgidriver(EGAVGA_driver);
   inittx();   initmouse();
   cleardevice();
   if(argc>=2)  strcpy(source, argv[1]);
   else         strcpy(source, "NONAME.SRC");
   strcpy(_source, source);
   for(k=strlen(_source)-1; k>=0; k--)  if(_source[k]=='\\') break;
   if(k>=0)      while(_source[i++] =_source[i+k+1]);
   for(k=strlen(source)-1; k>=0; k--)  if(source[k]=='.')  break;
   if(k<0)  strcat(source, ".SRC");
   if(access(source,0)==-1)  setmsg();
   else  readtext();
   if(argc>=3)  strcpy(object, argv[2]);
   else         if(!strlen(object))  strcpy(object, "NONAME.FNT");
   write_screen();  mouse_on();
   test_click();
   closegraph();  fcloseall();  return 0;
 }

void  write_screen()
 { setlinestyle(0, 0, 3);
   setcolor(LIGHTBLUE);
   line(FARX, 10, FARX, 450);   line(10, FARY, 630, FARY);
   setlinestyle(0, 0, 1);
   setcolor(LIGHTGREEN);
   show_str(FARX+20, 10, "源文本文件名:");
   show_str(FARX+20, 50, "目标库文件名:");
   show_str(FARX+20, SELY-20, "汉字点阵式:");
   show_str(FARX+20, 170, "汉字字体:");
   show_str(FARX+20, 217, "翻转:");
   show_str(FARX+20, 242, "旋转:");
   show_str(FARX+20, 267, "偏移:");
   show_str(FARX+20, 292, "编辑文字:");
   show_str(FARX+20, 339, "文件存取:");
   show_str(FARX+23, 382, ">>>Skyever<<<");
   setcolor(YELLOW);  setfillstyle(1, GREEN);  bar(FARX+20, 400, 639, 416);
   show_str(FARX+25, 400, "未  选");
   formatbutton(&renew);   showbutton(&renew);
   show_inputbar(&sourcehz);  showscroll(&tscroll);
   show_inputbar(&sourcename);  show_inputbar(&objectname);
   show_inputbar(&offsetbox);
   show_select(&dotsel);    show_inputbar(&elsedot);
   formatdlist(&fontlist);  showdlist(&fontlist);
   formatbutton(&mirro_0);  showbutton(&mirro_0);
   formatbutton(&mirro_1);  showbutton(&mirro_1);
   formatbutton(&round_0);  showbutton(&round_0);
   formatbutton(&round_1);  showbutton(&round_1);
   formatbutton(&copy);     showbutton(&copy);
   formatbutton(&paste);    showbutton(&paste);
   formatbutton(&cut);      showbutton(&cut);
   formatbutton(&new);      showbutton(&new);
   formatbutton(&save);     showbutton(&save);
   formatbutton(&aclose);   showbutton(&aclose);
   write_barline();
 }

void  Exit(int code)
 { fcloseall();   closegraph();  exit(code); }

void  test_click()
 { int code, k, roflag;   int  getindex(char *str);
   int _begin, _end;  void  show_bar(int);
BEGIN
   if(imouse_inbox(LX, LY, FARX-5, FARY-5))  select(0);
   if(button_click(&renew))    renew_desktop();
   if(test_mouseinputbox(&sourcehz)) hz: 
     { int i, k, len;
       putstr[0]='\0'; while(kbhit()) getch(); input_str(&sourcehz);  
       if((len=strlen(putstr))!=0)
	{ for(k=0; k<len; k++)  if(!(putstr[k]&0x80))
	    { for(i=k; i<len; i++)  putstr[i]=putstr[i+1]; len--; }
	}
     }
   if(test_mouseinputbox(&sourcename))  input_str(&sourcename);
       
   if(test_mouseinputbox(&objectname))  input_str(&objectname);
   if(test_mouseinputbox(&elsedot))    //从输入框输入点阵数
    { int bdot = dot, n, k;
      input_str(&elsedot);
      if((n=strtonum(dotstr))!=NUMERROR)
       { if(n-=n%8)
	  { dot = n;  sprintf(dotstr, "%d", dot);  show_inputbar(&elsedot);
            for(k=0; k<3; k++)   seldot[k].attrib &= ~BD;
	    if(dot==16||dot==24)  seldot[dot/8-2].attrib |= BD;  
	    else seldot[2].attrib |= BD;
	    show_select(&dotsel);
	  }
       }
      else
       { int k;
	 for(k=0; k<3; k++)   seldot[k].attrib &= ~BD;
	 if(bdot==16 || bdot==24)  seldot[bdot/8-2].attrib |= BD;
	 else { sprintf(dotstr, "%d", bdot); seldot[2].attrib |= BD;
		show_inputbar(&elsedot);
	       }
	 show_select(&dotsel);  dot = bdot;
       }
      renew_desktop();
    }

   if(test_mouseinputbox(&offsetbox) && input_str(&offsetbox) )  of:
     { int bn = FONT[fontlist.now].offset,  n = strtonum(offsetstr);
       if(n!=NUMERROR)	 FONT[fontlist.now].offset = n;
       else { sprintf(offsetstr, "%d", bn);  show_inputbar(&offsetbox); }
       renew_desktop();  saveflag = 1;
     }
   if((code=select_click(&dotsel))!=-1)  //选择点阵数
    { int bdot = dot, n;
      if(code==0)  dot = 16;   else if(code==1)  dot = 24;
      else if(input_str(&elsedot) && (n=strtonum(dotstr))!=NUMERROR )
       { if(n-=n%8)
	  { dot = n;  sprintf(dotstr, "%d", dot);  show_inputbar(&elsedot);
	    if(dot==16||dot==24)
	      { seldot[dot/8-2].attrib |= BD;  seldot[2].attrib &= ~BD;
		show_select(&dotsel);
	      }
	  }
	 else  goto error;
       }
      else  error:
       { int k;
	 for(k=0; k<3; k++)   seldot[k].attrib &= ~BD;
	 if(bdot==16 || bdot==24)  seldot[bdot/8-2].attrib |= BD;
	 else { sprintf(dotstr, "%d", bdot); seldot[2].attrib |= BD;
		show_inputbar(&elsedot);
	      }
	 show_select(&dotsel);  dot = bdot;
       }
      renew_desktop();  saveflag = 1;
    }
   Line = (FARX-LX)/dot;
   _begin = begin+Line*sLine;  _end = end+Line*sLine;
   if(begin==-1 && end==-1)  { _begin=0; _end=num-1; }

   if(selectdlist(&fontlist)!=-1)  //选择字型
     { int K = get_index(fontstr[fontlist.now]);
       sprintf(offsetstr, "%d", FONT[fontlist.now].offset);
       off(); show_inputbar(&offsetbox); on();
       for(k=_begin; k<=_end; k++)   HZ[k].index = K;
       if(begin==-1&&end==-1)
	{ begin = 0;  end = dT*Line>num-1?num-1:dT*Line;  renew_desktop();
	  show_bar(1);  begin = -1;  end = -1;
	}
       else   renew_desk();
     }
   roflag = 1;
   if(button_click(&mirro_0))
    { for(k=_begin; k<=_end; k++)
       if(mirro_0.attrib&BD)  HZ[k].attrib |= 0x01;
       else                   HZ[k].attrib &= ~0x01;      
    }
   else if(button_click(&mirro_1))
    { for(k=_begin; k<=_end; k++)
	if(mirro_1.attrib&BD)  HZ[k].attrib |= 0x02;
	else                   HZ[k].attrib &= ~0x02;
    }
   else if(button_click(&round_0))
    { for(k=_begin; k<=_end; k++)
	if(round_0.attrib&BD)  HZ[k].attrib |= 0x04;
	else                   HZ[k].attrib &= ~0x04;
    }
   else if(button_click(&round_1))
    { for(k=_begin; k<=_end; k++)
	if(round_1.attrib&BD)  HZ[k].attrib |= 0x08;
	else                   HZ[k].attrib &= ~0x08;      
    }
   else roflag = 0;
   if(roflag==1)
     if(begin==-1&&end==-1)  renew_desktop();
     else                    renew_desk();

   if(button_click(&copy))       //拷贝文字
     { int K = 0;
      for(k=_begin; k<=_end; k++)
	{ putstr[K++] = HZ[k].str[0];  
	  putstr[K++] = HZ[k].str[1]; 
	  if(K>=60)  break;
	}  
      putstr[K] = '\0';     show_inputbar(&sourcehz);
    }
   if(button_click(&paste) && strlen(putstr))  pas:   //粘贴文字
    { int len=strlen(putstr)/2, k, k0=0;   
      if(begin==-1&&end==-1)  _begin = -1;
      _end = _begin+len; end = begin+len;
      for(k=num-1;k>_begin;k--)   HZ[k+len] = HZ[k];
      for(k=_begin+1; k<=_end; k++)
	{ HZ[k].str[0] = putstr[2*k0]; HZ[k].str[1] = putstr[(2*k0++)+1];
	  HZ[k].index = get_index(fontstr[fontlist.now]);
	  HZ[k].attrib = 0;
	  if(mirro_0.attrib&BD)  HZ[k].attrib |= 0x01;
	  if(mirro_1.attrib&BD)  HZ[k].attrib |= 0x02;
	  if(round_0.attrib&BD)  HZ[k].attrib |= 0x04;
	  if(round_1.attrib&BD)  HZ[k].attrib |= 0x08;
	}
      num += len;  begin++;  renew_desktop();  saveflag = 1;
      sMax = (num/Line)-dT+2;  if(sMax<1)  sMax = 1;  showscroll(&tscroll); 
    }
   if(button_click(&cut)&&begin+1&&end+1)  del:   //剪切文字
    { int K = 0, len = _end-_begin+1;
      for(k=_begin; k<=_end; k++)
	{ putstr[K++] = HZ[k].str[0];  
	  putstr[K++] = HZ[k].str[1]; 
	  if(K>=60)  break;
	}
      putstr[K] = '\0';     show_inputbar(&sourcehz);
      for(k=_begin; k<=num-len; k++)   HZ[k] = HZ[k+len];
      num -= len;  begin = end = _begin-Line*sLine-1;  renew_desktop();  saveflag = 1;
    }
   if(button_click(&new))
      if(access(source,0)==-1)  setmsg();
      else                      readtext();
   if(button_click(&save))      savetext();    //保存文件
   if(button_click(&aclose))    exit___:
    { int code;      if(saveflag==0)  Exit(0);
      code = ask_box("未保存:", "是否现在保存?");
      if(code==0)  Exit(0);
      if(code==1)  if(savetext())  Exit(0);
    }
   if(scroll(&tscroll))   scr:
     { static int bnow;
       begin += (bnow-sLine)*Line;  end += (bnow-sLine)*Line;
       renew_desktop();  bnow = sLine;
     }
   if(kbhit())   switch(getch())
    { case SPACE_KEY: show_bar(1); begin=end; show_bar(1); goto hz;   
      case ENTER_KEY: goto pas;
      case ESC_KEY:   if(bioskey(2)&0x03)  goto exit___;
		      show_bar(1);  end = begin = -1; 
		      setfillstyle(1, GREEN); bar(FARX+20, 400, 639, 416);
		      setcolor(YELLOW); show_str(FARX+25, 400, "未  选"); break; 
      case DELETE_KEY:goto del;
      case TAB_KEY:   k=FONT[fontlist.now].offset;
		      if(k&&k!=1410)  break;  k = 1410-k;
		      sprintf(offsetstr, "%d", k);  goto of;
      case HOME_KEY:  sLine = 0;  goto scr;
      case END_KEY:   sLine = sMax-1; goto scr;
      case PAGEUP_KEY: case UP_KEY: if(sLine) { sLine--; goto scr; }
      case PAGEDOWN_KEY: case DOWN_KEY: if(sLine!=sMax-1) { sLine++; goto scr; }
    }
WEND
 }

int  test_index(int index)
 { int i;
   for(i=0; i<maxfont; i++)  if(FONT[i].index==index)  return 0;
   return 1;
 }

int  get_index(char *str)
 { int k;
   for(k=0; k<maxfont; k++)
     if(strcmp(str, FONT[k].fontname)==0)
       if(FONT[k].index!=-1)  return FONT[k].index;
       else
	{ int i;
	  for(i=0; i<maxfont; i++)  if(test_index(i))
	   { FONT[k].index = i;  Num++;  return i; }
	}
    return -1;
 }

int   find_hzkfile()
 { struct ffblk _ffblk;
   if(findfirst(pa("fnt\\*.*"), &_ffblk, 0)==-1)  return 0;
   do
    { int k=0, offset=0;
      strcat(hzkstr, _ffblk.ff_name);
      strcat(hzkstr, "|");
      strcpy(FONT[maxfont].fontname, strupr(_ffblk.ff_name));
      strcpy(fontstr[maxfont], strupr(_ffblk.ff_name));
      FONT[maxfont].index = -1;
      for(k=0; k<strlen(_ffblk.ff_name); k++) 
	if(_ffblk.ff_name[k]>='0'&&_ffblk.ff_name[k]<='9')  break;
      if(k!=strlen(_ffblk.ff_name))  
	 sscanf(_ffblk.ff_name+k, "%d%*s", &offset);
      if(offset==24)  FONT[maxfont].offset=1410;
      maxfont++;
    } while(findnext(&_ffblk)!=-1);
   return 1;
 }

int   strtonum(char *str)
 { int fs = 0, num=0;  char *ptr = str;
   if(ptr[0]==0)    return 0;
   if(ptr[0]=='-')  { fs = 1; ptr ++; }
   while(*ptr!='\0')
    { if(*ptr>'9' || *ptr<'0')  return NUMERROR;
      num = num*10 + ((int)*ptr++-'0');
    }
   if(fs==1)  return -num;
   else       return  num;
 }

int  savetext()
 { int  k, i, j, qh, wh, code;  int test_inuse(int);
   unsigned char fontdot[96][12], ch;
   sourcefp = fopen(source, "wt");
   if(sourcefp==NULL)
     { msg_box("错误:", "写入源文件出错!");  return 0; }
   fprintf(sourcefp, "object name: %s\n", object==NULL?"NULL":object);
   Num = 0;
   for(k=0; k<maxfont; k++)
    { if(!test_inuse(FONT[k].index))  FONT[k].index = -1;
      if(FONT[k].index!=-1)  Num ++;
    }
   fprintf(sourcefp, "font in use: %d\n", Num);
   Num = 0;
   for(k=0; k<maxfont; k++)
     { if(FONT[k].index==-1)  continue;
       fprintf(sourcefp, "<NO. %d>--filename: %s index: %d offset: %d\n",
	    Num+++1, FONT[k].fontname, FONT[k].index, FONT[k].offset );
     }
   fprintf(sourcefp, "font dot: %d\n", dot);
   fprintf(sourcefp, "font num: %d\n", num);
   fprintf(sourcefp, "font->index->round\n");
   for(k=0; k<num; k++)
     fprintf(sourcefp, "%c%c%c%c", HZ[k].str[0]&0x7f, HZ[k].str[1]&0x7f,
	 HZ[k].index+'A', HZ[k].attrib+'A' );
   fclose(sourcefp);                        
   objectfp = fopen(object, "wb");
   if(objectfp==NULL)
     { msg_box("错误:", "写入目标汉字库文件出错!");  return 0; }
   for(code=0; code<num; code++)
    { int x, y; 
      if(HZ[code].index!=index)
       { char str[30]; 
	 if(fp!=NULL)  fclose(fp);
	 index = HZ[code].index;
	 for(k=0; k<maxfont; k++)  
	   if(FONT[k].index == index) { n0 = k; break; }
	 sprintf(str, pa("fnt\\%s"), FONT[n0].fontname);
	 fp = fopen(str, "rb");
	 if(fp==NULL)
	  { sprintf(str, "未发现字库:%s", FONT[index].fontname);
	    msg_box("错误:", str);  fclose(objectfp); return 0;
	  }
       }
      qh = (unsigned char)HZ[code].str[0] - 0xa0;
      wh = (unsigned char)HZ[code].str[1] - 0xa0;
      fseek(fp, (94*(qh-1)+(wh-1)-FONT[n0].offset)*(dot*dot/8L),SEEK_SET);
      for(j=0; j<dot; j++)    for(i=0; i<dot/8; i++)
	fontdot[j][i] = fgetc(fp);
      change_dot(fontdot, HZ[code].attrib);
      for(j=0; j<dot; j++)
	for(i=0; i<dot/8; i++)
	  fputc(fontdot[j][i], objectfp);
    }
   fclose(objectfp);  msg_box("完毕:", "保存源文件和目标汉字库文件成功!");
   saveflag = 0;  return 1;
 }

int test_inuse(int index)
 { int k;
   for(k=0; k<num; k++)  if(HZ[k].index==index)  return 1;
   return 0;
 }

int  readtext()
 { int k, index, offset;  int test_inuse(int);  char str[20];
   char bit[4];
   sourcefp = fopen(source, "rt");
   if(sourcefp==NULL)
     { msg_box("错误:", "读入源文件出错!");  return 0; }
   for(k=0; k<50; k++)  FONT[k].index = -1;
   fscanf(sourcefp, "object name: %s\n", object);
   if(strcmp(object, "NULL")==0)  object[0] = '\0';
   fscanf(sourcefp, "font in use: %d\n", &Num);
   for(k=0; k<Num; k++)
     { int i;
       fscanf(sourcefp, "<NO. %*d>--filename: %s index: %d offset: %d\n",
	    str, &index, &offset );
       strupr(str);
       for(i=0; i<maxfont; i++)
	 if(strcmp(str, FONT[i].fontname)==0)
	   { FONT[i].index = index;  FONT[i].offset = offset; break; }
       if(i==maxfont)
	 { char ss[100]; sprintf("发现未注册的字体%s", str);
	   msg_box("出现错误:", ss);  return 0;
	 }
     }
   fscanf(sourcefp, "font dot: %d\n", &dot);
   fscanf(sourcefp, "font num: %d\n", &num);
   fscanf(sourcefp, "font->index->round\n");
   for(k=0; k<num; k++)
    { int i;
      fscanf(sourcefp, "%c%c%c%c",  HZ[k].str, HZ[k].str+1,
	 &(HZ[k].index), &(HZ[k].attrib) );
      HZ[k].attrib -= 'A';  HZ[k].index -= 'A';
      for(i=0;i<2;i++)  HZ[k].str[i] |= 0x80; 
    }
   sprintf(offsetstr, "%d", FONT[fontlist.now].offset);
   show_inputbar(&offsetbox);   fclose(sourcefp);
   off();  renew_desktop();  on();  
   reset();  begin = end = -1;  Line = (FARX-LX)/dot; 
   sMax = (num/Line)-dT+2;  if(sMax<1)  sMax = 1;  sLine = 0;
   showscroll(&tscroll);
   for(k=0; k<3; k++)  seldot[k].attrib &= ~BD;
   if(dot==16||dot==24)  { seldot[dot/8-2].attrib |= BD; strcpy(dotstr, ""); }
   else { seldot[2].attrib |= BD; sprintf(dotstr, "%d", dot); }
   show_select(&dotsel);   show_inputbar(&elsedot);
   return 1;
 }

int  setmsg()   {return 0;}

int  show_fontdot(int x, int y, int code)
 { int i, j, k, qh, wh;
   unsigned char fontdot[96][12], ch;
   if(HZ[code].index!=index)
     { char str[30]; 
       if(fp!=NULL)  fclose(fp);
       index = HZ[code].index;
       for(k=0; k<maxfont; k++)  
	 if(FONT[k].index == index) { n0 = k; break; }
       sprintf(str, "fnt\\%s", FONT[n0].fontname);
       fp = fopen(pa(str), "rb");
       if(fp==NULL)
	 { sprintf(str, "未发现字库:%s", FONT[index].fontname);
	   msg_box("错误:", str);  return 0;
	 }
     }
   qh = (unsigned char)HZ[code].str[0] - 0xa0;
   wh = (unsigned char)HZ[code].str[1] - 0xa0;
   fseek(fp, (94*(qh-1)+(wh-1)-FONT[n0].offset)*(dot*dot/8L),SEEK_SET);
   for(j=0; j<dot; j++)    for(i=0; i<dot/8; i++)
     fontdot[j][i] = fgetc(fp);
   change_dot(fontdot, HZ[code].attrib);
   mouse_off();
   for(j=0; j<dot; j++)
     for(i=0; i<dot/8; i++)
       { ch = fontdot[j][i];
	 for(k=0; k<8; k++)
	   if(ch&(0x80>>k))  putpixel(x+i*8+k, y+j, GREEN);
       }
   mouse_on();   return 1;
 }


void  renew_desktop()
 { int i, j, k, x=LX, y=LY, sbegin, send;
   void _bar(int,int);   clear_select();
   renewscroll();
   Line = (FARX-LX)/dot;   setfillstyle(0, 0);
   off(); bar(0, 0, FARX-3, FARY-3); on();
   sbegin = sLine*Line;  send = sbegin+dT*Line;
   if(send >= num)  send = num-1;
   for(k=sbegin; k<send; k++)
    { show_fontdot(x, y, k);
      x += dot;
      if(x+dot>FARX) { x = LX; y += dot; };
    }
   reset();  _bar(begin, end);  
 }

void  renew_desk()
 { int i, j, k, x, y, sbegin=begin, send=end; void _bar(int,int);  
   Line = (FARX-LX)/dot;   clear_select();
   setfillstyle(0, 0);   Line = (FARX-LX)/dot;
   if(sbegin<0)  sbegin = 0;
   if(send>=dT*Line)  send=dT*Line-1;
   mouse_off();
   for(k=sbegin; k<=send; k++)
    { x = LX+(k%Line)*dot; y = LY+(k/Line)*dot;
      bar(x, y, x+dot, y+dot);  show_fontdot(x, y, k+Line*sLine);
    }
   mouse_on();   _bar(begin, end);   saveflag = 1;
 }

#define readbit(j,i)  (ptr[j][(i)>>3]&(0x80>>((i)%8)))
#define setbit(j,i)   (ptr0[j][(i)>>3]|=(0x80>>((i)%8)))
#define clsbit(j,i)   (ptr0[j][(i)>>3]&=~(0x80>>((i)%8)))
void  change_dot(unsigned char ptr[][12], unsigned char attrib)
{ unsigned char ptr0[96][12];
  int i, j;
  if(attrib==0)  return;
  if(attrib&0x01)
    { for(j=0; j<dot; j++)  for(i=0; i<dot; i++)
       if(readbit(j,dot-i-1))  setbit(j,i);
       else                    clsbit(j,i);
      for(j=0; j<dot; j++)  for(i=0; i<dot/8; i++)
       ptr[j][i] = ptr0[j][i];
    }
  if(attrib&0x02)
    { for(j=0; j<dot; j++)  for(i=0; i<dot; i++)
       if(readbit(dot-j-1,i))  setbit(j,i);
       else                    clsbit(j,i);
      for(j=0; j<dot; j++)  for(i=0; i<dot/8; i++)
       ptr[j][i] = ptr0[j][i];
     }
  if(attrib&0x04)
    { for(j=0; j<dot; j++)  for(i=0; i<dot; i++)
       if(readbit(dot-i-1,j))  setbit(j,i);
       else                    clsbit(j,i);
      for(j=0; j<dot; j++)  for(i=0; i<dot/8; i++)
	ptr[j][i] = ptr0[j][i];
    }
  if(attrib&0x08)
    { for(j=0; j<dot; j++)  for(i=0; i<dot; i++)
       if(readbit(j,dot-i-1))  setbit(i,j);
       else                    clsbit(i,j);
      for(j=0; j<dot; j++)  for(i=0; i<dot/8; i++)
       ptr[j][i] = ptr0[j][i];
     }
}

void  select(int code)
 { void  show_sel(int x, int y);
   void  show_bar(int code);
   static int x=-1, y=-1;
   int X, Y;   getmouse_xy(&X, &Y);
   X = (X-LX)/dot;   Y = (Y-LY)/dot;
   if(code==-1)  { X=-1; Y=-1; }
   if(x==X && y==Y && getmousebutton()==NOBUTTON)  return;
   show_sel(x, y);   show_sel(x=X, y=Y);
   if(code==-1)  return;
   Line = (FARX-LX)/dot;
   if(getmousebutton()==RIGHTBUTTON)  
     { show_bar(1);  end = begin = -1; setfillstyle(1, GREEN); bar(FARX+20, 400, 639, 416);
       setcolor(YELLOW); show_str(FARX+25, 400, "未  选"); return; 
     }
   if(getmousebutton()==LEFTBUTTON)
    { void _bar(int,int);  char str[50];  _bar(begin, end);  
      end = begin = Y*Line+X;  
      if(end+Line*sLine>=num-1 || begin+Line*sLine>=num-1)  
	{ begin = end = -1; setfillstyle(1, GREEN); bar(FARX+20, 400, 639, 416);
	  setcolor(YELLOW); show_str(FARX+25, 400, "未  选"); return; 
	}
      show_bar(1);
      while(getmousebutton()!=NOBUTTON)
	{ int end0;
	  if(!imouse_inbox(LX, LY, FARX-3, FARY-3))  continue;
	  getmouse_xy(&X, &Y);
	  X = (X-LX)/dot;   Y = (Y-LY)/dot;
	  end0 = Y*Line+X;  
	  if(end0+Line*sLine>=num-1)  end0 = num-Line*sLine-2;
	  if(end==end0)  continue;
	  end = end0;  show_bar(0);
	  show_sel(x, y);   show_sel(x=X, y=Y);
	}
      if(begin>end) { int t; t=begin; begin=end; end=t; begin++; end--; }
      if(end+Line*sLine>=num-1)  end = num-Line*sLine-2;  
      if(begin>=num)  begin = num-1;
      setfillstyle(1, GREEN);  bar(FARX+20, 400, 639, 416);
      sprintf(str, "%d -> %d", begin+sLine*Line, end+sLine*Line);
      setcolor(YELLOW); show_str(FARX+25, 400, str);  reset();
     }
 }

void  reset()
 { int k, i, N=begin+Line*sLine==-1?0:begin+Line*sLine,index = HZ[N].index;  
   for(k=0; k<maxfont; k++) if(FONT[k].index==index)  break;
   for(i=0; i<maxfont; i++) 
     if(strcmp(FONT[k].fontname, fontstr[i])==0)  break;
   fontlist.now = i;
   if(HZ[N].attrib & 0x01)  mirro_0.attrib |= BD;
   else                     mirro_0.attrib &= ~BD;
   if(HZ[N].attrib & 0x02)  mirro_1.attrib |= BD;
   else                     mirro_1.attrib &= ~BD;
   if(HZ[N].attrib & 0x04)  round_0.attrib |= BD;
   else                     round_0.attrib &= ~BD;
   if(HZ[N].attrib & 0x08)  round_1.attrib |= BD;
   else                     round_1.attrib &= ~BD;
   sprintf(offsetstr, "%d", FONT[fontlist.now].offset);
   mouse_off();
   showdlist(&fontlist);
   showbutton(&mirro_0);      showbutton(&mirro_1);
   showbutton(&round_0);      showbutton(&round_1);
   show_inputbar(&offsetbox);                        
   mouse_on();
 }

void  show_bar(int code)
 { void _bar(int,int);
   static int begin0, end0; 
   if(code==1)  { begin0 = begin;  end0 = end; _bar(begin,end); return; }
   if(end0>end)  _bar(end+1, end0);   
   else          _bar(end, end0+1);
   end0 = end;
 }

void _bar(int begin, int end)
 { int x1, y1, x2, y2, k, j, _begin=begin, _end=end;
   if(begin==-1 && end==-1)  return;
   setwritemode(XOR_PUT);  setlinestyle(0, 0, 3);  setcolor(BLUE);
   if(begin>end)  { int t; t=begin; begin=end; end=t; }   
   if(_begin<0)  _begin = 0;
   if(_end>Line*dT)    _end = Line*dT;
   mouse_off();
   for(k=begin; k<=end; k++)
    { if(k>=num || k>=Line*dT)  break;
      x1 = LX+(k%Line)*dot;  y1 = LY+(k/Line)*dot;
      x2 = x1+dot-1;  y2 = y1;
      for(j=2; j<dot; j+=3)  line(x1, y1+j, x2, y2+j);
    }
   mouse_on();
   setwritemode(COPY_PUT);  setlinestyle(0, 0, 1);
 }


void  show_sel(int x, int y)
 { int x1=LX+x*dot, y1=LY+y*dot, x2=x1, y2=y1+dot;
   if(x==-1 || y==-1)  return;
   setwritemode(XOR_PUT);  setlinestyle(0, 0, 3);  setcolor(LIGHTGREEN);
   off();  line(x1, y1, x2, y2);  on();
   setwritemode(COPY_PUT); setlinestyle(0, 0, 1);
 }

void  write_barline()
 { setfillstyle(1, LIGHTGRAY);
   bar(0, 460, 639, 479);
   setcolor(LIGHTGRAY);
   line(0, 455, 639, 455);  line(0, 453, 639, 453);
   setcolor(BLACK);
   show_str(150, 462, "天恒软件开发中心  ----  通用汉字库建立系统");
 }
