#ifndef SCREEN_H_2004_12
#define SCREEN_H_2004_12

#include "Object.h"
#include <conio.h>

class Screen : public virtual Object
{
public:
	Screen();
	virtual ~Screen();

private:
	static int ScreenWidth, ScreenHeight, ScreenModel;
	static BOOL IsScreenInit;

protected:
	void SetScreenModel(int model);
	int  GetScreenWidth()  { return ScreenWidth;  }
	int  GetScreenHeight() { return ScreenHeight; }
	void ClrScr(int x1, int y1, int x2, int y2, char attrib);
	void WriteChar(int x, int y, char ch, unsigned char attrib=(BLUE<<4)|WHITE );
};

#endif
