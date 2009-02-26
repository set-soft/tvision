/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added callback, code page stuff and various details by Salvador Eduardo Tropea.
Added new class TButtonRef by Salvador Eduardo Tropea.
Added i18n support by Salvador Eduardo Tropea.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_ctype
#define Uses_string
#define Uses_TButton
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_TRect
#define Uses_TGroup
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#define Uses_TGKey
#define Uses_TScreen
#include <tv.h>

/* SET: F*#$%! why these values are defined here!!!
const ushort
    cmGrabDefault    = 61,
    cmReleaseDefault = 62;

  Moved to views.h
*/

#define cpButton "\x0A\x0B\x0C\x0D\x0E\x0E\x0E\x0F"

TButton::TButton( const TRect& bounds,
                  const char *aTitle,
                  ushort aCommand,
                  ushort aFlags) :
    TView( bounds ),
    title( newStr( aTitle ) ),
    intlTitle( NULL ),
    command( aCommand ),
    flags( aFlags ),
    amDefault( Boolean( (aFlags & bfDefault) != 0 ) )
{
    options |= ofSelectable | ofFirstClick | ofPreProcess | ofPostProcess;
    eventMask |= evBroadcast;
    if( !commandEnabled(aCommand) )
        state |= sfDisabled;
    callBack=0;
    // This class can be "Braille friendly"
    if (TScreen::getShowCursorEver())
       state |= sfCursorVis;
}

TButton::~TButton()
{
    DeleteArray((char *)title); // SET
    TVIntl::freeSt(intlTitle);
}


void TButton::draw()
{
    drawState(False);
}

void TButton::drawTitle( TDrawBuffer &b,
                         int s,
                         int i,
                         ushort cButton,
                         Boolean down
                       )
{
    int l, scOff;
    const char *theTitle = getText();
    if( (flags & bfLeftJust) != 0 )
        l = 1;
    else
        {
        l = (s - cstrlen(theTitle) - 1)/2;
        if( l < 1 )
            l = 1;
        }
    b.moveCStr( i+l, theTitle, cButton );

    if( showMarkers == True && !down )
        {
        if( (state & sfSelected) != 0 )
            scOff = 0;
        else if( amDefault )
            scOff = 2;
        else
            scOff = 4;
        b.putChar( 0, specialChars[scOff] );
        b.putChar( s, specialChars[scOff+1] );
        }
    if( (state & sfActive) && (state & sfSelected) )
        {
        setCursor( i+l-1 , 0 );
        resetCursor();
        }
}

void TButton::drawState(Boolean down)
{
    ushort cButton, cShadow;
    char   ch = ' ';
    int    i;
    TDrawBuffer b;

    if( (state & sfDisabled) != 0 )
        cButton = getColor(0x0404);
    else
        {
        cButton = getColor(0x0501);
        if( (state & sfActive) != 0 )
            {
            if( (state & sfSelected) != 0 )
                cButton = getColor(0x0703);
            else if( amDefault )
                cButton = getColor(0x0602);
            }
        }
    cShadow = getColor(8);
    int s = size.x-1;
    int T = size.y / 2 - 1;

    for( int y = 0; y <= size.y-2; y++ )
        {
        b.moveChar( 0, ' ', cButton, size.x );
        b.putAttribute( 0, cShadow );
        if( down )
            {
            b.putAttribute( 1, cShadow );
            i = 2;
            }
        else
            {
            b.putAttribute( s, cShadow );
            if( showMarkers == False )
                {
                if( y == 0 )
                    b.putChar( s, shadows[0] );
                else
                    b.putChar( s, shadows[1] );
                ch = shadows[2];
                }
            i =  1;
            }

        if( y == T && title != 0 )
            drawTitle( b, s, i, cButton, down );

        if( showMarkers && !down )
            {
            b.putChar( 1, markers[0] );
            b.putChar( s-1, markers[1] );
            }
        writeLine( 0, y, size.x, 1, b );
        }
    b.moveChar( 0, ' ', cShadow, 2 );
    b.moveChar( 2, ch, cShadow, s-1 );
    writeLine( 0, size.y-1, size.x, 1, b );
}

TPalette& TButton::getPalette() const
{
    static TPalette palette( cpButton, sizeof( cpButton )-1 );
    return palette;
}

