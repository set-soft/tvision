/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  It said:
 This code is GPL, see copying file for more details.
 Keyboard handler for Win32 Copyright by Anatoli Soltan (2000)
 Based on the handler for Linux Copyright by Salvador E. Tropea (SET) (1998,1999)

  Description:
  WinNT Keyboard routines.
  The original implementation was done by Anatoli, I removed some code, added
some routines and adapted it to the new architecture.

  ToDo:
  * Must check GetShiftState, I think the return value is wrong.
  
***************************************************************************/
#include <tv/configtv.h>

#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/winnt/screen.h>
#include <tv/winnt/key.h>

INPUT_RECORD TGKeyWinNT::inpRec;
unsigned     TGKeyWinNT::shiftState;
ushort       TGKeyWinNT::vk2kk[]=
{
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x002a, 0x002b, 0x0000, 0x0000, 0x0000, 0x002c, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x001f, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0034, 0x0047, 0x004c, 0x004a, 0x0045, 0x0048, 0x0046, 0x0049,
 0x004b, 0x0000, 0x0000, 0x0000, 0x0000, 0x004d, 0x004e, 0x0000,
 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
 0x0028, 0x0029, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
 0x0018, 0x0019, 0x001a, 0x0063, 0x0064, 0x0065, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0033, 0x0036, 0x0000, 0x0035, 0x0000, 0x0032,
 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040,
 0x0041, 0x0042, 0x0043, 0x0044, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x002d, 0x0038, 0x0059, 0x005e, 0x005a, 0x005b,
 0x0061, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0056, 0x001c, 0x0057, 0x002e, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

unsigned TGKeyWinNT::GetShiftState()
{
 return shiftState;
}

void TGKeyWinNT::Clear(void)
{
 FlushConsoleInputBuffer(TScreenWinNT::hIn);
}

int TGKeyWinNT::KbHit(void)
{
 DWORD cEvents;

 while (PeekConsoleInput(TScreenWinNT::hIn,&inpRec,1,&cEvents) && cEvents==1)
   {
    if (inpRec.EventType!=KEY_EVENT)
      {
       // The following two lines are commented because if someone waits for
       // a key press by calling kbhit in a loop and a mouse event is in the
       // queue, the loop will never end.
       //if (inpRec.EventType == MOUSE_EVENT)
       //  return 0;
       ReadConsoleInput(TScreenWinNT::hIn,&inpRec,1,&cEvents);
       continue;
      }
    if (!inpRec.Event.KeyEvent.bKeyDown) // skeek keyup events
       ReadConsoleInput(TScreenWinNT::hIn,&inpRec,1,&cEvents);
    else
       return 1;
   }
 return 0;
}

void TGKeyWinNT::FillTEvent(TEvent &e)
{
 DWORD cEvents;
 ReadConsoleInput(TScreenWinNT::hIn,&inpRec,1,&cEvents);

 DWORD dwCtlState=inpRec.Event.KeyEvent.dwControlKeyState;
 shiftState=0;
 if (dwCtlState & SHIFT_PRESSED)
    shiftState|=kbShiftCode;
 if (dwCtlState & (LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED))
    shiftState|=kbCtrlCode;
 if (dwCtlState & LEFT_ALT_PRESSED)
    shiftState|=kbAltLCode;
 if (dwCtlState & RIGHT_ALT_PRESSED)
    shiftState|=kbAltRCode;
 //if (dwCtlState & SCROLLLOCK_ON)
 //  shiftState |= kbScrollState;
 //if (dwCtlState & NUMLOCK_ON)
 //  shiftState |= kbNumState;
 //if (dwCtlState & CAPSLOCK_ON)
 //  shiftState |= kbCapsState;

 e.keyDown.charScan.scanCode=inpRec.Event.KeyEvent.wVirtualScanCode;
 uchar Symbol=inpRec.Event.KeyEvent.uChar.AsciiChar;
 e.keyDown.charScan.charCode=((shiftState & kbAltLCode) && (Symbol<128)) ?
                             0 : Symbol;
 e.keyDown.shiftState=shiftState;
 // To debug the codes
 //fprintf(stderr,"inpRec.Event.KeyEvent.wVirtualKeyCode=%d\n",
 //        inpRec.Event.KeyEvent.wVirtualKeyCode);
 e.keyDown.keyCode=(inpRec.Event.KeyEvent.wVirtualKeyCode<256) ?
                   vk2kk[inpRec.Event.KeyEvent.wVirtualKeyCode] :
                   (ushort)0;
 if (e.keyDown.keyCode==0)
    return;
 e.what=evKeyDown;
 e.keyDown.keyCode|=shiftState;
}

void TGKeyWinNT::Init()
{
 TGKey::kbhit=KbHit;
 TGKey::getShiftState=GetShiftState;
 TGKey::fillTEvent=FillTEvent;
 TGKey::clear=Clear;
}
#else

#include <tv/winnt/screen.h>
#include <tv/winnt/key.h>

#endif // TVOS_Win32

/*
This code was used by Anatoli to check what key codes generates Win32 API.

<------- start of codes.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

void main()
{
 INPUT_RECORD ir;
 HANDLE hInp;
 DWORD cEvents;
 
 hInp=GetStdHandle(STD_INPUT_HANDLE);
 
 while (1)
   {
    ReadConsoleInput(hInp, &ir, 1, &cEvents);
    if (ir.EventType == KEY_EVENT)
      {
       printf("KeyDown = %d, VK = %d, SC = %x Char = %d\n",
          ir.Event.KeyEvent.bKeyDown,
          ir.Event.KeyEvent.wVirtualKeyCode,
          ir.Event.KeyEvent.wVirtualScanCode,
          ir.Event.KeyEvent.uChar.AsciiChar);
      }
   }
}
<------- end of codes.cpp

Perl script used by Anatoli to generate the translation table

@table = qw(
kbUnknown=0x0000
kbA=0x0001=65
kbB=0x0002=66
kbC=0x0003=67
kbD=0x0004=68
kbE=0x0005=69
kbF=0x0006=70
kbG=0x0007=71
kbH=0x0008=72
kbI=0x0009=73
kbJ=0x000a=74
kbK=0x000b=75
kbL=0x000c=76
kbM=0x000d=77
kbN=0x000e=78
kbO=0x000f=79
kbP=0x0010=80
kbQ=0x0011=81
kbR=0x0012=82
kbS=0x0013=83
kbT=0x0014=84
kbU=0x0015=85
kbV=0x0016=86
kbW=0x0017=87
kbX=0x0018=88
kbY=0x0019=89
kbZ=0x001a=90

kbOpenBrace=0x001b
kbBackSlash=0x001c=220
kbCloseBrace=0x001d
kbPause=0x001e
kbEsc=0x001f=27

kb0=0x0020=48
kb1=0x0021=49
kb2=0x0022=50
kb3=0x0023=51
kb4=0x0024=52
kb5=0x0025=53
kb6=0x0026=54
kb7=0x0027=55
kb8=0x0028=56
kb9=0x0029=57

kbBackSpace=0x002a=8
kbTab=0x002b=9
kbEnter=0x002c=13

kbColon=0x002d=186
kbQuote=0x002e=222
kbGrave=0x002f
kbComma=0x0030

kbStop=0x0031
kbSlash=0x0032=111
kbAsterisk=0x0033=106
kbSpace=0x0034=32

kbMinus=0x0035=109
kbPlus=0x0036=107
kbPrnScr=0x0037
kbEqual=0x0038=187

kbF1=0x0039=112
kbF2=0x003a=113
kbF3=0x003b=114
kbF4=0x003c=115
kbF5=0x003d=116
kbF6=0x003e=117
kbF7=0x003f=118
kbF8=0x0040=119
kbF9=0x0041=120
kbF10=0x0042=121
kbF11=0x0043=122
kbF12=0x0044=123

kbHome=0x0045=36
kbUp=0x0046=38
kbPgUp=0x0047=33
kbLeft=0x0048=37
kbRight=0x0049=39
kbEnd=0x004a=35
kbDown=0x004b=40
kbPgDn=0x004c=34

kbInsert=0x004d=45
kbDelete=0x004e=46
kbCaret=0x004f
kbAdmid=0x0050

kbDobleQuote=0x0051
kbNumeral=0x0052
kbDolar=0x0053
kbPercent=0x0054

kbAmper=0x0055
kbOpenPar=0x0056=219
kbClosePar=0x0057=221
kbDoubleDot=0x0058

kbLessThan=0x0059=188
kbGreaterThan=0x005a=190
kbQuestion=0x005b=191
kbA_Roba=0x005c
kbOr=0x005d
kbUnderLine=0x005e=189
kbOpenCurly=0x005f
kbCloseCurly=0x0060
kbTilde=0x0061=192
kbMacro=0x0062
kbWinLeft=0x0063=91
kbWinRight=0x0064=92
kbWinSel=0x0065=93
kbMouse=0x0066
);

for $e (@table) {
	($kname, $kcode, $vcode) = split(/=/, $e);
	$map[$vcode] = $kcode if $vcode ne '';
}

for ($i = 0; $i < 256; $i++) {
	$map[$i] = '0x0000' if !defined($map[$i]);
}

for ($i = 0; $i < 256; ) {
	for ($j = 0; $j < 8; $j++, $i++) {
		print $map[$i], (($i == 255) ? '' : ', ');
	}
	print "\n";
}
*/

