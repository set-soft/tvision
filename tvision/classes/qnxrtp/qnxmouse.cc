/* QNX RtP mouse handler routines source.
   Copyright (c) 2002-2003 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_stdlib
#define Uses_string
#define Uses_stdio
#define Uses_time
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/dcmd_input.h>

#include <tv/qnxrtp/screen.h>
#include <tv/qnxrtp/mouse.h>

int THWMouseQNXRtP::MousePositionX=0;
int THWMouseQNXRtP::MousePositionY=0;
int THWMouseQNXRtP::MouseButtons;

int THWMouseQNXRtP::MouseMickeyPositionX=0;
int THWMouseQNXRtP::MouseMickeyPositionY=0;

THWMouseQNXRtP::~THWMouseQNXRtP()
{
   // this destructor is never called !
   // instead of it THWMouseQNXRtP::Suspend method called.
}

int THWMouseQNXRtP::mousehandle=-1;

int  THWMouseQNXRtP::OpenDeviMouse()
{
   mousehandle=open("/dev/devi/mouse0", O_RDONLY);
   if (mousehandle==-1)
   {
      return 0;
   }
   
   MousePositionX=0;
   MousePositionY=0;
   MouseButtons=0;
   MouseMickeyPositionX=0;
   MouseMickeyPositionY=0;
   
   return 1;
}

void THWMouseQNXRtP::CloseDeviMouse()
{
   if (mousehandle!=-1)
   {
      close(mousehandle);
      mousehandle=-1;
   }

   MousePositionX=0;
   MousePositionY=0;
   MouseButtons=0;
   MouseMickeyPositionX=0;
   MouseMickeyPositionY=0;
}

void THWMouseQNXRtP::ProcessDeviMouse()
{
   fd_set mev;
   int selectret;
   struct timeval twait;
   struct _mouse_packet pkt;
   int cols=TDisplayQNXRtP::GetCols();
   int rows=TDisplayQNXRtP::GetRows();
   int TempMousePositionX=MousePositionX;
   int TempMousePositionY=MousePositionY;

   FD_ZERO(&mev);
   FD_SET(mousehandle, &mev);

   twait.tv_sec=0;
   twait.tv_usec=1;

   selectret=select(mousehandle + 1, &mev, NULL, NULL, &twait);
   switch (selectret)
   {
      case -1:
      case  0:
               break;
      default: if (FD_ISSET(mousehandle, &mev))
               {
                  selectret=read(mousehandle, &pkt, sizeof(struct _mouse_packet));
                  if (selectret==sizeof(struct _mouse_packet))
                  {
                     MouseMickeyPositionX+=pkt.dx;
                     if (MouseMickeyPositionX<0)
                     {
                        MouseMickeyPositionX=0;
                     }
                     MouseMickeyPositionY-=pkt.dy;
                     if (MouseMickeyPositionY<0)
                     {
                        MouseMickeyPositionY=0;
                     }
                     TempMousePositionX=MouseMickeyPositionX/6;
                     TempMousePositionY=MouseMickeyPositionY/14;
                     if (TempMousePositionX>=cols)
                     {
                        TempMousePositionX=cols-1;
                        MouseMickeyPositionX=TempMousePositionX*6;
                     }
                     if (TempMousePositionY>=rows)
                     {
                        TempMousePositionY=rows-1;
                        MouseMickeyPositionY=TempMousePositionY*14;
                     }
                     if ((pkt.hdr.buttons & _POINTER_BUTTON_LEFT)==_POINTER_BUTTON_LEFT)
                     {
                        MouseButtons|=mbLeftButton;
                     }
                     else
                     {
                        MouseButtons&=~mbLeftButton;
                     }
                     if ((pkt.hdr.buttons & _POINTER_BUTTON_RIGHT)==_POINTER_BUTTON_RIGHT)
                     {
                        MouseButtons|=mbRightButton;
                     }
                     else
                     {
                        MouseButtons&=~mbRightButton;
                     }
                     if ((pkt.hdr.buttons & _POINTER_BUTTON_MIDDLE)==_POINTER_BUTTON_MIDDLE)
                     {
                        MouseButtons|=mbMiddleButton;
                     }
                     else
                     {
                        MouseButtons&=~mbMiddleButton;
                     }
                  }
               }
               break;
   }
   if ((TempMousePositionX!=MousePositionX) || (TempMousePositionY!=MousePositionY))
   {
      Hide();

      MousePositionX=TempMousePositionX;
      MousePositionY=TempMousePositionY;
      
      Show();
   }
}

void THWMouseQNXRtP::Init()
{
   if (OpenDeviMouse())
   {
      buttonCount=3;

      THWMouse::Show=Show;
      THWMouse::Hide=Hide;
      THWMouse::Suspend=Suspend;
      THWMouse::Resume=Resume;
      THWMouse::GetEvent=GetEvent;
   }
   else
   {
      buttonCount=0;
   }
}

void THWMouseQNXRtP::Show()
{
   if (mousehandle!=-1)
   {
      int cols=TDisplayQNXRtP::GetCols();
      ushort offset=MousePositionY*cols+MousePositionX;
      ushort mdata=TScreenQNXRtP::screenBuffer[offset];

      // a la Windows 2000 style text mouse cursor.
      mdata=(mdata & 0x00FF) | (mdata & 0x8800) | (~(mdata) & 0x7000) | (~(mdata) & 0x0700);
      TScreenQNXRtP::writeBlock(offset, 1, &mdata, &mdata);
   }
}

void THWMouseQNXRtP::Hide()
{
   if (mousehandle!=-1)
   {
      int cols=TDisplayQNXRtP::GetCols();
      ushort offset=MousePositionY*cols+MousePositionX;

      TScreenQNXRtP::writeBlock(offset, 1, TScreenQNXRtP::screenBuffer+offset, TScreenQNXRtP::screenBuffer+offset);
   }
}

void THWMouseQNXRtP::Suspend()
{
   CloseDeviMouse();
}

void THWMouseQNXRtP::Resume()
{
   if (OpenDeviMouse())
   {
      buttonCount=3;
   }
   else
   {
      buttonCount=0;
   }
}

void THWMouseQNXRtP::GetEvent(MouseEventType &me)
{
   if (mousehandle!=-1)
   {
      ProcessDeviMouse();
   }

   me.buttons = MouseButtons;
   me.doubleClick = False;
   me.where.x = MousePositionX;
   me.where.y = MousePositionY;
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnxrtp/screen.h>
   #include <tv/qnxrtp/mouse.h>

#endif // TVOS_UNIX && TVOSf_QNXRtP
