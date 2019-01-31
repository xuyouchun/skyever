#ifndef LISTBOX_H_2004_12
#define LISTBOX_H_2004_12

#include <conio.h>

#include "DblList.h"
#include "Control.h"

enum  LISTDIRECTION
{
	HORIZONTAL,
	VERTICAL
};

const int DEFAULTPOSTION = -1;

class ListBox : public DblList, public Control
{
public:
	ListBox(int x1, int y1, int x2=DEFAULTPOSTION, int y2=DEFAULTPOSTION, unsigned char attrib=(LIGHTGRAY<<4)|BLACK, LISTDIRECTION direction=VERTICAL);
	ListBox();
	virtual ~ListBox();

public:
	void Refresh();

protected:
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);

public:
	DblList::ListItem *Insert(void *ptr, void *pos=NULL);
	BOOL MoveNext();
	BOOL MovePrevious();
	BOOL Move(int pos);
	BOOL Move(void *ptr);
	virtual void SetPostion(int x, int y);

protected:
	LISTDIRECTION Direction;
	int x1, y1, x2, y2;
	int bkcolor, color;
	int Top, Left;
	int VisibleCount;

private:
	BOOL IsExpandWidth;
	BOOL IsExpandHeight;

protected:
	virtual void WriteStr(char far * &ptr, char far * endptr, char * &p, BOOL IsFocus);
	virtual void OnPreRefresh();
	virtual void AutoGrow();
};


#endif