#ifndef MENU_H_2004_12
#define MENU_H_2004_12

#include "ListBox.h"

class Menu : public ListBox
{
	class MenuItem;

public:
	Menu(int x1, int y1, unsigned char attrib, LISTDIRECTION Direction=VERTICAL, unsigned char StrickColor=RED);
	Menu();
	virtual ~Menu();

protected:
	unsigned char StrickColor;

public:
	virtual void Show();
	virtual void Hide();
	BOOL  GetIsShow() { return this->IsShow; }
	void InsertLine();
	DblList::ListItem *Insert(void *ptr, void *pos=NULL);
	MenuItem *GetItemByHotKey(unsigned int hotkey);

protected:
	DblList::ListItem *CreateItem(void *ptr);

public:
	void Refresh();

private:
	void *buffer;
	BOOL IsShow;

protected:
	void AutoGrow();
	void WriteStr(char far * &ptr, char far *endptr, char * &p, BOOL IsFocus);
	void OnPreRefresh();
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);

protected:
	int Strlen(const char *str);

private:
	struct MenuItem : public DblList::ListItem
	{
	   MenuItem(void *ptr);
	   virtual ~MenuItem();

	   unsigned int HotKey;
	};

};

#endif