#ifndef FRAMEMENU_H_2005_01
#define FRAMEMENU_H_2005_01

#include "MulMenu.h"

class FrameMenu : public MulMenu
{
public:
	FrameMenu(int x1=1, int y1=1, unsigned char attrib = (LIGHTGRAY<<4)|BLACK, unsigned char StrickColor=RED);
	virtual ~FrameMenu();

private:
	unsigned char tempStrickColor;

protected:
	void Refresh();
	void AutoGrow();
	void WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus);
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);
	void Hide();
};


#endif