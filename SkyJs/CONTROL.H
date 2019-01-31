#ifndef CONTROL_H_2004_12
#define CONTROL_H_2004_12

#include "Screen.h"
#include "KeyBoard.h"

enum MSGTYPE
{
	NOMSG,
	FOCUS,			BLUR,
	KEY_PRESS,		KEY_UP,		KEY_DOWN,
	MOUSE_CLICK,	MOUSE_UP,	MOUSE_DOWN,	MOUSE_MOVE,	MOUSE_DBLCLICK,
	COMMAND
};

enum REPLYTYPE
{
	BREAK    = 0,
	CONTINUE = 1
};

typedef long MSGARGUMENT;

class Control : protected Screen, virtual protected KeyBoard
{
public:
	Control();
	virtual ~Control();

public:
	void AppendControl(Control *control);
	void RemoveControl(Control *control);
	BOOL GetIsHasFocus() { return BOOL(StaticFocusControl!=NULL && this==StaticFocusControl); }
	BOOL GetIsSubHasFocus();
	BOOL GetIsHasChild() { return BOOL(this->Home!=NULL); }
	BOOL GetIsTopControl() { return BOOL(this->ParentControl==NULL); }
	Control * GetTopControl();
	Control * GetParentControl() { return this->ParentControl; }
	Control * GetFocusControl();
	void SetFocusControl(Control *control);
	void SetFocus();
	REPLYTYPE Run();
	REPLYTYPE SendMessage(MSGTYPE type, MSGARGUMENT argument);

public:
	REPLYTYPE (*OnPreEvent)(Control *sender, MSGTYPE type, MSGARGUMENT argument);
	REPLYTYPE (*OnPostEvent)(Control *sender, MSGTYPE type, MSGARGUMENT argument);
	REPLYTYPE (*OnCommand)(Control *sender, MSGTYPE type, MSGARGUMENT argument);

protected:
	virtual REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument) = 0;

public:
	virtual void Refresh();

private:
	struct ControlItem
	{
	  ControlItem(Control *control);
	  ControlItem *next;
	  Control *control;
	} *Home;

public:
	ControlItem *GetList() { return Home; }

private:
	Control *ParentControl;
	Control *FocusControl;
	BOOL    IsSubHasFocus;

private:
	static MSGTYPE MsgType;
	static MSGARGUMENT MsgArgument;
	static Control *StaticFocusControl;
	static Control *GetParent(Control *myControl1, Control *myControl2);
};


#endif