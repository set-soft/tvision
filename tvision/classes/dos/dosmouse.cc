/**[txh]********************************************************************

  Copyright (c) 2001-2002 by Salvador E. Tropea
  Based on code Copyright 1996-1998 by Robert H”hne.

  Description:
  DOS mouse drivers. Completly reworked by Salvador E. Tropea.
  Based on concepts by Jose Angel Sanchez Caso (JASC) and code by Robert
H”hne.
  Important:
  1) The registerHandler member can only register TEventQueue::mouseInt.
  2) The code lock for TEventQueue::mouseInt and CLY_Ticks is fuzzy, I
     incressed the locked area to 1 Kb but it should adjusted.

  The DOS/PollMouse configuration option forces mouse polling.
  
***************************************************************************/

#include <tv/configtv.h>

#ifdef TVCompf_djgpp
 #include <conio.h>
#endif
#define Uses_string
#define Uses_stdlib
#define Uses_stdio
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp

#include <dos.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <go32.h>

#include <tv/dos/mouse.h>

#define REGS       __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)
#define AX r.x.ax
#define BX r.x.bx
#define CX r.x.cx
#define DX r.x.dx
#define ES r.x.es
#define LD(x) _go32_dpmi_lock_data((void *)&x,sizeof(x))
//#define SHOW_COUNT() ((short)(_farpeekw(mouse_buffer_selector,118)))

// MS-Mouse services: [Thanks to Ralf Brown]
#define RESET_DRIVER_AND_READ_STATUS       0x0000
// MS-Mouse v1.0+ services
#define SHOW_MOUSE_CURSOR                  0x0001
#define HIDE_MOUSE_CURSOR                  0x0002
#define RETURN_POSITION_AND_BUTTON_STATUS  0x0003
#define POSITION_MOUSE_CURSOR              0x0004
#define DEFINE_HORIZONTAL_CURSOR_RANGE     0x0007
#define DEFINE_VERTICAL_CURSOR_RANGE       0x0008
// MS-Mouse v3.0+ services
#define EXCHANGE_INTERRUPT_SUBROUTINES     0x0014
// MS-Mouse v6.0+ services
#define RETURN_DRIVER_STORAGE_REQUIREMENTS 0x0015
#define SAVE_DRIVER_STATE                  0x0016
#define RESTORE_DRIVER_STATE               0x0017
#define SET_ALTERNATE_MOUSE_USER_HANDLER   0x0018
#define RETURN_USER_ALTERNATE_INTERRUPT_VECTOR 0x0019

char   THWMouseDOS::useMouseHandler=1;
char   THWMouseDOS::emulateMouse=0;
uchar  THWMouseDOS::myButtonCount;
ulong  THWMouseDOS::screenBase=0xB8000;
ushort THWMouseDOS::mouseChar;
int    THWMouseDOS::lastX=0,
       THWMouseDOS::lastY=0;
int    THWMouseDOS::cols;
MouseEventType
       THWMouseDOS::intEvent;
int    THWMouseDOS::bufferSegment;
int    THWMouseDOS::bufferSelector;
int    THWMouseDOS::bufferSize;
int    THWMouseDOS::bufferAllocated=0;
int    THWMouseDOS::myBufferSegment;
int    THWMouseDOS::myBufferSelector;
int    THWMouseDOS::myBufferSize;
int    THWMouseDOS::myBufferAllocated=0;

// Information for the Real Mode CallBack (RMCB)
static _go32_dpmi_seginfo mouseIntInfo;
// x86 registers values stored by the RMCB
static _go32_dpmi_registers mouseIntRegs;

/*****************************************************************************
  Functions in this area are called asynchronically on interrupts using an
  RMCB. For this reason they must be locked avoiding swap outs of their pages.
  The functions are delimited by two empty functions that just mark the range.
*****************************************************************************/

static volatile
void StartOfLockedFunctions()
{
}

volatile
void THWMouseDOS::getMouseChar()
{
 mouseChar=_farpeekw(_dos_ds,screenBase+(lastY*cols+lastX)*2);
}

