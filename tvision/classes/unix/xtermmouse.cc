/*****************************************************************************

  XTerm mouse routines.
  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  This driver is a little bit different than THWMouseXTerm because:
  1) Is specifically used with TScreenXTerm.
  2) Uses a finer reporting mechanism.
  3) TScreenXTerm::SendToTerminal doesn't exist.

  TODO:
  Why save/restore fails in Eterm?
  Bug Eterm people to get 1002 ;-)

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <tv/unix/mouse.h>
#include <tv/unix/xtmouse.h>
#include <tv/linux/log.h>

int THWMouseXTermFull::mode=modeEterm;

void THWMouseXTermFull::ResumeFull()
{
 buttonCount=5; // X uses at least 3 buttons, but 5 can be reported
 // Save old hilit tracking and enable mouse tracking
 // 1002 == Use Cell Motion Mouse Tracking.
 fprintf(stdout,"\E[?1002s\E[?1002h");
 show();
 LOG("XTerm mouse enabled (full)");
}

void THWMouseXTermFull::SuspendFull()
{
 // Disable mouse tracking and restore old hilittracking
 fprintf(stdout,"\E[?1002l\E[?1002r");
 LOG("XTerm mouse disabled (full)");
}

void THWMouseXTermFull::ResumeSimple()
{
 buttonCount=5; // X uses at least 3 buttons, but 5 can be reported
 //fprintf(stdout,"\E[?1000s\E[?1000h");
 fprintf(stdout,"\E[?1000h");
 show();
 LOG("XTerm mouse enabled (simple)");
}

void THWMouseXTermFull::SuspendSimple()
{
 // Disable mouse tracking and restore old hilittracking
 //fprintf(stdout,"\E[?1000l\E[?1000r"); Is Eterm broken or is just me?
 fprintf(stdout,"\E[?1000l");
 LOG("XTerm mouse disabled (simple)");
}

void THWMouseXTermFull::Init(int aMode)
{
 THWMouseUNIX::Init();
 mode=aMode;
 if (mode==modeXTerm)
   {
    THWMouse::Resume=ResumeFull;
    THWMouse::Suspend=SuspendFull;
   }
 else
   {
    THWMouse::Resume=ResumeSimple;
    THWMouse::Suspend=SuspendSimple;
   }
}

THWMouseXTermFull::~THWMouseXTermFull()
{
}
#else // TVOS_UNIX && !TVOSf_QNXRtP

#include <tv/unix/mouse.h>
#include <tv/unix/xtmouse.h>
#include <tv/linux/log.h>

#endif // else TVOS_UNIX && !TVOSf_QNXRtP
