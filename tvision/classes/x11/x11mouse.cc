/* X11 mouse routines.
   Copyright (c) 2001 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(HAVE_X11)
//#define Uses_stdio
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

// X11 defines their own values
#undef True
#undef False
#include <X11/Xlib.h>

#include <tv/x11scr.h>
#include <tv/x11mouse.h>

/*****************************************************************************

  THWMouseX11 mouse stuff.

*****************************************************************************/

unsigned THWMouseX11::mouseButtons=0;
unsigned THWMouseX11::mouseX=0;
unsigned THWMouseX11::mouseY=0;
unsigned THWMouseX11::buttonTranslate[8]=
{
 0, // First button is 1
 mbLeftButton,mbMiddleButton,mbRightButton,mbButton4,mbButton5,
 0,0
};

int THWMouseX11::getMouseEvent()
{
 XEvent event;

 while (1)
   {
    /* Get the next mouse event */
    if (XCheckMaskEvent(TScreenX11::disp,aMouseEvent,&event)!=True)
      {
       TScreenX11::ProcessGenericEvents();
       return 0;
      }
    /* Is that needed here? */
    if (XFilterEvent(&event,0)==True)
       continue;
    if (event.type==ButtonPress || event.type==ButtonRelease ||
        event.type==MotionNotify)
      {
       mouseX=event.xbutton.x/TScreenX11::fontW;
       mouseY=event.xbutton.y/TScreenX11::fontH;
       if (event.type==ButtonPress)
          mouseButtons|=buttonTranslate[event.xbutton.button & 0x7];
       else
       if (event.type==ButtonRelease)
          mouseButtons&=~buttonTranslate[event.xbutton.button & 0x7];
       //printf("Button Press %d\n",event.xbutton.button);
       //printf("Button Release %d (%d,%d) vs (%d,%d)\n",event.xbutton.button,event.xbutton.x/TScreenX11::fontW,event.xbutton.y/TScreenX11::fontH,mouseX,mouseY);
       //printf("Motion Notify %d,%d\n",mouseX,mouseY);
      }
   }
 return 0;
}

void THWMouseX11::GetEvent(MouseEventType &me)
{
 getMouseEvent();
 me.where.x=mouseX;
 me.where.y=mouseY;
 me.buttons=mouseButtons;
 me.doubleClick=False;
}

void THWMouseX11::Init()
{
 THWMouse::GetEvent=GetEvent;
 buttonCount=5;
}

THWMouseX11::~THWMouseX11()
{
}
#endif // defined(TVOS_UNIX) && defined(HAVE_X11)