volatile
void THWMouseDOS::setMouseChar()
{
 unsigned short c=mouseChar^0x7F00;
 _farpokew(_dos_ds,screenBase+(lastY*cols+lastX)*2,c);
}

volatile
void THWMouseDOS::resetMouseChar()
{
 _farpokew(_dos_ds,screenBase+(lastY*cols+lastX)*2,mouseChar);
}

volatile
void THWMouseDOS::showMouseChar()
{
 if (!visible) return;
 getMouseChar();
 setMouseChar();
}

int THWMouseDOS::DrawMouseEmulate(int x, int y)
{
 if (x!=lastX || y!=lastY)
   {
    if (visible)
      {
       drawCounter++;
       resetMouseChar();
      }
    lastX=x;
    lastY=y;
    showMouseChar();
    return 1;
   }
 return 0;
}

int THWMouseDOS::getRMCB_InfoDraw(int &buttonPress)
{
 int x,y;

 intEvent.doubleClick=False;
 intEvent.buttons    =mouseIntRegs.h.bl;
 x=intEvent.where.x  =(mouseIntRegs.x.cx>>3) & 0xFF;
 y=intEvent.where.y  =(mouseIntRegs.x.dx>>3) & 0xFF;
 buttonPress          =mouseIntRegs.x.ax & 0x1e;
 return drawMouse(x,y);
}

static volatile
void EndOfLockedFunctions()
{
}

/*****************************************************************************
 End of locked functions
*****************************************************************************/

int THWMouseDOS::DrawMouseDummy(int x, int y)
{
 if (x!=lastX || y!=lastY)
   {
    lastX=x;
    lastY=y;
    return 1;
   }
 return 0;
}

void THWMouseDOS::setEmulation(int emulate)
{
 emulateMouse=emulate;
 if (emulate)
   {
    THWMouse::Show=ShowEmulate;
    THWMouse::Hide=HideEmulate;
    THWMouse::drawMouse=DrawMouseEmulate;
   }
 else
   {
    THWMouse::Show=ShowDrv;
    THWMouse::Hide=HideDrv;
    THWMouse::drawMouse=DrawMouseDummy;
   }
}

#if 1
// SET: I think we should really test it, Robert is using a call of the v3.0+
// to set the mouse handler and one from v6.0+ to restore, why not using the
// same for both? My doubt is that the v6.0+ is for *alternate* handler and
// supports upto 3 different.
// Todo: it never checks the return value!!!
/*void THWMouseDOS::RegisterHandler(unsigned mask, void (*func)())
{
 REGS r;
 static int oldes=0;
 static int olddx=0;
 
 if (func==NULL)
   { // INT 33 - MS MOUSE v6.0+ - SET ALTERNATE MOUSE USER HANDLER
    DX=olddx;
    ES=oldes;
    AX=SET_ALTERNATE_MOUSE_USER_HANDLER;
    CX=mask;
    INTR(0x33,r);
    return;
   }
 else
   { // INT 33 - MS MOUSE v3.0+ - EXCHANGE INTERRUPT SUBROUTINES
    DX=mouseIntInfo.rm_offset;
    ES=mouseIntInfo.rm_segment;
    AX=EXCHANGE_INTERRUPT_SUBROUTINES;
    CX=mask;
    INTR(0x33,r);
    oldes=ES;
    olddx=DX;
   }
}*/
void THWMouseDOS::RegisterHandler(unsigned mask, void (*func)())
{
 REGS r;
 static int oldes=0;
 static int olddx=0;
 
 if (func==NULL)
   { // INT 33 - MS MOUSE v6.0+ - SET ALTERNATE MOUSE USER HANDLER
    DX=olddx; ES=oldes;
    AX=SET_ALTERNATE_MOUSE_USER_HANDLER;
    CX=mask;
    INTR(0x33,r);
    return;
   }
 else
   {// INT 33 - MS MOUSE v6.0+ - RETURN USER ALTERNATE INTERRUPT VECTOR
    AX=RETURN_USER_ALTERNATE_INTERRUPT_VECTOR;
    CX=mask;
    INTR(0x33,r);
    oldes=BX;
    olddx=DX;
    // INT 33 - MS MOUSE v6.0+ - SET ALTERNATE MOUSE USER HANDLER
    DX=mouseIntInfo.rm_offset;
    ES=mouseIntInfo.rm_segment;
    AX=SET_ALTERNATE_MOUSE_USER_HANDLER;
    CX=mask;
    INTR(0x33,r);
   }
}

