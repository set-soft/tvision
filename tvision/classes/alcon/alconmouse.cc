// -*- mode:C++; tab-width: 4 -*-
#include <tv/configtv.h>

#define Uses_stdio

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/mouse.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>


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
 // I get these printfs, but TVision ignores me!
 if (buttons & 1 << 3)
    printf("Mouse up\n");
 if (buttons & 1 << 4)
    printf("Mouse down\n");
}

