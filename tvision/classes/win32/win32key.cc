/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  It said:
  Keyboard handler for Win32 by Vadim Beloborodov
  Based on code by Salvador E. Tropea (SET) (c) (1998)

  Description:
  Win32 Keyboard routines.
  The original implementation was done by Vadim, I removed some code, added
some routines and adapted it to the new architecture.
  One important detail is that Vadim implemented it with threads. So we have
here an events handler with a queue.
  
***************************************************************************/

#include <tv/configtv.h>

#define Uses_string
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_ctype
#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/win32/screen.h>
#include <tv/win32/key.h>

KeyDownEvent    *TGKeyWin32::evKeyboardIn;
KeyDownEvent    *TGKeyWin32::evKeyboardOut;
KeyDownEvent     TGKeyWin32::evKeyboardQueue[eventKeyboardQSize];
unsigned         TGKeyWin32::evKeyboardLength;
CRITICAL_SECTION TGKeyWin32::lockKeyboard;
ushort           TGKeyWin32::LastControlKeyState=0;

void TGKeyWin32::Init()
{
 TGKey::kbhit=KbHit;
 TGKey::getShiftState=GetShiftState;
 TGKey::fillTEvent=FillTEvent;

 InitializeCriticalSection(&lockKeyboard);
 evKeyboardLength=0;
 evKeyboardIn=evKeyboardOut=&evKeyboardQueue[0];
}

void TGKeyWin32::DeInit()
{
 DeleteCriticalSection(&lockKeyboard);
}

int TGKeyWin32::KbHit(void)
{
 return evKeyboardLength>0;
}

void TGKeyWin32::FillTEvent(TEvent &e)
{
 getConsoleKeyboardEvent(e.keyDown);
 e.what=evKeyDown;
}

ushort TGKeyWin32::transShiftState(DWORD state)
{
 ushort tvstate=0;
 if (state & (RIGHT_ALT_PRESSED |LEFT_ALT_PRESSED) ) tvstate|=kbAltShift;
 if (state & (RIGHT_CTRL_PRESSED|LEFT_CTRL_PRESSED)) tvstate|=kbCtrlShift;
 if (state & SHIFT_PRESSED) tvstate|=kbLeftShift;
 if (state & NUMLOCK_ON   ) tvstate|=kbNumState;
 if (state & SCROLLLOCK_ON) tvstate|=kbScrollState;
 if (state & CAPSLOCK_ON  ) tvstate|=kbCapsState;

 return tvstate;
}

/* 
  Translate keyboard events to Salvador E. Tropea's key codes
  by Vadim Beloborodov
  Originally in trans.cc
*/

