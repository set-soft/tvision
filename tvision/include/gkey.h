#ifndef __GKEY_H__
#define __GKEY_H__
// A type for the BIOS ASCII/Scan
typedef union
{
 unsigned short full;
 struct
 {
   unsigned char ascii;
   unsigned char scan;
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

class TEvent;

// A class to encapsulate the globals, all is static!
class TGKey
{
public:
 TGKey() {};
 static int  kbhit(void);
 static void clear(void);
 static unsigned short gkey(void);
 static void fillTEvent(TEvent &e);

 // Values that must be provided by any replacement
 static int  Abstract;
 static char ascii;
 static char *KeyNames[]; // Because we need it for config.
 // Very used by: menues and hotkeys (buttons, status bar, etc).
 static char GetAltChar(unsigned short keyCode, uchar ascii);
 static unsigned short GetAltCode(char ch);
 static int CompareASCII(uchar val, uchar code);
 static ushort GetAltSettings(void) { return AltSet; }
 static void SetAltSettings(ushort altSet) { AltSet=altSet; }
 static ushort KeyNameToNumber(char *s);
 #ifdef __linux__
 #define KBD_OLD_STYLE      0
 #define KBD_REDHAT52_STYLE 1
 #define KBD_XTERM_STYLE    2
 #define KBD_NO_XTERM_STYLE 3
 #define KBD_ETERM_STYLE    4
 #else
 #define KBD_US             0
 #define KBD_GREEK_CP737    1
 #endif
 static void SetKbdMapping(int version);
 static int  GetKbdMapping(void) { return Mode; };

 // PC specific
 static int useBIOS;
 static int translateKeyPad;

 // Here just for test
 static unsigned short sFlags;

protected:
 static void GetRaw(void);

 // 0 => Left alt is used
 // 1 => Right alt is used
 // 2 => Both alts are the same
 static ushort AltSet;

 static KeyType rawCode;
 // SetKbdMapping:
 static int Mode;
};

extern unsigned short getshiftstate();
extern unsigned short __tv_getshiftstate();
extern unsigned short __tv_GetRaw();
extern int __tv_kbhit();
extern void __tv_clear();
#endif
