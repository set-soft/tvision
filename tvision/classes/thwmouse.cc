/**[txh]********************************************************************

  Mouse module, Copyright 2001-2003 by Salvador E. Tropea
  Description:
  This is the base class for low level mouse input.
  Most members are pointers to functions defined by each platform dependent
driver. Default members are provided.
  This is a complete new file created from concepts that José Angel Sanchez
Caso provide me and coded from scratch. The names of some members are the
same used in original Turbo Vision for compatibility purposes.
  
***************************************************************************/
// Definitions are located at the event.h header

#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

/*****************************************************************************
  Data members initialization
*****************************************************************************/

Boolean  THWMouse::handlerInstalled=False;
Boolean  THWMouse::noMouse         =False;
uchar    THWMouse::buttonCount     =0;
uchar    THWMouse::oldButtonCount  =0;
uchar    THWMouse::btBeforeForce   =0;
char     THWMouse::visible         =0;
char     THWMouse::forced          =0;
volatile
unsigned THWMouse::drawCounter     =0;
MouseEventType THWMouse::forcedME;

/*****************************************************************************
  Function pointer members initialization
*****************************************************************************/

void (*THWMouse::Show)()                               =defaultShow;
void (*THWMouse::Hide)()                               =defaultHide;
void (*THWMouse::setRange)(ushort, ushort)             =defaultSetRange;
void (*THWMouse::GetEvent)(MouseEventType&)            =defaultGetEvent;
void (*THWMouse::registerHandler)(unsigned, void (*)())=defaultRegisterHandler;
void (*THWMouse::Suspend)()                            =defaultSuspend;
void (*THWMouse::Resume)()                             =defaultResume;
int  (*THWMouse::drawMouse)(int x, int y)              =defaultDrawMouse;

/*****************************************************************************
  Default behaviors for the members
*****************************************************************************/

void THWMouse::defaultShow()
{
 visible=1;
}

void THWMouse::defaultHide()
{
 visible=0;
}

void THWMouse::defaultSuspend() {}
void THWMouse::defaultResume() { buttonCount=oldButtonCount; }
void THWMouse::defaultSetRange(ushort /*rx*/, ushort /*ry*/) {}
int  THWMouse::defaultDrawMouse(int /*x*/, int /*y*/) { return 0; }

void THWMouse::defaultRegisterHandler(unsigned, void (*)()) {}

void THWMouse::defaultGetEvent(MouseEventType &me)
{
 me.where.x=TEventQueue::curMouse.where.x;
 me.where.y=TEventQueue::curMouse.where.y;
 me.buttons=TEventQueue::curMouse.buttons;
 me.doubleClick=False;
}

/*****************************************************************************
  Real members
*****************************************************************************/

THWMouse::THWMouse()
{
 resume();
}

THWMouse::~THWMouse()
{
 suspend();
}

void THWMouse::show()
{
 if (!present() || visible) return;
 Show();
}

void THWMouse::hide()
{
 if (!present() || !visible) return;
 Hide();
}

void THWMouse::suspend()
{
 if (!present())
    return;
 if (visible)
    Hide();
 oldButtonCount=buttonCount;
 buttonCount=0;
 Suspend();
}

void THWMouse::resume()
{
 if (present())
    return;
 Resume();
 if (!visible)
    Show();
}

void THWMouse::forceEvent(int x, int y, int buttons)
{
 forced=0;
 if (TEventQueue::curMouse.where.x!=x || TEventQueue::curMouse.where.y!=y)
    forced++;
 if (TEventQueue::curMouse.buttons!=buttons)
    forced++;
 forcedME.where.x=x;
 forcedME.where.y=y;
 forcedME.doubleClick=False;
 btBeforeForce=forcedME.buttons;
 forcedME.buttons=buttons;
}

void THWMouse::getEvent(MouseEventType& me)
{
 if (!present())
   {
    me=TEventQueue::curMouse;
    return;
   }
 if (forced)
   {
    me=forcedME;
    if (forced==2)
       me.buttons=btBeforeForce;
    TEventQueue::curMouse=me;
    drawMouse(forcedME.where.x,forcedME.where.y);
    forced--;
   }
 else if (handlerInstalled)
   {
    me=TEventQueue::curMouse;
   }
 else
   {
    GetEvent(me);
    TEventQueue::curMouse=me;
   }
}

