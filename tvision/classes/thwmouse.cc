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

#ifdef __DJGPP__

#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

#include <dos.h>
#include <stdio.h>

uchar THWMouse::buttonCount = 0;
Boolean THWMouse::handlerInstalled = False;
Boolean THWMouse::noMouse = False;

#if defined( DJGPP ) && ( DJGPP > 1 )
#include <dpmi.h>
#define REGS __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)
#else
#define REGS union REGS
#define INTR(nr,r) int86(nr,&r,&r)
#endif
#include <conio.h>
#include <sys/farptr.h>
#include <go32.h>

#define AX r.x.ax
#define BX r.x.bx
#define CX r.x.cx
#define DX r.x.dx
#define ES r.x.es

static unsigned short mouse_char;
static int last_x = 0,last_y = 0;
static int cols;
static int visible = 0;
int emulate_mouse = 0;
int dual_display = 0;

#define SC_BASE (dual_display ? 0xb0000 : ScreenPrimary)

static volatile
void get_mouse_char()
{
  mouse_char = _farpeekw(_dos_ds,SC_BASE+(last_y*cols+last_x)*2);
}

static volatile
void set_mouse_char()
{
  unsigned short c = mouse_char ^ 0x7F00;
  _farpokew(_dos_ds,SC_BASE+(last_y*cols+last_x)*2,c);
}

static volatile
void reset_mouse_char()
{
  _farpokew(_dos_ds,SC_BASE+(last_y*cols+last_x)*2,mouse_char);
}

static volatile
void show_mouse_char()
{
  if (!visible) return;
  get_mouse_char();
  set_mouse_char();
}

volatile
int draw_mouse(int x,int y)
{
  if (x != last_x || y != last_y)
  {
    if (!emulate_mouse) return 1;
    if (visible) reset_mouse_char();
    last_x = x;
    last_y = y;
    show_mouse_char();
    return 1;
  }
  return 0;
}   

_go32_dpmi_seginfo mouseIntInfo;

#if 1
// SET: I think we should really test it, Robert is using a call of the v3.0+
// to set the mouse handler and one from v6.0+ to restore, why not using the
// same for both? My doubt is that the v6.0+ is for *alternate* handler and
// supports upto 3 different.
// Todo: it never checks the return value!!!
void THWMouse::registerHandler( unsigned mask, void (*func)() )
{
  REGS r;
  static int oldes = 0;
  static int olddx = 0;
  
  if (func == NULL)
  { // INT 33 - MS MOUSE v6.0+ - SET ALTERNATE MOUSE USER HANDLER
    DX = olddx;
    ES = oldes;
    AX = 0x0018;
    CX = mask;
    INTR(0x33,r);
    return;
  }
  else
  { // INT 33 - MS MOUSE v3.0+ - EXCHANGE INTERRUPT SUBROUTINES
    DX = mouseIntInfo.rm_offset;
    ES = mouseIntInfo.rm_segment;
    AX = 0x0014;
    CX = mask;
    INTR(0x33,r);
    oldes = ES;
    olddx = DX;
  }
}
#else
// SET: I moved it here to keep the old routine and clean the currently used.
// I don't know why Robert dropped it. The main drawback is that you kill
// any old mouse handler. Looks like v1.0 didn't implement a way to get the
// current handler, not strange since that's a Microsoft spec. So it kills
// the mouse of the debuggy. I think it was the reason.
void THWMouse::registerHandler( unsigned mask, void (*func)() )
{
  REGS r;
  if (func == NULL)
  { // MS MOUSE v1.0+ - DEFINE INTERRUPT SUBROUTINE PARAMETERS
    DX = ES = 0;
    AX = 0x000C;
    CX = mask;
    INTR(0x33,r);
    return;
  }
  else
  { // MS MOUSE v1.0+ - DEFINE INTERRUPT SUBROUTINE PARAMETERS
    DX = mouseIntInfo.rm_offset;
    ES = mouseIntInfo.rm_segment;
    AX = 0x000C;
    CX = mask;
    INTR(0x33,r);
  }
}
#endif

extern _go32_dpmi_registers mouseIntRegs;
extern MouseEventType tempMouse;

static ushort mx = 0;
static ushort my = 0;

#define LD(x) _go32_dpmi_lock_data(&x,sizeof(x))

static int mouse_buffer_segment;
static int mouse_buffer_selector;
static int mouse_buffer_size;
static int mouse_buffer_allocated = 0;

#define SHOW_COUNT() ((short)(_farpeekw(mouse_buffer_selector,118)))

static void restore_mouse_state()
{
  REGS r;
  if (mouse_buffer_allocated)
  {
    AX = 0x0017;
    BX = mouse_buffer_size;
    ES = mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
  }
}

static void save_mouse_state()
{
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
}

static int my_mouse_buffer_segment;
static int my_mouse_buffer_selector;
static int my_mouse_buffer_size;
static int my_mouse_buffer_allocated = 0;
static int my_buttonCount;
static int mouse_is_shown = 0;

