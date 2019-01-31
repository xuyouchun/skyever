#ifndef JSEDITOR_H_2004_12
#define JSEDITOR_H_2004_12

#include "Editor.h"

class JsEditor : public Editor
{
public:
	JsEditor(int x1, int y1, int x2, int y2, unsigned int step=100);
	virtual ~JsEditor();

protected:
	void WriteStr(char far *ptr, char *str, unsigned int start, unsigned int len);	//virtual


private:
	int GetStrBlock(char *begin, char *str, char * &end, char &attrib);

public:
	int Open(char *filename);
	int Save(char *filename);
	int Close();

protected:
	BOOL IsDigit(char ch);
};


#endif