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

#define Uses_TDirCollection
#define Uses_TDirEntry
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( NO_STREAM )
TStreamable *TDirCollection::build()
{
    return new TDirCollection( streamableInit );
}

void TDirCollection::writeItem( void *obj, opstream& os )
{
    TDirEntry *item = (TDirEntry *)obj;
    os.writeString( item->text() );
    os.writeString( item->dir() );
}

void *TDirCollection::readItem( ipstream& is )
{
    const char *txt = is.readString();
    const char *dir = is.readString();
    return new TDirEntry( txt, dir );
}
#endif // NO_STREAM

