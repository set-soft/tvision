/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added TRadioButtons32 and code page stuff by Salvador Eduardo Tropea.

 *
 *
 */

#define Uses_TRadioButtons
#define Uses_TStreamableClass
#include <tv.h>

void TRadioButtons::draw()
{
    drawBox( button, check );
}

Boolean TRadioButtons::mark( int item )
{
    return Boolean( (uint32)item == value );
}

void TRadioButtons::press( int item )
{
    value = item;
    TCluster::press(item);
    evaluateMasks();
}

void TRadioButtons::movedTo( int item )
{
    value = item;
    TCluster::movedTo(item);
    evaluateMasks();
}

void TRadioButtons::setData( void * rec )
{
    TCluster::setData(rec);
    sel = value;
    evaluateMasks();
}

void TRadioButtons::evaluateMasks()
{
    if( !enableMasks )
        return;
    int i;
    unsigned mask, theMask;

    theMask = enableMasks[value];
    for( i = 0, mask = 1; i < enableCViews; mask <<= 1, i++ )
        {
        TView *view = enableViews[i];
        if( theMask & mask )
           {// Enable this view
           if( view->state & sfDisabled )
              view->setState( sfDisabled, False );
           }
        else
           {// Disable this view
           if( !( view->state & sfDisabled ) )
              view->setState( sfDisabled, True );
           }
        }
}

uint32 TRadioButtons32::dataSize()
{
    return 4;
}

#if !defined( NO_STREAM )
TStreamable *TRadioButtons::build()
{
    return new TRadioButtons( streamableInit );
}

TRadioButtons::TRadioButtons( StreamableInit ) : TCluster( streamableInit )
{
}

TStreamable *TRadioButtons32::build()
{
    return new TRadioButtons32( streamableInit );
}

TRadioButtons32::TRadioButtons32( StreamableInit ) : TRadioButtons( streamableInit )
{
}
#endif // NO_STREAM

