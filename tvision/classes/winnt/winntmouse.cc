/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  WinNT Mouse routines.
  The original implementation was done by Anatoli, I removed some code and
adapted it to the new architecture.
  
***************************************************************************/
#include <tv/configtv.h>

#ifdef TVOS_Win32

#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/winnt/screen.h>
#include <tv/winnt/mouse.h>

INPUT_RECORD THWMouseWinNT::inpRec;

void THWMouseWinNT::GetEvent(MouseEventType &me)
{
 DWORD cEvents;
 INPUT_RECORD ir;

 while (PeekConsoleInput(TScreenWinNT::hIn,&ir,1,&cEvents) && cEvents==1)
   {
    if (ir.EventType==KEY_EVENT)
       break;
    if (ir.EventType!=MOUSE_EVENT)
      {
       ReadConsoleInput(TScreenWinNT::hIn,&ir,1,&cEvents);
       continue;
      }
    ReadConsoleInput(TScreenWinNT::hIn,&inpRec,1,&cEvents);
    break;
   }
 me.buttons=(uchar)inpRec.Event.MouseEvent.dwButtonState;
 me.doubleClick=False;
 me.where.x=inpRec.Event.MouseEvent.dwMousePosition.X;
 me.where.y=inpRec.Event.MouseEvent.dwMousePosition.Y;
}

void THWMouseWinNT::Init()
{
 THWMouse::GetEvent=THWMouseWinNT::GetEvent;
 buttonCount=2;
}
#endif // TVOS_Win32

