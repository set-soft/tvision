/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added palette color and draw code for disabled clusters by Salvador E. Tropea
Added TSItem::append by Salvador E. Tropea

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
//#define Uses_stdio
#define Uses_ctype
#define Uses_string
#define Uses_TKeys
#define Uses_TCluster
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_TPoint
#define Uses_TSItem
#define Uses_TStringCollection
#define Uses_TGroup
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#define Uses_TGKey
#include <tv.h>

class TStringCollectionCIntl : public TStringCollection
{
public:
 TStringCollectionCIntl( ccIndex aLimit, ccIndex aDelta ) :
   TStringCollection(aLimit,aDelta) {}
 virtual void freeItem( void* item );
 stTVIntl *atI( ccIndex item ) { return (stTVIntl *)at(item); };
};

void TStringCollectionCIntl::freeItem( void* item )
{
    if( item )
       {
       stTVIntl *p = (stTVIntl *)item;
       if( p->translation )
           delete[] p->translation;
       delete p;
       }
}


#define cpCluster "\x10\x11\x12\x12\x1F"

// SET: To report the movedTo and press as broadcasts, set it to 0 if you
// don't like it.
unsigned TCluster::extraOptions=ofBeVerbose;

TCluster::TCluster( const TRect& bounds, TSItem *aStrings ) :
    TView(bounds),
    value( 0 ),
    sel( 0 )
{
    options |= ofSelectable | ofFirstClick | ofPreProcess | ofPostProcess |
               extraOptions;
    int i = 0;
    TSItem *p;
    for( p = aStrings; p != 0; p = p->next )
        i++;

    strings = new TStringCollection( i, 0 );
    intlStrings = new TStringCollectionCIntl( i, 0 );

    while( aStrings != 0 )
        {
        p = aStrings;
        strings->atInsert( strings->getCount(), newStr(aStrings->value) );
        intlStrings->atInsert( intlStrings->getCount(), TVIntl::emptySt() );
        aStrings = aStrings->next;
        delete(p);
        }

    setCursor( 2, 0 );
    showCursor();
}

TCluster::~TCluster()
{
    CLY_destroy( (TCollection *)strings );
    CLY_destroy( (TCollection *)intlStrings );
}

const char *TCluster::getItemText( ccIndex item )
{
    const char *key = (const char *)strings->at( item );
    stTVIntl *cache = intlStrings->atI( item );
    //printf("getText(%s,...) [%d]\n",key,item);
    return TVIntl::getText( key, cache );
}

uint32  TCluster::dataSize()
{
 // I have changed value from ushort to uint32, but to be compatible
 // I set dataSize to 2.
 // SET: I added TRadioButtons32 and TCheckBox32
 return sizeof(ushort);
}

void TCluster::drawBox( const char *icon, char marker)
{
    TDrawBuffer b;
    ushort color;

    ushort cNorm = state & sfDisabled ? getColor( 0x0505 ) : getColor( 0x0301 );
    ushort cSel = getColor( 0x0402 );
    for( int i = 0; i <= size.y; i++ )
        {
        for( int j = 0; j <= (strings->getCount()-1)/size.y + 1; j++ )
            {
            int cur = j * size.y + i;
            int col = column( cur );
            if ( ( cur < strings->getCount() ) &&
                (col+cstrlen(getItemText(cur))+5 < maxViewWidth) &&
                (col<size.x) )
               {
                if( (cur == sel) && (state & sfSelected) != 0 )
                    color = cSel;
                else
                    color = cNorm;
                b.moveChar( col, ' ', color, size.x - col );
                b.moveCStr( col, icon, color );
                if( mark(cur) )
                    b.putChar( col+2, marker );
                b.moveCStr( col+5, getItemText(cur), color );
                if( showMarkers && (state & sfSelected) != 0 && cur == sel )
                    {
                    b.putChar( col, specialChars[0] );
                    b.putChar( column(cur+size.y)-1, specialChars[1] );
                    }
               }
            }
        writeBuf( 0, i, size.x, 1, b );
        }
    setCursor( column(sel)+2, row(sel) );
}

void TCluster::getData(void * rec)
{
    memcpy(rec,&value,dataSize());
#if 0 // Why this
    drawView();
#endif
}

ushort TCluster::getHelpCtx()
{
    if( helpCtx == hcNoContext )
        return hcNoContext;
    else
        return helpCtx + sel;
}

TPalette& TCluster::getPalette() const
{
    static TPalette palette( cpCluster, sizeof( cpCluster )-1 );
    return palette;
}

