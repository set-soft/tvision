/*****************************************************************************

 Keyboard handler for DOS by Salvador E. Tropea (SET) (1998)

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

#ifdef __DJGPP__
#include <go32.h>
#include <sys/farptr.h>
#include <dpmi.h>
#include <pc.h>
#define REGS __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)
#endif

#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#include <tv.h>
#include <ctype.h>
#include <string.h>

int TGKey::useBIOS=0;
int TGKey::translateKeyPad=1;
KeyType TGKey::rawCode;
unsigned short TGKey::sFlags;
int  TGKey::Abstract;
char TGKey::ascii;

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

#ifdef __DJGPP__
static
unsigned char kbWithASCII0[256] =
{
 kbPause,kbEsc,kb1,kb2,kb3,kb4,kb5,0,                           // 00-07
 kb7,kb8,kb9,kb0,0,kbEqual,kbBackSpace,kbTab,                   // 08-0F
 kbQ,kbW,kbE,kbR,kbT,kbY,kbU,kbI,                               // 10-17
 kbO,kbP,kbOpenBrace,kbCloseBrace,kbEnter,0,kbA,kbS,            // 18-1F
 kbD,kbF,kbG,kbH,kbJ,kbK,kbL,kbColon,                           // 20-27
 kbQuote,kbGrave,0,kbBackSlash,kbZ,kbX,kbC,kbV,                 // 28-2F
 kbB,kbN,kbM,kbComma,kbStop,kbSlash,0,kbAsterisk,               // 30-37
 0,kbSpace,0,kbF1,kbF2,kbF3,kbF4,kbF5,                          // 38-3F
 kbF6,kbF7,kbF8,kbF9,kbF10,0,0,kbHome,                          // 40-47
 kbUp,kbPgUp,kbMinus,kbLeft,kb5,kbRight,kbPlus,kbEnd,           // 48-4F
 kbDown,kbPgDn,kbInsert,kbDelete,kbF1,kbF2,kbF3,kbF4,           // 50-57
 kbF5,kbF6,kbF7,kbF8,kbF9,kbF10,kbF1,kbF2,                      // 58-5F
 kbF3,kbF4,kbF5,kbF6,kbF7,kbF8,kbF9,kbF10,                      // 60-67
 kbF1,kbF2,kbF3,kbF4,kbF5,kbF6,kbF7,kbF8,                       // 68-6F
 kbF9,kbF10,kbPrnScr,kbLeft,kbRight,kbEnd,kbPgDn,kbHome,        // 70-77
 kb1,kb2,kb3,kb4,kb5,kb6,kb7,kb8,                               // 78-7F
 kb9,kb0,kbMinus,kbEqual,kbPgUp,kbF11,kbF12,kbF11,              // 80-87
 kbF12,kbF11,kbF12,kbF11,kbF12,kbUp,kbMinus,kb5,                // 88-8F
 kbPlus,kbDown,kbInsert,kbStop,kbTab,kbSlash,kbAsterisk,kbHome, // 90-97
 kbUp,kbPgUp,0,kbLeft,0,kbRight,0,kbEnd,                        // 98-9F
 kbDown,kbPgDn,kbInsert,kbDelete,kbSlash,kbTab,kbEnter,kbMacro, // A0-A7
 // I don't know if the following will be used:
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0
};

// This table uses just some values
static
unsigned char kbWithASCIIE0[256] =
{
 0,0,0,0,0,0,0,0, // 00-07
 0,0,0,0,0,0,0,0, // 08-0F
 0,0,0,0,0,0,0,0, // 10-17
 0,0,0,0,0,0,0,0, // 18-1F
 0,0,0,0,0,0,0,0, // 20-27
 0,0,0,0,0,0,0,0, // 28-2F
 0,0,0,0,0,0,0,0, // 30-37
 0,0,0,0,0,0,0,0, // 38-3F
 0,0,0,0,0,0,0,kbHome, // 40-47
 kbUp,kbPgUp,0,kbLeft,0,kbRight,0,kbEnd, // 48-4F
 kbDown,kbPgDn,kbInsert,kbDelete,0,0,0,0, // 50-57
 0,0,0,0,0,0,0,0, // 58-5F
 0,0,0,0,0,0,0,0, // 60-67
 0,0,0,0,0,0,0,0, // 68-6F
 0,0,0,kbLeft,kbRight,kbEnd,kbPgDn,kbHome, // 70-77
 0,0,0,0,0,0,0,0, // 78-7F
 0,0,0,0,kbPgUp,0,0,0, // 80-87
 0,0,0,0,0,kbUp,0,0, // 88-8F
 0,kbDown,kbInsert,kbDelete,0,0,0,0, // 90-97
 0,0,0,0,0,0,0,0, // 98-9F
 0,0,0,0,0,0,0,0, // A0-A7
 0,0,0,0,0,0,0,0, // A8-AF
 // Here are keys from the 105 keys keyboards.
 0,0,0,0,0,0,kbWinLeft,kbWinRight, // B0-B7
 kbWinSel,0,0,0,0,0,0,0, // B8-BF
 0,0,kbWinLeft,kbWinRight,kbWinSel,0,0,0, // C0-C7
 0,0,0,0,0,0,kbWinLeft,kbWinRight, // C8-CF
 kbWinSel,0,0,0,0,0,0,0, // D0-D7
 0,0,kbWinLeft,kbWinRight,kbWinSel,0,0,0, // D8-DF
 0,0,0,0,0,0,0,0, // E0-E7
 0,0,0,0,0,0,0,0, // E8-EF
 0,0,0,0,0,0,0,0, // F0-F7
 0,0,0,0,0,0,0,0  // F8-FF
};

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

static
unsigned char kbByASCII[128] =
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

unsigned short getshiftstate(void)
{
 if (TGKey::useBIOS)
   {
    REGS r;
    r.h.ah = 0x12;
    INTR(0x16,r);
    return r.x.ax;
   }
 _farsetsel(_dos_ds);
 return _farnspeekw(0x417);
}

// All the info. from BIOS in one call
void TGKey::GetRaw(void)
{
 if (useBIOS)
   {
    REGS r;
    r.h.ah = 0x12;
    INTR(0x16,r);
    sFlags=r.x.ax;
    r.h.ah = 0x10;
    INTR(0x16,r);
    rawCode.full=r.x.ax;
   }
 else
  {
   _farsetsel(_dos_ds);
   sFlags=_farnspeekw(0x417);
   unsigned short keybuf_start = _farnspeekw(0x41a);
   rawCode.full=_farnspeekw(0x400 + keybuf_start);
   keybuf_start += 2;
   if (keybuf_start>0x3d) keybuf_start = 0x1e;
   _farnspokew(0x41a, keybuf_start);
  }
}

// The intelligence is here
unsigned short TGKey::gkey(void)
{
 Abstract=0;

 GetRaw();
 // Compose the shift flags:

 if (sFlags & 3)
    Abstract|=kbShiftCode;
 if (sFlags & 4)
    Abstract|=kbCtrlCode;
 switch (AltSet)
   {
    case 0: // Normal thing, left is left, right is right
         if (sFlags & 0x200)
            Abstract|=kbAltLCode;
         else
            if (sFlags & 8)
               Abstract|=kbAltRCode;
         break;
    case 1: // Reverse thing
         if (sFlags & 0x200)
            Abstract|=kbAltRCode;
         else
            if (sFlags & 8)
               Abstract|=kbAltLCode;
         break;
    default: // Compatibility
         if (sFlags & 0x208)
            Abstract|=kbAltLCode;
   }

 // Translate the rest
 if (rawCode.b.scan==0)
   {
    if (rawCode.b.ascii==0)
      { // Very singular 00/00
       Abstract|=kbPause;
       ascii=0;
      }
    else
      { // Generated by Alt-key pad or a TSR
       Abstract|=kbUnkNown;
       ascii=rawCode.b.ascii;
      }
   }
 else
 if (rawCode.b.ascii==0 || rawCode.b.ascii==0xF0)
   { // Alt keys and other strange things
    Abstract|=kbWithASCII0[rawCode.b.scan];
    ascii=0;//rawCode.b.scan; That's good bur annoying, so now I return 0
   }
 else
 if (rawCode.b.ascii==0xE0)
   {
    Abstract|=kbWithASCIIE0[rawCode.b.scan];
    ascii=0;//rawCode.b.scan;
   }
 else
   { // That's the rest, they are translated by the ASCII because depends
     // on the keyboard layout.
   ascii=rawCode.b.ascii;
#if 0
   // Making that I loose:
   // ^H == BackSpace, ^I == Tab, ^J == ^Enter, ^M == Enter, ^[ == Esc
   if (rawCode.b.ascii<128)
      Abstract|=kbByASCII[rawCode.b.ascii];
#else
   // As ^J and ^[ are very used by me I preffer that:
   if (rawCode.b.ascii<128)
     {
      unsigned char val=kbByASCII[rawCode.b.ascii];
      if (val<128)
        {
         // I think the following is a bug in the BIOS: Numlock OFF then
         // key pad 6 is right, OK, but if you press shift+6 you get the
         // ASCII of 6!!!!
         if (translateKeyPad && rawCode.b.ascii>='0' &&
             rawCode.b.ascii<='9' && (Abstract & kbShiftCode))
            Abstract|=kbWithASCIIE0[rawCode.b.scan];
         else
            Abstract|=val;
        }
      else
        {
         val&=0x7F;
         #define KC(a,c) case a: if (sFlags & 4) Abstract|=val; \
                 else Abstract|=c; break;
         switch (val)
           {
            KC(kbH,kbBackSpace)
            KC(kbI,kbTab)
            KC(kbM,kbEnter)
            // Here I assume that ESC, ENTER and pENTER doesn't change from
            // keyboard to keyboard
            case kbOpenBrace:
                 if (rawCode.b.scan==1)
                    Abstract|=kbEsc;
                 else
                    Abstract|=val;
                 break;
            case kbJ:
                 if (rawCode.b.scan==0x1C || rawCode.b.scan==0xE0)
                    Abstract|=kbEnter;
                 else
                    Abstract|=val;
                 break;
           }
         #undef KC
        }
     }
#endif
   }
 return rawCode.full;
}

int TGKey::kbhit(void)
{
 if (useBIOS)
    return ::kbhit();
 else
    return (_farpeekw(_dos_ds, 0x41a)!=_farpeekw(_dos_ds, 0x41c));
}

void TGKey::clear(void)
{
 if (useBIOS)
    while (::kbhit())
      gkey();
 else
    _farpokel(_dos_ds,0x41A,0x001E001EUL);
 // the bios has no function for clearing the key buffer
 // But you can loop until is empty ;-)
}

void TGKey::fillTEvent(TEvent &e)
{
 TGKey::gkey();
 e.keyDown.charScan.charCode=ascii;
 e.keyDown.charScan.scanCode=TGKey::rawCode.b.scan;
 e.keyDown.raw_scanCode=TGKey::rawCode.b.scan;
 e.keyDown.keyCode=Abstract;
 e.keyDown.shiftState=sFlags;
 e.what=evKeyDown;
}

ushort TGKey::AltSet=2;      // Default: Both ALT are the same
#endif

const int CantDef=0x39;
static const char altCodes[CantDef+1]=
"\0ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\0\0""0123456789\0\t\0;'`,./*\xf0-+\0=";

char TGKey::GetAltChar(ushort keyCode)
{
 // Only when ALT is present
 if ((keyCode & kbAltLCode)==0)
    return 0;
 keyCode&=kbKeyMask;
 if (keyCode>CantDef-1)
    return 0;
 return altCodes[keyCode];
}

ushort TGKey::GetAltCode(char c)
{
 int i;
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

//---------------- TEST
#ifdef GKEY
#include <stdio.h>


void InterpretAbstract(void)
{
 int key=TGKey::Abstract & kbKeyMask;
 printf("Abstract: kb%s",TGKey::KeyNames[key]);
 if (TGKey::Abstract & kbShiftCode)
    printf(" SHIFT");
 if (TGKey::Abstract & kbCtrlCode)
    printf(" CTRL");
 if (TGKey::Abstract & kbAltRCode)
    printf(" ALT-R");
 if (TGKey::Abstract & kbAltLCode)
    printf(" ALT-L");
}

#ifdef __DJGPP__
#include <signal.h>
#include <conio.h>
/* ungetch() is available only on DJGPP
static void PasarAC(int Sig)
{
 Sig=0;
}*/
#undef kbhit
void CtrlCOff(void)
{
 //signal(SIGINT,PasarAC);
 signal(SIGINT,SIG_IGN);
}
#endif

int count=0;

int main(int argc, char *argv[])
{
  unsigned short key=0;
#ifdef __linux__
  void startcurses();
  void stopcurses();
  void patch_keyboard();
  startcurses();
  patch_keyboard();
  if (argc>1 && strcmp(argv[1],"rh52")==0)
     TGKey::SetKbdMapping(KBD_REDHAT52_STYLE);
#else
  //TGKey::useBIOS=1;
  CtrlCOff();
#endif
  // Setup the mode where the alt left/right are different
  TGKey::SetAltSettings(0);
  do
  {
   while (!TGKey::kbhit());// {count++; if ((count%1000)==0) printf(".");}
   TEvent e;
   TGKey::fillTEvent(e);
   if (e.what!=evNothing)
     {
      key = TGKey::Abstract;
      printf("Shiftstate: %04x RawCode: %04x ASCII: %X  SCAN: %04X\r\n",
             TGKey::sFlags,key,key & 0xff,e.keyDown.raw_scanCode);
      InterpretAbstract();
      printf(" ASCII: %c\r\n",TGKey::ascii);
     }
  } while (key!=kbEsc);
#ifdef __linux__
  stopcurses();
#endif
}
#endif

