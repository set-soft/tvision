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

#define Uses_TKeys
#define Uses_TColorItem
#define Uses_TColorItemList
#define Uses_TEvent
#define Uses_TGroup
#define Uses_TRect
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#include <string.h>

TColorItem::TColorItem( const char *nm, uchar idx, TColorItem *nxt )
{
    index = idx;
    next = nxt;
    name = newStr( nm );
}

TColorItem::~TColorItem()
{
    delete (char *)name;
}

TColorItem& operator + ( TColorItem& i1, TColorItem& i2 )
{
    TColorItem *cur = &i1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &i2;
    return i1;
}

TColorItemList::TColorItemList( const TRect& bounds,
                                TScrollBar *aScrollBar,
                                TColorItem *aItems
                              ) :
    TListViewer( bounds, 1, 0, aScrollBar ),
    items( aItems )
{
    eventMask |= evBroadcast;
    int i = 0;
    while( aItems != 0 )
        {
        aItems = aItems->next;
        i++;
        }
    setRange( i );
}

void TColorItemList::focusItem( ccIndex item )
{
    TListViewer::focusItem( item );
    TColorItem *curItem = items;
    while( item-- > 0 )
        curItem = curItem->next;
    message( owner, evBroadcast, cmNewColorIndex, (void *)(curItem->index));
}

void TColorItemList::getText( char *dest, ccIndex item, short maxChars )
{
	TColorItem *curItem = items;
	while( item-- > 0 )
		curItem = curItem->next;
	strncpy( dest, curItem->name, maxChars );
	dest[maxChars] = '\0';
}

void TColorItemList::handleEvent( TEvent& event )
{
    TListViewer::handleEvent( event );
    if( event.what == evBroadcast && event.message.command == cmNewColorItem )
        {
        items = (TColorItem *)event.message.infoPtr;
        TColorItem *curItem = items;
        int i = 0;
        while( curItem != 0 )
            {
            curItem = curItem->next;
            i++;
            }
        setRange( i );
        focusItem( 0 );
        drawView();
        }
}

#if !defined( NO_STREAM )
TStreamable *TColorItemList::build()
{
    return new TColorItemList( streamableInit );
}

TColorItemList::TColorItemList( StreamableInit ) :
    TListViewer( streamableInit )
{
}

#endif // NO_STREAM

