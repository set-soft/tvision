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

#define Uses_TBackground
#define Uses_TDrawBuffer
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_TPalette
#define Uses_TScreen
#define Uses_TDeskTop
#include <tv.h>

#define cpBackground "\x01"      // background palette

TBackground::TBackground( const TRect& bounds, char aPattern ) :
    TView(bounds),
    pattern( aPattern )
{
    growMode = gfGrowHiX | gfGrowHiY;
}

void TBackground::draw()
{
    TDrawBuffer b;

    char ch = pattern;
    if( TScreen::avoidMoire && ch == TDeskTop::defaultBkgrnd )
        ch = TView::noMoireFill;
    b.moveChar( 0, ch, getColor(0x01), size.x );
    writeLine( 0, 0, size.x, size.y, b );
}

TPalette& TBackground::getPalette() const
{
    static TPalette palette( cpBackground, sizeof( cpBackground )-1 );
    return palette;
}

#if !defined( NO_STREAM )

void TBackground::write( opstream& os )
{
    TView::write( os );
    os << pattern;
}

void *TBackground::read( ipstream& is )
{
    TView::read( is );
    is >> pattern;
    return this;
}

TStreamable *TBackground::build()
{
    return new TBackground( streamableInit );
}

TBackground::TBackground( StreamableInit ) : TView( streamableInit )
{
}

#endif // NO_STREAM


