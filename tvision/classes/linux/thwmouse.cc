/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __linux__

#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>
#include <time.h>
#include <stdio.h>
#include <linux/keyboard.h>

extern "C" {
#include <gpm.h>
}

#ifdef DEBUG
#ifdef __linux__
extern char *program_invocation_short_name;
#define LOG(s) cerr << program_invocation_short_name << ": " << s << "\n"
#else
#define LOG(s) cerr << __FILE__": " << s << "\n"
#endif
#else
#define LOG(s)
#endif

uchar THWMouse::buttonCount = 0;
Boolean THWMouse::handlerInstalled = False;
Boolean THWMouse::noMouse = False;

static int last_x=0,last_y=0,visible=0;
static unsigned short mouse_char;
static int my_buttonCount=0;

extern int TScreen_suspended;

static
void get_mouse_char()
{
  mouse_char = TScreen::getCharacter(last_x+last_y*TScreen::screenWidth);
}

static
void set_mouse_char()
{
  unsigned short c = mouse_char ^ 0x7F00;
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
  if (!visible) return;
  if (last_x > TScreen::screenWidth)
    last_x = 0;
  if (last_y > TScreen::screenHeight)
    last_y = 0;
  get_mouse_char();
  set_mouse_char();
}

int draw_mouse(int x,int y)
{
  if (TScreen_suspended) return 0;
  if (x != last_x || y != last_y)
  {
    if (visible) reset_mouse_char();
    last_x = x;
    last_y = y;
    show_mouse_char();
    return 1;
  }
  return 0;
}   

void THWMouse::registerHandler( unsigned , void (*)() )
{
}

static
int SetUpGPMConnection()
{
 Gpm_Connect conn;
 
 conn.eventMask = ~0;	/* I want all events */
 conn.defaultMask = 0;	/* no default treatment */
 /* It doesn't report events from GPM when ALT or ALTGR are pressed.
    I saw it in mc and my friends from BALUG, Ivan and Laszlo bugged me
    about adding it. Was hyper-easy, no? (SET) */
 conn.maxMod = ~((1<<KG_ALT) | (1<<KG_ALTGR));
 conn.minMod = 0;
 gpm_zerobased = 1;	/* coordinates start from zero */
 
 return Gpm_Open(&conn, 0);
}

void THWMouse::resume()
{
  if (gpm_fd != -1)
    return;
  if (SetUpGPMConnection() < 0)
  {
    LOG("no gpm, running without mouse");
  }
  else
  {
    LOG("gpm server version " << Gpm_GetServerVersion(NULL));
    buttonCount = my_buttonCount;
  }
  show();
}

int use_mouse_handler = 1;

THWMouse::THWMouse()
{
  if (SetUpGPMConnection() < 0)
  {
    LOG("no gpm, running without mouse");
  }
  else
  {
    LOG("gpm server version " << Gpm_GetServerVersion(NULL));
    my_buttonCount = buttonCount = 2;
  }
  show();
}

THWMouse::~THWMouse()
{
  suspend();
}

void THWMouse::suspend()
{
  if (gpm_fd == -1) return;
  hide();
  my_buttonCount = buttonCount;
  buttonCount = 0;
  Gpm_Close();
}

void THWMouse::show()
{
  if (!present()) return;
  if (visible) return;
  visible = 1;
  show_mouse_char();
}

void THWMouse::hide()
{
  if (!present()) return;
  if (!visible) return;
  reset_mouse_char();
  visible = 0;
}

void THWMouse::setRange( ushort , ushort )
{
}

inline int range(int test, int min, int max)
{
	return test < min ? min : test > max ? max : test;
}

static int m_x=0,m_y=0,m_b=0;

void THWMouse::getEvent( MouseEventType &me )
{
  Gpm_Event mev;
  me.where.x = m_x;
  me.where.y = m_y;
  me.doubleClick = False;
  me.buttons = m_b;
  if (!Gpm_Repeat(1) && (Gpm_GetEvent(&mev) == 1))
  {
    int b = mev.buttons;
    if ((b & GPM_B_LEFT) && !(mev.type & GPM_UP))
      me.buttons |= mbLeftButton;
    else
      me.buttons &= ~mbLeftButton;
    if ((b & GPM_B_RIGHT) && !(mev.type & GPM_UP))
      me.buttons |= mbRightButton;
    else
      me.buttons &= ~mbRightButton;
    m_b = me.buttons;
    m_x = me.where.x = range(mev.x, 0, TScreen::screenWidth - 1);
    m_y = me.where.y = range(mev.y, 0, TScreen::screenHeight - 1);
    draw_mouse(m_x,m_y);
  }
  // curMouse must be set, because it is used by other functions
  TEventQueue::curMouse = me;
}

#endif // __linux__
