#ifndef EDITOR_H_2004_12
#define EDITOR_H_2004_12

#include <conio.h>
#include "MulStr.h"
#include "Control.h"

class Editor : public MulStr, public Control
{
public:
	Editor(int x1, int y1, int x2, int y2, unsigned char attrib=(BLUE<<4)|LIGHTGRAY, unsigned int step=50);
	virtual ~Editor();

public:
	int x1, y1, x2, y2;
	unsigned char bkcolor, color;

protected:
	virtual void InitScreen();
	virtual void ClearScreen();
	virtual void WriteStr(char far *ptr, char *str, unsigned int start, unsigned int len);

public:
	REPLYTYPE ReplyMessage(MSGTYPE type, MSGARGUMENT argument);

protected:
	int CurLine, Cursor;
	int Left, Top;

private:
	class Row : public String
	{
		public:
			Row(char *str);
			virtual ~Row();
		public:
			void  SetCursor(unsigned int cursor);
			int   GetCursor();
			void  InsertChar(char ch);
			void  DeleteChar();
		private:
			int   Cursor;
		protected:
			void Realloc();
		public:
			unsigned int GetLength();
		private:
			unsigned int GetLength(char *str);
	} * myRow;

private:
	void WriteLine(char *Str, unsigned int Length);
	void WriteLine(Row *myRow);

public:
	void Refresh();
	char * GetString();			//virtual
	unsigned int GetLength();	//virtual
};

#endif