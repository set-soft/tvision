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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_stdio
#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_TObject
#define Uses_TMouse
#define Uses_TApplication
#include <tv.h>

static TEventQueue *teq = 0;

#if 0 /************** Disabled ************/
#ifdef TVCompf_djgpp
#include <dpmi.h>

#define REGS __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)

#define AX r.x.ax
#define BX r.x.bx
#define CX r.x.cx
#define DX r.x.dx
#define ES r.x.es

static int mouse_buffer_segment;
static int mouse_buffer_selector;
static int mouse_buffer_size;
static int mouse_buffer_allocated = 0;

#endif

static void restore_mouse_state()
{
#ifdef TVCompf_djgpp
  REGS r;
  if (mouse_buffer_allocated)
  {
    AX = 0x0017;
    BX = mouse_buffer_size;
    ES = mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
  }
#endif
}

static void save_mouse_state()
{
#ifdef TVCompf_djgpp
  REGS r;
  if (!mouse_buffer_allocated)
  {
    AX = 0x0015;
    INTR(0x33,r);
    mouse_buffer_size = BX;
    mouse_buffer_segment = __dpmi_allocate_dos_memory(
                             (mouse_buffer_size+15)>>4,
                             &mouse_buffer_selector);
    if (mouse_buffer_segment != -1)
      mouse_buffer_allocated = 1;
  }
  if (mouse_buffer_allocated)
  {
    AX = 0x0016;
    BX = mouse_buffer_size;
    ES = mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
  }
#endif
}
#endif /************** Disabled ************/


/************** Disabled ************/
#if 0
static int my_mouse_buffer_segment;
static int my_mouse_buffer_selector;
static int my_mouse_buffer_size;
static int my_mouse_buffer_allocated = 0;

static void restore_my_mouse_state()
{
  REGS r;
  if (my_mouse_buffer_allocated)
  {
    AX = 0x0017;
    BX = my_mouse_buffer_size;
    ES = my_mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
  }
}

static void save_my_mouse_state()
{
  REGS r;
  if (!my_mouse_buffer_allocated)
  {
    AX = 0x0015;
    INTR(0x33,r);
    my_mouse_buffer_size = BX;
    my_mouse_buffer_segment = __dpmi_allocate_dos_memory(
                             (my_mouse_buffer_size+15)>>4,
                             &my_mouse_buffer_selector);
    if (my_mouse_buffer_segment != -1)
      my_mouse_buffer_allocated = 1;
  }
  if (my_mouse_buffer_allocated)
  {
    AX = 0x0016;
    BX = my_mouse_buffer_size;
    ES = my_mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
  }
}
#endif /************** Disabled ************/

void TApplication::resume()
{
  #if 0
  save_mouse_state();
  #endif
  TScreen::resume();
  TEventQueue::resume();
  #if 0
  restore_my_mouse_state();
  #endif
  resetIdleTime(); // Don't count this time
}

void TApplication::suspend()
{
  #if 0
  save_my_mouse_state();
  #endif
  TEventQueue::suspend();
  TScreen::suspend();
  #if 0
  restore_mouse_state();
  #endif
}

void initHistory();
void doneHistory();

TApplication::TApplication() :
    TProgInit( &TApplication::initStatusLine,
                  &TApplication::initMenuBar,
                  &TApplication::initDeskTop
                )
{
    if (!teq)
      teq = new TEventQueue();
    initHistory();
}

TApplication::~TApplication()
{
    doneHistory();
    delete teq;
    teq = 0;
}

