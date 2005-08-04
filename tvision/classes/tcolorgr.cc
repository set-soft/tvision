/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea for i18n support.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string

#define Uses_TKeys
#define Uses_TColorItem
#define Uses_TColorGroup
#define Uses_TColorGroupList
#define Uses_TGroup
#define Uses_TRect
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

TColorGroup::TColorGroup( const char *nm, TColorItem *itm, TColorGroup *nxt )
{
    items = itm;
    next = nxt;
    name = newStr( nm );
    cacheName = NULL;
}

TColorGroup::~TColorGroup()
{ 
    DeleteArray((char *)name);
    TVIntl::freeSt(cacheName);
}

TColorGroup& operator + ( TColorGroup& g, TColorItem& i )
{
    TColorGroup *grp = &g;
    while( grp->next != 0 )
        grp = grp->next;

    if( grp->items == 0 )
        grp->items = &i;
    else
        {
        TColorItem *cur = grp->items;
        while( cur->next != 0 )
            cur = cur->next;
        cur->next = &i;
        }
    return g;
}

TColorGroup& operator + ( TColorGroup& g1, TColorGroup& g2 )
{
    TColorGroup *cur = &g1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &g2;
    return g1;
}


TColorGroupList::TColorGroupList( const TRect& bounds,
                                  TScrollBar *aScrollBar,
                                  TColorGroup *aGroups
                                ) :
    TListViewer( bounds, 1, 0, aScrollBar ),
    groups( aGroups )
{
    int i = 0;
    while( aGroups != 0 )
        {
        aGroups = aGroups->next;
        i++;
        }
    setRange(i);
}

static void freeItems( TColorItem *curItem )
{
    while( curItem != 0 )
        {
        TColorItem *p = curItem;
        curItem = curItem->next;
        delete(p);
        }
}

static void freeGroups( TColorGroup *curGroup )
{
    while( curGroup != 0 )
        {
        TColorGroup *p = curGroup;
        freeItems( curGroup->items );
        curGroup = curGroup->next;
        delete(p);
        }
}

TColorGroupList::~TColorGroupList()
{
    freeGroups( groups );
}

void TColorGroupList::focusItem( ccIndex item )
{
    TListViewer::focusItem( item );
    TColorGroup *curGroup = groups;
    while( item-- > 0 )
        curGroup = curGroup->next;
    message( owner, evBroadcast, cmNewColorItem, curGroup->items);
}

void TColorGroupList::getText( char *dest, ccIndex item, short maxChars )
{
	TColorGroup *curGroup = groups;
	while( item-- > 0 )
		curGroup = curGroup->next;
    strncpy( dest, TVIntl::getText(curGroup->name,curGroup->cacheName), maxChars );
    dest[maxChars] = '\0';
}

#if !defined( NO_STREAM )
void TColorGroupList::writeItems( opstream& os, TColorItem *items )
{
    short count = 0;
    TColorItem *cur;

    for( cur = items; cur != 0; cur = cur->next )
	count++;

    os << count;

    for( cur = items; cur != 0; cur = cur->next )
	{
	os.writeString( cur->name );
	os << cur->index;
	}
}

void TColorGroupList::writeGroups( opstream& os, TColorGroup *groups )
{
    short count = 0;
    TColorGroup *cur;

    for( cur = groups; cur != 0; cur = cur->next )
	count++;

    os << count;

    for( cur = groups; cur != 0; cur = cur->next )
	{
	os.writeString( cur->name );
	writeItems( os, cur->items );
	}
}

void TColorGroupList::write( opstream& os )
{
    TListViewer::write( os );
    writeGroups( os, groups );
}

TColorItem *TColorGroupList::readItems( ipstream& is )
{
    short count;
    is >> count;
    TColorItem *items = 0;
    TColorItem **cur = &items;
    while( count-- > 0 )
	{
	const char *nm = is.readString();
	uchar index;
	is >> index;
	*cur = new TColorItem( nm, index );
	cur = &((*cur)->next);
	}
    *cur = 0;
    return items;
}

TColorGroup *TColorGroupList::readGroups( ipstream& is )
{
    short count;
    is >> count;
    TColorGroup *groups = 0;
    TColorGroup **cur = &groups;
    while( count-- > 0 )
	{
	const char *nm = is.readString();
	TColorItem *grp = readItems( is );
	*cur = new TColorGroup( nm, grp );
	cur = &((*cur)->next);
	}
    *cur = 0;
    return groups;
}

void *TColorGroupList::read( ipstream& is )
{
    TListViewer::read( is );
    groups = readGroups( is );
    return this;
}

TStreamable *TColorGroupList::build()
{
    return new TColorGroupList( streamableInit );
}

TColorGroupList::TColorGroupList( StreamableInit ) :
    TListViewer( streamableInit )
{
}

#endif // NO_STREAM

