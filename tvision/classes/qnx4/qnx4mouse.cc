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
ushort THWMouseQNX4::ConsoleMode=QNX_CONSOLE_RAW;

THWMouseQNX4::~THWMouseQNX4()
{
   // this destructor is never called !
   // instead of it ::Suspend method called.
}

void THWMouseQNX4::Init(ushort mousemode)
{
   ConsoleMode=mousemode;

   if (ConsoleMode!=QNX_CONSOLE_RAW)
   {
      // a bit of black magic :)
      printf("\x1B""/""0""t");    // clear mouse options.
      printf("\x1B""/"">""1""h"); // enable clicking events (press).
      printf("\x1B""/"">""6""h"); // enable clicking events. (release).
      printf("\x1B""/"">""7""h"); // enable movement events.
      printf("\x1B""/"">""9""l"); // intercept all stuff (X10 mouse events reporting).
      fflush(stdout);
   }

   if (term_mouse_on()==0)
   {
      buttonCount=3;
   }
   else
   {
      if (ConsoleMode==QNX_CONSOLE_XQSH)
      {
         // under qnxterm mouse is not detected by termlib.
         buttonCount=3;
      }
      else
      {
         buttonCount=0;
      }
   }
   
   term_mouse_handler(MouseHandler);
   
   if (ConsoleMode==QNX_CONSOLE_PTERM)
   {
      term_mouse_flags(TERM_MOUSE_RELEASE | TERM_MOUSE_ADJUST | TERM_MOUSE_MENU | TERM_MOUSE_SELECT, 
                       TERM_MOUSE_RELEASE | TERM_MOUSE_ADJUST | TERM_MOUSE_MENU | TERM_MOUSE_SELECT);
   };

   THWMouse::Show=Show;
   THWMouse::Hide=Hide;
   THWMouse::Suspend=Suspend;
   THWMouse::Resume=Resume;
   THWMouse::GetEvent=GetEvent;
}

void THWMouseQNX4::Show()
{
   if (term_mouse_on()==0)
   {
      buttonCount=3;
   }
   else
   {
      if (ConsoleMode==QNX_CONSOLE_XQSH)
      {
         // under qnxterm mouse is not detected by termlib.
         buttonCount=3;
      }
      else
      {
         buttonCount=0;
      }
   }

   term_mouse_move(-1, -1);
   term_flush();
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
   term_flush();

   if (ConsoleMode!=QNX_CONSOLE_RAW)
   {
      printf("\x1B""/""0""t");    // clear all mouse options.
      printf("\x1B""/"">""1""l"); // disable clicking events (press).
      printf("\x1B""/"">""6""l"); // disable clicking events. (release).
      printf("\x1B""/"">""7""l"); // disable movement events.
      printf("\x1B""/"">""9""h"); // release hook (Disable X10 mouse events reporting).
      fflush(stdout);
   }
}

void THWMouseQNX4::Resume()
{
   if (term_mouse_on()==0)
   {
      buttonCount=3;
   }
   else
   {
      if (ConsoleMode==QNX_CONSOLE_XQSH)
      {
         // under qnxterm mouse is not detected by termlib.
         buttonCount=3;
      }
      else
      {
         buttonCount=0;
      }
   }

   if (ConsoleMode!=QNX_CONSOLE_RAW)
   {
      // a bit of black magic :)
      printf("\x1B""/""0""t");    // clear mouse options.
      printf("\x1B""/"">""1""h"); // enable clicking events (press).
      printf("\x1B""/"">""6""h"); // enable clicking events. (release).
      printf("\x1B""/"">""7""h"); // enable movement events.
      printf("\x1B""/"">""9""l"); // intercept all stuff (X10 mouse events reporting).
      fflush(stdout);
   }
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
      // special case for the pterm.
      MousePositionX = term_state.mouse_col;
      MousePositionY = term_state.mouse_row;
      
      if ((key!=NULL) && (*key!=0x00000000UL))
      {
         if ((*key & K_MOUSE_POS) == K_MOUSE_POS)
         {
            if ((*key & K_MOUSE_BSELECT) == K_MOUSE_BSELECT)
            {
               if ((*key & K_MOUSE_CLICK) == K_MOUSE_CLICK)
               {
                  MouseButtons|=mbLeftButton; // left button;
               }
               if ((*key & K_MOUSE_RELEASE) == K_MOUSE_RELEASE)
               {
                  MouseButtons&=~mbLeftButton; // left button;
               }
            }
            if ((*key & K_MOUSE_BADJUST) == K_MOUSE_BADJUST)
            {
               if ((*key & K_MOUSE_CLICK) == K_MOUSE_CLICK)
               {
                  MouseButtons|=mbMiddleButton; // middle button;
               }
               if ((*key & K_MOUSE_RELEASE) == K_MOUSE_RELEASE)
               {
                  MouseButtons&=~mbMiddleButton; // middle button;
               }
            }
            if ((*key & K_MOUSE_BMENU) == K_MOUSE_BMENU)
            {
               if ((*key & K_MOUSE_CLICK) == K_MOUSE_CLICK)
               {
                  MouseButtons|=mbRightButton; // right button;
               }
               if ((*key & K_MOUSE_RELEASE) == K_MOUSE_RELEASE)
               {
                  MouseButtons&=~mbRightButton; // right button;
               }
            }
         }
      }

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
