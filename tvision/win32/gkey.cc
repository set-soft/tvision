/*****************************************************************************

 Keyboard handler for DOS by Salvador E. Tropea (SET) (1998)

 Modified by Vadim Beloborodov to be used on WIN32 console

 That's a gkey function that is supposed to:

1) Solve all the problems about multiple translations. That's because it
generates a unique code for each key. The format is:

bit 10       9       8    7   6-0
Left-Alt Right-Alt Ctrl Shift Code

  Additionally there are a char value to carry the ASCII.
  I'm detecting left and right alt but perhaps isn't good. The idea of
detecting it is that Left Alt could be used for the menu and Right Alt
(called Alt Gr in some keyboards) can be used for macros.
  But I think it will confuse to some people, what do you think?

2) Solve the keyboard layout problems. This routines uses a methode that
works OK with M$ keyb TSR. Keyb is relative smart and translates the Alt-X
codes, that's needed because the Alt keys are ASCII 0 and we need the scan
to differenciate one from the others. Keyb translates it.
 I tried with keyb gr and worked perfect.
 The main idea is just use ASCII for the things is possible and scans for
the rest, keyb takes care about the thing that can't be detected using
ASCIIs.

3) Solve the problem of the numeric pad. BIOS makes the work.

  The code is very fast because uses a few comparissons and look-up tables,
we don't need long switchs. The only exception is done to avoid the loose
of ^H,^I,^J,^M and ^[. For that I suppose some things and guess about what
key was.

Important!
i) Some constants have crazy names, take a look I used the first
name that came to my mind.
ii) Some ^symbol assigments aren't well mapped by keyb so they are very
specific cases that I think doesn't mather.

  Now, from the side of the editor:

  This methode generates 2048 key combinations, only a part can be really
triggered because a lot are like !, is impossible to get other thing that
Shift-! because you MUST press shift to get it, additionally Shift-1 doesn't
exist because you'll get Shift-!. That is logic because not all keyboards
have the relation Shift-1==!. So only some combinations can be used for
commands, the rest must mean: Insert a character with the ASCII provided.
  For the rest I think the system can be used.

Note: I did a TSR to detect 19 key combinations that BIOS doesn't report,
takes only 400 bytes of memory.

*****************************************************************************/

#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_ctype
#include <tv.h>
#include <string.h>

int TGKey::useBIOS=0;
int TGKey::translateKeyPad=1;
KeyType TGKey::rawCode;
unsigned short TGKey::sFlags;
int  TGKey::Abstract;
char TGKey::ascii;
int TGKey::Mode=0;

char *TGKey::KeyNames[]=
    {
        "Unknown",
        "A","B","C","D","E","F","G","H","I","J","K",
        "L","M","N","O","P","Q","R","S","T","U","V",
        "W","X","Y","Z",
        "OpenBrace","BackSlash","CloseBrace","Pause","Esc",
        "0","1","2","3","4","5","6","7","8","9",
        "BackSpace","Tab","Enter","Colon","Quote","Grave",
        "Comma","Stop","Slash","Asterisk","Space","Minus",
        "Plus","PrnScr","Equal","F1","F2","F3","F4","F5",
        "F6","F7","F8","F9","F10","F11","F12","Home",
        "Up","PgUp","Left","Right","End","Down","PgDn",
        "Insert","Delete","Caret","Admid","DobleQuote",
        "Numeral","Dolar","Percent","Amper","OpenPar",
        "ClosePar","DoubleDot","LessThan","GreaterThan",
        "Question","A_Roba","Or","UnderLine","OpenCurly",
        "CloseCurly","Tilde","Macro","WinLeft","WinRight","WinSel",
        "Mouse"
    };

const int NumKeyNames=sizeof(TGKey::KeyNames)/sizeof(char *);

// All the info. from BIOS in one call
void TGKey::GetRaw(void)
{
}

void TGKey::SetKbdMapping(int mode)
{
}

#if 0
static int International_To_Key(uchar ascii)
{
	return ascii;
}
#endif

static char International_To_ASCII(uchar ascii)
{
	return ascii;
}

int TGKey::CompareASCII(uchar val, uchar code)
{
	return val==code;
}

unsigned short TGKey::gkey(void)
{
	return 0;
}

int getConsoleKeyboardEvent(KeyDownEvent& key);
extern unsigned evKeyboardLength;

int TGKey::kbhit(void)
{
	return evKeyboardLength>0;
}

void TGKey::clear(void)
{
}


void TGKey::fillTEvent(TEvent &e)
{
	getConsoleKeyboardEvent(e.keyDown);
	e.what=evKeyDown;
}

ushort TGKey::AltSet=2;      // Default: Both ALT are the same

const int CantDef=0x39;
static const char altCodes[CantDef+1]=
    "\0ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\0\0""0123456789\0\t\0;'`,./*\xf0-+\0=";

char TGKey::GetAltChar(ushort keyCode, uchar ascii)
{
	// Only when ALT is present
	if ((keyCode & kbAltLCode)==0)
		return 0;
	keyCode&=kbKeyMask;
	// If the key is unknown but have an ASCII associated use it!
	if (keyCode==0 && ascii) {
		if (Mode)
			International_To_ASCII(ascii);
		else
			return ascii;
	}
	if (keyCode>CantDef-1)
		return 0;
	return altCodes[keyCode];
}

ushort TGKey::GetAltCode(char c)
{
	int i;
	if (Mode)
		c=International_To_ASCII(c);
	c=uctoupper(c);

	for (i=0; i<CantDef; i++)
		if (altCodes[i]==c)
			return i | kbAltLCode; // Report the left one
	return 0;
}

ushort TGKey::KeyNameToNumber(char *s)
{
	int i;
	for (i=0; i<NumKeyNames; i++)
		if (strcmp(KeyNames[i],s)==0)
			return i;
	return (ushort)-1;
}

