/**[txh]********************************************************************

  Keyboard module, Copyright 2001-2002 by Salvador E. Tropea
  Description:
  This is the base class for keyboard input.
  Most members are pointers to functions defined by each platform dependent
driver. Default members are provided.
  This is a complete new file created from concepts that José Angel Sanchez
Caso provide me and coded from scratch. Original Turbo Vision didn't even
have such a class.
  
***************************************************************************/

#define Uses_TEvent
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_FullSingleKeySymbols
#define Uses_ctype
#define Uses_string
#include <tv.h>

/*****************************************************************************
  Data members initialization
*****************************************************************************/

char   TGKey::suspended=1;
ushort TGKey::AltSet=0;    // Default: Left and right key are different ones
int    TGKey::Mode=0;
char  *TGKey::KeyNames[]=
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

#define NumKeyNames (sizeof(TGKey::KeyNames)/sizeof(char *))

/*****************************************************************************
  Function pointer members initialization
*****************************************************************************/

int      (*TGKey::kbhit)(void)                        =defaultKbhit;
void     (*TGKey::clear)(void)                        =defaultClear;
ushort   (*TGKey::gkey)()                             =defaultGkey;
unsigned (*TGKey::getShiftState)()                    =defaultGetShiftState;
void     (*TGKey::fillTEvent)(TEvent &e)              =defaultFillTEvent;
uchar    (*TGKey::NonASCII2ASCII)(uchar val)          =defaultNonASCII2ASCII;
int      (*TGKey::CompareASCII)(uchar val, uchar code)=defaultCompareASCII;
void     (*TGKey::SetKbdMapping)(int version)         =defaultSetKbdMapping;
int      (*TGKey::GetKbdMapping)(int version)         =defaultGetKbdMapping;
void     (*TGKey::Suspend)()                          =defaultSuspend;
void     (*TGKey::Resume)()                           =defaultResume;
int      (*TGKey::SetCodePage)(int id)                =defaultSetCodePage;

/*****************************************************************************
  Default behaviors for the members
*****************************************************************************/

int      TGKey::defaultKbhit() { return 0; }
unsigned TGKey::defaultGetShiftState() { return 0; }
ushort   TGKey::defaultGkey() { return 0; }
void     TGKey::defaultClear() {}
void     TGKey::defaultSuspend() {}
void     TGKey::defaultResume() {}
uchar    TGKey::defaultNonASCII2ASCII(uchar val) { return val; }
int      TGKey::defaultCompareASCII(uchar val, uchar code) { return val==code; }
void     TGKey::defaultSetKbdMapping(int version) { Mode=version; }
int      TGKey::defaultGetKbdMapping(int version) { return Mode; }
void     TGKey::defaultFillTEvent(TEvent &/*e*/) {};

/*****************************************************************************
  Real members
*****************************************************************************/

void TGKey::suspend()
{
 if (suspended) return;
 suspended=1;
 Suspend();
}

void TGKey::resume()
{
 if (!suspended) return;
 suspended=0;
 Resume();
}

const char *TGKey::NumberToKeyName(unsigned val)
{
 if (val<NumKeyNames)
    return KeyNames[val];
 return KeyNames[0];
}

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
 if (keyCode==kbUnkNown && ascii)
    return NonASCII2ASCII(ascii);
 if (keyCode>CantDef-1)
    return 0;
 return altCodes[keyCode];
}

ushort TGKey::GetAltCode(uchar c)
{
 int i;
 c=NonASCII2ASCII(c);
 c=uctoupper(c);

 for (i=0; i<CantDef; i++)
     if (altCodes[i]==c)
        return i | kbAltLCode; // Report the left one
 return 0;
}

ushort TGKey::KeyNameToNumber(char *s)
{
 unsigned i;
 for (i=0; i<NumKeyNames; i++)
     if (strcmp(KeyNames[i],s)==0)
        return i;
 return (ushort)-1;
}

/*****************************************************************************
  Here are some generic translation routines for known keyboards/code pages.
  They are shared by various drivers.
*****************************************************************************/

int TGKey::defaultSetCodePage(int id)
{
 switch (id)
   {
    case TVCodePage::KOI8r:
         NonASCII2ASCII=KOI8_NonASCII2ASCII;
         CompareASCII=KOI8_CompareASCII;
         break;
    default:
         return 0;
   }
 return 1;
}

/* Linux KOI8 */
char TGKey::KOI8Layout[64]=
{
 '.','f',',','w','L','t','a','u','[','b','q','r','K','v','y','j', // 192 ...
 'g','z','h','c','n','e',';','d','m','s','p','i','\'','o','x','}',
 '>','F','<','W','l','T','A','U','{','B','Q','R','k','V','Y','J',
 'G','Z','H','C','N','E',':','D','M','S','P','I','"','O','X',']'
};

uchar TGKey::KOI8_NonASCII2ASCII(uchar val)
{
 return val>=192 ? KOI8Layout[val-192] : val;
}

int TGKey::KOI8_CompareASCII(uchar val, uchar code)
{
 if (val>=0xC0)  val=KOI8Layout[val-0xC0];
 if (code>=0xC0) code=KOI8Layout[code-0xC0];
 return val==code;
}


