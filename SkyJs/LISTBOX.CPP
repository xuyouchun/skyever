#include <string.h>
#include "KeyBoard.h"
#include "ListBox.h"

#define MAX3(x1,x2,x3)  ((x1)>(x2)?(x1):(x2)>(x3)?(x2):(x3))
#define MIN3(x1,x2,x3)  ((x1)<(x2)?(x1):(x2)<(x3)?(x2):(x3))
ListBox::ListBox(int x1, int y1, int x2, int y2, unsigned char attrib, LISTDIRECTION direction)
{
	if(x2==DEFAULTPOSTION) { this->IsExpandWidth = TRUE;  x2 = x1; }
	else		this->IsExpandWidth = FALSE;
	if(y2==DEFAULTPOSTION) { this->IsExpandHeight = TRUE; y2 = y1; }
	else		this->IsExpandHeight = FALSE;

	int ScreenWidth = this->GetScreenWidth(), ScreenHeight = this->GetScreenHeight();

	//init arguments
	if(x1<1)	x1 = 1;		if(x1>ScreenWidth)	x1 = ScreenWidth;
	if(x2<1)	x2 = 1;		if(x2>ScreenWidth)	x2 = ScreenWidth;
	if(y1<1)	y1 = 1;		if(y1>ScreenHeight)	y1 = ScreenHeight;
	if(y2<1)	y2 = 1;		if(y2>ScreenHeight)	y2 = ScreenHeight;

	if(x1>x2)	{ int t = x1; x1 = x2; x2 = t; }
	if(y1>y2)	{ int t = y2; y2 = y1; y1 = t; }

	this->x1 = x1;	this->y1 = y1;
	this->x2 = x2;	this->y2 = y2;

	this->bkcolor = attrib>>4;
	this->color   = attrib&0x0F;

	this->Direction = direction;

	this->Left = 0;
	this->Top  = 0;

	this->ClrScr(x1, y1, x2, y2, (bkcolor<<4)|color);
}

ListBox::ListBox()
{
	this->x1 = this->x2 = 1;
	this->y1 = this->y2 = 1;

	this->Left = 0;
	this->Top  = 0;

	this->Direction = VERTICAL;

	this->bkcolor = LIGHTGRAY;
	this->color = BLACK;
}

ListBox::~ListBox()
{

}

void ListBox::Refresh()
{
	int ScreenWidth = this->GetScreenWidth();
	char far *Ptr = (char far*)(0xB8000000L + (y1-1)*ScreenWidth*2 + (x1-1)*2);
	char *p;	int k=0;
	char *curpos = (char *)this->GetContent();

	this->VisibleCount = 0;

	this->OnPreRefresh();
	DblList::Move(Top);

	if(this->Direction==VERTICAL)
	{
		do
		{
			p = (char *)this->GetContent();
			if(p!=NULL)
			{
			   char far *ptr = Ptr + k*ScreenWidth*2;
			   char far *endptr = ptr + (x2-x1+1)*2;

			   this->WriteStr(ptr, endptr, p, BOOL(p==curpos) );
			   this->VisibleCount++;
			}
			if(++k>=y2-y1+1)		break;
		}
		while(DblList::MoveNext());
		for(; k<y2-y1+1; k++)
		{
			char far *ptr = Ptr + k*ScreenWidth*2;
			char attrib = (bkcolor<<4) | color;
			char far *endptr = ptr + (x2-x1+1)*2;

			while(ptr<endptr)   { *ptr++ = ' '; *ptr++ = attrib; }
		}

	}
	else
	{
		char far *ptr = Ptr, far *endptr = ptr + (x2-x1+1) * 2;
		do
		{
			p = (char *)this->GetContent();
			if(p!=NULL)
			{
				this->WriteStr(ptr, endptr, p, BOOL(p==curpos) );
				this->VisibleCount++;
			}
			if(ptr>=endptr)	break;
		}
		while(DblList::MoveNext());

		while(ptr<endptr) { *ptr++ = ' '; *ptr++ = (bkcolor<<4) | color; }
	}

	DblList::Move(curpos);
}

void ListBox::WriteStr(char far * &ptr, char far * endptr, char * &p, BOOL IsFocus)
{
	char attrib = ((IsFocus?GREEN:bkcolor)<<4) | color;

	if(this->Direction==VERTICAL)
	{
	   while(ptr<endptr)
	   {
		 if(*p!='\0')  *ptr++ = *p++;
		 else		   *ptr++ = ' ';
		 *ptr++ = attrib;
	   }
   }
   else
   {
		if(ptr<endptr) { *ptr++ = ' ';	*ptr++ = attrib; }
		while(ptr<endptr)
		{
			if(*p!='\0')	{ *ptr++ = *p++; *ptr++ = attrib; }
			else 			break;
		}
		if(ptr<endptr) { *ptr++ = ' ';	*ptr++ = attrib; }
   }
}

