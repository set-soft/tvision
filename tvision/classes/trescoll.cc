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

#define Uses_TResourceCollection
#define Uses_TResourceItem
#include <tv.h>

TResourceCollection::TResourceCollection( ccIndex aLimit, ccIndex aDelta) :
    TStringCollection( aLimit, aDelta)
{
}

void TResourceCollection::freeItem( void* item )
{
    delete ((TResourceItem*)item)->key; 
    delete((TResourceItem*)item);
}

void* TResourceCollection::keyOf( void* item ) 
{
    return ((TResourceItem *)item)->key;
} 

#if !defined( NO_STREAM )
TStreamable *TResourceCollection::build()
{
    return new TResourceCollection( streamableInit );
}

void TResourceCollection::writeItem( void *obj, opstream& os )
{
   
   os << ((TResourceItem *)obj)->pos;
   os << ((TResourceItem *)obj)->size;
   os.writeString(((TResourceItem *)obj)->key);

}

void *TResourceCollection::readItem( ipstream& is )
{
    TResourceItem *obj;

    obj = new TResourceItem;
    is >> obj->pos;
    is >> obj->size;
    obj->key = is.readString();
    return (void *)obj;
}
#endif // NO_STREAM

