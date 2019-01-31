#include <bios.h>
#include <dos.h>
#include <conio.h>
#include "KeyBoard.h"

KeyBoard::KeyBoard()
{
	if(counterforsetvect++==0)
	{
		oldkey = getvect(0x09);
		disable();
		setvect(0x09, newkey);
		enable();
	}
}

KeyBoard::~KeyBoard()
{
	if(--counterforsetvect==0)
	{
		disable();
		setvect(0x09, oldkey);
		enable();

		while(Point!=NULL)
		{
			KeyItem *temp = Point->next;
			delete Point;
			Point = temp;
		}
	}
}

KeyBoard::KeyItem *KeyBoard::Point = NULL;
KeyBoard::KeyItem *KeyBoard::End   = NULL;
void interrupt (far *KeyBoard::oldkey)(...) = NULL;
unsigned int KeyBoard::counterforsetvect = 0;
BOOL KeyBoard::ismeet0xe0 = FALSE;

void interrupt far KeyBoard::newkey(...)
{
	static BOOL busy = FALSE;
	if(busy) { oldkey(); return; }
	busy = TRUE;

	unsigned char keycode = inport(0x60);

	if(keycode==0xE0)   ismeet0xe0 = TRUE;
	else
	{
		KeyItem *item = new KeyItem();
		item->KeyCode = (ismeet0xe0?0xFE00:0xFF00) | keycode;
		item->KeyState = bioskey(2);

		if(Point==NULL)	 Point = item;
		else 			 End->next = item;
		End = item;

		ismeet0xe0 = FALSE;
	}

	oldkey();

	unsigned int &ptr0 = *(unsigned int far *)0x0041AL;
	unsigned int &ptr1 = *(unsigned int far *)0x0041CL;

	while(ptr0 != ptr1)
	{
		unsigned int keycode = *(unsigned int far *)MK_FP(0x0040, ptr0);
		ptr0 += 2;
		if(ptr0==0x3E)	 ptr0 = 0x1E;

		KeyItem *item = new KeyItem();
		item->KeyCode = keycode;
		item->KeyState = bioskey(2);

		if(Point==NULL)	 Point = item;
		else			 End->next = item;
		End = item;
	}

	busy = FALSE;
}

KEYSIDE   	KeyBoard::keyside    = KEYSIDE_DEFAULT;
KEYTYPE  	KeyBoard::keytype    = KEYTYPE_DEFAULT;
KEYPOSTION  KeyBoard::keypostion = KEYPOSTION_DEFAULT;
KEYSTATE	KeyBoard::keystate   = 0;
unsigned int KeyBoard::keycode   = 0;

