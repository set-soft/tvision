/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 
Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console 
 *
 *
 */

#ifdef _WIN32

#include <stdlib.h>
#define Uses_TEvent
#define Uses_TEventQueue
#include <tv.h>

uchar THWMouse::buttonCount = 2;
Boolean THWMouse::handlerInstalled = False;
Boolean THWMouse::noMouse = False;



void THWMouse::registerHandler( unsigned mask, void (*func)() )
{
}

void THWMouse::resume()
{
}

int use_mouse_handler = 1;

THWMouse::THWMouse()
{
	resume();
}

THWMouse::~THWMouse()
{
	suspend();
}

void THWMouse::suspend()
{
}

void THWMouse::show()
{
}

void THWMouse::hide()
{
}

void THWMouse::setRange( ushort rx, ushort ry )
{
}


/**[txh]********************************************************************
 
  Description:
  It forces the state of the mouse externally, the next call to getEvent
will return this values instead of values from the mouse driver. That's
used to simuate the mouse with other events like keyboard. (SET)
 
***************************************************************************/

static int m_x=0,m_y=0,m_b=0;
static int forced=0;

void THWMouse::forceEvent(int x, int y, int buttons)
{
	m_x=x;
	m_y=y;
	m_b=buttons;
	forced=1;
}

extern int getConsoleMouseEvent( MouseEventType& me );

void THWMouse::getEvent( MouseEventType& me )
{
	if (forced) {
		forced=0;
		me.where.x = m_x;
		me.where.y = m_y;
		me.doubleClick = False;
		me.buttons = m_b;
		TEventQueue::curMouse = me;
	} else {
		getConsoleMouseEvent(TEventQueue::curMouse);
		me = TEventQueue::curMouse;
	}
}

#endif // _WIN32

