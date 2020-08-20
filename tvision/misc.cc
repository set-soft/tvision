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

#define Uses_string
#define Uses_ctype
#define Uses_TView
#define Uses_TEvent
#define Uses_TObject
#include <tv.h>

void *message( TView *receiver, ushort what, ushort command, void *infoPtr)
{
    if( receiver == 0 )
        return 0;

    TEvent event;
    event.what = what;
    event.message.command = command;
    event.message.infoPtr = infoPtr;
    receiver->handleEvent( event );
    if( event.what == evNothing )
        return event.message.infoPtr;
    else
        return 0;
}

Boolean lowMemory()
{
#if 0
    return Boolean(TVMemMgr::safetyPoolExhausted());
#else
    return False;
#endif
}

char hotKey( const char *s )
{
    const char *p;

    if( (p = strchr( s, '~' )) != 0 )
        return uctoupper(p[1]);
    else
        return 0;
}

