/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea. [also added i18n support]

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string

#define Uses_TRect
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TMenuBox
#define Uses_TScreen // getShowCursorEver()
#include <tv.h>

static TRect getRect( const TRect& bounds, TMenu *aMenu )
{
    short w =  10;
    short h =  2;
    if( aMenu != 0 )
        {
        for( TMenuItem *p = aMenu->items; p != 0; p = p->next )
            {
            if( p->name != 0 )
                {
                short l = cstrlen(TVIntl::getText(p->name,p->intlName)) + 6;
                if( p->command == 0 )
                    l += 3;
                else
                    if( p->param != 0 )
                        l += cstrlen(p->param) + 2;
                w = max( l, w );
                }
            h++;
            }
        }

    TRect r( bounds );

    if( r.a.x + w < r.b.x )
        r.b.x = r.a.x + w;
    else
        r.a.x = r.b.x - w;

    if (r.a.y + h < r.b.y)
        r.b.y = r.a.y + h;
    else
        r.a.y = r.b.y - h;
    return r;
}

TMenuBox::TMenuBox( const TRect& bounds,
                    TMenu *aMenu,
                    TMenuView *aParentMenu) :
    TMenuView( getRect( bounds, aMenu ), aMenu, aParentMenu )
{
    state |= sfShadow;
    // This class can be "Braille friendly"
    if (TScreen::getShowCursorEver())
       state |= sfCursorVis;
    options |= ofPreProcess;
}

static ushort cNormal, color;

void TMenuBox::frameLine( TDrawBuffer& b, short n )
{
    b.moveBuf( 0, &frameChars[n], cNormal, 2 );
    b.moveChar( 2, frameChars[n+2], color, size.x - 4 );
    b.moveBuf( size.x-2, &frameChars[n+3], cNormal, 2 );
}

void TMenuBox::draw()
{
    TDrawBuffer    b;

    cNormal = getColor(0x0301);
    ushort cSelect = getColor(0x0604);
    ushort cNormDisabled = getColor(0x0202);
    ushort cSelDisabled = getColor(0x0505);
    int y=0, yCur=-1;
    color =  cNormal;
    frameLine( b, 0 );
    writeBuf( 0, y++, size.x, 1, b );
    if( menu != 0 )
        {
        for( TMenuItem *p = menu->items; p != 0; p = p->next )
            {
            color = cNormal;
            if( p->name == 0 )
                frameLine( b, 15 );
            else
                {
                if( p->disabled )
                    if( p ==  current )
                        {
                        color = cSelDisabled;
                        yCur = y;
                        }
                    else
                        color = cNormDisabled;
                else if( p == current )
                    {
                    color = cSelect;
                    yCur = y;
                    }
                frameLine( b, 10 );
                b.moveCStr( 3, TVIntl::getText(p->name,p->intlName), color );
                if( p->command == 0 )
                    b.putChar( size.x-4, rightArrow );
                else if( p->param != 0 )
                    b.moveStr( size.x-3-strlen(p->param),
                               p->param,
                               color);
                }
            writeBuf( 0, y++, size.x, 1, b );
            }
        }
    color = cNormal;
    frameLine( b, 5 );
    writeBuf( 0, y++, size.x, 1, b );
    // SET: Force a cursor movement to indicate which one is selected.
    // This helps Braille Terminals, but the cursor must be visible!
    if( yCur != -1 )
        {
        setCursor( 2 , yCur );
        resetCursor();
        }
}

TRect TMenuBox::getItemRect( TMenuItem *item )
{
    short  y = 1;
    TMenuItem *p = menu->items;

    while( p != item )
        {
        y++;
        p =  p->next;
        }
    return TRect( 2, y, size.x-2, y+1 );
}

#if !defined( NO_STREAM )
TStreamable *TMenuBox::build()
{
    return new TMenuBox( streamableInit );
}

TMenuBox::TMenuBox( StreamableInit ) : TMenuView( streamableInit )
{
}
#endif // NO_STREAM


