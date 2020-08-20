/**[txh]********************************************************************

  Keyboard module, Copyright 2001-2003 by Salvador E. Tropea
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
#define Uses_stdlib
#include <tv.h>

/*****************************************************************************
  Data members initialization
*****************************************************************************/

char   TGKey::suspended=1;
ushort TGKey::AltSet=0;    // Default: Left and right key are different ones
int    TGKey::Mode=0;
int    TGKey::inputMode=TGKey::codepage;
const char *TGKey::KeyNames[]=
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
int      (*TGKey::AltInternat2ASCII)(TEvent &event)   =defaultAltInternat2ASCII;
void     (*TGKey::fillCharCode)(TEvent &e)            =defaultFillCharCode;


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
int      TGKey::defaultGetKbdMapping(int /*version*/) { return Mode; }
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

/* This table maps the Unicode for greek letters with the latin letter for
 the key that generates it. Example: alpha (with any accent) is generated
 pressing the key with the latin A letter.

This note is what I wrote for the old DOS code
 Greek keyboards: That's for code page 737. This
keyboards have two modes. One mode makes them work just
as an US keyboard, pressing LShift+Alt enters in the
greek mode. In this mode a-z and A-Z generates greek
letters. They are reported without any scan code.
Additionally ;/: key is an accent key (q/Q holds ;/:).
*/
stIntCodePairs TGKey::GreekKeyboard[]=
{
// With tonos
{ 0x0386,'A' },/* 0x0387 */{ 0x0388,'E' },{ 0x0389,'H' },{ 0x038A,'I' },
 /* 0x038B */{ 0x038C,'O' },/* 0x038D */{ 0x038E,'Y' },{ 0x038F,'V' },
// Dialytica and tonos
{ 0x0390,'i' },
// Capitals
{ 0x0391,'A' },{ 0x0392,'B' },{ 0x0393,'G' },{ 0x0394,'D' },{ 0x0395,'E' },
{ 0x0396,'Z' },{ 0x0397,'H' },{ 0x0398,'U' },{ 0x0399,'I' },{ 0x039A,'K' },
{ 0x039B,'L' },{ 0x039C,'M' },{ 0x039D,'N' },{ 0x039E,'J' },{ 0x039F,'O' },
{ 0x03A0,'P' },{ 0x03A1,'R' },{ 0x03A3,'S' },{ 0x03A4,'T' },{ 0x03A5,'Y' },
{ 0x03A6,'F' },{ 0x03A7,'X' },{ 0x03A8,'C' },{ 0x03A9,'V' },
// With dialytica
{ 0x03AA,'I' },{ 0x03AB,'Y' },
// With tonos
{ 0x03AC,'a' },{ 0x03AD,'e' },{ 0x03AE,'h' },{ 0x03AF,'i' },
// Dialytica and tonos
{ 0x03B0,'y' },
// Smalls
{ 0x03B1,'a' },{ 0x03B2,'b' },{ 0x03B3,'g' },{ 0x03B4,'d' },{ 0x03B5,'e' },
{ 0x03B6,'z' },{ 0x03B7,'h' },{ 0x03B8,'u' },{ 0x03B9,'i' },{ 0x03BA,'k' },
{ 0x03BB,'l' },{ 0x03BC,'m' },{ 0x03BD,'n' },{ 0x03BE,'j' },{ 0x03BF,'o' },
{ 0x03C0,'p' },{ 0x03C1,'r' },{ 0x03C3,'s' },{ 0x03C4,'t' },{ 0x03C5,'y' },
{ 0x03C6,'f' },{ 0x03C7,'x' },{ 0x03C8,'c' },{ 0x03C9,'v' },
// With dialytica
{ 0x03CA,'i' },{ 0x03CB,'y' },
// With tonos
{ 0x03CC,'o' },{ 0x03CD,'y' },{ 0x03CE,'v' }
};