const char TGKeyWin32::KeyTo[256]=
{
 /* 00 */                           0,
 /* 01 VK_LBUTTON */                0,
 /* 02 VK_RBUTTON */                0,
 /* 03 VK_CANCEL */                 0,
 /* 04 VK_MBUTTON */                0,
 /* 05 unassigned */                0,
 /* 06 unassigned */                0,
 /* 07 unassigned */                0,
 /* 08 VK_BACK */                   kbBackSpace,
 /* 09 VK_TAB */                    kbTab,
 /* 0A unassigned */                0,
 /* 0B unassigned */                0,
 /* 0C VK_CLEAR ?? */               0,
 /* 0D VK_RETURN */                 kbEnter,
 /* 0E unassigned */                0,
 /* 0F unassigned */                0,
 /* 10 VK_SHIFT */                  0,
 /* 11 VK_CONTROL */                0,
 /* 12 VK_MENU */                   0,
 /* 13 VK_PAUSE */                  0,
 /* 14 VK_CAPITAL */                0,
 /* 15 Kanji systems*/              0,
 /* 16 Kanji systems*/              0,
 /* 17 Kanji systems*/              0,
 /* 18 Kanji systems*/              0,
 /* 19 Kanji systems*/              0,
 /* 1A unassigned */                0,
 /* 1B VK_ESCAPE */                 kbEsc,
 /* 1C Kanji systems*/              0,
 /* 1D Kanji systems*/              0,
 /* 1E Kanji systems*/              0,
 /* 1F Kanji systems*/              0,
 /* 20 VK_SPACE */                  kbSpace,
 /* 21 VK_PRIOR */                  kbPgUp,
 /* 22 VK_NEXT */                   kbPgDn,
 /* 23 VK_END */                    kbEnd,
 /* 24 VK_HOME */                   kbHome,
 /* 25 VK_LEFT */                   kbLeft,
 /* 26 VK_UP */                     kbUp,
 /* 27 VK_RIGHT */                  kbRight,
 /* 28 VK_DOWN */                   kbDown,
 /* 29 VK_SELECT */                 0,
 /* 2A OEM specific */              0,
 /* 2B VK_EXECUTE */                0,
 /* 2C VK_SNAPSHOT */               0,
 /* 2D VK_INSERT */                 kbInsert,
 /* 2E VK_DELETE */                 kbDelete,
 /* 2F VK_HELP */                   0,
 /* 30 VK_0 '0' */                  kb0,
 /* 31 VK_1 '1' */                  kb1,
 /* 32 VK_2 '2' */                  kb2,
 /* 33 VK_3 '3' */                  kb3,
 /* 34 VK_4 '4' */                  kb4,
 /* 35 VK_5 '5' */                  kb5,
 /* 36 VK_6 '6' */                  kb6,
 /* 37 VK_7 '7' */                  kb7,
 /* 38 VK_8 '8' */                  kb8,
 /* 39 VK_9 '9' */                  kb9,
 /* 3A unassigned */                0,
 /* 3B unassigned */                0,
 /* 3C unassigned */                0,
 /* 3D unassigned */                0,
 /* 3E unassigned */                0,
 /* 3F unassigned */                0,
 /* 40 unassigned */                0,
 /* 41 VK_A 'A' */                  kbA,
 /* 42 VK_B 'B' */                  kbB,
 /* 43 VK_C 'C' */                  kbC,
 /* 44 VK_D 'D' */                  kbD,
 /* 45 VK_E 'E' */                  kbE,
 /* 46 VK_F 'F' */                  kbF,
 /* 47 VK_G 'G' */                  kbG,
 /* 48 VK_H 'H' */                  kbH,
 /* 49 VK_I 'I' */                  kbI,
 /* 4A VK_J 'J' */                  kbJ,
 /* 4B VK_K 'K' */                  kbK,
 /* 4C VK_L 'L' */                  kbL,
 /* 4D VK_M 'M' */                  kbM,
 /* 4E VK_N 'N' */                  kbN,
 /* 4F VK_O 'O' */                  kbO,
 /* 50 VK_P 'P' */                  kbP,
 /* 51 VK_Q 'Q' */                  kbQ,
 /* 52 VK_R 'R' */                  kbR,
 /* 53 VK_S 'S' */                  kbS,
 /* 54 VK_T 'T' */                  kbT,
 /* 55 VK_U 'U' */                  kbU,
 /* 56 VK_V 'V' */                  kbV,
 /* 57 VK_W 'W' */                  kbW,
 /* 58 VK_X 'X' */                  kbX,
 /* 59 VK_Y 'Y' */                  kbY,
 /* 5A VK_Z 'Z' */                  kbZ,
 /* 5B unassigned */                0,
 /* 5C unassigned */                0,
 /* 5D unassigned */                0,
 /* 5E unassigned */                0,
 /* 5F unassigned */                0,
 /* 60 VK_NUMPAD0 NumKeyPad '0' */  kb0,
 /* 61 VK_NUMPAD1 NumKeyPad '1' */  kb1,
 /* 62 VK_NUMPAD2 NumKeyPad '2' */  kb2,
 /* 63 VK_NUMPAD3 NumKeyPad '3' */  kb3,
 /* 64 VK_NUMPAD4 NumKeyPad '4' */  kb4,
 /* 65 VK_NUMPAD5 NumKeyPad '5' */  kb5,
 /* 66 VK_NUMPAD6 NumKeyPad '6' */  kb6,
 /* 67 VK_NUMPAD7 NumKeyPad '7' */  kb7,
 /* 68 VK_NUMPAD8 NumKeyPad '8' */  kb8,
 /* 69 VK_NUMPAD9 NumKeyPad '9' */  kb9,
 /* 6A VK_MULTIPLY */               kbAsterisk,
 /* 6B VK_ADD */                    kbPlus,
 /* 6C VK_SEPARATOR */              kbBackSlash,
 /* 6D VK_SUBSTRACT */              kbMinus,
 /* 6E VK_DECIMAL */                kbGrave,
 /* 6F VK_DIVIDE */                 kbSlash,
 /* 70 VK_F1 'F1' */                kbF1,
 /* 71 VK_F2 'F2' */                kbF2,
 /* 72 VK_F3 'F3' */                kbF3,
 /* 73 VK_F4 'F4' */                kbF4,
 /* 74 VK_F5 'F5' */                kbF5,
 /* 75 VK_F6 'F6' */                kbF6,
 /* 76 VK_F7 'F7' */                kbF7,
 /* 77 VK_F8 'F8' */                kbF8,
 /* 78 VK_F9 'F9' */                kbF9,
 /* 79 VK_F10 'F10' */              kbF10,
 /* 7A VK_F11 'F11' */              kbF11,
 /* 7B VK_F12 'F12' */              kbF12,
 /* 7C VK_F13 */                    0,
 /* 7D VK_F14 */                    0,
 /* 7E VK_F15 */                    0,
 /* 7F VK_F16 */                    0,
 /* 80 VK_F17 */                    0,
 /* 81 VK_F18 */                    0,
 /* 82 VK_F19 */                    0,
 /* 83 VK_F20 */                    0,
 /* 84 VK_F21 */                    0,
 /* 85 VK_F22 */                    0,
 /* 86 VK_F23 */                    0,
 /* 87 VK_F24 */                    0,
 /* 88 unassigned */                0,
 /* 89 VK_NUMLOCK */                0,
 /* 8A VK_SCROLL */                 0,
 /* 8B unassigned */                0,
 /* 8C unassigned */                0,
 /* 8D unassigned */                0,
 /* 8E unassigned */                0,
 /* 8F unassigned */                0,
 /* 90 unassigned */                0,
 /* 91 unassigned */                0,
 /* 92 unassigned */                0,
 /* 93 unassigned */                0,
 /* 94 unassigned */                0,
 /* 95 unassigned */                0,
 /* 96 unassigned */                0,
 /* 97 unassigned */                0,
 /* 98 unassigned */                0,
 /* 99 unassigned */                0,
 /* 9A unassigned */                0,
 /* 9B unassigned */                0,
 /* 9C unassigned */                0,
 /* 9D unassigned */                0,
 /* 9E unassigned */                0,
 /* 9F unassigned */                0,
 /* A0 unassigned */                0,
 /* A1 unassigned */                0,
 /* A2 unassigned */                0,
 /* A3 unassigned */                0,
 /* A4 unassigned */                0,
 /* A5 unassigned */                0,
 /* A6 unassigned */                0,
 /* A7 unassigned */                0,
 /* A8 unassigned */                0,
 /* A9 unassigned */                0,
 /* AA unassigned */                0,
 /* AB unassigned */                0,
 /* AC unassigned */                0,
 /* AD unassigned */                0,
 /* AE unassigned */                0,
 /* AF unassigned */                0,
 /* B0 unassigned */                0,
 /* B1 unassigned */                0,
 /* B2 unassigned */                0,
 /* B3 unassigned */                0,
 /* B4 unassigned */                0,
 /* B5 unassigned */                0,
 /* B6 unassigned */                0,
 /* B7 unassigned */                0,
 /* B8 unassigned */                0,
 /* B9 unassigned */                0,
 /* BA OEM specific */              0,
 /* BB OEM specific */              0,
 /* BC OEM specific */              0,
 /* BD OEM specific */              0,
 /* BE OEM specific */              0,
 /* BF OEM specific */              0,
 /* C0 OEM specific */              0,
 /* C1 unassigned */                0,
 /* C2 unassigned */                0,
 /* C3 unassigned */                0,
 /* C4 unassigned */                0,
 /* C5 unassigned */                0,
 /* C6 unassigned */                0,
 /* C7 unassigned */                0,
 /* C8 unassigned */                0,
 /* C9 unassigned */                0,
 /* CA unassigned */                0,
 /* CB unassigned */                0,
 /* CC unassigned */                0,
 /* CD unassigned */                0,
 /* CE unassigned */                0,
 /* CF unassigned */                0,
 /* D0 unassigned */                0,
 /* D1 unassigned */                0,
 /* D2 unassigned */                0,
 /* D3 unassigned */                0,
 /* D4 unassigned */                0,
 /* D5 unassigned */                0,
 /* D6 unassigned */                0,
 /* D7 unassigned */                0,
 /* D8 unassigned */                0,
 /* D9 unassigned */                0,
 /* DA unassigned */                0,
 /* DB OEM specific */              0,
 /* DC OEM specific */              0,
 /* DD OEM specific */              0,
 /* DE OEM specific */              0,
 /* DF OEM specific */              0,
 /* E0 OEM specific */              0,
 /* E1 OEM specific */              0,
 /* E2 OEM specific */              0,
 /* E3 OEM specific */              0,
 /* E4 OEM specific */              0,
 /* E5 unassigned */                0,
 /* E6 OEM specific */              0,
 /* E7 unassigned */                0,
 /* E8 unassigned */                0,
 /* E9 OEM specific */              0,
 /* EA OEM specific */              0,
 /* EB OEM specific */              0,
 /* EC OEM specific */              0,
 /* ED OEM specific */              0,
 /* EE OEM specific */              0,
 /* EF OEM specific */              0,
 /* F0 OEM specific */              0,
 /* F1 OEM specific */              0,
 /* F2 OEM specific */              0,
 /* F3 OEM specific */              0,
 /* F4 OEM specific */              0,
 /* F5 OEM specific */              0,
 /* F6 unassigned */                0,
 /* F7 unassigned */                0,
 /* F8 unassigned */                0,
 /* F9 unassigned */                0,
 /* FA unassigned */                0,
 /* FB unassigned */                0,
 /* FC unassigned */                0,
 /* FD unassigned */                0,
 /* FE unassigned */                0,
 /* FF unassigned */                0
};

