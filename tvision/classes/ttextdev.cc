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

#include <tv/configtv.h>

#define Uses_TTextDevice
#include <tv.h>

TTextDevice::TTextDevice( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar) :
    TScroller(bounds,aHScrollBar,aVScrollBar)
{
}

int TTextDevice::overflow( int c )
{
    if( c != EOF )
        {
        char b = c;
        do_sputn( &b, 1 );
        }
    return 1;
}

