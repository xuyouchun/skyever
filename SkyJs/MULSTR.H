#ifndef MULSTR_H_2004_12
#define MULSTR_H_2004_12

#include "String.h"

class MulStr : public String
{
public:
	MulStr();
	MulStr(unsigned int step);
	virtual ~MulStr();
public:
	char *FindRow(unsigned int rowindex);

	void ReplaceRow(unsigned int rowindex, char *str, unsigned int Len);
	void ReplaceRow(unsigned int rowindex, char *str);

	void InsertStr(unsigned int rowindex, unsigned int start, char *str, unsigned int Len);
	void InsertStr(unsigned int postion, char *str, unsigned int Len) { String::InsertStr(postion, str, Len); }
	void DeleteStr(unsigned int rowindex, unsigned int start, unsigned int Len);

	void InsertRow(unsigned int rowindex);
	void InsertRow() { this->InsertRow(this->Lines); }

	void DeleteRow(unsigned int rowindex) { MulStr::ReplaceRow(rowindex, NULL, 0); }

	unsigned int GetLength() { return this->Length; }
	unsigned int GetLines()  { return this->Lines;  }

protected:
	unsigned int Lines;

};

#endif