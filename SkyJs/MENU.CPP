#include <alloc.h>
#include <string.h>

#include "Menu.h"


Menu::Menu(int x1, int y1, unsigned char attrib, LISTDIRECTION Direction, unsigned char StrickColor)
{
	this->IsShow = FALSE;

	this->x1 = MAX(x1+1, 1);
	this->y1 = MAX(y1+1, 1);
	this->x2 = this->x1;
	this->y2 = this->y1;

	this->Direction = Direction;
	this->bkcolor = attrib>>4;
	this->color = attrib&0x0F;

	this->StrickColor = StrickColor;
}

Menu::Menu()
{
	this->IsShow = FALSE;
	this->x1 = this->x2 = 1;
	this->y1 = this->y2 = 1;
	this->Direction = VERTICAL;
	this->bkcolor = LIGHTGRAY;
	this->color = BLACK;
	this->StrickColor = RED;
}

Menu::~Menu()
{
	this->Hide();
}

void Menu::Show()
{
	if(this->IsShow==TRUE)	return;

	this->buffer = malloc( (x2-x1+4)*(y2-y1+4)*2 );
	gettext(x1-1, y1-1, MIN(x2+2, this->GetScreenWidth()), MIN(y2+2, this->GetScreenHeight()), buffer);

	this->IsShow = TRUE;
	this->Refresh();
}

void Menu::Hide()
{
	if(this->IsShow==FALSE)	return;

	puttext(x1-1, y1-1, MIN(x2+2, this->GetScreenWidth()), MIN(y2+2, this->GetScreenHeight()), buffer);
	free(buffer);

	this->IsShow = FALSE;

	if(this->GetIsHasFocus())	this->SetFocusControl(NULL);
}

void Menu::AutoGrow()
{
	int ScreenWidth = this->GetScreenWidth(), ScreenHeight = this->GetScreenHeight();
	if(this->Direction==VERTICAL)
	{
		int maxlen = 0;
		MenuItem *item = (MenuItem*)this->GetHome();
		while(item!=NULL)
		{
			char *str = (char *)item->Content;
			if(str!=NULL && Strlen(str)>maxlen)		maxlen = Strlen(str);
			item = (MenuItem*)item->next;
		}
		x2 = x1+maxlen-1;
		if(x2>ScreenWidth)	{ x2 = ScreenWidth-1;  x1 = MAX(2, x2-(maxlen-1)); }

		y2 = y1+this->GetCount()-1;
		if(y2>ScreenHeight-1)	{ y2 = ScreenHeight-1; y1 = MAX(2, y2-(GetCount()-1)); }
	}
	else
	{
		int len = 0;
		MenuItem *item = (MenuItem *)this->GetHome();
		while(item!=NULL)
		{
			char *str = (char *)item->Content;
			if(str!=NULL)
				if(str[0]=='-'&&str[1]=='\0')	len += 1;
				else	len += Strlen(str) + 2;
			item = (MenuItem*)item->next;
		}

		x2 = x1+len-1;
		if(x2>ScreenWidth-1)	{ x2 = ScreenWidth-1; x1 = MAX(2, x2-(len-1)); }
	}
}

void Menu::InsertLine()
{
	this->Insert("-");
}

DblList::ListItem *Menu::CreateItem(void *ptr)
{
	return new MenuItem(ptr);
}

DblList::ListItem *Menu::Insert(void *ptr, void *pos)
{
	MenuItem *item = (MenuItem *)ListBox::Insert(ptr, pos);
	char *str = (char *)item->Content;
	while(*str!=NULL)
	{
		if(*str++=='&' && *str!='\0')  item->HotKey = this->TranslateKey(*str);
	}
	return item;
}