// Same for russian keyboards
stIntCodePairs TGKey::RussianKeyboard[]=
{
{ 0x0410,'F' },{ 0x0411,'<' },{ 0x0412,'D' },{ 0x0413,'U' },{ 0x0414,'l' },
{ 0x0415,'T' },{ 0x0416,':' },{ 0x0417,'P' },{ 0x0418,'B' },{ 0x0419,'Q' },
{ 0x041a,'R' },{ 0x041b,'k' },{ 0x041c,'V' },{ 0x041d,'Y' },{ 0x041e,'J' },
{ 0x041f,'G' },{ 0x0420,'H' },{ 0x0421,'C' },{ 0x0422,'N' },{ 0x0423,'E' },
{ 0x0424,'A' },{ 0x0425,'{' },{ 0x0426,'W' },{ 0x0427,'X' },{ 0x0428,'I' },
{ 0x0429,'O' },{ 0x042a,']' },{ 0x042b,'S' },{ 0x042c,'M' },{ 0x042d,'"' },
{ 0x042e,'>' },{ 0x042f,'Z' },{ 0x0430,'f' },{ 0x0431,',' },{ 0x0432,'d' },
{ 0x0433,'u' },{ 0x0434,'L' },{ 0x0435,'t' },{ 0x0436,';' },{ 0x0437,'p' },
{ 0x0438,'b' },{ 0x0439,'q' },{ 0x043a,'r' },{ 0x043b,'K' },{ 0x043c,'v' },
{ 0x043d,'y' },{ 0x043e,'j' },{ 0x043f,'g' },{ 0x0440,'h' },{ 0x0441,'c' },
{ 0x0442,'n' },{ 0x0443,'e' },{ 0x0444,'a' },{ 0x0445,'[' },{ 0x0446,'w' },
{ 0x0447,'x' },{ 0x0448,'i' },{ 0x0449,'o' },{ 0x044a,'}' },{ 0x044b,'s' },
{ 0x044c,'m' },{ 0x044d,'\'' },{ 0x044e,'.' },{ 0x044f,'z' }
};


int TGKey::defaultSetCodePage(int id)
{
 switch (id)
   {
    case TVCodePage::PC855:
    case TVCodePage::PC866:
    case TVCodePage::ISORussian:
    case TVCodePage::KOI8r:
    case TVCodePage::KOI8_CRL_NMSU:
    case TVCodePage::CP1251:
    case TVCodePage::MacCyr:
    case TVCodePage::MacOSUkrainian:
         // Not sure about the rest of russian code pages.
         FillKeyMapForCP(id,RussianKeyboard,sizeof(RussianKeyboard)/sizeof(stIntCodePairs));
         NonASCII2ASCII=Generic_NonASCII2ASCII;
         CompareASCII=Generic_CompareASCII;
         AltInternat2ASCII=Generic_AltInternat2ASCII;
         break;
    case TVCodePage::PC737:
    case TVCodePage::PC869:
    case TVCodePage::CP1253:
    case TVCodePage::ISOGreek:
         FillKeyMapForCP(id,GreekKeyboard,sizeof(GreekKeyboard)/sizeof(stIntCodePairs));
         NonASCII2ASCII=Generic_NonASCII2ASCII;
         CompareASCII=Generic_CompareASCII;
         AltInternat2ASCII=Generic_AltInternat2ASCII;
         break;
    default:
         NonASCII2ASCII=defaultNonASCII2ASCII;
         CompareASCII=defaultCompareASCII;
         AltInternat2ASCII=defaultAltInternat2ASCII;
         return 0;
   }
 return 1;
}

uchar TGKey::KeyMap[128];

static
int compare(const void *v1, const void *v2)
{
 stIntCodePairs *p1=(stIntCodePairs *)v1;
 stIntCodePairs *p2=(stIntCodePairs *)v2;
 return (p1->unicode>p2->unicode)-(p1->unicode<p2->unicode);
}

/**[txh]********************************************************************

  Description:
  Fills the KeyMap table using the provided keyboard layout.
  
***************************************************************************/

