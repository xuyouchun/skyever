#ifndef SKYJS_H_2004_12
#define SKYJS_H_2004_12

#include "Aplction.h"
#include "FrMenu.h"
#include "JsEditor.h"

class SkyJs : public Application
{
public:
	SkyJs();
	virtual ~SkyJs();

protected:
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);

private:
	JsEditor myJsEditor;
	FrameMenu  myFrameMenu;

private:
	void InitMenu();
	void InitEditor();

private:
	void InitScreen();
	static REPLYTYPE myJsEditor_OnPreEvent(Control *sender, MSGTYPE type, MSGARGUMENT argument);
	static REPLYTYPE myFrameMenu_OnCommand(Control *sender, MSGTYPE type, MSGARGUMENT argument);
};


#endif