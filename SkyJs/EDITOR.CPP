#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <alloc.h>
#include <string.h>

#include "KeyBoard.h"

#include "Editor.h"

//////////////////////////// class Editor /////////////////////////////

Editor::Editor(int x1, int y1, int x2, int y2, unsigned char attrib, unsigned int step)
 : MulStr(step)
{
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
	this->color	  = attrib&0x0F;

	this->CurLine = 0;
	this->Cursor  = 0;
	this->Left    = 0;
	this->Top	  = 0;

	InitScreen();

	this->myRow = NULL;
}

Editor::~Editor()	//virtual
{
	ClearScreen();
}

void Editor::InitScreen()	//virtual protected
{
	this->ClrScr(x1, y1, x2, y2, (bkcolor<<4)|color);
	gotoxy(this->x1, this->y1);
}

void Editor::ClearScreen()	//virtual protected
{
	textbackground(BLACK);
	clrscr();
}

#define COLSTEP 10
#define TABSIZE 4

REPLYTYPE Editor::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	if(!this->GetIsHasFocus())	return CONTINUE;

	if(myRow==NULL)
	{
		myRow = new Row(this->FindRow(this->CurLine));
		myRow->SetCursor(this->Cursor);
		this->WriteLine(myRow);
	}

	if(type==KEY_PRESS && argument!=0)
	{
		unsigned key = argument;
		switch(key)
		{
			case DELETE_KEY:
			{
				unsigned int Height = y2-y1+1;
				if(bioskey(2)&(char)SHIFT_STATUS)	//Delete Row
				{
				   this->DeleteRow(CurLine);
				   if(Top>0 && Lines-Top<Height)	Top --;
				   Cursor = 0;
				   if(CurLine==this->Lines)	CurLine--;
				   this->Refresh();
					   delete myRow; myRow = NULL;
				}
				else	  //Delete Char
				{
					unsigned int Len = myRow->GetLength();
					if(Cursor<Len)
					{
						char *ptr = myRow->GetString()+Cursor;
						char *endptr = ptr + myRow->GetLength();
						if(*ptr!=' ')	myRow->DeleteChar();
						else
						{
							int size = 0;
							while(ptr<endptr && *ptr++==' ')	size ++;
							int count = MIN(TABSIZE, size);
							for(int k=0; k<count; k++)  myRow->DeleteChar();
						}
						this->WriteLine(myRow);
					}
					else
					{
						if(CurLine<Lines-1)
						{
							unsigned Len = myRow->GetLength();
							this->ReplaceRow(CurLine, myRow->GetString(), MAX(Cursor, Len));
							delete myRow;  myRow = NULL;
							myRow = new Row(this->FindRow(CurLine+1));
							this->InsertStr(CurLine, 0xFFFF, myRow->GetString(), myRow->GetLength());
							this->DeleteRow(CurLine+1);

							if(Top>0 && Lines-Top<Height)	Top --;
							this->Refresh();

							delete myRow;  myRow = NULL;
						}
					}
				}
			}
				break;
			case BACKSPACE_KEY:
			{
				if(Cursor>0)
				{
					int count1 = Cursor%TABSIZE;
					if(count1==0)	count1 = TABSIZE;
					int count;
					char *ptr = myRow->GetString() + Cursor - 1;
					if(*ptr!=' ')	count = 1;
					else for(count=0; count<count1; count++) if(*ptr--!=' ') break;
						Cursor -= MIN(Cursor, count);
					myRow->SetCursor(Cursor);
					for(int k=0; k<count; k++)	myRow->DeleteChar();
					this->WriteLine(myRow);
				}
				else if(CurLine>0)
				{
					if(--CurLine<Top)	Top --;
					unsigned int Len = myRow->GetLength();
					this->InsertStr(CurLine, 0xFFFF, myRow->GetString(), Len);
					this->DeleteRow(CurLine+1);
					this->Refresh();
					delete myRow;
					myRow = new Row(this->FindRow(this->CurLine));
					Cursor = myRow->GetLength()-Len;
					myRow->SetCursor(Cursor);
					this->WriteLine(myRow);
				}
			}
				break;
			case LEFT_KEY:
			{
				if(Cursor>0) myRow->SetCursor(--Cursor);
				if(Cursor<Left) { Left --; this->Refresh(); }
				this->WriteLine(myRow);
			}
				break;
			case RIGHT_KEY:
			{
				int Width = x2-x1+1;
				myRow->SetCursor(++Cursor);
				if(Cursor-Left>=Width)	{ Left++; this->Refresh(); }
				this->WriteLine(myRow);
			}
				break;
			case HOME_KEY:
				myRow->SetCursor( (Cursor=0) );
				if(Left>0) { Left=0; this->Refresh(); }
				this->WriteLine(myRow);
				break;
			case END_KEY:
			{
				int Width = x2-x1+1;
				myRow->SetCursor( (Cursor=myRow->GetLength()) );
				if(Cursor>=Width)
				{
					Left = Cursor-Width+1;
					this->Refresh();
				}
				else if(Left>0)
				{
					Left = 0;
					this->Refresh();
				}
				this->WriteLine(myRow);
			}
				break;
			case UP_KEY:
			{
				this->ReplaceRow(CurLine, myRow->GetString(), myRow->GetLength());
				if(CurLine>0)	CurLine --;
				if(CurLine<Top)	{ Top --; this->Refresh(); }
				delete myRow;	myRow = NULL;
			}
				break;
			case DOWN_KEY:
			{
				this->ReplaceRow(CurLine, myRow->GetString(), myRow->GetLength());
				int Height = y2-y1+1;
				if(CurLine<Lines-1)		CurLine ++;
				if(Top+Height<=CurLine)	{ Top ++; this->Refresh(); }
				delete myRow;	myRow = NULL;
			}
				break;
			case TABLE_KEY:
			{
				int count = TABSIZE-Cursor%TABSIZE;
				for(int k=0; k<count; k++)	myRow->InsertChar(' ');
				Cursor = myRow->GetCursor();
				int Width = x2-x1+1;
				if(Cursor-Left>=Width) { Left+=MIN(COLSTEP,Width-1); this->Refresh(); }
				this->WriteLine(myRow);
			}
				break;
			case ENTER_KEY:
			{
				char *str1 = myRow->GetString();
				char *str2 = str1 + Cursor;
				int  len = myRow->GetLength();
				char *endstr1 = str1 + Cursor;
				this->ReplaceRow(CurLine, str1, endstr1-str1);

				this->InsertRow(++CurLine);
				char *endstr2 = str1 + len;
				if(len>Cursor)	this->ReplaceRow(CurLine, str2, endstr2-str2);
				int Height = y2-y1+1;
				if(Top+Height<=CurLine)	Top ++;

				int count = 0;
				char *str=str1, *endstr=str+Cursor;
				while(str<endstr) if(*str++==' ') count ++; else break;
				delete myRow;
				myRow = new Row(this->FindRow(CurLine));

				Cursor = count; Left = 0;
				int Width = x2-x1+1;
				if(Cursor>=Left+Width-1)   Left = Cursor-Width + MIN(COLSTEP,Width-1);
				if(myRow->GetLength()==0)  myRow->SetCursor(Cursor);
				else
				{
					myRow->SetCursor(0);
					for(int k=0; k<Cursor; k++)	myRow->InsertChar(' ');
				}
				this->Refresh();
				this->WriteLine(myRow);
			}
				break;
			case PAGEUP_KEY:
				if(this->CurLine>0)
				{
					this->ReplaceRow(CurLine, myRow->GetString(), myRow->GetLength());
					int Height = y2-y1+1;
					if(Lines<Height) { CurLine = 0; Top = 0; }
					else
					{
						if(Top==0)  CurLine = 0;
						else if(Top<Height) { CurLine=CurLine-Top; Top=0; }
						else  { CurLine-=Height; Top-=Height; }
						this->Refresh();
					}

					delete myRow; myRow = NULL;
				}
				break;
			case PAGEDOWN_KEY:
				if(CurLine<this->Lines-1)
				{
					this->ReplaceRow(this->CurLine, myRow->GetString(), myRow->GetLength());
					int Height = y2-y1+1;

					if(Lines<Height) { CurLine = Lines-1; Top = 0; }
					else
					{
						if(Top==Lines-Height)	CurLine = Lines-1;
						else if(Top+2*Height>Lines-1)	{ CurLine=(Lines-Height)+(CurLine-Top); Top=Lines-Height; }
						else { CurLine+=Height; Top+=Height; }
					}
					this->Refresh();

					delete myRow; myRow = NULL;
				}
				break;

			default:
				if(key>=' ' && key<0x8F)
				{
					myRow->InsertChar(key);
					Cursor = myRow->GetCursor();
					int Width = x2-x1+1;
					if(Cursor-Left>=Width) { Left+=COLSTEP>Width-1?Width-1:COLSTEP; this->Refresh(); }
					this->WriteLine(myRow);
				}
				break;
		}
	}
	return CONTINUE;
}