void TGKey::FillKeyMapForCP(int id, stIntCodePairs *keyboard, size_t szKb)
{
 stIntCodePairs cp[256];
 TVCodePage::GetUnicodesForCP(id,cp);
 ushort *tr=TVCodePage::GetTranslate(id);
 int i;
 stIntCodePairs s;
 for (i=128; i<256; i++)
    {
     s.unicode=TVCodePage::UnicodeForInternalCode(tr[i]);
     void *res=bsearch(&s,keyboard,szKb,sizeof(stIntCodePairs),compare);
     KeyMap[i-128]=res ? ((stIntCodePairs *)res)->code : i;
    }
}

uchar TGKey::Generic_NonASCII2ASCII(uchar ascii)
{
 return ascii>=0x80 ? KeyMap[ascii-0x80] : ascii;
}

int TGKey::Generic_CompareASCII(uchar val, uchar code)
{
 if (val >=0x80) val =KeyMap[val- 0x80];
 if (code>=0x80) code=KeyMap[code-0x80];
 return val==code;
}

int TGKey::defaultAltInternat2ASCII(TEvent &)
{
 return 0;
}

#if 0
#define SP1 kbBackSpace
#define SP2 kbTab
#define SP3 kbEnter
#define SP4 kbEnter
#define SP5 kbEsc
#else
#define SP1 0x80 | kbH
#define SP2 0x80 | kbI
#define SP3 0x80 | kbJ
#define SP4 0x80 | kbM
#define SP5 0x80 | kbOpenBrace
#endif

const uchar TGKey::kbByASCII[128]=
{
 0,kbA,kbB,kbC,kbD,kbE,kbF,kbG,
 SP1,SP2,SP3,kbK,kbL,SP4,kbN,kbO,
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,
 kbX,kbY,kbZ,SP5,kbBackSlash,kbCloseBrace,kb6,kbMinus,
 kbSpace,kbAdmid,kbDobleQuote,kbNumeral,kbDolar,kbPercent,kbAmper,kbQuote,
 kbOpenPar,kbClosePar,kbAsterisk,kbPlus,kbComma,kbMinus,kbStop,kbSlash,
 kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,
 kb8,kb9,kbDoubleDot,kbColon,kbLessThan,kbEqual,kbGreaterThan,kbQuestion,
 kbA_Roba,kbA,kbB,kbC,kbD,kbE,kbF,kbG,
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,
 kbX,kbY,kbZ,kbOpenBrace,kbBackSlash,kbCloseBrace,kbCaret,kbUnderLine,
 kbGrave,kbA,kbB,kbC,kbD,kbE,kbF,kbG,
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,
 kbX,kbY,kbZ,kbOpenCurly,kbOr,kbCloseCurly,kbTilde,kbBackSpace
};

/**[txh]********************************************************************

  Description:
  When using a "non-ASCII" keyboard the application gets Alt+Non-ASCII. This
routine tries to figure out which key was used and changes the event to
be Alt+ASCII. Example: A greek keyboard can generate Alt+Alfa, in this case
the routine will convert it into Alt+A (because Alfa is in the key that A
is located).
  
  Return: !=0 if the event was altered.
  
***************************************************************************/

int TGKey::Generic_AltInternat2ASCII(TEvent &e)
{
 if (e.what==evKeyDown &&
     e.keyDown.charScan.charCode>=0x80 &&
     (e.keyDown.keyCode & (kbAltRCode | kbAltLCode)) &&
     (e.keyDown.keyCode & kbKeyMask)==kbUnkNown)
   {
    uchar key=KeyMap[e.keyDown.charScan.charCode-0x80];
    if (key<0x80)
      {
       e.keyDown.keyCode|=kbByASCII[key];
       return 1;
      }
   }
 return 0;
}

/**[txh]********************************************************************

  Description:
  Used by objects that needs the TEvent.keyDown.charCode filled.
  
***************************************************************************/

void TGKey::defaultFillCharCode(TEvent &e)
{
 if (e.keyDown.charCode!=0xFFFFFFFF && e.keyDown.charScan.charCode)
    e.keyDown.charCode=TVCodePage::convertInpCP_2_U16(e.keyDown.charScan.charCode);
}

