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
}

void TRadioButtons::movedTo( int item )
{
    value = item;
}

void TRadioButtons::setData( void * rec )
{
    TCluster::setData(rec);
    sel = value;
}

#if !defined( NO_STREAM )
TStreamable *TRadioButtons::build()
{
    return new TRadioButtons( streamableInit );
}

TRadioButtons::TRadioButtons( StreamableInit ) : TCluster( streamableInit )
{
}
#endif // NO_STREAM

