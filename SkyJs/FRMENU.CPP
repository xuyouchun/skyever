#include "FrMenu.h"

FrameMenu::FrameMenu(int x1, int y1, unsigned char attrib, unsigned char StrickColor)
 : MulMenu(x1, y1, attrib, HORIZONTAL, StrickColor)
{
	this->x2 = --this->x1;
	this->y2 = --this->y1;
}

FrameMenu::~FrameMenu()
{
}

void FrameMenu::WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus)
{
	if(*p=='-' && *(p+1)=='\0')
	{
		*ptr++ = '³';  *ptr++ = (bkcolor<<4) | color;
	}
	else Menu::WriteStr(ptr, endptr, p, this->GetIsSubHasFocus()?IsFocus:FALSE);
}

void FrameMenu::Refresh()
{
	ListBox::Refresh();
}

void FrameMenu::AutoGrow()
{
	Menu::AutoGrow();
}

void FrameMenu::Hide()
{
	MulMenu *FocusMenu = (MulMenu*)this->GetFocusControl();
	if(FocusMenu!=NULL)	   FocusMenu->Hide();
	if(this->GetIsHasFocus())	this->SetFocusControl(NULL);
}

REPLYTYPE FrameMenu::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	switch(type)
	{
		case FOCUS: case BLUR:	this->Refresh(); break;
		case KEY_PRESS:
			if(!this->GetIsHasFocus())	return CONTINUE;
			switch(argument)
			{
				case UP_KEY: case ESC_KEY:
					this->SetFocusControl(NULL);
					return BREAK;
				default: return MulMenu::ReplyMessage(type, argument);
			}
			//break;
		case KEY_DOWN:
			switch(argument)
			{
				case ALT_CODE:
					if(this->StrickColor!=WHITE)
					{
						this->tempStrickColor = this->StrickColor;
						this->StrickColor = WHITE;
						if(this->GetIsHasFocus())   			this->SetFocusControl(NULL);
						else if(!this->GetIsHasExpandNode())	this->SetFocus();
						this->Refresh();
					}
					break;
				default:
					if(this->GetKeyState(ALT_STATUS))
					{
						MulMenu::MenuItem *item = (MulMenu::MenuItem *)this->GetItemByHotKey(argument);
						if(item!=NULL)
						{
							if(item->GetIsDir())
							{
								this->Hide();
								this->SetFocusControl(item->Dir);
								this->Move(item->Content);
							}
							else
							{
								this->Hide();
								this->SendMessage(COMMAND, MSGARGUMENT(item->Content));
								return BREAK;
							}
						}
					}
					else return MulMenu::ReplyMessage(type, argument);
					break;
			}
			break;
		case KEY_UP:
			switch(argument)
			{
				case ALT_CODE:
					if(this->StrickColor==WHITE)
					{
						this->StrickColor = this->tempStrickColor;
						this->Refresh();
					}
					break;
				default: return MulMenu::ReplyMessage(type, argument);
			}
			break;
		default: return MulMenu::ReplyMessage(type, argument);
	}
	return MulMenu::ReplyMessage(type, argument);
}
