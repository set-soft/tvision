/* X11 mouse routines.
   Copyright (c) 2001-2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && defined(HAVE_X11)

// X11 defines their own values
#undef True
#undef False
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <tv/x11/screen.h>
#include <tv/x11/mouse.h>

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

 SEMAPHORE_ON;
 while (1)
   {
    /* Get the next mouse event */
    if (XCheckMaskEvent(TScreenX11::disp,aMouseEvent,&event)!=True)
      {
       if (!IS_SECOND_THREAD_ON)
          TScreenX11::ProcessGenericEvents();
       SEMAPHORE_OFF;
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
       // Clamp the values, they could be negative
       if (event.xbutton.x<0) mouseX=0;
       if (event.xbutton.y<0) mouseY=0;
       if (event.type==ButtonPress)
          mouseButtons|=buttonTranslate[event.xbutton.button & 0x7];
       else
       if (event.type==ButtonRelease)
          mouseButtons&=~buttonTranslate[event.xbutton.button & 0x7];
       if (0)
         {
          if (event.type==ButtonPress)
             printf("Button Press %d mouseButtons=%d\n",event.xbutton.button,mouseButtons);
          else if (event.type==ButtonRelease)
             printf("Button Release %d (%d,%d) vs (%d,%d)\n",event.xbutton.button,event.xbutton.x/TScreenX11::fontW,event.xbutton.y/TScreenX11::fontH,mouseX,mouseY);
          else
             printf("Motion Notify %d,%d\n",mouseX,mouseY);
         }
       SEMAPHORE_OFF;
       return 1;
      }
   }
 SEMAPHORE_OFF;
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
#else

#include <tv/x11/screen.h>
#include <tv/x11/mouse.h>

#endif // defined(TVOS_UNIX) && defined(HAVE_X11)