#else
// SET: I moved it here to keep the old routine and clean the currently used.
// I don't know why Robert dropped it. The main drawback is that you kill
// any old mouse handler. Looks like v1.0 didn't implement a way to get the
// current handler, not strange since that's a Microsoft spec. So it kills
// the mouse of the debuggy. I think it was the reason.
void THWMouseDOS::RegisterHandler( unsigned mask, void (*func)() )
{
 REGS r;
 if (func == NULL)
   { // MS MOUSE v1.0+ - DEFINE INTERRUPT SUBROUTINE PARAMETERS
    DX=ES=0;
    AX=0x000C;
    CX=mask;
    INTR(0x33,r);
    return;
   }
 else
   { // MS MOUSE v1.0+ - DEFINE INTERRUPT SUBROUTINE PARAMETERS
    DX=mouseIntInfo.rm_offset;
    ES=mouseIntInfo.rm_segment;
    AX=0x000C;
    CX=mask;
    INTR(0x33,r);
   }
}
#endif

void THWMouseDOS::biosRestoreState()
{
 REGS r;
 if (bufferAllocated)
   {
    AX=RESTORE_DRIVER_STATE;
    BX=bufferSize;
    ES=bufferSegment;
    DX=0;
    INTR(0x33,r);
   }
}

int THWMouseDOS::biosRestoreMyState()
{
 REGS r;
 if (myBufferAllocated)
   {
    AX=RESTORE_DRIVER_STATE;
    BX=myBufferSize;
    ES=myBufferSegment;
    DX=0;
    INTR(0x33,r);
    return 1;
   }
 return 0;
}

void THWMouseDOS::biosSaveState()
{
 REGS r;
 if (!bufferAllocated)
   {
    AX=RETURN_DRIVER_STORAGE_REQUIREMENTS;
    INTR(0x33,r);
    bufferSize=BX;
    bufferSegment=__dpmi_allocate_dos_memory((bufferSize+15)>>4,
                  &bufferSelector);
    if (bufferSegment!=-1)
       bufferAllocated = 1;
   }
 if (bufferAllocated)
   {
    AX=SAVE_DRIVER_STATE;
    BX=bufferSize;
    ES=bufferSegment;
    DX=0;
    INTR(0x33,r);
   }
}

void THWMouseDOS::biosSaveMyState()
{
 REGS r;
 if (!myBufferAllocated)
   {
    AX=RETURN_DRIVER_STORAGE_REQUIREMENTS;
    INTR(0x33,r);
    myBufferSize=BX;
    myBufferSegment=__dpmi_allocate_dos_memory((myBufferSize+15)>>4,
                    &myBufferSelector);
    if (myBufferSegment!=-1)
       myBufferAllocated = 1;
   }
 if (myBufferAllocated)
   {
    AX=SAVE_DRIVER_STATE;
    BX=myBufferSize;
    ES=myBufferSegment;
    DX=0;
    INTR(0x33,r);
   }
}

void THWMouseDOS::Resume()
{
 REGS r;
 biosSaveState();
 buttonCount=myButtonCount;
 if (!biosRestoreMyState())
   {// Default to x=y=0
    AX=POSITION_MOUSE_CURSOR;
    CX=0;
    DX=0;
    INTR(0x33,r);
   }
}

void THWMouseDOS::Suspend()
{
 biosSaveMyState();
 biosRestoreState();
}

void THWMouseDOS::DeInit()
{
 if (handlerInstalled==True)
   {
    registerHandler(0xFFFF,0);
    _go32_dpmi_free_real_mode_callback(&mouseIntInfo);
   }
}

THWMouseDOS::~THWMouseDOS()
{
}

