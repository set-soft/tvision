/*
  Copyright (c) 2001-2009 by Salvador E. Tropea (SET) <set@ieee.org>
  This code is covered by the GPL license. A copy of the license should
be provided in the same package.

  Modified by Grzegorz Adam Hankiewicz <gradha@hankiewicz.datanet.co.uk>
  to be used as a Turbo Vision driver.

  conio.h emulation library on top of Allegro
*/
#ifndef AlCon_HEADER_INCLUDED
#define AlCon_HEADER_INCLUDED

#ifdef HAVE_ALLEGRO

#include <allegro.h>

#define AlCon_VERSION_MAJOR  0
#define AlCon_VERSION_MIDDLE 0
#define AlCon_VERSION_MINOR  2
#define AlCon_VERSION "0.0.2"

typedef unsigned short uint16;
typedef unsigned char uchar;
/* Must match TV's TScreenColor */
struct AlCon_Color
{
 uchar R,G,B,Alpha;
};

/* Functions to hide/unhide the mouse and text cursors. */
extern void AlCon_ScareCursor();
extern void AlCon_UnScareCursor();
extern void AlCon_DisableAsync();
extern void AlCon_EnableAsync();

/* Cursor positioning routines. */
extern void AlCon_CursorHome();
extern void AlCon_GotoXY(int x, int y);
extern int AlCon_WhereX();
extern int AlCon_WhereY();
extern int AlCon_ScreenCols();
extern int AlCon_ScreenRows();

/* Color selection routines. */
extern void AlCon_SetColors(int aFg, int aBg);
extern void AlCon_TextAttr(int attr);
extern void AlCon_TextBackground(int aBg);
extern void AlCon_TextColor(int aFg);

/* Clear routines */
extern void AlCon_Clear();
extern void AlCon_ClrScr();

/* Text output routines, including newline and tab management */
extern void AlCon_NewLine();
extern void AlCon_PutStr(const char *s);
extern void AlCon_Puts(const char *s);
extern void AlCon_Tab();
extern void AlCon_CPrintf(const char *format, ...);
extern void AlCon_PutBuf(unsigned offset, uint16 *buffer, int len);
extern void AlCon_PutChar(unsigned offset, uint16 value);
extern void AlCon_GetScrChars(unsigned offset, uint16 *buf, unsigned count);

/* Text cursor routines. */
extern void AlCon_UnDrawCursor(int *aFgCol);
extern void AlCon_IntCursor();
extern void AlCon_SetCursorShape(int from, int to);
extern void AlCon_GetCursorShape(int *from, int *to);
extern void AlCon_HideCursor();
extern void AlCon_ShowCursor();
extern int AlCon_IsVisCursor();

/* Initialization routines */
extern int AlCon_Init(int w, int h, int fw=-1, int fh=-1, uchar *fdata=NULL,
                      AlCon_Color *pal=NULL);
extern int AlCon_Resize(unsigned sW, unsigned sH, int new_font_width,
                        int new_font_height);
extern void AlCon_Exit();

/* Keyboard input routines */
extern int AlCon_GetCh();
extern int AlCon_KbHit();
extern int AlCon_GetKey(unsigned *symbol, uchar *scan, unsigned *flags);
extern void AlCon_ClearKeyBuf();

/* Mouse input routines */
extern int AlCon_GetMouseButtons(void);
extern void AlCon_GetMousePos(int *x, int *y, int *buttons);

/* Fonts */
extern void AlCon_SetFont(int which, uchar *fnt, unsigned w, unsigned h);
extern void AlCon_GetFontGeometry(unsigned int *w, unsigned int *h);
extern void AlCon_EnableSecFont();
extern void AlCon_DisableSecFont();
/* Our default font */
extern uchar AlCon_ShapeFont8x16[];
extern uchar AlCon_ShapeFont10x20[];

/* Externally forced keyboard mapping */
extern char AlCon_ForcedKeyboard[4];

/* Extra bits which can be set for strange cursor effects */
extern int AlCon_CursorBits;
#define ALCON_CURSOR_HIDE_NORMAL       0x0001
#define ALCON_CURSOR_ON_SQUARE         0x0002
#define ALCON_CURSOR_ON_CROSS          0x0004

int AlCon_SetDisPaletteColors(int from, int number, AlCon_Color *colors);
/* Only for readback */
extern AlCon_Color AlCon_CurPalette[16];

extern void AlCon_Redraw();

#ifndef AlCon_NO_KEYDEFS

