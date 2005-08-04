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

#define Uses_TKeys
#define Uses_TColorDisplay
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGroup
#define Uses_TRect
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

TColorDisplay::TColorDisplay( const TRect& bounds, const char *aText ) :
    TView( bounds ),
    color( 0 ),
    text( newStr( aText ) )
{
  eventMask |= evBroadcast;
}

TColorDisplay::~TColorDisplay()
{
    DeleteArray((char *)text);
}

void TColorDisplay::draw()
{
    uchar c = *color;
    if( c == 0 )
        c = errorAttr;
    const int len = strlen( text );
    TDrawBuffer b;
    for( int i = 0; i <= size.x/len; i++ )
        b.moveStr( i*len, text, c );
    writeLine( 0, 0, size.x, size.y, b );
}
 
void TColorDisplay::handleEvent( TEvent& event )
{
    TView::handleEvent( event );
    if( event.what == evBroadcast )
        switch( event.message.command )
            {
            case cmColorBackgroundChanged:
                *color = (*color & 0x0F) | ((event.message.infoLong << 4) & 0xF0);
                drawView();
                break;

            case cmColorForegroundChanged:
                *color = (*color & 0xF0) | (event.message.infoLong & 0x0F);
                drawView();
            }
}

void TColorDisplay::setColor( uchar *aColor )
{
    color = aColor;
    message( owner, evBroadcast, cmColorSet, (void *)(uipointer)(*color) );
    drawView();
}

#if !defined( NO_STREAM )

void TColorDisplay::write( opstream& os )
{
    TView::write( os );
    os.writeString( text );
}

void *TColorDisplay::read( ipstream& is )
{
    TView::read( is );
    text = is.readString();
    color = 0;
    return this;
}

TStreamable *TColorDisplay::build()
{
    return new TColorDisplay( streamableInit );
}

TColorDisplay::TColorDisplay( StreamableInit ) : TView( streamableInit )
{
}

#endif

