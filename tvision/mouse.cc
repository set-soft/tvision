/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#define Uses_TEventQueue
#define Uses_TEvent
#include <tv.h>


#ifdef __DJGPP__
#include <dpmi.h>

int draw_mouse(int,int);
MouseEventType tempMouse;
_go32_dpmi_registers mouseIntRegs;

void TEventQueue::mouseInt()
{
    int x,y;
    tempMouse.buttons = mouseIntRegs.h.bl;
    tempMouse.doubleClick = False;
    x = tempMouse.where.x = (mouseIntRegs.x.cx >> 3) & 0xFF;
    y = tempMouse.where.y = (mouseIntRegs.x.dx >> 3) & 0xFF;
    if (draw_mouse(x,y)) mouseIntFlag = True;
    if( (mouseIntRegs.x.ax & 0x1e) != 0 && eventCount < eventQSize )
        {
        eventQTail->what = TICKS();
        eventQTail->mouse = curMouse;
        if( ++eventQTail >= eventQueue + eventQSize )
            eventQTail = eventQueue;
        eventCount++;
        }

    curMouse = tempMouse;
}

#else

void TEventQueue::mouseInt()
{
}

#endif