void Menu::Refresh()
{
	if(this->IsShow==FALSE)	return;

	char far *ptr, *endptr;
	int x1=this->x1-1, y1=this->y1-1, x2=this->x2+1, y2=this->y2+1;
	char attrib = (bkcolor<<4) | color;

	int ScreenWidth = this->GetScreenWidth(), ScreenHeight = this->GetScreenHeight();

	if(x2<ScreenWidth)
	{
		ptr = (char far*)(0xB8000000L + (y1-1)*ScreenWidth*2 + x2*2);
		for(int k=0; k<y2-y1; k++)
		{
			ptr += ScreenWidth * 2;
			*(ptr+1) = (BLACK<<4) | DARKGRAY;
		}
	}
	if(y2<ScreenHeight)
	{
		ptr = (char far*)(0xB8000000L + y2*ScreenWidth*2 + x1*2);
		endptr = ptr + (x2-x1)*2;
		while(ptr<=endptr) { ptr++;	*ptr++ = (BLACK<<4) | DARKGRAY; }
	}

	ptr = (char far*)(0xB8000000L + (y1-1)*ScreenWidth*2 + (x1-1)*2);
	endptr = ptr + (x2-x1+1) * 2;
	*ptr++ = 'Ú'; *ptr++ = attrib;
	while(ptr<endptr-2)   { *ptr++ = 'Ä'; *ptr++ = attrib; }
	*ptr++ = '¿'; *ptr++ = attrib;

	ptr = (char far*)(0xB8000000L + (y2-1)*ScreenWidth*2 + (x1-1)*2);
	endptr = ptr + (x2-x1+1) * 2;
	*ptr++ = 'À'; *ptr++ = attrib;
	while(ptr<endptr-2)   { *ptr++ = 'Ä'; *ptr++ = attrib; }
	*ptr++ = 'Ù'; *ptr++ = attrib;

	ListBox::Refresh();

	if(this->Direction == HORIZONTAL)
	{
		ptr = (char far*)(0xB8000000L + y1*ScreenWidth*2 + (x1-1)*2);
		*ptr = Top>0?'®':'³';	   *(ptr+1) = (bkcolor<<4) | color | (Top>0?0x80:0x00);
		BOOL IsAtEnd = BOOL(Top+VisibleCount>=this->GetCount());
		*(ptr+(x2-x1)*2) = IsAtEnd?'³':'¯';    *(ptr+(x2-x1)*2+1) = (bkcolor<<4) | color | (IsAtEnd?0x00:0x80);
	}
}

void Menu::WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus)
{
	char attrib = ((IsFocus?GREEN:bkcolor)<<4) | color;
	int ScreenWidth = this->GetScreenWidth();
	if(this->Direction==VERTICAL)
	{
		if(*p=='-' && *(p+1)=='\0')
		{
			*(ptr-2) = 'Ã';		*(ptr-1) = attrib;
			while(ptr<endptr)	{ *ptr++ = 'Ä'; *ptr++ = attrib; }
			*ptr = '´';			*(ptr+1) = attrib;
		}
		else
		{
			*(ptr-2) = '³';		*(ptr-1) = (bkcolor<<4) | color;

			while(ptr<endptr)
			{
				if(*p=='&')			{ if(*++p!='\0') { *ptr++ = *p++; *ptr++ = ((IsFocus?GREEN:bkcolor)<<4) | StrickColor; } }
				else if(*p!='\0')	{ *ptr++ = *p++; *ptr++ = attrib; }
				else				{ *ptr++ = ' ';  *ptr++ = attrib; }
			}

			*(ptr) = '³';		*(ptr+1) = (bkcolor<<4) | color;
		}
	}
	else
	{
		if(*p=='-' && *(p+1)=='\0')
		{
			*(ptr-ScreenWidth*2) = 'Â'; *(ptr-ScreenWidth*2+1) = (bkcolor<<4) | color;
			*ptr = '³';		*(ptr+1) = (bkcolor<<4) | color;
			*(ptr+ScreenWidth*2) = 'Á'; *(ptr+ScreenWidth*2+1) = (bkcolor<<4) | color;
			ptr += 2;
		}
		else
		{
			if(ptr<endptr) { *ptr++ = ' ';	*ptr++ = attrib; }
			while(ptr<endptr)
			{
				if(*p=='&')			{ if(*++p!='\0') { *ptr++ = *p++; *ptr++ = ((IsFocus?GREEN:bkcolor)<<4) | StrickColor; } }
				else if(*p!='\0')	{ *ptr++ = *p++; *ptr++ = attrib; }
				else 			break;
			}
			if(ptr<endptr) { *ptr++ = ' ';	*ptr++ = attrib; }
		}
	}
}

