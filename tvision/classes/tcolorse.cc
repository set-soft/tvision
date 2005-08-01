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
#define Uses_TColorSelector
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TRect
#define Uses_TView
#define Uses_TGroup
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

TColorSelector::TColorSelector( const TRect& bounds, ColorSel aSelType ) :
    TView( bounds )
{
    options |= ofSelectable | ofFirstClick | ofFramed;
    eventMask |= evBroadcast;
    selType = aSelType;
    color = 0;
}

void TColorSelector::draw()
{
    TDrawBuffer b;
    b.moveChar( 0, ' ', 0x70, size.x );
    for(int i = 0; i <= size.y; i++ )
        {
        if( i < 4 )
            {
            for( int j = 0; j < 4; j++ )
                {
                int c = i*4+j;
                b.moveChar( j*3, icon, c, 3 );
                if( c == color )
                    {
                    b.putChar( j*3+1, mark );
                    if( c == 0 )
                        b.putAttribute( j*3+1, 0x70 );
                    }
                }
            }
        writeLine( 0, i, size.x, 1, b );
        }
}

void TColorSelector::colorChanged()
{
    int msg;
    if( selType == csForeground )
        msg = cmColorForegroundChanged;
    else
        msg = cmColorBackgroundChanged;
    message( owner, evBroadcast, msg, (void *)(uipointer)color );
}
 
void TColorSelector::handleEvent( TEvent& event )
{
    const int width = 4;
    uchar oldColor = color;
#if 0
    int maxCol = (selType == csBackground) ? 7 : 15;
#else
    int maxCol = size.y == 2 ? 7 : 15;
#endif

    TView::handleEvent( event );

    switch( event.what )
        {

        case evMouseDown:
            do  {
                if( mouseInView( event.mouse.where ) )
                    {
                    TPoint mouse = makeLocal( event.mouse.where );
                    color = mouse.y*4 + mouse.x/3;
                    }
                else
                    color = oldColor;
                colorChanged();
                drawView();
                } while( mouseEvent( event, evMouseMove ) );
            break;

        case evKeyDown:

            switch( ctrlToArrow( event.keyDown.keyCode ) )
                {
		case kbLeft:
                    if( color > 0 )
                        color--;
                    else
                        color = maxCol;
                    break;

                case kbRight:
                    if( color < maxCol )
                        color++;
                    else
                        color = 0;
                    break;

                case kbUp:
                    if( color > width-1 )
                        color -= width;
                    else if( color == 0 )
                        color = maxCol;
                    else
                        color += maxCol - width;
                    break;

                case kbDown:
                    if( color < maxCol - (width-1) )
                        color += width;
                    else if( color == maxCol )
                        color = 0;
                    else
                        color -= maxCol - width;
                    break;

                default:
                    return;
                }
            break;

        case evBroadcast:
            if( event.message.command == cmColorSet )
                {
                if( selType == csBackground )
                    color = event.message.infoLong >> 4;
                else
                    color = event.message.infoLong & 0x0F;
                drawView();
                return ;
                }
            else
                return;
        default:
            return ;
        }
    drawView();
    colorChanged();
    clearEvent( event );
}

#if !defined( NO_STREAM )
void TColorSelector::write( opstream& os )
{
    TView::write( os );
    os << color << (short)selType;
}

void *TColorSelector::read( ipstream& is )
{
    short temp;
    TView::read( is );
    is >> color >> temp;
    selType = ColorSel(temp);
    return this;
}

TStreamable *TColorSelector::build()
{
    return new TColorSelector( streamableInit );
}

TColorSelector::TColorSelector( StreamableInit ) : TView( streamableInit )
{
}

#endif // NO_STREAM