void ListBox::OnPreRefresh()
{
	if(this->Direction==VERTICAL)
	{
		unsigned int Cursor = this->GetCursor();
		int Height = y2-y1+1;
		if(Cursor!=0xFFFF)
			if(Height+Top<=Cursor) Top = Cursor-Height+1;
			else if(Cursor<Top)	   Top = Cursor;
	}
	else
	{
		char *curpos = (char *)this->GetContent();
		int begin=0, end=0;
		DblList::Move(0);
		do
		{
			char *str = (char *)this->GetContent();
			if(str!=NULL)
			{
				if(str==curpos)	{ end = begin+strlen(str)+2; break; }
				else              begin += strlen(str)+2;
			}
		} while(DblList::MoveNext());

		int top1=0, top2=0, len=0, Width=x2-x1+1;
		DblList::Move(0);
		do
		{
			if(end > len+Width)   top1 ++;
			if(begin > len) 	  top2 ++;
			char *str = (char *)this->GetContent();
			len += str==NULL?0:strlen(str)+2;
		} while(DblList::MoveNext());

		Top = MAX(Top, top1);
		Top = MIN(Top, top2);
		DblList::Move(curpos);
	}
}

DblList::ListItem *ListBox::Insert(void *ptr, void *pos)
{
	DblList::ListItem *item = DblList::Insert(ptr, pos);
	this->AutoGrow();
	return item;
}

void ListBox::AutoGrow()
{
	int ScreenWidth = this->GetScreenWidth(), ScreenHeight = this->GetScreenHeight();
	if(this->Direction==VERTICAL)
	{
		if(this->IsExpandWidth)
		{
			void *curpos = this->GetContent();
			int maxlen = 0;
			DblList::Move(0);
			do
			{	char *str =(char*)this->GetContent();
				if(str!=NULL && strlen(str)>maxlen)	 maxlen = strlen(str);
			} while(DblList::MoveNext());
			x2 = MIN(ScreenWidth, x1+maxlen-1);
			DblList::Move(curpos);
		}

		if(this->IsExpandHeight)	y2 = MIN(ScreenHeight, y1+this->GetCount()-1);
	}
	else
	{
		if(this->IsExpandWidth)
		{
			void *curpos = this->GetContent();
			int len = 0;
			do
			{	char *str = (char*)this->GetContent();
				if(str!=NULL)   len += strlen(str) + 2;
			} while(DblList::MoveNext());
			x2 = MIN(ScreenWidth, x1+len-1);
			DblList::Move(curpos);
		}
	}
}

BOOL ListBox::MoveNext()
{
	if(DblList::MoveNext())	 { this->Refresh(); return TRUE; }
	else return FALSE;
}

BOOL ListBox::MovePrevious()
{
	if(DblList::MovePrevious())	{ this->Refresh(); return TRUE; }
	else return FALSE;
}

BOOL ListBox::Move(int pos)
{
	if(DblList::Move(pos))	{ this->Refresh(); return TRUE; }
	else return FALSE;
}

BOOL ListBox::Move(void *ptr)
{
	if(DblList::Move(ptr))	{ this->Refresh(); return TRUE; }
	else return FALSE;
}

void ListBox::SetPostion(int x, int y)
{
	int Width = x2-x1+1, Height = y2-y1+1;
	this->x1 = x;   			  this->y1 = y;
	this->x2 = this->x1+Width-1;  this->y2 = this->y1+Height-1;
	this->AutoGrow();
}

REPLYTYPE ListBox::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	if(!this->GetIsHasFocus())	return CONTINUE;

	if(type==KEY_PRESS)
	{
		unsigned int Key = argument;
		switch(Key)
		{
			case UP_KEY: 	if(this->Direction==VERTICAL) this->MovePrevious();   break;
			case DOWN_KEY:	if(this->Direction==VERTICAL) this->MoveNext();	  	  break;
			case LEFT_KEY:	if(this->Direction==HORIZONTAL) this->MovePrevious(); break;
			case RIGHT_KEY: if(this->Direction==HORIZONTAL) this->MoveNext(); 	  break;
			case HOME_KEY:	this->Move(0);	break;
			case END_KEY:   this->Move(this->GetCount()-1);     break;
		}
	}
	return CONTINUE;
}

