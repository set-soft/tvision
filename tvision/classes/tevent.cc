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

//#define Uses_stdio
#define Uses_string
#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TScreen
#define Uses_TVCodePage
#include <tv.h>
#include <tv/gkey.h>

TMouse *TEventQueue::mouse = NULL;
// SET: egcs gets upset if we partially initialize structures and egcs
// 2.91.66 even crash under Linux (not in DOS, but prints "(null)").
TEvent TEventQueue::eventQueue[eventQSize];
TEvent *TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent *TEventQueue::eventQTail = TEventQueue::eventQueue;
Boolean TEventQueue::mouseIntFlag = False;

ushort TEventQueue::eventCount = 0;

Boolean TEventQueue::mouseEvents  = False;
Boolean TEventQueue::mouseReverse = False;
ushort TEventQueue::doubleDelay = 8;
ushort TEventQueue::repeatDelay = 8;
ushort TEventQueue::autoTicks = 0;
ushort TEventQueue::autoDelay = 0;

MouseEventType TEventQueue::lastMouse;
MouseEventType TEventQueue::curMouse;
MouseEventType TEventQueue::downMouse;
ushort TEventQueue::downTicks = 0;

TEventQueue::TEventQueue()
{
 // SET: Just in case. Note: I can't debug it!
 memset((void *)&eventQueue[0],0,sizeof(TEvent)*eventQSize);
 resume();
}

static int TEventQueue_suspended = 1;

void TEventQueue::resume()
{
    if (!TEventQueue_suspended) return;
    // SET: We resumed, no matters if mouse fails or not
    TEventQueue_suspended = 0;
    TGKey::resume();
    mouseEvents = False;
    if( !mouse )
        mouse = new TMouse();
    if( mouse->present() == False )
        mouse->resume();
    if( mouse->present() == False )
        return;
    mouse->getEvent( curMouse );
    lastMouse = curMouse;
    
    mouseEvents = True;
    mouse->setRange( TScreen::getCols()-1,   TScreen::getRows()-1 );
}

void TEventQueue::suspend()
{
  if (TEventQueue_suspended)
     return;
  if (mouse->present())
     mouse->suspend();
  /* RH: I think here is the right place for clearing the
     buffer */
  TGKey::clear();
  TGKey::suspend();
  TEventQueue_suspended = 1;
}

TEventQueue::~TEventQueue()
{
 suspend();
 // SET: Destroy the mouse object
 if (mouse)
    delete mouse;
}

#ifdef TVComp_BCPP
// it works better (faster)
#define AUTO_DELAY_VAL 0
#else
#define AUTO_DELAY_VAL 1
#endif

void TEventQueue::getMouseEvent( TEvent& ev )
{
    if( mouseEvents == True )
        {

        getMouseState( ev );

        if( ev.mouse.buttons == 0 && lastMouse.buttons != 0 )
            {
            ev.what = evMouseUp;
//            int buttons = lastMouse.buttons;
            lastMouse = ev.mouse;
//            ev.mouse.buttons = buttons;
            return;
            }

        if( ev.mouse.buttons != 0 && lastMouse.buttons == 0 )
            {
            if( ev.mouse.buttons == downMouse.buttons &&
                ev.mouse.where == downMouse.where &&
                ev.what - downTicks <= doubleDelay )
                    ev.mouse.doubleClick = True;

            downMouse = ev.mouse;
            autoTicks = downTicks = ev.what;
            autoDelay = repeatDelay;
            ev.what = evMouseDown;
            lastMouse = ev.mouse;
            return;
            }

        ev.mouse.buttons = lastMouse.buttons;

        if( ev.mouse.where != lastMouse.where )
            {
            ev.what = evMouseMove;
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.buttons != 0 && ev.what - autoTicks > autoDelay )
            {
            autoTicks = ev.what;
            autoDelay = AUTO_DELAY_VAL;
            ev.what = evMouseAuto;
            lastMouse = ev.mouse;
            return;
            }
        }

    ev.what = evNothing;
}

void TEventQueue::getMouseState( TEvent & ev )
{
    if( eventCount == 0 )
        {
        TMouse::getEvent(ev.mouse);
        ev.what = CLY_Ticks();
        }
    else
        {
   ev = *eventQHead;
        if( ++eventQHead >= eventQueue + eventQSize )
            eventQHead = eventQueue;
        eventCount--;
        }
    if( mouseReverse != False && ev.mouse.buttons != 0 && ev.mouse.buttons != 3 )
        ev.mouse.buttons ^= 3;
}

#ifdef TVCompf_djgpp
#include <tv/dos/mouse.h>

void TEventQueue::mouseInt()
{
 int buttonPress;

 if (THWMouseDOS::getRMCB_InfoDraw(buttonPress))
    mouseIntFlag=True;
 if (buttonPress && eventCount<eventQSize)
   {
    eventQTail->what =CLY_Ticks();
    eventQTail->mouse=curMouse;
    if (++eventQTail>=eventQueue+eventQSize)
       eventQTail=eventQueue;
    eventCount++;
   }

 curMouse=THWMouseDOS::intEvent;
}

#else

void TEventQueue::mouseInt()
{
}

#endif


void TEvent::getKeyEvent()
{
 if (TGKey::kbhit())
   {
    TGKey::fillTEvent(*this);
    // SET: That's a special case, when the keyboard indicates the event
    // is mouse up it means the keyboard forced an event in the mouse module.
    if (what==evMouseUp)
       getMouseEvent();
    else
      {
       if (TVCodePage::OnTheFlyRemapInpNeeded() && keyDown.charScan.charCode>32)
          keyDown.charScan.charCode=TVCodePage::OnTheFlyInpRemap(keyDown.charScan.charCode);
       TGKey::AltInternat2ASCII(*this);
      }
   }
 else
    what=evNothing;
}


