/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TScreen
#define Uses_TEventQueue

#include <tv.h>

#ifdef DJGPP
#include <dpmi.h>
#include <sys/farptr.h>
#endif

int dual_display_supported()
{
#ifdef DJGPP
  __dpmi_regs r;
  r.x.ax = 0x1a00;
  __dpmi_int(0x10,&r);
  if (r.h.al != 0x1a) return 0;
  if (r.h.bh == 0x01) return 1;
#endif
#ifdef __linux__
  extern unsigned short *mono_mem;
  return (mono_mem != NULL);
#endif
  return 0;
}

void SetDualDisplay()
{
  if (dual_display) return;
  if (!dual_display_supported()) return;
  TEventQueue::suspend();
  TScreen::suspend();
  dual_display = 1;
  TScreen::resume();
  TEventQueue::resume();
}


void SetNormalDisplay()
{
  if (!dual_display) return;
  TEventQueue::suspend();
  TScreen::suspend();
  dual_display = 0;
  TScreen::resume();
  TEventQueue::resume();
}

