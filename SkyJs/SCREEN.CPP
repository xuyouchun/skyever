#include <conio.h>

#include "Screen.h"

Screen::Screen()
{
	if(!IsScreenInit)	this->SetScreenModel(ScreenModel);
}

Screen::~Screen()
{

}

BOOL Screen::IsScreenInit =  FALSE;
int  Screen::ScreenModel = C80;
int  Screen::ScreenWidth = 80;
int  Screen::ScreenHeight = 25;

void Screen::SetScreenModel(int model)
{
	textmode(model);
	struct text_info myinfo;
	gettextinfo(&myinfo);

	ScreenWidth  = myinfo.screenwidth;
	ScreenHeight = myinfo.screenheight;

}

void Screen::ClrScr(int x1, int y1, int x2, int y2, char attrib)
{
	for(int y=y1; y<=y2; y++)
	{
		char far *ptr = (char far *)(0xB8000000L + (y-1)*ScreenWidth*2 + (x1-1)*2);
		char far *endptr = ptr + (x2-x1+1)*2;
		while(ptr<endptr)   { *ptr++ = ' '; *ptr++ = attrib; }
	}
}

void Screen::WriteChar(int x, int y, char ch, unsigned char attrib)
{
	char far *ptr = (char far *)(0xB8000000L + (y-1)*(ScreenWidth<<1) + ((x-1)<<1));
	*ptr = ch;  *(ptr+1) = attrib;
}

