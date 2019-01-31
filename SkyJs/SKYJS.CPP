#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "JsEditor.h"
#include "SkyJs.h"

SkyJs::SkyJs() :
 myJsEditor(2, 3, this->GetScreenWidth()-1, this->GetScreenHeight()-2),
 myFrameMenu(1, 1)
{
	this->InitScreen();

	this->InitMenu();
	this->InitEditor();

	this->AppendControl(&myJsEditor);
	this->AppendControl(&myFrameMenu);

	myFrameMenu.Show();
	myJsEditor.SetFocus();
}

SkyJs::~SkyJs()
{
}

void SkyJs::InitScreen()
{
	int k;
	char attrib = (BLUE<<4)|WHITE;
	int ScreenWidth = this->GetScreenWidth(),
		ScreenHeight = this->GetScreenHeight();

	this->WriteChar(1, 2, 'Ú', attrib);
	this->WriteChar(1, ScreenHeight-1, 'À', attrib);
	this->WriteChar(ScreenWidth, 2, '¿', attrib);
	this->WriteChar(ScreenWidth, ScreenHeight-1, 'Ù', attrib);

	for(k=2; k<=ScreenWidth-1; k++)
	{
		this->WriteChar(k, 2, 'Ä', attrib);
		this->WriteChar(k, ScreenHeight-1, 'Ä', attrib);
	}
	for(k=3; k<=ScreenHeight-2; k++)
	{
	  this->WriteChar(1, k, '³', attrib);
	  this->WriteChar(ScreenWidth, k, '³', attrib);
	}
}

void SkyJs::InitMenu()
{
	MulMenu *ptr;
	ptr = myFrameMenu.CreateDir("&File");
	MulMenu *ptr0 = ptr->CreateDir("&New");
	ptr0->Insert("&Create New File...");
	ptr->Insert("&Open...");
	ptr->Insert("&Save");
	ptr0 = ptr->CreateDir("S&ave As...");
	ptr0->Insert("&ABCDEFG");
	ptr0->Insert("&HIJKLMN");
	ptr0->Insert("&OPQ RST");
	ptr0->Insert("&UVW XYZ");
	ptr->InsertLine();
	ptr->Insert("&Change Dir...");
	ptr->Insert("&Quit");
//	myFrameMenu.InsertLine();
	ptr = myFrameMenu.CreateDir("&Edit", HORIZONTAL);
	ptr0 = ptr->CreateDir("&Cut");
	ptr0->Insert("Copy &A");
	ptr0->Insert("Copy &B");
	ptr0->Insert("Copy &C");
	ptr0->Insert("Copy &D");
	ptr0->Insert("Copy &EFGHIJKLMNOPQRSTUVWXYZ");
	ptr->Insert("&Copy");
	ptr->Insert("&Paste");
	ptr->Insert("&Delete");
	myFrameMenu.Insert("&Run");
	myFrameMenu.Insert("&Options");
	myFrameMenu.Insert("&Window");
	myFrameMenu.Insert("&Help");

	myFrameMenu.OnCommand = myFrameMenu_OnCommand;
}

void SkyJs::InitEditor()
{
	myJsEditor.Open("myFunc.txt");
	myJsEditor.OnPreEvent = myJsEditor_OnPreEvent;
}

REPLYTYPE SkyJs::ReplyMessage(MSGTYPE type, MSGARGUMENT argument)
{
	if(type==KEY_PRESS && argument==ESC_KEY && myJsEditor.GetIsHasFocus())
	{
		myJsEditor.Save("myFunc.txt");
		return BREAK;
	}
//	else if(type==KEY_UP && argument==ALT_CODE && this->GetKeySide()==KEYSIDE_LEFT)	return BREAK;
	else	return CONTINUE;
}

REPLYTYPE SkyJs::myJsEditor_OnPreEvent(Control *sender, MSGTYPE type, MSGARGUMENT argument)
{
	if(sender->GetIsHasFocus())
	{
		SkyJs *mySkyJs = (SkyJs*)sender->GetParentControl();
		if(argument==F10_KEY)
		{
			mySkyJs->SetFocusControl(&mySkyJs->myFrameMenu);
		}
	}
	return CONTINUE;
}

REPLYTYPE SkyJs::myFrameMenu_OnCommand(Control *sender, MSGTYPE type, MSGARGUMENT argument)
{
	char *str = (char *)argument;
	return CONTINUE;
}
