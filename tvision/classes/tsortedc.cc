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

#define Uses_opstream
#define Uses_ipstream
#define Uses_TSortedCollection
#include <tv.h>

#if !defined( NO_STREAM )
void TSortedCollection::write( opstream& os )
{
    TCollection::write( os );
    os << (int)duplicates;
}

void *TSortedCollection::read( ipstream& is )
{
    TCollection::read( is );
    int temp;
    is >> temp;
    duplicates = Boolean(temp);
    return this;
}

TSortedCollection::TSortedCollection( StreamableInit ) :
    TCollection( streamableInit )
{
}
#endif // NO_STREAM