void Editor::WriteLine(char *Str, unsigned int Length)
{
	char far *ptr = (char far*)(0xB8000000L + (CurLine-Top+y1-1)*this->GetScreenWidth()*2 + (x1-1)*2 );
	char far *EndPtr = ptr + (x2-x1+1)*2;

	unsigned int Len = Length-Left;
	if(Len>x2-x1+1)	Len = x2-x1+1;
	char *ptr0 = Str;

	if(Length>=Left) { this->WriteStr(ptr, ptr0, Left, Len); ptr += Len*2; }

	char attrib = (bkcolor<<4) | color;
	while(ptr<EndPtr)
	{
		*ptr++ = ' ';
		*ptr++ = attrib;
	}

	int x = x1+Cursor-Left, y = y1+CurLine-Top;
	gotoxy(x>x2?x2:x<x1?x1:x, y>y2?y2:y<y1?y1:y);
}

void Editor::WriteStr(char far *ptr, char *str, unsigned int start, unsigned int len)
{
	str += start;
	char *End = str + len;
	char attrib = (bkcolor<<4) | color;

	while(str<End)
	{
		*ptr++ = *str++;
		*ptr++ = attrib;
	}
}

void Editor::WriteLine(Row *myRow)
{
	this->WriteLine(myRow->GetString(), myRow->GetLength());
}