unsigned int KeyBoard::GetKeyPress()
{
	if(Point==NULL)	 return 0;

	KeyItem *ptr = Point;
	Point = Point->next;
	keycode = ptr->KeyCode;
	keystate = Point->KeyState;
	delete ptr;

	unsigned char high = keycode>>8, low = keycode&0x00FF;
	unsigned int result = 0;

	keyside = KEYSIDE_DEFAULT;
	keytype = KEYTYPE_DEFAULT;
	keypostion = KEYPOSTION_NORMAL;

	if(high==0xFF || high==0xFE)
	{
		keytype = (low&0x80)==0?KEYTYPE_DOWN:KEYTYPE_UP;
		result = low&0x7F;
		keycode = result;

		switch(result)
		{
			case ALT_CODE: case CTRL_CODE:
				if(high==0xFF)	keyside = KEYSIDE_LEFT;
				else			keyside = KEYSIDE_RIGHT;
				break;
			case LEFT_SHIFT_CODE:
				keyside = KEYSIDE_LEFT;
				break;
			case RIGHT_SHIFT_CODE:
				keyside = KEYSIDE_RIGHT;
				break;
			case ASSIST_STAR_CODE:	case ASSIST_MINUS_CODE:
			case ASSIST_PLUS_CODE:	case ASSIST_MIDDLE_CODE:
				keypostion = KEYPOSTION_ASSIST;
				break;
			case HOME_CODE:		case END_CODE:
			case UP_CODE:		case DOWN_CODE:
			case LEFT_CODE:		case RIGHT_CODE:
			case PAGEUP_CODE:	case PAGEDOWN_CODE:
			case INSERT_CODE:	case DELETE_CODE:
				if(high==0xFF)	keypostion = KEYPOSTION_ASSIST;
				else			keypostion = KEYPOSTION_NORMAL;
				break;
			case BIAS_CODE:		case ENTER_CODE:
				if(high==0xFF)	keypostion = KEYPOSTION_NORMAL;
				else			keypostion = KEYPOSTION_ASSIST;
				break;
			case NUMLOCK_CODE:
				keypostion = KEYPOSTION_ASSIST;
				break;
		}
	}
	else
	{
		if(low==0xE0||low==0)
		{
			result = high<<8;
			keycode = high;
			switch(result)
			{
				case LEFT_KEY: case RIGHT_KEY: case UP_KEY: case DOWN_KEY:
				case PAGEUP_KEY: case PAGEDOWN_KEY: case HOME_KEY: case END_KEY:
				case INSERT_KEY: case DELETE_KEY:
					if(low==0xE0)   keypostion = KEYPOSTION_NORMAL;
					else			keypostion = KEYPOSTION_ASSIST;
				break;
			}
		}
		else
		{
			result = low;
			keycode = high;
			switch(result)
			{
				case ENTER_KEY:
					if(high==0x1C)	keypostion = KEYPOSTION_NORMAL;
					else  { keypostion = KEYPOSTION_ASSIST; keycode=0x1C; }
					break;
				case '-': case '+': case '*': case '/': case '.':
					if(high>=0x37)	keypostion = KEYPOSTION_ASSIST;
					else			keypostion = KEYPOSTION_NORMAL;
					if(result=='/')  keycode = 0x35;
					break;
				default:
					if(result>='0' && result<='9')
					{
						if(high>=47)	keypostion = KEYPOSTION_ASSIST;
						else			keypostion = KEYPOSTION_NORMAL;
					}
					break;
			}
		}
		keytype = KEYTYPE_PRESS;
	}
	return result;
}

unsigned int KeyBoard::TranslateKey(unsigned key)
{
	static int KeyCode[] =
	{
		//0 - 25
		A_CODE,		B_CODE,		C_CODE,		D_CODE,		E_CODE,		F_CODE,		G_CODE,
		H_CODE,		I_CODE,		J_CODE,		K_CODE,		L_CODE,		M_CODE,		N_CODE,
		O_CODE,		P_CODE,		Q_CODE,		R_CODE,		S_CODE,		T_CODE,
		U_CODE,		V_CODE,		W_CODE,		X_CODE,		Y_CODE,		Z_CODE,

		//26 - 35
		NUM0_CODE,	NUM1_CODE,	NUM2_CODE,	NUM3_CODE,	NUM4_CODE,
		NUM5_CODE,	NUM6_CODE,	NUM7_CODE,	NUM8_CODE,	NUM9_CODE
	};

	if(key<='z' && key>='a') 		return KeyCode[key-'a'];
	else if(key<='Z' && key>='A')   return KeyCode[key-'A'];
	else if(key<='9' && key>='0')	return KeyCode[key-'0'+26];
	else switch(key)
	{
		case '~': case '`':	 return WAVE_CODE;
		case '!':			 return NUM1_CODE;
		case '@':			 return NUM2_CODE;
		case '#':			 return NUM3_CODE;
		case '$':			 return NUM4_CODE;
		case '%':			 return NUM5_CODE;
		case '^':			 return NUM6_CODE;
		case '&':			 return NUM7_CODE;
		case '*':			 return NUM8_CODE;
		case '(':			 return NUM9_CODE;
		case ')':			 return NUM0_CODE;
		case '-': case '_':  return MINUS_CODE;
		case '=': case '+':  return PLUS_CODE;
		case '\\': case '|': return VERTICAL_CODE;
		case '{': case '[':  return LEFT_SQUARE_CODE;
		case '}': case ']':  return RIGHT_SQUARE_CODE;
		case ':': case ';':  return SEMICOLON_CODE;
		case '"': case '\'': return QUOTATION_CODE;
		case '<': case ',':  return COMMA_CODE;
		case '>': case '.':  return PERROD_CODE;
		case '/': case '?':  return QUESTION_CODE;
	}

	return 0;
}

//////////////////////////// KeyItem ////////////////////////////
KeyBoard::KeyItem::KeyItem()
{
	this->KeyCode = 0;
	this->KeyState = 0;
	this->next = this->previous = NULL;
}