void TButton::handleEvent( TEvent& event )
{
    TPoint mouse;
    TRect clickRect;
    Boolean down = False;
    char c = hotKey( getText() );

    clickRect = getExtent();
    clickRect.a.x++;
    clickRect.b.x--;
    clickRect.b.y--;

    if( event.what == evMouseDown )
        {
        mouse = makeLocal( event.mouse.where );
        if( !clickRect.contains(mouse) )
            clearEvent( event );
        }
    TView::handleEvent(event);

    switch( event.what )
        {
        case evMouseDown:
            clickRect.b.x++;
            do  {
                mouse = makeLocal( event.mouse.where );
                if( down != clickRect.contains( mouse ) )
                    {
                    down = Boolean( !down );
                    drawState( down );
                    }
                } while( mouseEvent( event, evMouseMove ) );
            if( down )
                {
                press();
                drawState( False );
                }
            clearEvent( event );
            break;

        case evKeyDown:
            if( event.keyDown.keyCode == TGKey::GetAltCode(c) ||
                ( owner->phase == phPostProcess &&
                  c != 0 &&
                  TGKey::CompareASCII(uctoupper(event.keyDown.charScan.charCode),c)
                  //uctoupper(event.keyDown.charScan.charCode) == c
                ) ||
                ( (state & sfFocused) != 0 &&
                  event.keyDown.charScan.charCode == ' '
                )
              )
                {
                press();
                clearEvent( event );
                }
            break;

        case evBroadcast:
            switch( event.message.command )
                {
                case cmDefault:
                    if( amDefault && !(state & sfDisabled) )
                        {
                        press();
                        clearEvent(event);
                        }
                    break;

                case cmGrabDefault:
                case cmReleaseDefault:
                    if( (flags & bfDefault) != 0 )
                        {
                        amDefault = Boolean(event.message.command == cmReleaseDefault);
                        drawView();
                        }
                    break;

                case cmCommandSetChanged:
                    if (((state & sfDisabled) && commandEnabled(command)) ||
                        (!(state & sfDisabled) && !commandEnabled(command)))
                    {
                      setState(sfDisabled,Boolean(!commandEnabled(command)));
                      drawView();
                    }
                    break;
                }
        break;
        }
}

void TButton::makeDefault( Boolean enable )
{
    if( (flags & bfDefault) == 0 )
        {
        message( owner,
                 evBroadcast,
                 (enable == True) ? cmGrabDefault : cmReleaseDefault,
                 this
               );
        amDefault = enable;
        drawView();
        }
}

void TButton::setState( ushort aState, Boolean enable )
{
    TView::setState(aState, enable);
    if( aState & (sfSelected | sfActive) )
    {
        if(!enable)
        {                           // BUG FIX - EFW - Thu 10/19/95
            state &= ~sfFocused;
            makeDefault(False);
        }
        drawView();
    }
    if( (aState & sfFocused) != 0 )
        makeDefault( enable );
}

void TButton::press()
{
 message(owner,evBroadcast,cmRecordHistory,0);
 if (flags & bfBroadcast)
    message(owner,evBroadcast,command,this);
 else
   {
    if (callBack)
      {// SET: That's really useful
       int ret=callBack(command,cbData);
       if (ret==btcbEndModal && owner)
          owner->endModal(command);
      }
    else
      {
       TEvent e;
       e.what=evCommand;
       e.message.command=command;
       e.message.infoPtr=this;
       putEvent(e);
      }
   }
}

#if !defined( NO_STREAM )

void TButton::write( opstream& os )
{
    TView::write( os );
    os.writeString( title );
    os << command << flags << (int)amDefault;
}

void *TButton::read( ipstream& is )
{
    TView::read( is );
    title = is.readString();
    intlTitle = NULL;
    int temp;
    is >> command >> flags >> temp;
    amDefault = Boolean(temp);
    if( TButton::commandEnabled( command ) )
        state &= ~sfDisabled;
    else
        state |= sfDisabled;
    return this;
}

TStreamable *TButton::build()
{
    return new TButton( streamableInit );
}


/**[txh]********************************************************************

  Description:
  This class is just like TButton but the title of the button is a reference
and not a copy of the one passed by the user. It allows changes from the
application side to be reflected in the button with a simple redraw. That's
*very* useful for remappings.
  
***************************************************************************/

TButtonRef::TButtonRef(const TRect& bounds, const char *aTitle,
                       ushort aCommand, ushort aFlags) :
  TButton(bounds,aTitle,aCommand,aFlags)
{
 DeleteArray((char *)title);
 title=aTitle;
}

TButtonRef::~TButtonRef()
{
 title=NULL;
}

#endif // NO_STREAM

