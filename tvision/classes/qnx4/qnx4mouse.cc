/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#define Uses_stdlib
#define Uses_string
#define Uses_stdio
#define Uses_time
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

extern "C"
{
   #include <sys/term.h>
   #include <sys/qnxterm.h>
}

#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>

int THWMouseQNX4::MousePositionX=0;
int THWMouseQNX4::MousePositionY=0;
int THWMouseQNX4::MouseButtons=0;

THWMouseQNX4::~THWMouseQNX4()
{
   term_mouse_off();
}

void THWMouseQNX4::Init()
{
   if (term_mouse_on()==0)
   {
      buttonCount=2;
   }
   else
   {
      buttonCount=0;
   }
   
   term_mouse_handler(MouseHandler);
   
   THWMouse::Show=Show;
   THWMouse::Hide=Hide;
   THWMouse::Suspend=Suspend;
   THWMouse::Resume=Resume;
   THWMouse::GetEvent=GetEvent;
}

void THWMouseQNX4::Show()
{
   buttonCount=1;
   term_mouse_move(-1, -1);
   term_flush();

   term_mouse_on();
}

void THWMouseQNX4::Hide()
{
   buttonCount=0;
   term_mouse_hide();
   term_flush();
}

void THWMouseQNX4::Suspend()
{
   buttonCount=0;
   term_mouse_off();
}

void THWMouseQNX4::Resume()
{
   buttonCount=2;
   term_mouse_on();
}

int THWMouseQNX4::MouseHandler(unsigned int* key, struct mouse_event* event)
{
   if (term_mouse_process(key, event))
   {
      return 1;
   }
   term_mouse_move(-1, -1);

   if (event==NULL)
   {
      return 0;
   }

   MouseButtons=0;
   if (event->buttons&_MOUSE_LEFT)
   {
      MouseButtons|=mbLeftButton;   // left button;
   }
   if (event->buttons & _MOUSE_RIGHT)
   {
      MouseButtons|=mbRightButton;  // right button;
   }
   if (event->buttons & _MOUSE_MIDDLE)
   {
      MouseButtons|=mbMiddleButton; // middle button;
   }
   
   MousePositionX = term_state.mouse_col;
   MousePositionY = term_state.mouse_row;

   return 0;
}

void THWMouseQNX4::GetEvent(MouseEventType &me)
{
   me.buttons=MouseButtons;
   me.doubleClick = False;
   me.where.x = MousePositionX;
   me.where.y = MousePositionY;
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnx4/screen.h>
   #include <tv/qnx4/mouse.h>

#endif // TVOS_UNIX && TVOSf_QNX4
