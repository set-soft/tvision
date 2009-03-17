/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea to add functionality.
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string
#define Uses_AllocLocal
#define Uses_TKeys
#define Uses_TListViewer
#define Uses_TScrollBar
#define Uses_TDrawBuffer
#define Uses_TPoint
#define Uses_TEvent
#define Uses_TGroup
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_TPalette
#include <tv.h>

#define cpListViewer "\x1A\x1A\x1B\x1C\x1D"

// SET: By default I make it report more messages than original TV. In this
// case the focusItem(ccIndex item) member generates a broadcast.
unsigned TListViewer::extraOptions=ofBeVerbose;

TListViewer::TListViewer( const TRect& bounds,
                          ushort aNumCols,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar) :
    TView( bounds ),
    topItem( 0 ),
    focused( 0 ),
    range( 0 ),
    handleSpace(True)
{
    options |= ofFirstClick | ofSelectable | extraOptions;
    eventMask |= evBroadcast;

    hScrollBar = aHScrollBar;
    vScrollBar = aVScrollBar;
    center = False;
    setNumCols(aNumCols);
}

// SET: Moved here to allow changes from sub-classes
void TListViewer::setNumCols(int aNumCols)
{
 int arStep,pgStep;

 numCols=aNumCols;
 // Compute the scroll bar changes.
 if (vScrollBar!=0)
   {
    if (numCols==1)
      {
       pgStep=size.y-1;
       arStep=1;
      }
    else
      {
       pgStep=size.y*numCols;
       arStep=size.y;
      }
    vScrollBar->setStep(pgStep,arStep);
   }
 if (hScrollBar)
    hScrollBar->setStep(size.x/numCols,1);
}

void TListViewer::changeBounds( const TRect& bounds )
{
    TView::changeBounds( bounds );
    if( hScrollBar != 0 )
        hScrollBar->setStep( size.x / numCols, 1 );
}

void TListViewer::draw()
{
 short i, j;
 ccIndex item;
 ushort normalColor, selectedColor, focusedColor=0, color;
 short colWidth, curCol, indent;
 TDrawBuffer b;
 uchar scOff;

    if( (state&(sfSelected | sfActive)) == (sfSelected | sfActive))
        {
        normalColor = getColor(1);
        focusedColor = getColor(3);
        selectedColor = getColor(4);
        }
    else
        {
        normalColor = getColor(2);
        selectedColor = getColor(4);
        }

    if( hScrollBar != 0 )
        indent = hScrollBar->value;
    else
        indent = 0;

    colWidth = size.x / numCols + 1;
    for( i = 0; i < size.y; i++ )
        {
        for( j = 0; j < numCols; j++ )
            {
            int width;;
            item =  j * size.y + i + topItem;
            curCol = j * colWidth;
            if (j == numCols-1) width = size.x - curCol + 1; 
            else width = colWidth;
            if( (state & (sfSelected | sfActive)) == (sfSelected | sfActive) &&
                focused == item &&
                range > 0)
                {
                color = focusedColor;
                setCursor( curCol + 1, i );
                scOff = 0;
                }
            else if( item < range && isSelected(item) )
                {
                color = selectedColor;
                scOff = 2;
                }
            else
                {
                color = normalColor;
                scOff = 4;
                }

            b.moveChar( curCol, ' ', color, width );
            if( item < range )
                {
                // This was probably the
                // reason for a bug, because
                // getText assumes a buffer
                // with a length of maxLen + 1
                AllocLocalStr(text,width+indent+1);
                AllocLocalStr(buf,width+1);
                getText( text, item, width + indent );
                int tl = strlen(text);
                if (tl <= indent)
                  buf[0] = 0;
                else
                {
                  memcpy( buf, text+indent, width );
                  buf[tl-indent] = EOS;
                }
                b.moveStr( curCol+1, buf, color );
                if( showMarkers )
                    {
                    b.putChar( curCol, specialChars[scOff] );
                    b.putChar( curCol+width-2, specialChars[scOff+1] );
                    }
                }
            else if( i == 0 && j == 0 )
                b.moveStr( curCol+1, _("<empty>"), getColor(1) );

            // It was a really nasty thing, this call used 179 instead of
            // a configurable value like now.
            b.moveChar( curCol+width-1, columnSeparator, getColor(5), 1 );
            }
        writeLine( 0, i, size.x, 1, b );
        }
}

