#ifndef DBLLIST_H_2004_12
#define DBLLIST_H_2004_12

#include "Object.h"

class DblList : virtual public Object
{
public:
	DblList();
	virtual ~DblList();

public:
	struct ListItem;
	virtual void Delete(void *ptr);
	virtual ListItem* Insert(void *ptr, void *pos=NULL);
	virtual BOOL Move(int pos);
	virtual BOOL Move(void *ptr);
	virtual BOOL MoveNext();
	virtual BOOL MovePrevious();

protected:
	virtual ListItem * CreateItem(void *ptr);

public:
	int  GetCount()  { return this->Count;  }
	void *GetContent(int index=0xFFFF);
	int  GetCursor();
	int  GetIndex(void *ptr);

private:
	struct ListItem
	{
		ListItem(void *Content);
		struct ListItem *previous, *next;
		void   *Content;
	} *Begin, *Point;

	int Count;

protected:
	ListItem *GetHome()  { return this->Begin; }
	ListItem *GetPoint() { return this->Point; }
};



#endif