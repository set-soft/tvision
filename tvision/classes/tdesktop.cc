/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified cursor behavior while desktop locked by Salvador E. Tropea (SET)

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdlib.h>

#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TPoint
#define Uses_TEvent
#define Uses_TBackground
#define Uses_opstream
#define Uses_ipstream
#define Uses_TScreen
#define Uses_TVCodePage
#include <tv.h>

TDeskInit::TDeskInit( TBackground *(*cBackground)( TRect ) ) :
    createBackground( cBackground )
{
}

TDeskTop::TDeskTop( const TRect& bounds ) :
    TDeskInit( &TDeskTop::initBackground )
    , TGroup(bounds)
{
    growMode = gfGrowHiX | gfGrowHiY;

    TScreen::setCursorPos( bounds.a.x , bounds.b.y );
        
    if( createBackground != 0 &&
        (background = createBackground( getExtent() )) != 0 )
        insert( background );
}

void TDeskTop::shutDown()
{
    background = 0;
    TGroup::shutDown();
}

inline Boolean Tileable( TView *p )
{
    return Boolean( (p->options & ofTileable) != 0 && (p->state & sfVisible) != 0 );
}

static short cascadeNum;
static TView *lastView;
             
void doCount( TView* p, void * )
{
    if( Tileable( p ) )
        {
        cascadeNum++;
        lastView = p;
        }
}

void doCascade( TView* p, void *r )
{
    if( Tileable( p ) && cascadeNum >= 0 )
        {
        TRect NR = *(TRect *)r;
        NR.a.x += cascadeNum;
        NR.a.y += cascadeNum;
        p->locate( NR );
        cascadeNum--;
        }
}

void TDeskTop::cascade( const TRect &r )
{
    TPoint min, max;
    cascadeNum = 0;
    forEach( doCount, 0 );
    if( cascadeNum > 0 )
        {
        lastView->sizeLimits( min, max );
        if( (min.x > r.b.x - r.a.x - cascadeNum) || 
            (min.y > r.b.y - r.a.y - cascadeNum) )
            tileError();
        else
            {
            cascadeNum--;
            lock();
            forEach( doCascade, (void *)&r );
            unlock();
            }
        }
}

void TDeskTop::handleEvent(TEvent& event)
{
    if( (event.what == evBroadcast) && (event.message.command == cmReleasedFocus) )
        // SET: Move the cursor away, hopefully we will have a status bar.
        // Helps Braille Terminals to know the object lost the focus.
        TScreen::setCursorPos( origin.x , origin.y + size.y );
    TGroup::handleEvent( event );
    if( event.what == evBroadcast && event.message.command == cmUpdateCodePage &&
        background )
        background->changePattern(TVCodePage::RemapChar(
                                  TDeskTop::odefaultBkgrnd,
                                  (ushort *)event.message.infoPtr));

    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case cmNext:
                if (valid(cmReleasedFocus))
                    selectNext( False );
                break;
            case cmPrev:
                if (valid(cmReleasedFocus))
                    current->putInFrontOf( background );
                break;
            default:
                return;
            }
        clearEvent( event );
        }
}

TBackground *TDeskTop::initBackground( TRect r )
{
    return new TBackground( r, defaultBkgrnd );
}

// SET: made static and used unsigned instead of short.
// It calculates the square root truncating the decimals.
static
unsigned iSqr( unsigned i )
{
    unsigned res1 = 2;
    unsigned res2 = i/res1;
    while( abs( res1 - res2 ) > 1 )
        {
        res1 = (res1 + res2)/2;
        res2 = i/res1;
        }
    return res1 < res2 ? res1 : res2;
}

void mostEqualDivisors(int n, int& x, int& y)
{
    int i;

    i = iSqr( n );
    if( n % i != 0 )
        if( n % (i+1) == 0 )
            i++;
    if( i < (n/i) )
        i = n/i;

    x = n/i;
    y = i;
}

// SET: All to ints, they are the best type for any compiler
static int numCols, numRows, numTileable, leftOver, tileNum;

void doCountTileable( TView* p, void * )
{
    if( Tileable( p ) )
        numTileable++;
}

int dividerLoc( int lo, int hi, int num, int pos)
{
    return int(long(hi-lo)*pos/long(num)+lo);
}

TRect calcTileRect( int pos, const TRect &r )
{
    int x, y;
    TRect nRect;

    int d = (numCols - leftOver) * numRows;
    if( pos <  d )
        {
        x = pos / numRows;
        y = pos % numRows;
        }
    else
        {
        x = (pos-d)/(numRows+1) + (numCols-leftOver);
        y = (pos-d)%(numRows+1);
        }
    nRect.a.x = dividerLoc( r.a.x, r.b.x, numCols, x );
    nRect.b.x = dividerLoc( r.a.x, r.b.x, numCols, x+1 );
    if( pos >= d )
        {
        nRect.a.y = dividerLoc(r.a.y, r.b.y, numRows+1, y);
        nRect.b.y = dividerLoc(r.a.y, r.b.y, numRows+1, y+1);
        }
    else
        {
        nRect.a.y = dividerLoc(r.a.y, r.b.y, numRows, y);
        nRect.b.y = dividerLoc(r.a.y, r.b.y, numRows, y+1);
        }
    return nRect;
}

void doTile( TView* p, void *lR )
{
    if( Tileable( p ) )
        {
        TRect r = calcTileRect( tileNum, *(const TRect *)lR );
        p->locate(r);
        tileNum--;
        }
}

void TDeskTop::tile( const TRect& r )
{
    numTileable =  0;
    forEach( doCountTileable, 0 );
    if( numTileable > 0 )
        {
        // SET: This trick makes the partitions in the reverse order
        if( getOptions() & dsktTileVertical )
            mostEqualDivisors( numTileable, numRows, numCols );
        else
            mostEqualDivisors( numTileable, numCols, numRows );
        if( ( (r.b.x - r.a.x)/numCols ==  0 ) || 
            ( (r.b.y - r.a.y)/numRows ==  0) )
            tileError();
        else
            {
            leftOver = numTileable % numCols;
            tileNum = numTileable - 1;
            lock();
            forEach( doTile, (void *)&r );
            unlock();
            }
        }
}

void  TDeskTop::tileError()
{
}

// SET: TViews will ask us if that's good time to draw cursor changes
Boolean TDeskTop::canShowCursor()
{
 return lockFlag ? False : True;
}

// SET: If nobody will recover the focus move the cursor to the status line
ushort TDeskTop::execView( TView *p )
{
 ushort ret=TGroup::execView(p);
 if (p && !current)
    TScreen::setCursorPos(0,TScreen::screenHeight-1);
 return ret;
}

#if !defined( NO_STREAM )
TStreamable *TDeskTop::build()
{
    return new TDeskTop( streamableInit );
}

TDeskTop::TDeskTop( StreamableInit ) :
    TDeskInit( NULL )
    , TGroup( streamableInit )
{
}
#endif // NO_STREAM

