#include "DblList.h"


DblList::DblList()
{
	this->Begin  = NULL;
	this->Point  = NULL;

	this->Count  = 0;
}

DblList::~DblList()
{
	ListItem *p = Begin;
	while(p!=NULL)
	{
		ListItem *p0 = p->next;
		delete p;
		p = p0;
	}
}

void DblList::Delete(void *ptr)
{
	ListItem *p = this->Begin;
	while(p!=NULL)
	{
		if(p->Content==ptr)
		{
			ListItem *previous = p->previous;
			ListItem *next	  = p->next;
			if(p==this->Begin)
			{
				this->Begin    = next;
				next->previous = this->Begin;
			}
			else
			{
				previous->next = next;
				if(next!=NULL)	next->previous = previous;
			}
			if(p==this->Point)	this->Point = this->Begin;
			delete p;
			this->Count --;
			break;
		}
		p = p->next;
	}
}

DblList::ListItem * DblList::CreateItem(void *ptr)
{
	return new ListItem(ptr);
}

DblList::ListItem *DblList::Insert(void *ptr, void *pos)
{
	ListItem *item = NULL;
	if(Begin==NULL)
	{
		item = CreateItem(ptr);
		Begin = item; Count ++;
	}
	else if(pos==NULL)
	{
		item = CreateItem(ptr);
		ListItem *p0 = Begin;
		while(p0->next!=NULL)	p0 = p0->next;
		p0->next = item;
		item->previous = p0;
		Count ++;
	}
	else
	{
		ListItem *p0 = Begin;
		while(p0!=NULL)
		{
		   if(p0->Content==pos)
		   {
			  item = CreateItem(ptr);
			  ListItem *p1 = p0->next;
			  if(p1!=NULL)
			  {
				p1->previous = item;
				item->next = p1;
			  }
			  p1->next = item;
			  item->previous = p1;
			  Count ++;
			  break;
		   }
		   p0 = p0->next;
		}
	}
	if(this->Point==NULL && this->Begin!=NULL)	this->Point = this->Begin;
	return item;
}

BOOL DblList::Move(int pos)
{
	if(pos>=this->Count)	return FALSE;
	if(pos<0)				return FALSE;

	this->Point = this->Begin;
	for(register int k=0; k<pos; k++)	Point = Point->next;

	return TRUE;
}

BOOL DblList::Move(void *pos)
{
	ListItem *p = this->Begin;
	while(p!=NULL)
	{
		if(p->Content==pos)
		{
			this->Point = p;
			return TRUE;
		}
		p = p->next;
	}
	return FALSE;
}

BOOL DblList::MoveNext()
{
	if(this->Point==NULL)		return FALSE;
	if(this->Point->next==NULL) return FALSE;

	this->Point = this->Point->next;
	return TRUE;
}

BOOL DblList::MovePrevious()
{
	if(this->Point==NULL)			return FALSE;
	if(this->Point->previous==NULL) return FALSE;

	this->Point = this->Point->previous;
	return TRUE;
}

void *DblList::GetContent(int Index)
{
	if((unsigned int)Index==0xFFFF)
	{
		if(this->Point==NULL)	return NULL;
		else					return this->Point->Content;
	}
	else
	{
		ListItem *p = this->Begin;
		if(p==NULL)   return NULL;
		for(register int k=0; k<Index; k++)
		{
			if(p==NULL)	return NULL;
			else		p = p->next;
		}
		if(p==NULL)	 return NULL;
		else		 return p->Content;
	}
}

int DblList::GetCursor()
{
	if(this->Begin==NULL)	return 0xFFFF;
	ListItem *p = this->Begin;
	int Cursor = 0;
	while(p!=NULL)
	{
		if(p==this->Point)	return Cursor;
		Cursor ++;
		p = p->next;
	}
	return 0xFFFF;
}

int DblList::GetIndex(void *ptr)
{
	if(this->Begin==NULL)	return 0xFFFF;
	ListItem *p = this->Begin;
	int Index = 0;
	while(p!=NULL)
	{
		if(p->Content==ptr)		return Index;
		Index ++;
		p = p->next;
	}
	return 0xFFFF;
}


//////////////////////////// struct ListItem ///////////////////////////

DblList::ListItem::ListItem(void *Content)
{
	this->Content  = Content;
	this->previous = NULL;
	this->next     = NULL;
}

