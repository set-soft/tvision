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
#define Uses_TCollection
#include <tv.h>

#if !defined( NO_STREAM )

void TCollection::write( opstream& os )
{
    os << count << limit << delta;
    for( ccIndex idx = 0; idx < count; idx++ )
        writeItem( items[idx], os );
}

void *TCollection::read( ipstream& is )
{
    int limit;
    is >> count >> limit >> delta;
    limit = 0;
    setLimit(limit);
    for( ccIndex idx = 0; idx < count; idx++ )
        items[idx] = readItem( is );
    return this;
}

TCollection::TCollection( StreamableInit )
{
}

#endif // NO_STREAM

