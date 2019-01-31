#include <string.h>
#include <alloc.h>

#include "mulstr.h"

MulStr::MulStr(unsigned int step) : String("\n", 1, step)
{
	this->Str[1] = '\0';
	this->Lines = 1;
}

MulStr::~MulStr()
{
	if(this->Str!=NULL)
	{
		free(this->Str);
		this->Str = NULL;
	}
}

char* MulStr::FindRow(unsigned int rowindex)
{
	if(rowindex>=this->Lines)	return NULL;

	char *ptr = this->Str;
	unsigned int index=0, length=this->Length, k=0;

	if(rowindex==0)	return this->Str;
	while(k++<length)
	{
	   if(*ptr++=='\n')
	   {
			if(++index == rowindex)	 return k==length?NULL:ptr;
	   }
	}
	return NULL;
}

void MulStr::ReplaceRow(unsigned int rowindex, char *str, unsigned int Len)	//virtual public
{
	char *ptr = this->FindRow(rowindex);
	if(ptr==NULL)	return;

	unsigned int newlen = 0;
	if(str!=NULL)	newlen = Len + 1;

	char *ptr1 = ptr;
	while( *ptr1!='\n' )	ptr1 ++;
	unsigned int oldlen = ptr1-ptr + 1;

	int src = ptr1-this->Str, dest = src + (newlen-oldlen);
	int len = this->Length-(ptr1-this->Str) + 1;
	this->Length += newlen-oldlen;
	int begin = ptr-this->Str;

	if(newlen>oldlen)
	{
		this->Realloc();
		memmove(this->Str+dest, this->Str+src, len);
	}
	else if(newlen<oldlen)
	{
		memmove(this->Str+dest, this->Str+src, len);
		this->Realloc();
	}

	if(str!=NULL)
	{
		memcpy(this->Str+begin, str, newlen-1);
		*(this->Str+begin+newlen-1) = '\n';
	}

	if(str==NULL)	this->Lines --;
	else
	{
		int lines = 0;
		char *p = str, *end = p+Len;
		while(p<end) { if(*p++=='\n')	lines ++; }
		this->Lines += lines;
	}

	if(*this->Str=='\0')
	{
		*this->Str = '\n';
		*(this->Str+1) = '\0';
		this->Length = 1;
		this->Lines = 1;
	}
	return;
}

void MulStr::ReplaceRow(unsigned int rowindex, char *str)
{
	this->ReplaceRow(rowindex, str, strlen(str));
}

void MulStr::InsertStr(unsigned int rowindex, unsigned int start, char *str, unsigned int Len)
{
	if(str==NULL||Len==0)	return;

	char *ptr = this->FindRow(rowindex);
	if(ptr==NULL)	return;
	unsigned int pos = ptr-Str;

	char *ptr1 = ptr;
	while(*ptr1!='\n')  ptr1 ++;
	unsigned int oldlen = ptr1-ptr;
	if(start>oldlen)	start = oldlen;

	unsigned int newlen = Len;
	String::InsertStr(pos+start, str, newlen);

	char *p = str, *end = str+newlen;
	int lines = 0;
	while(p<end) { if(*p++=='\n') lines ++; }
	this->Lines += lines;
}

void MulStr::DeleteStr(unsigned int rowindex, unsigned int start, unsigned int Len)
{

}


void MulStr::InsertRow(unsigned int rowindex)	//virtual public
{
	char *ptr = this->FindRow(rowindex);
	int begin = ptr-this->Str;

	unsigned int NewLength = ++this->Length;
	this->Realloc();
	if(this->Str==NULL)		return;

	if(ptr==NULL)
	{
		this->Str[NewLength-1] = '\n';
		this->Str[NewLength] = '\0';
	}
	else
	{
		ptr = this->Str + begin;
		memmove(ptr+1, ptr, this->Length-begin);
		*ptr = '\n';
	}
	this->Lines ++;
}
