/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1999-2002 Salvador Eduardo Tropea */
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
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && !defined(TVOSf_QNXRtP)

#include <termios.h>
#include <tv/unix/screen.h>
#include <tv/unix/mouse.h>

//----------------- Common stuff ---------------

//#define DEBUG
#ifdef DEBUG
 #ifdef TVOSf_Linux
  extern char *program_invocation_short_name;
  #define LOG(s) do {cerr << program_invocation_short_name << ": " << s << endl; fflush(stderr);} while(0)
 #else
  #define LOG(s) do {cerr << s << endl; fflush(stderr);} while(0)
 #endif
#else
 #define LOG(s) do {;} while(0)
#endif

#ifdef TV_BIG_ENDIAN
 #define MouseAttrMask 0x007F;
#else
 #define MouseAttrMask 0x7F00;
#endif

static int    last_x=0,last_y=0;
static ushort mouse_char;

static
void get_mouse_char()
{
 mouse_char=TScreen::getCharacter(last_x+last_y*TScreen::screenWidth);
}

static
void set_mouse_char()
{
 ushort c=mouse_char ^ MouseAttrMask;
 TScreen::setCharacter(last_x+last_y*TScreen::screenWidth,c);
}

static
void reset_mouse_char()
{
 TScreen::setCharacter(last_x+last_y*TScreen::screenWidth,mouse_char);
}

static volatile
void show_mouse_char()
{
 if (last_x>TScreen::screenWidth)
   last_x=0;
 if (last_y>TScreen::screenHeight)
   last_y=0;
 get_mouse_char();
 set_mouse_char();
}

int THWMouseUNIX::DrawMouse(int x, int y)
{
 if (TScreen::suspended) return 0;
 if (x!=last_x || y!=last_y)
   {
    if (visible) reset_mouse_char();
    last_x=x;
    last_y=y;
    if (visible) show_mouse_char();
    return 1;
   }
 return 0;
}

void THWMouseUNIX::Show()
{
 visible=1;
 show_mouse_char();
}

void THWMouseUNIX::Hide()
{
 reset_mouse_char();
 visible=0;
}

void THWMouseUNIX::Init()
{
 THWMouse::Show=Show;
 THWMouse::Hide=Hide;
 THWMouse::drawMouse=DrawMouse;
}

THWMouseUNIX::~THWMouseUNIX()
{
}

#ifdef HAVE_NCURSES
//---------------- XTerm version -----------------

void THWMouseXTerm::Resume()
{
 buttonCount=3; // X uses at least 3 buttons
 // Save old hilit tracking and enable mouse tracking (GPM_XTERM_ON)
 TScreenUNIX::SendToTerminal("\x1B[?1000s\x1B[?1000h");
 LOG("using xterm* mouse");
 show();
}

void THWMouseXTerm::Suspend()
{
 // Disable mouse tracking and restore old hilittracking (GPM_XTERM_OFF)
 TScreenUNIX::SendToTerminal("\x1B[?1000l\x1B[?1000r");
 LOG("xterm mouse disabled");
}

void THWMouseXTerm::Init()
{
 THWMouseUNIX::Init();
 THWMouse::Resume=Resume;
 THWMouse::Suspend=Suspend;
}

THWMouseXTerm::~THWMouseXTerm()
{
}
#endif // HAVE_NCURSES
#else // TVOS_UNIX && !TVOSf_QNXRtP

#include <tv/unix/screen.h>
#include <tv/unix/mouse.h>

#endif // else TVOS_UNIX && !TVOSf_QNXRtP

