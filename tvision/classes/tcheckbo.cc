/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added TCheckBoxes32 and code page stuff by Salvador Eduardo Tropea.

 *
 *
 */

#define Uses_TCheckBoxes
#include <tv.h>

void TCheckBoxes::draw()
{
    drawBox( button, 'X' );
}

Boolean TCheckBoxes::mark(int item)
{
    return Boolean( (value & (1 <<  item)) != 0 );
}

void TCheckBoxes::press(int item)
{
    value = value^(1 << item);
    TCluster::press(item);
}

uint32 TCheckBoxes32::dataSize()
{
    return 4;
}

#if !defined( NO_STREAM )

TStreamable *TCheckBoxes::build()
{
    return new TCheckBoxes( streamableInit );
}

TCheckBoxes::TCheckBoxes( StreamableInit ) : TCluster( streamableInit )
{
}

TStreamable *TCheckBoxes32::build()
{
    return new TCheckBoxes32( streamableInit );
}

TCheckBoxes32::TCheckBoxes32( StreamableInit ) : TCheckBoxes( streamableInit )
{
}

#endif // NO_STREAM
