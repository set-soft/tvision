/**[txh]********************************************************************

  Keyboard handler for DOS.
  Copyright (c) 1998-2004 by Salvador E. Tropea (SET)

  Description: 
  This module implements the low level keyboard routines for DOS.
  The original comment about the first TGKey class I sent to Robert when was
the maintainer of TV is located at the end of this file. It have some
intersting information.

  Configuration variables:
  BIOSKey

***************************************************************************/

#include <tv/configtv.h>

#define Uses_ctype
#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#define Uses_string
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp

#include <go32.h>
#include <sys/farptr.h>
#include <dpmi.h>
#include <pc.h>
#include <bios.h>
#define REGS __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)

#define DEF_REGS   __dpmi_regs r
#define KEY_INTR() __dpmi_int(0x16,&r)
#define AH         r.h.ah
#define AX         r.x.ax

#include <tv/dos/key.h>

#define GET_ENHANCED_KEYSTROKE          0x0010
#define GET_EXTENDED_SHIFT_STATES       0x0012
// Keyboard definitions:
#define KEYBOARD_STATUS_FLAGS_1         0x0417
// Bitfields for keyboard status flags 1:
// Bit(s)  Description     (Table M010)
//  7      INSert active
//  6      Caps Lock active
//  5      Num Lock active
//  4      Scroll Lock active
//  3      either Alt pressed
//  2      either Ctrl pressed
//  1      Left Shift pressed
//  0      Right Shift pressed
#define KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF   0x041A
#define KBD_POINTER_TO_FIRST_FREE_SLOT_IN_BUF 0x041C
// KM00400080  - KEYBOARD BUFFER START OFFSET FROM SEGMENT 40h (normally 1Eh)
#define KEYBOARD_BUFFER_START_OFFSET          0x0480
// KM00400082  - KEYBOARD BUFFER END+1 OFFSET FROM SEGMENT 40h (normally 3Eh)
#define KEYBOARD_BUFFER_END_1_OFFSET          0x0482


KeyType TGKeyDOS::rawCode;
int     TGKeyDOS::Abstract;
char    TGKeyDOS::ascii;
char    TGKeyDOS::useFixKbdBuffer;
ushort  TGKeyDOS::sFlags;
int     TGKeyDOS::useBIOS=0;
int     TGKeyDOS::translateKeyPad=1;
void  (*TGKeyDOS::GetRaw)(void)=GetRawDirect;

const uchar TGKeyDOS::kbWithASCII0[256]=
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
const uchar TGKeyDOS::kbWithASCIIE0[256]=
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

unsigned TGKeyDOS::GetShiftStateBIOS(void)
{
 DEF_REGS;
 AH=GET_EXTENDED_SHIFT_STATES;
 KEY_INTR();
 return AX;
}

unsigned TGKeyDOS::GetShiftStateDirect(void)
{
 _farsetsel(_dos_ds);
 return _farnspeekw(KEYBOARD_STATUS_FLAGS_1);
}

// All the info. from BIOS in one call
void TGKeyDOS::GetRawBIOS(void)
{
 DEF_REGS;
 AH=GET_EXTENDED_SHIFT_STATES;
 KEY_INTR();
 sFlags=AX;
 AH=GET_ENHANCED_KEYSTROKE;
 KEY_INTR();
 rawCode.full=AX;
}

void TGKeyDOS::GetRawDirect(void)
{
 _farsetsel(_dos_ds);
 sFlags=_farnspeekw(KEYBOARD_STATUS_FLAGS_1);
 ushort keybuf_nextchar=_farnspeekw(KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF);
 rawCode.full=_farnspeekw(0x400+keybuf_nextchar);
 keybuf_nextchar+=2;
 if (useFixKbdBuffer)
   {
    if (keybuf_nextchar>=0x3e)
       keybuf_nextchar=0x1e;
   }
 else
   {
    if (keybuf_nextchar>=_farnspeekw(KEYBOARD_BUFFER_END_1_OFFSET)) // if past the buffer...
       keybuf_nextchar=_farnspeekw(KEYBOARD_BUFFER_START_OFFSET);   // ...wrap to it's start
   }
 _farnspokew(KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF,keybuf_nextchar);
}


