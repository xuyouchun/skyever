#include "JsEditor.h"

JsEditor::JsEditor(int x1, int y1, int x2, int y2, unsigned int step)
  : Editor(x1, y1, x2, y2, (BLUE<<4)|LIGHTGRAY, step)
{
}

JsEditor::~JsEditor()
{
}

void JsEditor::WriteStr(char far *ptr, char *str, unsigned int start, unsigned int len)
{
	char attrib=0, *end = str+start+len-1, *End = end;
	char *pos = str + start;
	while(this->GetStrBlock(str, pos, end, attrib))
	{
		while(pos<=end) { *ptr++ = *pos++;  *ptr++ = attrib; }
		end = End;
	}
}

inline BOOL JsEditor::IsDigit(char ch)
{
	return BOOL
	(
		ch>='A'&&ch<='Z' ||
		ch>='a'&&ch<='z' ||
		ch>='0'&&ch<='9' ||
		ch=='_'
	);
}
int JsEditor::GetStrBlock(char *begin, char *pos, char * &end, char &attrib)
{
	if(pos>end)	return 0;

	char *ptr;
	if(attrib==0)
	{
		ptr = begin;
		while(ptr<=pos)
		{
		   switch(*ptr)
		   {
				case '/':
					if(*(ptr+1)=='/')  attrib = (bkcolor<<4) | CYAN;
					break;
				case '"': case '\'':
				{
					char flag = *ptr++;
					while(ptr<=end)
					{
						if(*ptr=='\\')  { ptr+=2; continue; }
						if(*ptr==flag)   break;
						ptr ++;
					}
					if(ptr>=pos)
					{
						attrib = (bkcolor<<4) | BROWN;
						end = MIN(ptr, end);
					}
				}
					break;
		   }
		   ptr ++;
		}
		if(attrib!=0)	return 1;
	}

	attrib = (bkcolor<<4) | color;
	ptr = pos;
	if(*ptr==' ')
	{
		while(*++ptr==' ' && ptr<=end);
		end = --ptr;
		return 1;
	}
	else if(IsDigit(*ptr))
	{
		char ch;
		while(IsDigit(*(--ptr)) && ptr>=begin);

		char *endptr = ++ptr;
		while( IsDigit( *(++endptr) ) );
		endptr --;

		struct
		{
			char *flag;
			char attrib;
		} FlagColors[] =
		{
			#define FLAGCOUNT 3
			{ "|if|else|return|continue|break|for|in|while|do|"
			  "switch|case|default|goto|var|function|this|new|"
			  "delete|event|true|false|typeof|null|",
												(bkcolor<<4)|GREEN  },
			{ "|Object|Array|String|Date|Function|RegExp|"
			  "Math|Global|Number|Boolean|",
												(bkcolor<<4)|WHITE  },
			{ "|try|catch|finally|throw|" ,
												(bkcolor<<4)|YELLOW }
		};

		attrib = (bkcolor<<4) | color;
		for(int index=0; index<FLAGCOUNT; index++)
		{
			char *p = FlagColors[index].flag;

		  start:
			while(*p && *p++!='|');
			if(*p=='\0')	continue;
			char *p0 = ptr;
			while(p0<=endptr)
			{
			  if(*p=='|')	goto start;
			  if(*p0!=*p)	break;
			  p0 ++; p ++;
			}
			if(*p=='|')	{ attrib = FlagColors[index].attrib; break; }
			else goto start;
		}
		end = MIN(endptr, end);
		return 1;
	}
	else
	{
		switch(*ptr)
		{
			case '/':
				if(*(ptr+1)=='/')	attrib = (bkcolor<<4) | CYAN;
				else				goto _default;
				break;
			case '"': case '\'':
			{
				char flag = *ptr;
				char *p = ptr+1;
				while(p<=end)
				{
					if(*p=='\\') { p+=2; continue; }
					if(*p==flag)	break;
					p ++;
				}
				attrib = (bkcolor<<4) | BROWN;
				end = MIN(p, end);
			}
				break;
			default: _default:
				attrib = (bkcolor<<4) | CYAN;
				end = ptr;
				break;
		}
		return 1;
	}
}

int JsEditor::Open(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if(fp==NULL)	return 0;

	this->Str[0] = '\n';
	this->Length = 1;
	this->Top    = 0;
	this->Left   = 0;
	this->Cursor = 0;
	this->CurLine = 0;

	char buffer[1024];
	while(!feof(fp))
	{
		int len = fread(buffer, 1, 1024, fp);
		this->InsertStr(this->Length-1, buffer, len);
	}
	fclose(fp);

	this->Lines = 0;
	char *p = this->Str, *endp = p + this->Length;
	while(p<endp)	{ if(*p++=='\n')	this->Lines ++; }

	this->Realloc();
	this->Refresh();

	return 1;
}

int JsEditor::Save(char *filename)
{
	FILE *fp = fopen(filename, "w");
	if(fp==NULL)	return 0;
	fwrite(this->GetString(), this->GetLength()-1, 1, fp);
	fclose(fp);
	return 1;
}

int JsEditor::Close()
{
	return 1;
}
