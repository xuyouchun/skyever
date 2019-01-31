#ifndef STRING_H_2004_12
#define STRING_H_2004_12

#include "Object.h"

class String : virtual public Object
{
public:
	String(char *str="", unsigned int len=0, unsigned int step=50);
	virtual ~String();

public:
	void InsertChar(unsigned int postion, char ch);
	void DeleteChar(unsigned int postion);
	void InsertStr(unsigned int postion, char *str, unsigned int len);
	void DeleteStr(unsigned int postion, unsigned int len);

protected:
	char *Str;
	unsigned int Size;
	unsigned int Length;
	unsigned int Step;

protected:
	virtual void Realloc();

public:
	virtual char * GetString() { return this->Str; }
	virtual unsigned int GetLength() { return this->Length; }
};


#endif