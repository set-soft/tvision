/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Vadim Beloborodov.

  Description:
  Win32 Mouse routines.
  The original implementation was done by Vadim, I removed some code and
adapted it to the new architecture.
  One important detail is that Vadim implemented it with threads. So we have
here an events handler with a queue.
  
***************************************************************************/

#include <tv/configtv.h>

#define Uses_stdlib
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TGKey
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/win32/screen.h>
#include <tv/win32/mouse.h>
#include <tv/win32/key.h>

MouseEventType  *THWMouseWin32::evMouseIn;
MouseEventType  *THWMouseWin32::evLastMouseIn=NULL;
MouseEventType  *THWMouseWin32::evMouseOut;
MouseEventType   THWMouseWin32::evMouseQueue[eventMouseQSize];
unsigned         THWMouseWin32::evMouseLength;
CRITICAL_SECTION THWMouseWin32::lockMouse;

void THWMouseWin32::Init()
{
 THWMouse::GetEvent=THWMouseWin32::GetEvent;
 THWMouse::Resume=THWMouseWin32::Resume;
 InitializeCriticalSection(&lockMouse);
 evMouseLength=0;
 evMouseIn=evMouseOut=&evMouseQueue[0];
 Resume();
}

void THWMouseWin32::Resume()
{
 // SET: This is supposed to report if mouse is present and how many buttons
 // have. I put a detailed comment in winnt driver.
 buttonCount=GetSystemMetrics(SM_CMOUSEBUTTONS);
}

void THWMouseWin32::DeInit()
{
 DeleteCriticalSection(&lockMouse);
}

void THWMouseWin32::putConsoleMouseEvent(MouseEventType& mouse)
{
 EnterCriticalSection(&lockMouse);
 if (evMouseLength<eventMouseQSize)
   {// Compress mouse events
    if (evLastMouseIn && evMouseLength && (evLastMouseIn->buttons==mouse.buttons))
       *evLastMouseIn=mouse;
    else
      {
       evMouseLength++;
       *evMouseIn=mouse;
       evLastMouseIn=evMouseIn;
       if (++evMouseIn>=&evMouseQueue[eventMouseQSize])
          evMouseIn=&evMouseQueue[0];
      }
   }
 LeaveCriticalSection(&lockMouse);
}

void THWMouseWin32::GetEvent(MouseEventType &me)
{
 EnterCriticalSection(&lockMouse);
 int hasmouseevent=evMouseLength>0;
 if (hasmouseevent)
   {
    evMouseLength--;
    me=*evMouseOut;
    if (++evMouseOut>=&evMouseQueue[eventMouseQSize])
       evMouseOut=&evMouseQueue[0];
   }
 else
    // If no event is available use the last values so TV reports no changes
    me=TEventQueue::curMouse;
 LeaveCriticalSection(&lockMouse);
}

void THWMouseWin32::HandleMouseEvent()
{
 INPUT_RECORD ir;
 DWORD dwRead;
 ReadConsoleInput(TScreenWin32::hIn,&ir,1,&dwRead);
 if ((dwRead==1) && (ir.EventType==MOUSE_EVENT))
   {
    MouseEventType mouse;
    mouse.where.x=ir.Event.MouseEvent.dwMousePosition.X;
    mouse.where.y=ir.Event.MouseEvent.dwMousePosition.Y;
    mouse.buttons=0;
    mouse.doubleClick=False;
    if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
       mouse.buttons|=mbLeftButton;
    if (ir.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
       mouse.buttons|=mbRightButton;
    putConsoleMouseEvent(mouse);
    TGKeyWin32::ProcessControlKeyState(&ir);
   }
}
#else

#include <tv/win32/screen.h>
#include <tv/win32/mouse.h>
#include <tv/win32/key.h>

#endif // TVOS_Win32

