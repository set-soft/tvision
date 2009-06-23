/*****************************************************************************

  Copyright (c) 2003-2004 by Grzegorz Adam Hankiewicz
  Copyright (c) 2004-2009 by Salvador E. Tropea (SET) <set@ieee.org>
  
  THWMouseAlcon
  AlCon driver for Turbo Vision. It uses the conio.h emulation library on top
  of Allegro.

  Contributed by Grzegorz Adam Hankiewicz <gradha@hankiewicz.datanet.co.uk>

  This is part of Turbo Vision ditribution and covered by the same license.

*****************************************************************************/

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/mouse.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>

#ifdef HAVE_ALLEGRO

void THWMouseAlcon::Init()
{
 THWMouse::GetEvent=GetEvent;
 buttonCount=AlCon_GetMouseButtons();
}

THWMouseAlcon::~THWMouseAlcon()
{
}

void THWMouseAlcon::GetEvent(MouseEventType &me)
{
 int buttons;
 AlCon_GetMousePos(&me.where.x, &me.where.y, &buttons);
 me.buttons = uchar(buttons & 255);
 me.doubleClick=False;
}
#endif // HAVE_ALLEGRO
