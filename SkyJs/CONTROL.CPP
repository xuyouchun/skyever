#include <conio.h>
#include "Control.h"

Control::Control()
{
	this->ParentControl = NULL;
	this->FocusControl = NULL;

	this->OnPreEvent = NULL;
	this->OnPostEvent = NULL;
	this->OnCommand = NULL;

	this->Home = NULL;
}

Control::~Control()
{
	ControlItem *p = this->Home;
	while(p!=NULL)
	{
		ControlItem *p0 = p->next;
		if(p->control->GetIsHasFocus())  this->SetFocusControl(NULL);
		delete p;
		p = p0;
	}
}

MSGTYPE Control::MsgType = NOMSG;
MSGARGUMENT Control::MsgArgument = 0;
Control *Control::StaticFocusControl = NULL;

REPLYTYPE Control::Run()
{
start:
	if(FocusControl==NULL&&GetIsHasChild())
	{
		if(StaticFocusControl==NULL)	this->SetFocusControl(Home->control);
		else							this->FocusControl = Home->control;
	}

	if(GetIsTopControl())
	{
		MsgType = NOMSG;  MsgArgument = 0;
		unsigned int key = this->GetKeyPress();
		if(key!=NULL)
		{
		   KEYTYPE keytype = this->GetKeyType();
		   MsgType = keytype==KEYTYPE_UP?KEY_UP:keytype==KEYTYPE_DOWN?KEY_DOWN:KEY_PRESS;
		   MsgArgument = (MSGARGUMENT)key;
		}
	}

	REPLYTYPE result = CONTINUE;
	if(this->OnPreEvent==NULL || this->OnPreEvent(this, MsgType, MsgArgument)!=BREAK)
	{
		result = this->ReplyMessage(MsgType, MsgArgument);
		if(this->OnPostEvent!=NULL)	this->OnPostEvent(this, MsgType, MsgArgument);
	}

	if(result!=BREAK && this->GetIsHasChild())
	{
		ControlItem *p = this->Home;
		while(p!=NULL)
		{
			p->control->Run();
			p = p->next;
		}
	}

	if(result==BREAK)   return BREAK;

	if(this->GetIsTopControl())	goto start;

	return result;
}

REPLYTYPE Control::SendMessage(MSGTYPE type, MSGARGUMENT argument)
{
	if(this->OnCommand!=NULL)	return this->OnCommand(this, type, argument);
	else						return CONTINUE;
}

void Control::SetFocusControl(Control *control)
{
	Control *Old = StaticFocusControl, *New = control;
	if(Old==New)	return;
	Control *Parent;
	if(Old==NULL||New==NULL)	Parent = this->GetTopControl();
	else						Parent = GetParent(Old, New);

	static long Flag = 0;
	long flag = ++Flag;

	if(Old!=NULL)			//blur
	{
		Control *p = Old;
		while(p!=Parent)
		{
			StaticFocusControl = p->ParentControl;

			if(p->OnPreEvent!=NULL)		p->OnPreEvent(p, BLUR, 0);
			if(!p->GetIsHasFocus())
			{
				p->ReplyMessage(BLUR, 0);
				if(p->OnPostEvent!=NULL)	p->OnPostEvent(p, BLUR, 0);
			}
			if(flag!=Flag)	goto end;
			p = p->ParentControl;
		}
	}

	if(New!=NULL)
	{
		Control *myControl = New;
		while(myControl != Parent)
		{
			myControl->ParentControl->FocusControl = myControl;
			myControl = myControl->ParentControl;
		}

		Control *p = Parent;
		do
		{
			p = p->GetFocusControl();
			StaticFocusControl = p;

			if(p->OnPreEvent!=NULL)	  p->OnPreEvent(p, FOCUS, 0);
			if(p->GetIsHasFocus())
			{
				p->ReplyMessage(FOCUS, 0);
				if(p->OnPostEvent!=NULL)	p->OnPostEvent(p, FOCUS, 0);
			}
			if(flag!=Flag)	goto end;
		} while(p!=New);
	}
	else
	{
		StaticFocusControl = NULL;
		GetTopControl()->FocusControl = NULL;
	}

	end: return;
}

void Control::SetFocus()
{
	if(this->ParentControl==NULL)	return;
	else this->ParentControl->SetFocusControl(this);
}

BOOL Control::GetIsSubHasFocus()
{
	Control *p = StaticFocusControl;
	while(p!=NULL)
	{
		if(p==this)	 return TRUE;
		p = p->ParentControl;
	}
	return FALSE;
}

Control *Control::GetFocusControl()
{
	if(this->FocusControl==NULL&&this->Home!=NULL)    this->FocusControl = this->Home->control;
	return this->FocusControl;
}

void Control::AppendControl(Control *control)
{
	if(control->ParentControl!=NULL)	return;

	ControlItem *item = new ControlItem(control);
	control->ParentControl = this;

	if(Home==NULL) 	Home=item;
	else
	{
		ControlItem *p = Home;
		while(p!=NULL)
		{
			if(p->next==NULL) { p->next = item; break; }
			p = p->next;
		}
	}
}

void Control::RemoveControl(Control *control)
{
	if(Home->control==control)	{ Home = Home->next; delete Home; }
	else
	{
		ControlItem *p = Home;
		while(p!=NULL)
		{
			if(p->next->control==control)
			{
				ControlItem *p0 = p->next;	p->next = p0->next;
				if(control->GetIsHasFocus())   SetFocusControl(NULL);
				control->ParentControl = NULL;
				delete p0;	break;
			}
			p = p->next;
		}
	}
	if(FocusControl==control)	this->FocusControl = NULL;
}

void Control::Refresh()
{
	ControlItem *p = this->Home;
	while(p!=NULL)
	{
		p->control->Refresh();
		p = p->next;
	}
}

Control * Control::GetTopControl()
{
	Control *p = this;
	while(p->ParentControl!=NULL)	p = p->ParentControl;
	return p;
}

Control * Control::GetParent(Control *myControl1, Control *myControl2)
{
	if(myControl1==NULL || myControl2==NULL)	return NULL;

	Control *p = myControl1;
	while(p!=NULL)
	{
		Control *p0 = myControl2;
		while(p0!=NULL)
		{
			p0 = p0->ParentControl;
			if(p0==p)	return p;
		}
		p = p->ParentControl;
	}
	return NULL;
}

////////////////////////////// ControlItem /////////////////////////////

Control::ControlItem::ControlItem(Control *control)
{
	this->control = control;
	this->next = NULL;
}