int TGKeyWin32::transKeyEvent(KeyDownEvent& dst, KEY_EVENT_RECORD& src)
{
 if (src.wVirtualKeyCode!=VK_MENU &&
     src.wVirtualKeyCode!=VK_CONTROL &&
     src.wVirtualKeyCode!=VK_SHIFT)
   {
    dst.keyCode=KeyTo[src.wVirtualKeyCode];
    dst.charScan.charCode=src.uChar.AsciiChar;
    dst.charScan.scanCode=(uchar)src.wVirtualScanCode;
    dst.raw_scanCode=(uchar)src.wVirtualScanCode;
    dst.shiftState=transShiftState(src.dwControlKeyState);
    if (dst.shiftState&kbAltShift)  dst.keyCode|=kbAltLCode;
    if (dst.shiftState&kbCtrlShift) dst.keyCode|=kbCtrlCode;
    if (dst.shiftState&kbShift)     dst.keyCode|=kbShiftCode;
    return 1;
   }
 return 0;   
}

/* 
  Win32 console events handlers
  by Vadim Beloborodov
  Originally in console.cc
*/

const char TGKeyWin32::testChars[]=
"`1234567890-="
"~!@#$%^&*()_+"
"qwertyuiop[]"
"QWERTYUIOP{}"
"asdfghjkl;'\\"
"ASDFGHJKL:\"|"
"zxcvbnm,./"
"ZXCVBNM<>?";