void TListViewer::focusItem( ccIndex item )
{
    focused = item;

    if( item < topItem )
        {
        if( numCols == 1 )
            topItem = item;
        else
            topItem = item - item % size.y;
        }
    else
        {
        if( item >= topItem + size.y*numCols )
            {
            if( numCols == 1 )
                topItem = item - size.y + 1;
            else
                topItem = item - item % size.y - (size.y * (numCols-1));
            }
        }
    if( vScrollBar != 0 )
        vScrollBar->setValue( item );
    else
        drawView();
    if (owner && (options & ofBeVerbose))
       message(owner,evBroadcast,cmListItemFocused,this);
}


/**[txh]********************************************************************

  Description:
  That's a variant of focusItem that tries to center the focused item when
the list have only one column.
  
***************************************************************************/

void TListViewer::focusItemCentered( ccIndex item )
{
    if( numCols != 1 )
        {
        focusItem( item );
        return;
        }
    center = True;
    focused = item;

    if( item < topItem )
        {
        topItem = item - size.y/2;
        if( topItem < 0)
            topItem = 0;
        }
    else
        {
        if( item >= topItem + size.y*numCols )
            {
            topItem = item - size.y/2;
            if( topItem + size.y >= range && range > size.y)
                topItem = range - size.y;
            }
        }
    if( vScrollBar != 0 )
        vScrollBar->setValue( item );
    else
        drawView();
    if (owner && (options & ofBeVerbose))
       message(owner,evBroadcast,cmListItemFocused,this);
    center = False;
}

void TListViewer::focusItemNum( ccIndex item )
{
    if( item < 0 )
        item = 0;
    else
        if( item >= range && range > 0 )
            item = range - 1;

    if( range !=  0 )
        {
        if( center )
            focusItemCentered( item );
        else
            focusItem( item );
        }
}

TPalette& TListViewer::getPalette() const
{
    static TPalette palette( cpListViewer, sizeof( cpListViewer )-1 );
    return palette;
}

void TListViewer::getText( char *dest, ccIndex, short )
{
    *dest = EOS;
}

Boolean TListViewer::isSelected( ccIndex item )
{
    return Boolean( item == focused );
}

