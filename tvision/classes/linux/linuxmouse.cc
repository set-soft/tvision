/*****************************************************************************

  Linux mouse routines.
  Copyright (c) 1996-1998 by Robert Hoehne.
  Copyright (c) 1999-2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdlib
#define Uses_string
#define Uses_stdio
#define Uses_time
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_signal
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOSf_Linux) && defined(HAVE_GPM)

//---------------- GPM version -----------------

#include <termios.h>
#include <tv/linux/screen.h>
#include <tv/linux/mouse.h>
#include <tv/linux/log.h>

extern "C" {
#include <gpm.h>
// Kernel 2.6.11 (from Gentoo) seems to need it:
// Not yet confirmed but doesn't interfere with 2.6.0 headers.
#include <linux/keyboard.h>
}

static
int SetUpGPMConnection()
{
 Gpm_Connect conn;
 
 conn.eventMask = ~0;   /* I want all events */
 conn.defaultMask = 0;  /* no default treatment */
 /* It doesn't report events from GPM when ALT or ALTGR are pressed.
    I saw it in mc and my friends from BALUG, Ivan and Laszlo bugged me
    about adding it. Was hyper-easy, no? (SET) */
 conn.maxMod = ~((1<<KG_ALT) | (1<<KG_ALTGR));
 conn.minMod = 0;
 gpm_zerobased = 1;   /* coordinates start from zero */
 
 return Gpm_Open(&conn, 0);
}

void THWMouseGPM::Resume()
{
 if (gpm_fd!=-1)
   {
    buttonCount=3;
    return;
   }
 if (SetUpGPMConnection()<0)
    LOG("no gpm, running without mouse");
 else
   {
    LOG("gpm server version " << Gpm_GetServerVersion(NULL));
    buttonCount=3;
   }
 show();
}

void THWMouseGPM::Suspend()
{
 if (gpm_fd==-1)
    return;
 Gpm_Close();
 gpm_fd=-1;
 LOG("gpm connection closed");
}

inline
int range(int test, int min, int max)
{
 return test < min ? min : test > max ? max : test;
}

void THWMouseGPM::GetEvent(MouseEventType &me)
{
 Gpm_Event mev;

 me.buttons=TEventQueue::curMouse.buttons & ~(mbButton4 | mbButton5);
 me.doubleClick=False;
 if (!Gpm_Repeat(1) && (Gpm_GetEvent(&mev)==1))
   {
    int b=mev.buttons;
    if ((b & GPM_B_LEFT) && !(mev.type & GPM_UP))
       me.buttons|= mbLeftButton;
    else
       me.buttons&= ~mbLeftButton;
    if ((b & GPM_B_RIGHT) && !(mev.type & GPM_UP))
       me.buttons|= mbRightButton;
    else
       me.buttons&= ~mbRightButton;
    if (mev.wdy<0)
       me.buttons=mbButton5;
    else
       if (mev.wdy>0)
          me.buttons=mbButton4;
    me.where.x=range(mev.x,0,TScreen::screenWidth-1);
    me.where.y=range(mev.y,0,TScreen::screenHeight-1);
    DrawMouse(me.where.x,me.where.y);
   }
 else
   {
    me.where.x=TEventQueue::curMouse.where.x;
    me.where.y=TEventQueue::curMouse.where.y;
   }
}

void THWMouseGPM::Init()
{
 THWMouseUNIX::Init();
 THWMouse::Resume=Resume;
 THWMouse::Suspend=Suspend;
 THWMouse::GetEvent=GetEvent;
}

THWMouseGPM::~THWMouseGPM()
{
}

#else // defined(TVOSf_Linux) && defined(HAVE_GPM)

#if defined(TVOSf_Linux)
 #include <termios.h>
#endif

#include <tv/linux/screen.h>
#include <tv/linux/mouse.h>
#include <tv/linux/log.h>

#endif // else defined(TVOSf_Linux) && defined(HAVE_GPM)
