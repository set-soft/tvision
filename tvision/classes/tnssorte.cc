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

#define Uses_TNSSortedCollection
#include <tv.h>


ccIndex TNSSortedCollection::indexOf(void *item)
{
    ccIndex  i;

    if( search( keyOf(item), i ) == 0 )
        return ccNotFound;
    else
        {
        if( duplicates )
            {
            while( i < count && item != items[i] )
                i++;
            }
        if( i < count )
            return i;
        else
            return ccNotFound;
        }
}

ccIndex TNSSortedCollection::insert( void *item )
{
    ccIndex  i;
    if( search( keyOf(item), i ) == 0 || duplicates )   // order dependency!
        atInsert( i, item );                            // must do Search
                                                        // before calling
                                                        // AtInsert
    return i;
}

void *TNSSortedCollection::keyOf( void *item )
{
    return item;
}

Boolean TNSSortedCollection::search( void *key, ccIndex& index )
{
    ccIndex l = 0;
    ccIndex h = count - 1;
    Boolean res = False;
    if ( !key ) return False;
    while( l <= h )
        {
        ccIndex i = (l +  h) >> 1;
        ccIndex c = compare( keyOf( items[i] ), key );
        if( c < 0 )
            l = i + 1;
        else
            {
            h = i - 1;
            if( c == 0 )
                {
                res = True;
                if( !duplicates )
                    l = i;
                }
            }
        }
    index = l;
    return res;
}

