/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  WinNT Mouse routines.
  The original implementation was done by Anatoli, I removed some code and
adapted it to the new architecture.
  
***************************************************************************/
#include <tv/configtv.h>

#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

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
 THWMouse::Resume=THWMouseWinNT::Resume;
 Resume();
}

void THWMouseWinNT::Resume()
{
 // SET: According to Win32 API reference an application should check if the
 // mouse is installed and can find how many buttons have the mouse using
 // calls to GetSystemMetrics. Windows supports upto 3 buttons :-(
 // Note that in my system I have a 5 buttons mouse, 3 of them generates
 // events and this call returns 2. I don't thing Windows is doing the right
 // thing ;-) In my system the wheel works for IE, but I don't get mouse
 // events from the wheel.
 buttonCount=GetSystemMetrics(SM_CMOUSEBUTTONS);
}

#else

#include <tv/winnt/screen.h>
#include <tv/winnt/mouse.h>

#endif // TVOS_Win32

