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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string

#define Uses_TMenuBar
#define Uses_TDrawBuffer
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TSubMenu
#include <tv.h>

TMenuBar::TMenuBar( const TRect& bounds, TMenu *aMenu ) :
    TMenuView( bounds )
{
    menu = aMenu;
    growMode = gfGrowHiX;
    options |= ofPreProcess;
    computeLength();
}

TMenuBar::TMenuBar( const TRect& bounds, TSubMenu& aMenu ) :
    TMenuView( bounds )
{
    menu = new TMenu( aMenu );
    growMode = gfGrowHiX;
    options |= ofPreProcess;
    computeLength();
}


TMenuBar::~TMenuBar()
{
    delete(menu);
}


/**[txh]********************************************************************

  Description:
  This routine computes the length of the menu bar, if that's greater than
the size.x the menu becomes compacted to allow more options.@*
  Added by SET.

***************************************************************************/

void TMenuBar::computeLength()
{
    int l = 0;
    TMenuItem *p;

    if( menu != 0 )
        {
        p = menu->items;
        while( p != 0 )
            {
            if( p->name != 0 )
                l += cstrlen(p->name) + 2;
            p = p->next;
            }
        }
    compactMenu = l > size.x;
}

/**[txh]********************************************************************

  Description:
  Calls TMenuView::changeBounds, additionally re-computes the length of the
bar to select the no/compact mode.@*
  Added by SET.

***************************************************************************/

void TMenuBar::changeBounds(const TRect& bounds)
{
 TMenuView::changeBounds(bounds);
 int oldCompact=compactMenu;
 computeLength();
 if (compactMenu!=oldCompact)
    draw();
}

void TMenuBar::draw()
{
    ushort color;
    int x, l, inc;
    TMenuItem *p;
    TDrawBuffer b;

    ushort cNormal = getColor(0x0301);
    ushort cSelect = getColor(0x0604);
    ushort cNormDisabled =  getColor(0x0202);
    ushort cSelDisabled =  getColor(0x0505);
    b.moveChar( 0, ' ', cNormal, size.x );
    inc = (compactMenu ? 1 : 2); // SET
    if( menu != 0 )
        {
        x = 0;
        p = menu->items;
        while( p != 0 )
            {
            if( p->name != 0 )
                {
                l = cstrlen(p->name);
                if( x + l < size.x )
                    {
                    if( p->disabled )
                        if( p == current )
                            color = cSelDisabled;
                        else
                            color = cNormDisabled;
                    else
                        if( p == current )
                            color = cSelect;
                        else
                            color = cNormal;

                    b.moveChar( x, ' ', color, 1 );
                    b.moveCStr( x+1, p->name, color );
                    b.moveChar( x+l+1, ' ', color, 1 );
                    }
                x += l + inc;
                }
            p = p->next;
            }
        }
    writeBuf( 0, 0, size.x, 1, b );
}

TRect TMenuBar::getItemRect( TMenuItem *item )
{
    int y = compactMenu ? 1 : 0; // SET
    TRect r( y, 0, y, 1 );
    y = compactMenu ? 1 : 2; // SET
    TMenuItem *p = menu->items;
    while( True )
        {
        r.a.x = r.b.x;
        if( p->name != 0 )
            r.b.x += cstrlen(p->name) + y;
        if( p == item )
            return r;
        p = p->next;
        }
    //return TRect(0, 0, 0, 0); // SAA: should not ever happen
}

#if !defined( NO_STREAM )
TStreamable *TMenuBar::build()
{
    return new TMenuBar( streamableInit );
}

TMenuBar::TMenuBar( StreamableInit ) : TMenuView( streamableInit )
{
}
#endif // NO_STREAM