static int restore_my_mouse_state()
{
  REGS r;
  if (my_mouse_buffer_allocated)
  {
    AX = 0x0017;
    BX = my_mouse_buffer_size;
    ES = my_mouse_buffer_segment;
    DX = 0;
    INTR(0x33,r);
    return 1;
  }
  return 0;
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

void THWMouse::resume()
{
  REGS r;
  save_mouse_state();
  buttonCount = my_buttonCount;
  if (!restore_my_mouse_state())
  {
    AX = 4;
    CX = mx;
    DX = my;
    INTR(0x33,r);
  }
  show();
}

int use_mouse_handler = 1;

THWMouse::THWMouse()
{
  REGS r;
  AX = 0;
  INTR(0x33,r);
  if (!AX)
      return;
  my_buttonCount = buttonCount = BX & 0x00ff;
  if (handlerInstalled == False && use_mouse_handler)
  {
    LD(mouseIntRegs);
    LD(tempMouse);
    LD(mouse_char);
    LD(last_x);
    LD(last_y);
    LD(visible);
    LD(emulate_mouse);
    LD(cols);
    LD(dual_display);
    LD(TEventQueue::eventCount);
    LD(TEventQueue::curMouse);
    LD(TEventQueue::eventQueue);
    LD(TEventQueue::mouseIntFlag);
    LD(TEventQueue::eventQTail);
    _go32_dpmi_lock_data(TEventQueue::eventQueue,eventQSize*sizeof(TEvent));
    _go32_dpmi_lock_code(get_mouse_char,(int)THWMouse::registerHandler -
                                        (int)get_mouse_char);
    _go32_dpmi_lock_code(TEventQueue::mouseInt,500);
    _go32_dpmi_lock_code(TICKS,100);
    mouseIntInfo.pm_offset = (int)TEventQueue::mouseInt;
    _go32_dpmi_allocate_real_mode_callback_retf(&mouseIntInfo,&mouseIntRegs);
    handlerInstalled = True;
  }
  resume();
  hide();
  if (handlerInstalled == True)
    registerHandler( 0xFFFF, TEventQueue::mouseInt );
}

THWMouse::~THWMouse()
{
  suspend();
  if (handlerInstalled == True)
  {
    registerHandler( 0xFFFF, 0 );
    // SET: to avoid a leak reported.
    _go32_dpmi_free_real_mode_callback(&mouseIntInfo);
  }
}

void THWMouse::suspend()
{
  if (!present()) return;
  hide();
  save_my_mouse_state();
  my_buttonCount = buttonCount;
  buttonCount = 0;
  restore_mouse_state();
}

void THWMouse::show()
{
  if (!present()) return;
  if (mouse_is_shown) return;
  mouse_is_shown = 1;
  if (!emulate_mouse)
  {
    REGS r;
    if ( present() )
    {
      AX = 1;
      INTR(0x33,r);
    }
  }
  else
  {
    if (!present() || visible) return;
    visible = 1;
    show_mouse_char();
  }
}

void THWMouse::hide()
{
  if (!present()) return;
  if (!mouse_is_shown) return;
  mouse_is_shown = 0;
  if (!emulate_mouse)
  {
    REGS r;
    if ( buttonCount != 0 )
    {
      AX = 2;
      INTR(0x33,r);
    }
  }
  else
  {
    if (!present() || !visible) return;
    visible = 0;
    reset_mouse_char();
  }
}

void THWMouse::setRange( ushort rx, ushort ry )
{
    cols = (int)rx+1;
    REGS r;
    DX = rx << 3;
    CX = 0;
    AX = 7;
    INTR(0x33,r);
    DX = ry << 3;
    CX = 0;
    AX = 8;
    INTR(0x33,r);
}

static int m_x=0,m_y=0,m_b=0;
static int forced=0;

/**[txh]********************************************************************

  Description:
  It forces the state of the mouse externally, the next call to getEvent
will return this values instead of values from the mouse driver. That's
used to simuate the mouse with other events like keyboard. (SET)

***************************************************************************/

void THWMouse::forceEvent(int x, int y, int buttons)
{
 m_x=x; m_y=y; m_b=buttons;
 forced=1;
}

void THWMouse::getEvent( MouseEventType& me )
{
  if (forced)
  {
    forced=0;
    me.where.x = m_x;
    me.where.y = m_y;
    me.doubleClick = False;
    me.buttons = m_b;
    TEventQueue::curMouse = me;
    draw_mouse(m_x,m_y);
  }
  else
  if (handlerInstalled == True)
  {
    me = TEventQueue::curMouse;
  }
  else
  {
    REGS r;
    int x,y;
    AX = 3;
    INTR(0x33,r);
    me.buttons = BX & 0x00ff;
    x = me.where.x = (CX >> 3) & 0xFF;
    y = me.where.y = (DX >> 3) & 0xFF;
    me.doubleClick = False;
    // curMouse must be set, because it used by other functions
    TEventQueue::curMouse = me;
    draw_mouse(x,y);
  }
}

#endif // __DJGPP__

