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

#define Uses_TEvent
#include <tv.h>

TMouse::TMouse()
{
    show();
}

TMouse::~TMouse()
{
    hide();
}


