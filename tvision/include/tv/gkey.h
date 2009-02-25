/* Keyboard handler routines header.
   Copyright by Salvador E. Tropea (SET) (1998,1999,2001)
   Covered by the GPL license. */
#ifndef GKEY_HEADER_INCLUDED
#define GKEY_HEADER_INCLUDED
// A type for the BIOS ASCII/Scan
typedef union
{
 unsigned short full;
 struct
 {
   #ifdef TV_BIG_ENDIAN
   unsigned char scan;
   unsigned char ascii;
   #else
   unsigned char ascii;
   unsigned char scan;
   #endif
 } b;
} KeyType;

// Shift constants
const ushort
      kbShiftCode=0x080,
      kbCtrlCode =0x100,
      kbAltRCode =0x400,
      kbAltLCode =0x200,
      kbKeyMask  =0x07F;

#ifdef Uses_FullSingleKeySymbols
// Key constants, basically they are from the US keyboard, but all of them
// are standard ASCII and not extended.
const ushort
kbUnkNown=0,
kbA= 1,kbB= 2,kbC= 3,kbD= 4,kbE= 5,kbF= 6,kbG= 7,kbH= 8,kbI= 9,kbJ=10,kbK=11,
kbL=12,kbM=13,kbN=14,kbO=15,kbP=16,kbQ=17,kbR=18,kbS=19,kbT=20,kbU=21,kbV=22,
kbW=23,kbX=24,kbY=25,kbZ=26,
kbOpenBrace=27,kbBackSlash=28,kbCloseBrace=29,kbPause=30,kbEsc=31,
kb0=32,kb1=33,kb2=34,kb3=35,kb4=36,kb5=37,kb6=38,kb7=39,kb8=40,kb9=41,
kbBackSpace=42,kbTab=43,kbEnter=44,kbColon=45,kbQuote=46,kbGrave=47,
kbComma=48,kbStop=49,kbSlash=50,kbAsterisk=51,kbSpace=52,kbMinus=53,
kbPlus=54,kbPrnScr=55,kbEqual=56,kbF1=57,kbF2=58,kbF3=59,kbF4=60,kbF5=61,
kbF6=62,kbF7=63,kbF8=64,kbF9=65,kbF10=66,kbF11=67,kbF12=68,kbHome=69,
kbUp=70,kbPgUp=71,kbLeft=72,kbRight=73,kbEnd=74,kbDown=75,kbPgDn=76,
kbInsert=77,kbDelete=78,kbCaret=79,kbAdmid=80,kbDobleQuote=81,
kbNumeral=82,kbDolar=83,kbPercent=84,kbAmper=85,kbOpenPar=86,
kbClosePar=87,kbDoubleDot=88,kbLessThan=89,kbGreaterThan=90,
kbQuestion=91,kbA_Roba=92,kbOr=93,kbUnderLine=94,kbOpenCurly=95,
kbCloseCurly=96,kbTilde=97,kbMacro=98,kbWinLeft=99,kbWinRight=100,
kbWinSel=101,
kbMouse=102,kbEterm=103
;
#endif

/* IBM BIOS flags, not all implemented in most platforms */
/*
bit 15: SysReq key pressed;
bit 14: Caps lock key currently down;
bit 13: Num lock key currently down;
bit 12: Scroll lock key currently down;
bit 11: Right alt key is down;
bit 10: Right ctrl key is down;
bit 9: Left alt key is down;
bit 8: Left ctrl key is down;
bit 7: Insert toggle;
bit 6: Caps lock toggle;
bit 5: Num lock toggle;
bit 4: Scroll lock toggle;
bit 3: Either alt key is down (some machines, left only);
bit 2: Either ctrl key is down;
bit 1: Left shift key is down;
bit 0: Right shift key is down
*/
const unsigned
 kbSysReqPress     =0x8000,
 kbCapsLockDown    =0x4000,
 kbNumLockDown     =0x2000,
 kbScrollLockDown  =0x1000,
 kbRightAltDown    =0x0800,
 kbRightCtrlDown   =0x0400,
 kbLeftAltDown     =0x0200,
 kbLeftCtrlDown    =0x0100,
 kbInsertToggle    =0x0080,
 kbCapsLockToggle  =0x0040,
 kbNumLockToggle   =0x0020,
 kbScrollLockToggle=0x0010,
 kbAltDown         =0x0008,
 kbCtrlDown        =0x0004,
 kbLeftShiftDown   =0x0002,
 kbRightShiftDown  =0x0001;


