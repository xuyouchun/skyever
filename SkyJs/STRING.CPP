#include <string.h>
#include <alloc.h>

#include "string.h"

String::String(char *str, unsigned int len, unsigned int step)
{
	this->Step = MAX(step, 5);
	this->Size = (len/step+1) * step;
	this->Length = len;
	this->Str = (char*)malloc(this->Size * sizeof(char));
	memcpy(this->Str, str, len);
}

String::~String()
{
	if(this->Str!=NULL)
	{
		free(this->Str);
		this->Str = NULL;
	}
}

void String::InsertChar(unsigned int postion, char ch)
{
	this->Length = MAX(Length, postion) + 1 ;
	this->Realloc();
	int len = Length-postion-1;
	if(len>0)	memmove(Str+postion+1, Str+postion, len);
	*(Str+postion) = ch;
}

void String::DeleteChar(unsigned int postion)
{
	if(Length==postion)	return;
	int len = Length-postion;
	if(len>0)	memmove(Str+postion, Str+postion+1, len);
}

void String::InsertStr(unsigned int postion, char *str, unsigned int len)
{
	if(postion>Length)	postion = Length;
	this->Length += len;
	this->Realloc();
	char *ptr = this->Str + postion;
	memmove(ptr+len, ptr, Length-(ptr-Str+len)+1);
	memcpy(ptr, str, len);
}

void String::DeleteStr(unsigned int postion, unsigned int len)
{

}

void String::Realloc()		//protected
{
	int size = ((Length+1)/Step+1) * Step;
	if(size != this->Size)
	{
		this->Size = size;
		this->Str = (char*)realloc(this->Str, size * sizeof(char));
	}
}