int Menu::Strlen(const char *str)
{
	const char *ptr = str;
	int len = 0;
	while(*ptr!='\0')	if(*ptr++!='&')		len++;
	return len;
}

void Menu::OnPreRefresh()
{
	if(this->Direction==VERTICAL)  ListBox::OnPreRefresh();
	else
	{
		int begin=0, end=0;
		char *curpos = (char *)this->GetContent();
		MenuItem *item = (MenuItem*)this->GetHome();
		while(item!=NULL)
		{
			char *str = (char *)item->Content;
			if(str!=NULL)
			{
				int len;
				if(str[0]=='-' && str[1]=='\0')  len = 1;
				else							 len = Strlen(str)+2;
				if(str==curpos)	{ end = begin+len; break; }
				else              begin += len;
			}
			item = (MenuItem*)item->next;
		}

		int top1=0, top2=0, len=0, Width=x2-x1+1;
		item = (MenuItem*)this->GetHome();
		{
			if(end > len+Width)   top1 ++;
			if(begin > len) 	  top2 ++;
			char *str = (char *)item->Content;
			if(str!=NULL)
			{
				int length;
				if(str[0]=='-' && str[1]=='\0')  length = 1;
				else							 length = Strlen(str) + 2;
				len += length;
			}
			item = (MenuItem*)item->next;
		}

		Top = MAX(Top, top1);
		Top = MIN(Top, top2);
		DblList::Move(curpos);
	}
}

REPLYTYPE Menu::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	switch(type)
	{
		case FOCUS:	this->Show(); break;
		case KEY_PRESS:
			if(!this->GetIsHasFocus())	return CONTINUE;
			switch(argument)
			{
				case DOWN_KEY: case RIGHT_KEY:
					if(argument==DOWN_KEY&&this->Direction==VERTICAL ||
						argument==RIGHT_KEY&&this->Direction==HORIZONTAL)
					{
						while(DblList::MoveNext())
						{
							char *str = (char *)this->GetContent();
							if(!(*str=='-'&&*(str+1)=='\0'))	break;
						}
						this->Refresh();
					}
					return BREAK;
				case UP_KEY: case LEFT_KEY:
					if(argument==UP_KEY&&this->Direction==VERTICAL ||
						argument==LEFT_KEY&&this->Direction==HORIZONTAL)
					{
						while(DblList::MovePrevious())
						{
							char *str = (char *)this->GetContent();
							if(!(*str=='-'&&*(str+1)=='\0'))	break;
						}
						this->Refresh();
					}
					return BREAK;
				case ENTER_KEY:
					this->Hide();
					return this->SendMessage(COMMAND, MSGARGUMENT(this->GetContent()));
				default:
					unsigned int keycode = this->GetKeyCode();
					MenuItem *item = (MenuItem *)this->GetItemByHotKey(keycode);
					if(item!=NULL)
					{
						this->Hide();
						return this->SendMessage(COMMAND, MSGARGUMENT(item->Content));
					}
					else return ListBox::ReplyMessage(type, argument);
			}
		default:
			return ListBox::ReplyMessage(type, argument);
	}
	return CONTINUE;
}

Menu::MenuItem *Menu::GetItemByHotKey(unsigned int hotkey)
{
	if(hotkey==0)	return NULL;

	MenuItem *item = (MenuItem*)this->GetHome();
	while(item!=NULL)
	{
		if(item->HotKey==hotkey)	return item;
		item = (MenuItem*)item->next;
	}
	return NULL;
}

///////////////////////////// MenuItem ///////////////////////////////

Menu::MenuItem::MenuItem(void *ptr) : DblList::ListItem(ptr)
{
	this->HotKey = 0;
}

Menu::MenuItem::~MenuItem()
{
}

