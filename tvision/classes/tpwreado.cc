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

#define Uses_TPReadObjects
#include <tv.h>

#include <assert.h>

TPReadObjects::TPReadObjects() : TNSCollection( 5, 5 ), curId( 0 )
{
  shouldDelete = False;
}

TPReadObjects::~TPReadObjects()
{
}

void TPReadObjects::registerObject( const void *adr )
{
    ccIndex loc = insert( (void *)adr );
    assert( (unsigned)loc == curId);   // to be sure that TNSCollection
                                // continues to work the way
                                // it does now...
    curId++;  // Increment moved out of assertion check...
}

const void *TPReadObjects::find( P_id_type id )
{
    return at( id );
}

