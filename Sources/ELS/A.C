void interrupt (*oldkey)();
char  down, busy; unsigned char ch;
void  setkey(int key, char down);
void  interrupt  newkey()
 {  char flag = 0;
    if(busy==1)  return;   busy = 1;
    ch = inport(0x60);
    down = ch&(~0x7f)?0:1;
    switch(ch&0x7f)
     { case 0x4b: left = down ; break;
       case 0x4d: right = down ; break;
       case 0x48: up = down ; break;
       case 0x50: down = down; break;
       case 0x39: space = down; break;
       default: flag = 1;
     }
    oldkey();
    if(flag==0)  *((char far*)0x0041aL) = *((char far*)0x0041cL);
    busy = 0;
  }