void TCluster::handleEvent( TEvent& event )
{
    TView::handleEvent(event);
    if( event.what == evMouseDown )
        {
        TPoint mouse = makeLocal( event.mouse.where );
        int i = findSel(mouse);
        if( i != -1 )
            sel = i;
        drawView();
        do  {
            mouse = makeLocal( event.mouse.where );
            if( findSel(mouse) == sel )
                showCursor();
            else
                hideCursor();
            } while( mouseEvent(event,evMouseMove) );
        showCursor();
        mouse = makeLocal( event.mouse.where );
        if( findSel(mouse) == sel )
            {
            press(sel);
            drawView();
            }
        clearEvent(event);
        }
    else if( event.what == evKeyDown )
        switch (ctrlToArrow(event.keyDown.keyCode))
            {
            case kbUp:
                if( (state & sfFocused) != 0 )
                    {
                    if( --sel < 0 )
                        sel = strings->getCount()-1;
                    movedTo(sel);
                    drawView();
                    clearEvent(event);
                    }
                break;

            case kbDown:
                if( (state & sfFocused) != 0 )
                    {
                    if( ++sel >= strings->getCount() )
                        sel = 0;
                    movedTo(sel);
                    drawView();
                    clearEvent(event);
                    }
                break;
            case kbRight:
                if( (state & sfFocused) != 0 )
                    {
                    sel += size.y;
                    if( sel >= strings->getCount() )
                        {
                        sel = (sel +  1) % size.y;
                        if( sel >= strings->getCount() )
                            sel =  0;
                        }
                    movedTo(sel);
                    drawView();
                    clearEvent(event);
                    }
                break;
            case kbLeft:
                if( (state & sfFocused) != 0 )
                    {
                    if( sel > 0 )
                        {
                        sel -= size.y;
                        if( sel < 0 )
                            {
                            sel = ((strings->getCount()+size.y-1) /size.y)*size.y + sel - 1;
                            if( sel >= strings->getCount() )
                                sel = strings->getCount()-1;
                            }
                        }
                    else
                        sel = strings->getCount()-1;
                    movedTo(sel);
                    drawView();
                    clearEvent(event);
                    }
                break;
            default:
                for( int i = 0; i < strings->getCount(); i++ )
                    {
                    char c = hotKey( getItemText(i) );
                    if( TGKey::GetAltCode(c) == event.keyDown.keyCode ||
                        ( ( owner->phase == phPostProcess ||
                            (state & sfFocused) != 0
                          ) &&
                          c != 0 &&
                          TGKey::CompareASCII(uctoupper(event.keyDown.charScan.charCode),c)
                        )
                      )
                        {
                        select();
                        sel =  i;
                        movedTo(sel);
                        press(sel);
                        drawView();
                        clearEvent(event);
                        return;
                        }
                    }
                if( event.keyDown.charScan.charCode == ' ' &&
                    (state & sfFocused) != 0
                  )
                    {
                    press(sel);
                    drawView();
                    clearEvent(event);
                    }
            }
}

void TCluster::setData(void * rec)
{
    memcpy(&value,rec,dataSize());
    drawView();
}

void TCluster::setState( ushort aState, Boolean enable )
{
    TView::setState( aState, enable );
    if( aState == sfSelected || aState == sfDisabled )
        drawView();
}

Boolean TCluster::mark( int )
{
    return False;
}

void TCluster::movedTo( int /*item*/ )
{
 if (owner && (options & ofBeVerbose))
    message(owner,evBroadcast,cmClusterMovedTo,this);
}

void TCluster::press( int /*item*/ )
{
 if (owner && (options & ofBeVerbose))
    message(owner,evBroadcast,cmClusterPress,this);
}

int TCluster::column( int item )
{
    if( item < size.y )
        return 0;
    else
        {
        int width = 0;
        int col = -6;
        int l = 0;
        for( int i = 0; i <= item; i++ )
            {
            if( i % size.y == 0 )
                {
                col += width + 6;
                width = 0;
                }

            if( i < strings->getCount() )
                l = cstrlen( getItemText(i) );
            if( l > width )
                width = l;
            }
        return col;
        }
}

int TCluster::findSel( TPoint p )
{
    TRect r = getExtent();
    if( !r.contains(p) )
        return -1;
    else
        {
        int i = 0;
        while( p.x >= column( i + size.y ) )
            i += size.y;
        int s = i + p.y;
        if( s >= strings->getCount() )
            return -1;
        else
            return s;
        }
}

int TCluster::row( int item )
{
    return item % size.y;
}

/**[txh]********************************************************************

  Description:
  Appends the provided TMenuItem at the end of the chain.@*
  Coded by SET. That's a mix between Dean Wakerley and TVTools ideas.
  
***************************************************************************/

void TSItem::append( TSItem *aNext )
{
    TSItem *item = this;
    for ( ; item->next; item = item->next );
    item->next = aNext;
}

#if !defined( NO_STREAM )
void TCluster::write( opstream& os )
{
    TView::write( os );
    os << value << sel << strings;
}

void *TCluster::read( ipstream& is )
{
    TView::read( is );
    is >> value >> sel >> strings;
    setCursor( 2, 0 );
    showCursor();
    return this;
}

TStreamable *TCluster::build()
{
    return new TCluster( streamableInit );
}

TCluster::TCluster( StreamableInit ) : TView( streamableInit )
{
}
#endif // NO_STREAM