void Editor::Refresh()
{
	int Height = y2-y1+1;
	int CurLine = this->CurLine;

	if(this->Cursor<this->Left)			this->Cursor = Left;

	char *ptr = this->FindRow(this->Top);
	for(int k=0; k<Height; k++)
	{
		if(ptr==NULL)	break;
		char *ptr0 = ptr;
		while(*ptr0!='\n')	ptr0 ++;
		this->CurLine = this->Top + k;
		this->WriteLine(ptr, ptr0-ptr);

		ptr = ++ptr0;
		if(*ptr=='\0') for(k++; k<Height; k++)
		{
			char attrib = (bkcolor<<4) | color;
			char far *Ptr = (char far*)(0xB8000000L + (k+y1-1)*this->GetScreenWidth()*2 + (x1-1)*2);
			char far *EndPtr = Ptr + (x2-x1+1)*2;
			while(Ptr<EndPtr)  { *Ptr++ = ' '; *Ptr++ = attrib; }
		}
	 }
	this->CurLine = CurLine;
}

char *Editor::GetString()
{
	if(this->myRow!=NULL)
	{
		this->ReplaceRow(CurLine, myRow->GetString(), myRow->GetLength());
		delete myRow; myRow = NULL;
	}
	return String::GetString();
}

unsigned int Editor::GetLength()
{
	if(this->myRow!=NULL)
	{
		this->ReplaceRow(CurLine, myRow->GetString(), myRow->GetLength());
		delete myRow; myRow = NULL;
	}
	return String::GetLength();
}

/////////////////////////// class Row //////////////////////////////
#define STEP 30

Editor::Row::Row(char *str) : String(str, GetLength(str), STEP)
{
	memset(Str+Length, ' ', Size-Length);
	this->Cursor = 0;
}

unsigned int Editor::Row::GetLength(char *str)
{
	char *ptr = str;
	while(*ptr++ != '\n');
	return ptr-str-1;
}

Editor::Row::~Row()
{
	if(this->Str!=NULL)
	{
		free(this->Str);
		this->Str = NULL;
	}
}

unsigned int Editor::Row::GetLength()
{
	char *ptr = this->Str+this->Size-1;
	while(*ptr==' ')	ptr--;
	this->Length = ptr-this->Str+1;
	return this->Length;
}

void Editor::Row::Realloc()
{
	unsigned int size = this->Size;
	String::Realloc();
	if(this->Size>size)	memset(this->Str+size, ' ', this->Size-size);
}

int Editor::Row::GetCursor()
{
	return this->Cursor;
}

void Editor::Row::SetCursor(unsigned int cursor)
{
	this->Cursor = cursor;
	if(cursor>=this->Size)
	{
		this->Length = cursor;
		this->Realloc();
		this->GetLength();
	}
}

void Editor::Row::InsertChar(char ch)
{
	String::InsertChar(Cursor, ch);
	this->Cursor++;
	this->GetLength();
}

void Editor::Row::DeleteChar()
{
	String::DeleteChar(this->Cursor);
	this->GetLength();
}