void TListViewer::handleEvent( TEvent& event )
{
    TPoint mouse;
    ushort colWidth;
    ccIndex  oldItem, newItem;
    ushort count;
    int mouseAutosToSkip = 4;

    TView::handleEvent(event);

    if( event.what == evMouseDown )
        {
        // They must be before doubleClick to avoid "b4 double click"
        if( event.mouse.buttons == mbButton4 )
            {
            focusItemNum(focused - size.y * numCols);
            clearEvent( event );
            return;
            }
        if( event.mouse.buttons == mbButton5 )
            {
            focusItemNum(focused + size.y * numCols);
            clearEvent( event );
            return;
            }
        if( event.mouse.doubleClick && range > focused )
            {
            selectItem( focused );
            clearEvent( event );
            return;
            }
        colWidth = size.x / numCols + 1;
        oldItem =  focused;
        mouse = makeLocal( event.mouse.where );
        newItem = mouse.y + (size.y * (mouse.x / colWidth)) + topItem;
        count = 0;
        do  {
            if( newItem != oldItem )
                focusItemNum( newItem );
            oldItem = newItem;
            mouse = makeLocal( event.mouse.where );
            if( mouseInView( event.mouse.where ) )
                newItem = mouse.y + (size.y * (mouse.x / colWidth)) + topItem;
            else
                {
                if( numCols == 1 )
                    {
                    if( event.what == evMouseAuto )
                        count++;
                    if( count == mouseAutosToSkip )
                        {
                        count = 0;
                        if( mouse.y < 0 )
                            newItem = focused - 1;
                        else
                            if( mouse.y >= size.y )
                                newItem = focused + 1;
                        }
                    }
                else
                    {
                    if( event.what == evMouseAuto )
                        count++;
                    if( count == mouseAutosToSkip )
                        {
                        count = 0;
                        if( mouse.x < 0 )
                            newItem = focused - size.y;
                        else if( mouse.x >= size.x )
                            newItem = focused + size.y;
                        else if( mouse.y < 0 )
                            newItem = focused - focused % size.y;
                        else if( mouse.y > size.y )
                            newItem = focused - focused % size.y + size.y - 1;
                        }
                    }
                }
            } while( mouseEvent( event, evMouseMove | evMouseAuto ) );
        focusItemNum( newItem );
        if( event.mouse.doubleClick && range > focused )
            selectItem( focused );
        clearEvent( event );
        }
    else if( event.what == evKeyDown )
        {
        if ((handleSpace == True) &&
            (event.keyDown.charScan.charCode ==  ' ') && focused < range )
            {
            selectItem( focused );
            newItem = focused;
            }
        else
            {
            switch (ctrlToArrow(event.keyDown.keyCode))
                {
                case kbUp:
                    newItem = focused - 1;
                    break;
                case kbDown:
                    newItem = focused + 1;
                    break;
                case kbRight:
                    if( numCols > 1 )
                        newItem = focused + size.y;
                    else
                        { // SET: if the user put a scroll bar with one column
                          // that's what he wants
                        if (hScrollBar) hScrollBar->handleEvent(event);
                        return;
                        }
                    break;
                case kbLeft:
                    if( numCols > 1 )
                        newItem = focused - size.y;
                    else
                        { // SET: see kbRight
                        if (hScrollBar) hScrollBar->handleEvent(event);
                        return;
                        }
                    break;
                case kbPgDn:
                    newItem = focused + size.y * numCols;
                    break;
                case  kbPgUp:
                    newItem = focused - size.y * numCols;
                    break;
                case kbHome:
                    newItem = topItem;
                    break;
                case kbEnd:
                    newItem = topItem + (size.y * numCols) - 1;
                    break;
                case kbCtrlPgDn:
                    newItem = range - 1;
                    break;
                case kbCtrlPgUp:
                    newItem = 0;
                    break;
                default:
                    return;
                }
            focusItemNum(newItem);
            }
        clearEvent(event);
        }
    else if( event.what == evBroadcast )
        {
        if( (options & ofSelectable) != 0 )
            {
            if( event.message.command == cmScrollBarClicked &&
                  ( event.message.infoPtr == hScrollBar || 
                    event.message.infoPtr == vScrollBar ) )
                select();
            else if( event.message.command == cmScrollBarChanged )
                {
                if( vScrollBar == event.message.infoPtr )
                    {
                    focusItemNum( vScrollBar->value );
                    drawView();
                    }
                else if( hScrollBar == event.message.infoPtr )
                    drawView();
                }
            }
        }
}

void TListViewer::selectItem( ccIndex )
{
    message( owner, evBroadcast, cmListItemSelected, this );
}

void TListViewer::setRange( ccIndex aRange )
{
    range = aRange;
    if (focused >= aRange)
       focused = (aRange - 1 >= 0) ? aRange - 1 : 0;
    if( vScrollBar != 0 )
        {
        vScrollBar->setParams( focused,
                               0,
                               aRange - 1,
                               vScrollBar->pgStep,
                               vScrollBar->arStep
                             );
        } 
    else
        drawView();
}

void TListViewer::setState( ushort aState, Boolean enable)
{
    TView::setState( aState, enable );
    if( (aState & (sfSelected | sfActive)) != 0 )
        {
        if( hScrollBar != 0 )
            {
            if( getState(sfActive) )
                hScrollBar->show();
            else
                hScrollBar->hide();
            }
        if( vScrollBar != 0 )
            {
            if( getState(sfActive) )
                vScrollBar->show();
            else
                vScrollBar->hide();
            }
        drawView();
        }
}

void TListViewer::shutDown()
{
     hScrollBar = 0;
     vScrollBar = 0;
     TView::shutDown();
}

#if !defined( NO_STREAM )
void TListViewer::write( opstream& os )
{
    TView::write( os );
    os << hScrollBar << vScrollBar << numCols
       << topItem << focused << range;
}

void *TListViewer::read( ipstream& is )
{
    TView::read( is );
    is >> hScrollBar >> vScrollBar >> numCols
       >> topItem >> focused >> range;
    return this;
}

TStreamable *TListViewer::build()
{
    return new TListViewer( streamableInit );
}

TListViewer::TListViewer( StreamableInit ) : TView( streamableInit )
{
}
#endif // NO_STREAM


