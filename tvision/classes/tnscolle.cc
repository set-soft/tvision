/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea (atReplace and other stuff)
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string
#define Uses_abort
#define Uses_TNSCollection
#include <tv.h>

TNSCollection::TNSCollection( ccIndex aLimit, ccIndex aDelta ) :
    items( 0 ),
    count( 0 ),
    limit( 0 ),
    delta( aDelta ),
    shouldDelete( True )
{
    setLimit( aLimit );
}

TNSCollection::TNSCollection() :
    items( 0 ),
    count( 0 ),
    limit( 0 ),
    delta( 0 ),
    shouldDelete( True )
{
    items = 0;
}

TNSCollection::~TNSCollection()
{
    delete[] items;
}

void TNSCollection::shutDown()
{
    if( shouldDelete )
        freeAll();
    else
        count = 0;
    setLimit(0);
    TObject::shutDown();
}

void TNSCollection::atRemove( ccIndex index )
{
    if( index < 0 || index >= count )
        error(1,0);

    count--;
    CLY_memcpy( &items[index], &items[index+1], (count-index)*sizeof(void *) );
}

void TNSCollection::atFree( ccIndex index )
{
    void *item = at( index ); // It could be: (*this)[index];
    atRemove( index );
    freeItem( item );
}

void TNSCollection::atInsert(ccIndex index, void *item)
{
    if( index < 0 )
        error(1,0);
    if( count == limit )
        setLimit(count + delta);

    memmove( &items[index+1], &items[index], (count-index)*sizeof(void *) );
    count++;

    items[index] = item;
}

void TNSCollection::atPut( ccIndex index, void *item )
{
    if( index >= count )
        error(1,0);

    items[index] = item;
}

// by SET
void TNSCollection::atReplace( ccIndex index, void *item )
{
    freeItem( at( index ) );
    atPut( index, item );
}

void TNSCollection::remove( void *item )
{
    atRemove( indexOf(item) );
}

void TNSCollection::removeAll()
{
    count = 0;
}

void TNSCollection::error( ccIndex /* code */, ccIndex )
{
#if 0
    exit(212 - code);
#else
    abort();
#endif
}

void *TNSCollection::firstThat( ccTestFunc Test, void *arg )
{
    for( ccIndex i = 0; i < count; i++ )
        {
        if( Test( items[i], arg ) == True )
            return items[i];
        }
    return 0;
}

void *TNSCollection::lastThat( ccTestFunc Test, void *arg )
{
    for( ccIndex i = count; i > 0; i-- )
        {
        if( Test( items[i-1], arg ) == True )
            return items[i-1];
        }
    return 0;
}

void TNSCollection::forEach( ccAppFunc action, void *arg )
{
    for( ccIndex i = 0; i < count; i++ )
        action( items[i], arg );
}

// Avoid replacing free by MSS's macro
#include <tv/no_mss.h>

void TNSCollection::free( void *item )
{
    remove( item );
    freeItem( item );
}

#include <tv/yes_mss.h>

void TNSCollection::freeAll()
{
    for( ccIndex i =  0; i < count; i++ )
        freeItem( at(i) );
    count = 0;
}

void TNSCollection::freeItem( void *item )
{
    delete[] (char *)item;
}

ccIndex TNSCollection::indexOf(void *item)
{
    for( ccIndex i = 0; i < count; i++ )
        if( item == items[i] )
            return i;

    error(1,0);
    return -1;
}

ccIndex TNSCollection::insert( void *item )
{
    ccIndex loc = count;
    atInsert( count, item );
    return loc;
}

void TNSCollection::pack()
{
    if (count == 0) return;
    void **curDst = items;
    void **curSrc = items;
    void **last = items + count;
    while( curSrc < last )
        {
        if( *curSrc != 0 )
            *curDst++ = *curSrc;
        curSrc++;
        }
}

void TNSCollection::setLimit(ccIndex aLimit)
{
    if( aLimit < count )
        aLimit =  count;
    if( aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if( aLimit != limit )
        {
        void **aItems;
        if (aLimit == 0 )
            aItems = 0;
        else
            {
            aItems = new void *[aLimit];
            if( count !=  0 && items )
                memcpy( aItems, items, count*sizeof(void *) );
            }
        if (items)
           delete[] items;
        items =  aItems;
        limit =  aLimit;
        }
}

/**[txh]********************************************************************

  Description:
  Just like at() but not inline and easier to call in some cases.

***************************************************************************/

void *TNSCollection::operator[](ccIndex i)
{
 if (i<0 || i>=count)
    error(1,0);
 return items[i];
}