struct TEvent;

// A class to encapsulate the globals, all is static!
class CLY_EXPORT TGKey
{
public:
 TGKey() { resume(); };

 static void       suspend();
 static void       resume();
 static void     (*Suspend)();
 static void     (*Resume)();

 static int      (*kbhit)();
 static void     (*clear)();
 static ushort   (*gkey)();
 // This is optional. Some platforms doesn't support it and the returned value
 // can be outdated.
 static unsigned (*getShiftState)();
 static void     (*fillTEvent)(TEvent &e);
 // This is used by objects that needs Unicode16 as input
 static void     (*fillCharCode)(TEvent &e);

 // Very used by: menues and hotkeys (buttons, status bar, etc).
 // Get the ascii associated to Alt+key, example Alt+A => A
 static char     GetAltChar(unsigned short keyCode, uchar ascii);
 // The reverse (know the alt code when we know the ASCII)
 static ushort   GetAltCode(uchar ch);
 // Find the code for the name of a key
 static ushort   KeyNameToNumber(char *s);
 static const
           char *NumberToKeyName(unsigned val);

 // Alt keys interpretation
 static ushort   GetAltSettings(void) { return AltSet; }
 static void     SetAltSettings(ushort altSet) { AltSet=altSet; }

 // Special functions when we must deal with international stuff like a
 // shortcut in greek or cyrilic.
 // When a key character is over 128 and we want to know the ascii of
 // the key that generates it.
 static uchar  (*NonASCII2ASCII)(uchar val);
 // Compares two values according to the associated ASCII.
 static int    (*CompareASCII)(uchar val, uchar code);
 // Finds the Alt that generated it.
 static int    (*AltInternat2ASCII)(TEvent &event);

 // Routine to setup the input code page
 static int    (*SetCodePage)(int id);

 enum keyMode
 {
  // Linux styles
  linuxDisableKeyPatch=1,
  linuxEnableKeyPatch=2,
  // DOS
  // Should be incorporated to the remapping but I need volunteers
  //dosUS=0,
  //dosGreek737=20, No longer used
  dosUseBIOS=21,
  dosUseDirect=22,
  dosTranslateKeypad=23,
  dosNormalKeypad=24,
  // UNIX styles
  unixXterm=40,
  unixNoXterm=41,
  unixEterm=42
 };
 static void   (*SetKbdMapping)(int version);
 static int    (*GetKbdMapping)(int version);

 enum { codepage=0, unicode16=1 };
 static int     getInputMode() { return inputMode; }

protected:
 static uchar    defaultNonASCII2ASCII(uchar val);
 static int      defaultCompareASCII(uchar val, uchar code);
 static void     defaultSetKbdMapping(int version);
 static int      defaultGetKbdMapping(int version);
 static int      defaultKbhit(void);
 static void     defaultClear(void);
 static ushort   defaultGkey(void);
 static unsigned defaultGetShiftState();
 static void     defaultFillTEvent(TEvent &e);
 static void     defaultSuspend();
 static void     defaultResume();
 static int      defaultSetCodePage(int id);
 static int      defaultAltInternat2ASCII(TEvent &event);
 static void     defaultFillCharCode(TEvent &e);

 // Greek table
 static stIntCodePairs GreekKeyboard[];
 // Russian table
 static stIntCodePairs RussianKeyboard[];
 // Generic tables
 static uchar KeyMap[128];
 static const uchar
              kbByASCII[128];
 static void  FillKeyMapForCP(int id, stIntCodePairs *keyboard, size_t szKb);
 static uchar Generic_NonASCII2ASCII(uchar ascii);
 static int   Generic_CompareASCII(uchar val, uchar code);
 static int   Generic_AltInternat2ASCII(TEvent &e);

 // Needed for configuration.
 static const char *KeyNames[];
 // 0 => Left alt is used
 // 1 => Right alt is used
 // 2 => Both alts are the same
 static ushort   AltSet;
 static char     suspended;

 //static KeyType rawCode;
 // SetKbdMapping:
 static int      Mode;
 static int      inputMode;
};

extern unsigned short getshiftstate();
extern unsigned short __tv_getshiftstate();
extern unsigned short __tv_GetRaw();
extern int __tv_kbhit();
extern void __tv_clear();
#endif // GKEY_HEADER_INCLUDED
