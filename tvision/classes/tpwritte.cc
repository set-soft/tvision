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

#define Uses_TPWrittenObjects
#define Uses_TStreamableClass
#include <tv.h>

TPWrittenObjects::TPWrittenObjects() : TNSSortedCollection( 5, 5 ), curId( 0 )
{
  // SET: Why this? it just leaks memory, anybody knows?
  //shouldDelete = False;
}

TPWrittenObjects::~TPWrittenObjects()
{
}

void TPWrittenObjects::registerObject( const void *adr )
{
    TPWObj *o = new TPWObj( adr, curId++ );
    insert( o );
}

void TPWrittenObjects::freeItem( void *item )
{
    delete (TPWObj *)item;
}

P_id_type TPWrittenObjects::find( const void *d )
{
    ccIndex loc;
    if( search( (void *)d, loc ) )
        return ((TPWObj *)at( loc ))->ident;
    else
        return P_id_notFound;
}

void *TPWrittenObjects::keyOf( void *d )
{
    return (void *)((TPWObj *)d)->address;
}

int TPWrittenObjects::compare( void *o1, void *o2 )
{
    if( o1 == o2 )
        return 0;
    else if( ((char *)o1)+1 < ((char *)o2)+1 ) // force normalization
        return -1;
    else
        return 1;
}