#ifdef __cplusplus
const uchar
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
kbMouse=102,kbEterm=103;

/* Shift constants */
//const uint16
//      kbShiftCode=0x080,
//      kbCtrlCode =0x100,
//      kbAltRCode =0x400,
//      kbAltLCode =0x200,
//      kbKeyMask  =0x07F;
#else /* __cplusplus */

#define kbUnkNown 0
#define kbA  1
#define kbB  2
#define kbC  3
#define kbD  4
#define kbE  5
#define kbF  6
#define kbG  7
#define kbH  8
#define kbI  9
#define kbJ 10
#define kbK 11
#define kbL 12
#define kbM 13
#define kbN 14
#define kbO 15
#define kbP 16
#define kbQ 17
#define kbR 18
#define kbS 19
#define kbT 20
#define kbU 21
#define kbV 22
#define kbW 23
#define kbX 24
#define kbY 25
#define kbZ 26
#define kbOpenBrace 27
#define kbBackSlash 28
#define kbCloseBrace 29
#define kbPause 30
#define kbEsc 31
#define kb0 32
#define kb1 33
#define kb2 34
#define kb3 35
#define kb4 36
#define kb5 37
#define kb6 38
#define kb7 39
#define kb8 40
#define kb9 41
#define kbBackSpace 42
#define kbTab 43
#define kbEnter 44
#define kbColon 45
#define kbQuote 46
#define kbGrave 47
#define kbComma 48
#define kbStop 49
#define kbSlash 50
#define kbAsterisk 51
#define kbSpace 52
#define kbMinus 53
#define kbPlus 54
#define kbPrnScr 55
#define kbEqual 56
#define kbF1 57
#define kbF2 58
#define kbF3 59
#define kbF4 60
#define kbF5 61
#define kbF6 62
#define kbF7 63
#define kbF8 64
#define kbF9 65
#define kbF10 66
#define kbF11 67
#define kbF12 68
#define kbHome 69
#define kbUp 70
#define kbPgUp 71
#define kbLeft 72
#define kbRight 73
#define kbEnd 74
#define kbDown 75
#define kbPgDn 76
#define kbInsert 77
#define kbDelete 78
#define kbCaret 79
#define kbAdmid 80
#define kbDobleQuote 81
#define kbNumeral 82
#define kbDolar 83
#define kbPercent 84
#define kbAmper 85
#define kbOpenPar 86
#define kbClosePar 87
#define kbDoubleDot 88
#define kbLessThan 89
#define kbGreaterThan 90
#define kbQuestion 91
#define kbA_Roba 92
#define kbOr 93
#define kbUnderLine 94
#define kbOpenCurly 95
#define kbCloseCurly 96
#define kbTilde 97
#define kbMacro 98
#define kbWinLeft 99
#define kbWinRight 100
#define kbWinSel 101
#define kbMouse 102
#define kbEterm 103

//#define kbShiftCode 0x080
//#define kbCtrlCode  0x100
//#define kbAltRCode  0x400
//#define kbAltLCode  0x200
//#define kbKeyMask   0x07F

#endif /* __cplusplus */

#endif /* !AlCon_NO_KEYDEFS */

extern const char *AlCon_KeyNames[];

#define charPos 0
#define attrPos 1

#ifdef Uses_AlCon_conio
/*****************************************************************************

  Conio style definitions

*****************************************************************************/

#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGRAY     7
#define DARKGRAY      8
#define LIGHTBLUE     9
#define LIGHTGREEN   10
#define LIGHTCYAN    11
#define LIGHTRED     12
#define LIGHTMAGENTA 13
#define YELLOW       14
#define WHITE        15

#define cprintf(a...)     AlCon_CPrintf(a)
#define gotoxy(a,b)       AlCon_GotoXY(a,b)
#define getch()           AlCon_GetCh()
#define kbhit()           AlCon_KbHit()
#define textattr(a)       AlCon_TextAttr(a)
#define cputs(a)          AlCon_PutStr(a)
#define blinkvideo()      // Not supported
#define intensevideo()    // Ever in this mode
#define clrsrc()          AlCon_ClrScr()
#define wherex()          AlCon_WhereX()
#define wherey()          AlCon_WhereY()
#define textbackground(a) AlCon_TextBackground(a)
#define textcolor(a)      AlCon_TextColor(a)
#define ScreenCols()      AlCon_ScreenCols()
#define ScreenRows()      AlCon_ScreenRows()
#endif

#endif // HAVE_ALLEGRO
#endif
