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

#ifdef TVOS_Win32

#include <stdlib.h>
#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//#include <tv/win32/screen.h>
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
 InitializeCriticalSection(&lockMouse);
 evMouseLength=0;
 evMouseIn=evMouseOut=&evMouseQueue[0];
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
    mouse=*evMouseOut;
    if (++evMouseOut>=&evMouseQueue[eventMouseQSize])
       evMouseOut=&evMouseQueue[0];
   }
 LeaveCriticalSection(&lockMouse);
}

void THWMouseWin32::HandleMouseEvent()
{
 INPUT_RECORD ir;
 DWORD dwRead;
 ReadConsoleInput(hIn,&ir,1,&dwRead);
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

#endif // TVOS_Win32

