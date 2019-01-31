#include "MulMenu.h"

MulMenu::MulMenu(int x1, int y1, unsigned char attrib, LISTDIRECTION Direction, unsigned char StrickColor)
 : Menu(x1, y1, attrib, Direction, StrickColor)
{
	this->Parent = NULL;
}

MulMenu::MulMenu()
{
	this->Parent = NULL;
}

MulMenu::~MulMenu()
{
	MenuItem *p = (MenuItem*)this->GetHome();
	while(p!=NULL)
	{
		if(p->Dir!=NULL)  delete p->Dir;
		p = (MenuItem*)p->next;
	}
}

DblList::ListItem * MulMenu::CreateItem(void *ptr)
{
	return new MenuItem(ptr);
}

MulMenu * MulMenu::CreateDir(void *ptr, LISTDIRECTION Direction)
{
	MenuItem *item = (MenuItem *)this->Insert(ptr);
	MulMenu *menu = new MulMenu(1, 1, (bkcolor<<4)|color, Direction, StrickColor);
	item->Dir = menu;
	menu->Parent = this;
	this->AppendControl(menu);
	this->AutoGrow();
	return menu;
}

void MulMenu::Hide()
{
	ControlItem *p = this->GetList();
	while(p!=NULL)
	{
		MulMenu *menu = (MulMenu*)p->control;
		menu->Hide();
		p = p->next;
	}
	this->Menu::Hide();
	if(this->GetIsHasFocus())
		this->SetFocusControl(NULL);
}

void MulMenu::Show()
{
	if(this->GetIsSubMenu())
	{
		int x1 = Parent->x1, y1 = Parent->y1;
		if(Parent->Direction==VERTICAL)
		{
			MenuItem *item = (MenuItem *)Parent->GetHome();
			int index = 0;
			while(item->Dir!=this)
			{
				index ++;
				item = (MenuItem*)item->next;
			}
			this->SetPostion(x1+1, y1+index+2);
		}
		else
		{
			MenuItem *item = (MenuItem *)Parent->GetHome();
			int pos = 0;
			while(item->Dir!=this)
			{
				char *str = (char *)item->Content;
				pos += this->Strlen(str);
				if( !(*str=='-' && *(str+1)=='\0') )    pos += 2;
				item = (MenuItem*)item->next;
			}
			this->SetPostion(x1+pos+1, y1+2);
		}
	}
	Menu::Show();
}

void MulMenu::AutoGrow()
{
	Menu::AutoGrow();
	int ScreenWidth = this->GetScreenWidth();

	MenuItem *p = (MenuItem*)this->GetHome();
	int DirCount=0;
	while(p!=NULL)
	{
		if(p->GetIsDir())	DirCount ++;
		p = (MenuItem*)p->next;
	}

	if(DirCount>0)
	{
		if(this->Direction == VERTICAL)   this->x2 += 2;
		else		   this->x2 += DirCount;

		if(x2>ScreenWidth-1)	{ x1 -= x2-(ScreenWidth-1); x2 = ScreenWidth-1; }
		x1 = MAX(2, x1);
	}
}

void MulMenu::WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus)
{
	Menu::WriteStr(ptr, endptr, p, IsFocus);
	MenuItem *item = (MenuItem *)this->GetPoint();
	if(item->Dir!=NULL)
	{
		if(this->Direction==VERTICAL)
		{
			*(endptr-2) = 16;
			*(endptr-1) = ((IsFocus?GREEN:bkcolor)<<4)|color|0x80;
		}
		else
		{
			*(ptr-2) = 31;
			*(ptr-1) = ((IsFocus?GREEN:bkcolor)<<4)|color|0x80;
		}
	}
}

MulMenu *MulMenu::GetTopMenu()
{
	MulMenu *p = this;
	while(p->Parent!=NULL)	p = p->Parent;
	return p;
}

BOOL MulMenu::GetIsHasExpandNode()
{
	MenuItem *menu = (MenuItem *)this->GetHome();
	while(menu!=NULL)
	{
		if(menu->GetIsDir() && menu->Dir->GetIsShow())	return TRUE;
		menu = (MenuItem *)menu->next;
	}
	return FALSE;
}

REPLYTYPE MulMenu::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	switch(type)
	{
		case FOCUS:	this->Show(); break;
		case KEY_PRESS:
			if(!this->GetIsHasFocus())	return CONTINUE;
			switch(argument)
			{
				case RIGHT_KEY: case DOWN_KEY: case ENTER_KEY:
					if(argument==RIGHT_KEY&&this->Direction==VERTICAL ||
					   argument==DOWN_KEY&&this->Direction==HORIZONTAL ||
					   argument==ENTER_KEY)
					{
					   MenuItem *item = (MenuItem*)this->GetPoint();
					   if(item->GetIsDir())
					   {
							item->Dir->Show();
							item->Dir->SetFocus();
					   }
					   else if(argument==ENTER_KEY)
					   {
							this->GetTopMenu()->Hide();
							(this->OnCommand==NULL?this->GetTopMenu():this)
								->SendMessage(COMMAND, MSGARGUMENT(this->GetContent()));
					   }
					   //else	if(this->Parent!=NULL)	this->Parent->MoveNext();
					   return BREAK;
					}
					else return Menu::ReplyMessage(type, argument);
					break;
				case LEFT_KEY: case UP_KEY: case ESC_KEY: case BACKSPACE_KEY:
					if(argument==LEFT_KEY&&this->Direction==VERTICAL ||
					   argument==UP_KEY&&this->Direction==HORIZONTAL ||
					   argument==ESC_KEY || argument==BACKSPACE_KEY)
					{
						this->Hide();
						if(this->GetIsSubMenu())  this->GetParentMenu()->SetFocus();
						return BREAK;
					}
					else return Menu::ReplyMessage(type, argument);
					//break;
				default:
					MenuItem *item = (MenuItem *)this->GetItemByHotKey(this->GetKeyCode());
					if(item!=NULL)
					{
						if(item->GetIsDir())
						{
							this->Move(item->Content);
							item->Dir->Show();
							item->Dir->SetFocus();
							return BREAK;
						}
						else
						{
							this->GetTopMenu()->Hide();
							(this->OnCommand==NULL?this->GetTopMenu():this)
								->SendMessage(COMMAND, MSGARGUMENT(this->GetContent()));
						}
					}
					else return Menu::ReplyMessage(type, argument);
					break;
			}
			break;
		default: return Menu::ReplyMessage(type, argument);
	}
}

////////////////////////////// MenuItem ////////////////////////////////

MulMenu::MenuItem::MenuItem(void *ptr) : Menu::MenuItem(ptr)
{
	this->Dir = NULL;
}

MulMenu::MenuItem::~MenuItem()
{
}