void THWMouseDOS::ShowDrv()
{
 REGS r;
 visible=1;
 AX=SHOW_MOUSE_CURSOR;
 INTR(0x33,r);
}

void THWMouseDOS::HideDrv()
{
 REGS r;
 visible=0;
 AX=HIDE_MOUSE_CURSOR;
 INTR(0x33,r);
}

void THWMouseDOS::ShowEmulate()
{
 visible=1;
 showMouseChar();
}

void THWMouseDOS::HideEmulate()
{
 visible=0;
 resetMouseChar();
}

void THWMouseDOS::SetRange(ushort rx, ushort ry)
{
 REGS r;
 // Used internally to draw the mouse
 cols=(int)rx+1;
 // Communicate it to the driver. Needed for video modes where the driver
 // fails to know the screen size.
 DX=rx<<3;
 CX=0;
 AX=DEFINE_HORIZONTAL_CURSOR_RANGE;
 INTR(0x33,r);
 DX=ry<<3;
 CX=0;
 AX=DEFINE_VERTICAL_CURSOR_RANGE;
 INTR(0x33,r);
}

void THWMouseDOS::GetEvent(MouseEventType &me)
{
 REGS r;
 int x,y;

 AX=RETURN_POSITION_AND_BUTTON_STATUS;
 INTR(0x33,r);
 me.buttons=BX & 0x00ff;
 x=me.where.x=(CX >> 3) & 0xFF;
 y=me.where.y=(DX >> 3) & 0xFF;
 me.doubleClick=False;
 drawMouse(x,y);
}

void THWMouseDOS::Init()
{
 char *OS=getenv("OS");
 // SET: NT reacts crashing when we use the mouse handler, don't know why
 // and personally don't care, so just disable the handler.
 if (OS && strcmp(OS,"Windows_NT")==0)
    useMouseHandler=0;

 // Look in the user options
 long pollMouse=1;
 TScreen::optSearch("PollMouse",pollMouse);
 if (pollMouse)
    useMouseHandler=0;

 REGS r;
 AX=RESET_DRIVER_AND_READ_STATUS;
 INTR(0x33,r);
 if (!AX)
    return;
 myButtonCount=buttonCount=BX & 0x00ff;

 // Ok mouse is present, set the driver functions
 setEmulation(emulateMouse);
 THWMouse::setRange=SetRange;
 THWMouse::GetEvent=GetEvent;
 THWMouse::registerHandler=RegisterHandler;
 THWMouse::Suspend=Suspend;
 THWMouse::Resume=Resume;
 
 screenBase=ScreenPrimary;
 if (handlerInstalled==False && useMouseHandler)
   {// Lock variables
    LD(mouseIntRegs);
    LD(intEvent);
    LD(mouseChar);
    LD(lastX);
    LD(lastY);
    LD(visible);
    //LD(emulateMouse);
    LD(drawCounter);
    LD(cols);
    LD(screenBase);
    LD(TEventQueue::eventCount);
    LD(TEventQueue::curMouse);
    LD(TEventQueue::eventQueue);
    LD(TEventQueue::mouseIntFlag);
    LD(TEventQueue::eventQTail);
    _go32_dpmi_lock_data(TEventQueue::eventQueue,eventQSize*sizeof(TEvent));
    // Lock functions
    _go32_dpmi_lock_code((void*)StartOfLockedFunctions,
                         (int)EndOfLockedFunctions-(int)StartOfLockedFunctions);
    _go32_dpmi_lock_code((void *)TEventQueue::mouseInt,1024);
    _go32_dpmi_lock_code((void *)CLY_Ticks,1024);
    // Setup the RMCB
    mouseIntInfo.pm_offset=(int)TEventQueue::mouseInt;
    _go32_dpmi_allocate_real_mode_callback_retf(&mouseIntInfo,&mouseIntRegs);
    handlerInstalled=True;
   }
 if (handlerInstalled==True)
    registerHandler(0xFFFF,TEventQueue::mouseInt);
}

#else // DJGPP

#include <tv/dos/mouse.h>

#endif // else DJGPP

