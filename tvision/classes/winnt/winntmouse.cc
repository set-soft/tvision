/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Anatoli Soltan to be used under Win32 consoles.

 *
 *
 */
#include <tv/configtv.h>

#ifdef TVOS_Win32

#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int use_mouse_handler = 1;

extern HANDLE __tvWin32ConInp;

uchar THWMouse::buttonCount = 2;
Boolean THWMouse::handlerInstalled = False;
Boolean THWMouse::noMouse = False;

static INPUT_RECORD inpRec;

void THWMouse::registerHandler( unsigned , void (*)() )
{
}

void THWMouse::resume()
{
}

THWMouse::THWMouse()
{
}

THWMouse::~THWMouse()
{
}

void THWMouse::suspend()
{
}

void THWMouse::show()
{
}

void THWMouse::hide()
{
}

void THWMouse::setRange( ushort , ushort  )
{
}

/**[txh]********************************************************************

  Description:
  It forces the state of the mouse externally, the next call to getEvent
will return this values instead of values from the mouse driver. That's
used to simuate the mouse with other events like keyboard. (SET)

***************************************************************************/

void THWMouse::forceEvent(int , int , int )
{
}

void THWMouse::getEvent( MouseEventType& me )
{
  DWORD cEvents;
  INPUT_RECORD ir;
  while (PeekConsoleInput(__tvWin32ConInp, &ir, 1, &cEvents) && cEvents == 1)
    {
     if (ir.EventType == KEY_EVENT)
       break;
     if (ir.EventType != MOUSE_EVENT)
       {
        ReadConsoleInput(__tvWin32ConInp, &ir, 1, &cEvents);
        continue;
       }
     ReadConsoleInput(__tvWin32ConInp, &inpRec, 1, &cEvents);
     break;
    }
  me.buttons = (uchar)inpRec.Event.MouseEvent.dwButtonState;
  me.doubleClick = False;
  me.where.x = inpRec.Event.MouseEvent.dwMousePosition.X;
  me.where.y = inpRec.Event.MouseEvent.dwMousePosition.Y;
}

#endif // TVOS_Win32

