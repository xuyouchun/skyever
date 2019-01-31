#ifndef MULMENU_H_2004_12
#define MULMENU_H_2004_12

#include "Menu.h"

class MulMenu : public Menu
{
public:
	MulMenu();
	MulMenu(int x1, int y1, unsigned char attrib=(LIGHTGRAY<<4)|BLACK, LISTDIRECTION Direction=VERTICAL, unsigned char StrickColor=RED);
	virtual ~MulMenu();

protected:
	DblList::ListItem *CreateItem(void *ptr);

public:
	virtual MulMenu *CreateDir(void *ptr, LISTDIRECTION Direction=VERTICAL);

public:
	BOOL GetIsSubMenu() { return BOOL(this->Parent!=NULL); }
	MulMenu *GetParentMenu() { return this->Parent; }
	MulMenu *GetTopMenu();
	void Hide(), Show();
	BOOL GetIsHasExpandNode();

private:
	struct MenuItem : public Menu::MenuItem
	{
		MenuItem(void *ptr);
		virtual ~MenuItem();

		BOOL GetIsDir() { return BOOL(this->Dir!=NULL); }
		MulMenu *Dir;
	};

private:
	MulMenu *Parent;

protected:
	void AutoGrow();
	void WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus);
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);
};


#endif