unsigned TGKeyWin32::GetShiftState()
{
 return LastControlKeyState;
}

void TGKeyWin32::putConsoleKeyboardEvent(KeyDownEvent &key)
{
 EnterCriticalSection(&lockKeyboard);
 if (evKeyboardLength<eventKeyboardQSize)
   {
    evKeyboardLength++;
    *evKeyboardIn=key;
    if (++evKeyboardIn>=&evKeyboardQueue[eventKeyboardQSize])
       evKeyboardIn=&evKeyboardQueue[0];
   }
 LeaveCriticalSection(&lockKeyboard);
}

int TGKeyWin32::getConsoleKeyboardEvent(KeyDownEvent &key)
{
 EnterCriticalSection(&lockKeyboard);
 int haskeyevent=evKeyboardLength>0;
 if (haskeyevent)
   {
    evKeyboardLength--;
    key=*evKeyboardOut;
    if (++evKeyboardOut>=&evKeyboardQueue[eventKeyboardQSize])
       evKeyboardOut=&evKeyboardQueue[0];
   }
 LeaveCriticalSection(&lockKeyboard);
 return haskeyevent;
}

void TGKeyWin32::HandleKeyEvent()
{
 INPUT_RECORD ir;
 DWORD dwRead;
 PeekConsoleInput(TScreenWin32::hIn,&ir,1,&dwRead);
 if ((dwRead==1) && (ir.EventType==KEY_EVENT))
   {
    if (ir.Event.KeyEvent.bKeyDown)
      {
       //support for non US keyboard layout on Windows95
       if (((ir.Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED|LEFT_ALT_PRESSED|RIGHT_CTRL_PRESSED|LEFT_CTRL_PRESSED))==0) &&
           (ir.Event.KeyEvent.uChar.AsciiChar) &&
           strchr(testChars,ir.Event.KeyEvent.uChar.AsciiChar))
         {
          uchar chr;
          ReadConsole(TScreenWin32::hIn,&chr,1,&dwRead,NULL);
          ir.Event.KeyEvent.uChar.AsciiChar=chr;
          dwRead=0;
         }
       else
         {
          ReadConsoleInput(TScreenWin32::hIn,&ir,1,&dwRead);
          dwRead = 0;
         }
       //translate event
       KeyDownEvent key;
       if (transKeyEvent(key,ir.Event.KeyEvent))
          putConsoleKeyboardEvent(key);
      }
    ProcessControlKeyState(&ir);
   }
 if (dwRead==1)
    ReadConsoleInput(TScreenWin32::hIn,&ir,1,&dwRead);
}

void TGKeyWin32::ProcessControlKeyState(INPUT_RECORD *ir)
{
 LastControlKeyState=transShiftState(ir->Event.KeyEvent.dwControlKeyState);
}
#else

#include <tv/win32/screen.h>
#include <tv/win32/key.h>

#endif // TVOS_Win32