// The intelligence is here
unsigned short TGKeyDOS::GKey(void)
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
       ascii=rawCode.b.ascii;
       Abstract|=kbUnkNown;
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
   // Making that I lose:
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

int TGKeyDOS::KbHitBIOS(void)
{
 return bioskey(1);
}

int TGKeyDOS::KbHitDirect(void)
{
 return (_farpeekw(_dos_ds,KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF)
         !=_farpeekw(_dos_ds,KBD_POINTER_TO_FIRST_FREE_SLOT_IN_BUF));
}

void TGKeyDOS::ClearBIOS(void)
{
 while (kbhit())
   GKey();
 // the bios has no function for clearing the key buffer
 // But you can loop until is empty ;-)
}

void TGKeyDOS::ClearDirect(void)
{
 if (useFixKbdBuffer)
    _farpokel(_dos_ds,KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF,0x001E001EUL);
 else
   {
    _farsetsel(_dos_ds);
    ulong keybuf_start=_farnspeekw(KEYBOARD_BUFFER_START_OFFSET);
    _farnspokel(KBD_POINTER_TO_NEXT_CHAR_IN_KBD_BUF,(keybuf_start<<16)+keybuf_start);
   }
}

void TGKeyDOS::FillTEvent(TEvent &e)
{
 GKey();
 e.keyDown.charScan.charCode=ascii;
 e.keyDown.charScan.scanCode=rawCode.b.scan;
 e.keyDown.raw_scanCode=rawCode.b.scan;
 e.keyDown.keyCode=Abstract;
 e.keyDown.shiftState=sFlags;
 e.what=evKeyDown;
}

void TGKeyDOS::SetKbdMapping(int version)
{
 if (version==dosUseBIOS)
   {
    useBIOS=1;
    TGKey::kbhit=KbHitBIOS;
    TGKey::getShiftState=GetShiftStateBIOS;
    TGKey::clear=ClearBIOS;
    GetRaw=GetRawBIOS;
    return;
   }
 else if (version==dosUseDirect)
   {
    useBIOS=0;
    TGKey::kbhit=KbHitDirect;
    TGKey::getShiftState=GetShiftStateDirect;
    TGKey::clear=ClearDirect;
    GetRaw=GetRawDirect;
    return;
   }
 else if (version==dosTranslateKeypad)
   {
    translateKeyPad=1;
   }
 else if (version==dosNormalKeypad)
   {
    translateKeyPad=0;
   }
 Mode=version;
}

int TGKeyDOS::GetKbdMapping(int version)
{
 if (version==dosUseBIOS)
   {
    return useBIOS;
   }
 else if (version==dosUseDirect)
   {
    return !useBIOS;
   }
 else if (version==dosTranslateKeypad)
   {
    return translateKeyPad;
   }
 else if (version==dosNormalKeypad)
   {
    return !translateKeyPad;
   }
 return 0;
}

void TGKeyDOS::Init()
{
 AltSet=2;
 TGKey::gkey=GKey;
 TGKey::fillTEvent=FillTEvent;
 TGKey::SetKbdMapping=SetKbdMapping;
 TGKey::GetKbdMapping=GetKbdMapping;
 // Default to direct mode
 long useBIOS=0;
 TScreen::optSearch("BIOSKey",useBIOS);
 SetKbdMapping(useBIOS ? dosUseBIOS : dosUseDirect);
 // SET: Original code assumed the circular buffer starts at offset 0x1E and
 // ends at offset 0x3D. But some utilities to grow the buffer exists.
 // Egon Eckert <egon.heaven.industries.cz> contributed a patch to fix it.
 // As it was incorporated between 2.0.3 RC1 and 2.0.3 I added an option to
 // rollback without recompiling.
 long aux=0;
 TScreen::optSearch("FixKbdBuffer",aux);
 useFixKbdBuffer=aux!=0;
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

int count=0;

int main(int argc, char *argv[])
{
  unsigned short key=0;
  TGKey::useBIOS=0;
  CtrlCOff();
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
}
#endif

#else // TVCompf_djgpp

#include <tv/dos/key.h>

#endif // else TVCompf_djgpp

/*
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
*/
