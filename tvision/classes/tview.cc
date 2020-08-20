/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified cursor behavior while desktop locked by Salvador E. Tropea (SET)
         Rewrote call50() to support on the fly remapping and avoid using
         setCharacter.
Modified reworked the core drawing stuff (resetCursor(), exposed() and
         writeView()) to make the code easy to understand and maintain
         by Salvador E. Tropea (SET).
Added support for Unicode buffers Copyright (c) 2003 by by Salvador E.
         Tropea (SET).
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string
#define Uses_stdio
#define Uses_AllocLocal
#define Uses_alloca
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TView
#define Uses_TCommandSet
#define Uses_TPoint
#define Uses_TGroup
#define Uses_TRect
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#define Uses_TVCodePage

#include <tv.h>

#include <limits.h>
#if 0 // def TVCompf_djgpp
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#endif

TPoint shadowSize = {2,1};
uchar shadowAttr = 0x08;
Boolean TView::showMarkers = False;
uchar TView::errorAttr = 0xCF;
Boolean TView::commandSetChanged = False;

extern TView *TheTopView;

static TCommandSet initCommands()
{
    TCommandSet temp;
    temp.enableAllCommands();
    temp.disableCmd( cmZoom );
    temp.disableCmd( cmClose );
    temp.disableCmd( cmResize );
    temp.disableCmd( cmNext );
    temp.disableCmd( cmPrev );
    return temp;
}

TCommandSet TView::curCommandSet = initCommands();

TView::TView( const TRect& bounds) :
    next( 0 ),
    options( 0 ), eventMask( evMouseDown | evKeyDown | evCommand ),
    state( sfVisible ),
    growMode( 0 ), dragMode( dmLimitLoY ), helpCtx( hcNoContext ),
    owner( 0 )
{
    setBounds( bounds);
    cursor.x = cursor.y = 0;
}

TView::~TView()
{
}

void TView::blockCursor()
{
     setState(sfCursorIns, True);
}

#define grow(i) (( (growMode & gfGrowRel)) ? \
                (i = (i * s + ((s - d) >> 1)) / (s - d)) : (i += d))

inline int range( int val, int min, int max )
{
    if( val < min )
        return min;
    else if( val > max )
        return max;
    else
        return val;
}
                
void TView::calcBounds( TRect& bounds, TPoint delta )
{
    bounds = getBounds();

    int s = owner->size.x;
    int d = delta.x;

    if( (growMode & gfGrowLoX) != 0 )
        grow(bounds.a.x);

    if( (growMode & gfGrowHiX) != 0 )
        grow(bounds.b.x);

    s = owner->size.y;
    d = delta.y;

    if( (growMode & gfGrowLoY) != 0 )
        grow(bounds.a.y);

    if( (growMode & gfGrowHiY) != 0 )
        grow(bounds.b.y);

    TPoint minLim, maxLim;
    sizeLimits( minLim, maxLim );
    bounds.b.x = bounds.a.x + range( bounds.b.x-bounds.a.x, minLim.x, maxLim.x );
    bounds.b.y = bounds.a.y + range( bounds.b.y-bounds.a.y, minLim.y, maxLim.y );
}

void TView::changeBounds( const TRect& bounds )
{
    setBounds(bounds);
    drawView();
}

void TView::clearEvent( TEvent& event )
{
    event.what = evNothing;
    event.message.infoPtr = this;
}

Boolean TView::commandEnabled( ushort command )
{
    return Boolean(/* (command > 0x3FF) || // is now handled by
                                           // curCommandSet.has(command) */
                   curCommandSet.has(command));
}

uint32 TView::dataSize()
{
    return 0;
}

void TView::disableCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                !(curCommandSet & commands).isEmpty());
    curCommandSet.disableCmd(commands);
}

void TView::disableCommand( ushort command )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                 curCommandSet.has(command) );
    curCommandSet.disableCmd(command);
}

void TView::moveGrow( TPoint p,
                      TPoint s,
                      TRect& limits,
                      TPoint minSize,
                      TPoint maxSize,
                      uchar mode
                    )
{
    TRect   r;
    s.x = min(max(s.x, minSize.x), maxSize.x);
    s.y = min(max(s.y, minSize.y), maxSize.y);
    p.x = min(max(p.x, limits.a.x - s.x+1), limits.b.x-1);
    p.y = min(max(p.y, limits.a.y - s.y+1), limits.b.y-1);

    if( (mode & dmLimitLoX) != 0 )
        p.x = max(p.x, limits.a.x);
    if( (mode & dmLimitLoY) != 0 )
        p.y = max(p.y, limits.a.y);
    if( (mode & dmLimitHiX) != 0 )
        p.x = min(p.x, limits.b.x-s.x);
    if( (mode & dmLimitHiY) != 0 )
        p.y = min(p.y, limits.b.y-s.y);
    r = TRect(p.x, p.y, p.x +  s.x, p.y +  s.y);
    locate(r);
}

// No longer needed, now passed as parameter "grow"
//extern unsigned short getshiftstate(void);

void TView::change( uchar mode, TPoint delta, TPoint& p, TPoint& s, int grow )
{
    if( (mode & dmDragMove) != 0 && !grow )
        p += delta;
    else if( (mode & dmDragGrow) != 0 && grow )
        s += delta;
}

void TView::dragView( TEvent& event,
                      uchar mode,
                      TRect& limits,
                      TPoint minSize,
                      TPoint maxSize
                    )
{
    TRect saveBounds;

    TPoint p, s;
    setState( sfDragging, True );

    if( event.what == evMouseDown )
        {
        if( (mode & dmDragMove) != 0 )
            {
            p = origin - event.mouse.where;
            do  {
                event.mouse.where += p;
                moveGrow( event.mouse.where,
                          size,
                          limits,
                          minSize,
                          maxSize,
                          mode
                        );
                } while( mouseEvent(event,evMouseMove) );
            }
        else
            {
            p = size - event.mouse.where;
            do  {
                event.mouse.where += p;
                moveGrow( origin,
                          event.mouse.where,
                          limits,
                          minSize,
                          maxSize,
                          mode
                        );
                } while( mouseEvent(event,evMouseMove) );
            }
        }
    else
        {
        static TPoint 
            goLeft      =   {-1, 0}, 
            goRight     =   { 1, 0}, 
            goUp        =   { 0,-1}, 
            goDown      =   { 0, 1}, 
            goCtrlLeft  =   {-8, 0}, 
            goCtrlRight =   { 8, 0};
            
        saveBounds = getBounds();
        do  {
            p = origin;
            s = size;
            keyEvent(event);
            switch (event.keyDown.keyCode)
                {
                case kbLeft:
                    change(mode,goLeft,p,s);
                    break;
                case kbRight:
                    change(mode,goRight,p,s);
                    break;
                case kbUp:
                    change(mode,goUp,p,s);
                    break;
                case kbDown:
                    change(mode,goDown,p,s);
                    break;
                case kbCtLeft:
                    change(mode,goCtrlLeft,p,s);
                    break;
                case kbCtRight:
                    change(mode,goCtrlRight,p,s);
                    break;
                // Shift info goes in the key
                case kbShLeft:
                    change(mode,goLeft,p,s,1);
                    break;
                case kbShRight:
                    change(mode,goRight,p,s,1);
                    break;
                case kbShUp:
                    change(mode,goUp,p,s,1);
                    break;
                case kbShDown:
                    change(mode,goDown,p,s,1);
                    break;
                case kbShCtLeft:
                    change(mode,goCtrlLeft,p,s,1);
                    break;
                case kbShCtRight:
                    change(mode,goCtrlRight,p,s,1);
                    break;
                case kbHome:
                    p.x = limits.a.x;
                    break;
                case kbEnd:
                    p.x = limits.b.x - s.x;
                    break;
                case kbPgUp:
                    p.y = limits.a.y;
                    break;
                case kbPgDn:
                    p.y = limits.b.y - s.y;
                    break;
                }
            moveGrow( p, s, limits, minSize, maxSize, mode );
            } while( event.keyDown.keyCode != kbEsc &&
                     event.keyDown.keyCode != kbEnter
                   );
        if( event.keyDown.keyCode == kbEsc )
            locate(saveBounds);
        }
    setState(sfDragging, False);
}

void TView::draw()
{
    TDrawBuffer  b;

    b.moveChar( 0, ' ', getColor(1), size.x );
    writeLine( 0, 0, size.x, size.y, b );
}

void TView::drawCursor()
{
    // SET: do it only if our owner gives permission
    if( (state & sfFocused) != 0 && owner && owner->canShowCursor())
        resetCursor();
}

void TView::drawHide( TView* lastView )
{
    drawCursor();
    drawUnderView(Boolean(state & sfShadow), lastView);
}

void TView::drawShow( TView* lastView )
{
    drawView();
    if( (state & sfShadow) != 0 )
        drawUnderView( True, lastView );
}

void TView::drawUnderRect( TRect& r, TView* lastView )
{
    owner->clip.intersect(r);
    owner->drawSubViews(nextView(), lastView);
    owner->clip = owner->getExtent();
}

void TView::drawUnderView( Boolean doShadow, TView* lastView )
{
    TRect r = getBounds();
    if( doShadow != False )
        r.b += shadowSize;
    drawUnderRect( r, lastView );
}

void TView::drawView()
{
    if (exposed())
        {
        draw();
        drawCursor();
        }
}

void TView::enableCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                ((curCommandSet&commands) != commands) );
    curCommandSet += commands;
}

void TView::enableCommand( ushort command )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                 !curCommandSet.has( command ) );
    curCommandSet += command;
}

void TView::endModal( ushort command )
{
    if( TopView() != 0 )
        TopView()->endModal(command);
}

Boolean  TView::eventAvail()
{
    TEvent event;
    getEvent(event);
    if( event.what != evNothing )
        putEvent(event);
    return Boolean( event.what != evNothing );
}

TRect TView::getBounds()
{
    return TRect( origin, origin+size );
}

ushort  TView::execute()
{
    return cmCancel;
}

TRect TView::getClipRect()
{
    TRect clip = getBounds();
    if( owner != 0 )
        clip.intersect(owner->clip);
    clip.move(-origin.x, -origin.y);
    return clip;
}

ushort TView::getColor( ushort color )
{
    ushort colorPair = color >> 8;

    if( colorPair != 0 )
        colorPair = mapColor(colorPair) << 8;

    colorPair |= mapColor( uchar(color) );

    return colorPair;
}

void TView::getCommands( TCommandSet& commands )
{
    commands = curCommandSet;
}

void TView::getData( void * )
{
}

void TView::getEvent( TEvent& event )
{
    if( owner != 0 )
        owner->getEvent(event);
}

TRect TView::getExtent()
{
    return TRect( 0, 0, size.x, size.y );
}

ushort TView::getHelpCtx()
{
    if( (state & sfDragging) != 0 )
        return hcDragging;
    return helpCtx;
}

TPalette& TView::getPalette() const
{
    static char ch = 0;
    static TPalette palette( &ch, 0 );
    return palette;
}

Boolean TView::getState( ushort aState )
{
    return Boolean( (state & aState) == aState );
}

void TView::growTo( int x, int y )
{
    TRect r = TRect(origin.x, origin.y, origin.x + x, origin.y + y);
    locate(r);
}

void TView::handleEvent(TEvent& event)
{
    if( event.what == evMouseDown )
        {
        if(!(state & (sfSelected | sfDisabled)) && (options & ofSelectable) )
            {
            select();
            if( !(state & sfSelected) || // SET: If we failed to get the focus forget
                                         // about this mouse click.
                !(options & ofFirstClick) )
                clearEvent(event);
            }
        }
}

void TView::hide()
{
    if( (state & sfVisible) != 0 )
        setState( sfVisible, False );
}

void TView::hideCursor()
{
    setState( sfCursorVis, False );
}

void TView::keyEvent( TEvent& event )
{
    do {
       getEvent(event);
        } while( event.what != evKeyDown );
}

#define range(Val, Min, Max)    (((Val < Min) ? Min : ((Val > Max) ? Max : Val)))

void TView::locate( TRect& bounds )
{
    TPoint   min, max;
    sizeLimits(min, max);
    bounds.b.x = bounds.a.x + range(bounds.b.x - bounds.a.x, min.x, max.x);
    bounds.b.y = bounds.a.y + range(bounds.b.y - bounds.a.y, min.y, max.y);
    TRect r = getBounds();
    if( bounds != r )
        {
        changeBounds( bounds );
        if( owner != 0 && (state & sfVisible) != 0 )
            {
            if( (state & sfShadow) != 0 )
                {
                r.Union(bounds);
                r.b += shadowSize;
                }
            drawUnderRect( r, 0 );
            }
        }
}

void TView::makeFirst()
{
    putInFrontOf(owner->first());
}

TPoint TView::makeGlobal( TPoint source )
{
    TPoint temp = source + origin;
    TView *cur = this;
    while( cur->owner != 0 )
        {
        cur = cur->owner;
        temp += cur->origin;
        }
    return temp;
}

TPoint TView::makeLocal( TPoint source )
{
    TPoint temp = source - origin;
    TView* cur = this;
    while( cur->owner != 0 )
        {
        cur = cur->owner;
        temp -= cur->origin;
        }
    return temp;
}

Boolean TView::mouseEvent(TEvent& event, ushort mask)
{
    do {
       getEvent(event);
        } while( !(event.what & (mask | evMouseUp)) );

    return Boolean(event.what != evMouseUp);
}

Boolean TView::mouseInView(TPoint mouse)
{
     mouse = makeLocal( mouse );
     TRect r = getExtent();
     return r.contains(mouse);
}

void TView::moveTo( int x, int y )
{
     TRect r( x, y, x+size.x, y+size.y );
     locate(r);
}

TView *TView::nextView()
{
    if( this == owner->last )
        return 0;
    else
        return next;
}

void TView::normalCursor()
{
    setState(sfCursorIns, False);
}

TView *TView::prev()
{
    TView* res = this;
    while( res->next != this )
        res = res->next;
    return res;
}

TView *TView::prevView()
{
    if( this == owner->first() )
        return 0;
    else
        return prev();
}

void TView::putEvent( TEvent& event )
{
    if( owner != 0 )
        owner->putEvent(event);
}

/**[txh]********************************************************************

  Description:
  Helper function to fill a TEvent structure and call putEvent.
@x{::PutEvent}.
  
***************************************************************************/

void TView::putEvent( ushort what, ushort command, void *infoPtr )
{
    TEvent event;

    event.what = what;
    event.message.command = command;
    event.message.infoPtr = infoPtr;
    putEvent( event );
}

void TView::putInFrontOf( TView *Target )
{
    TView *p, *lastView;

    if( owner != 0 && Target != this && Target != nextView() &&
         ( Target == 0 || Target->owner == owner)
      )
        {
        if( (state & sfVisible) == 0 )
            {
            owner->removeView(this);
            owner->insertView(this, Target);
            }
        else
            {
            lastView = nextView();
            p = Target;
            while( p != 0 && p != this )
                p = p->nextView();
            if( p == 0 )
                lastView = Target;
            state &= ~sfVisible;
            if( lastView == Target )
                drawHide(lastView);
            owner->removeView(this);
            owner->insertView(this, Target);
            state |= sfVisible;
            if( lastView != Target )
                drawShow(lastView);
            if( (options & ofSelectable) != 0 )
                owner->resetCurrent();
            }
        }
}

void TView::select()
{
    if( (options & ofTopSelect) != 0 )
        makeFirst();
    else if( owner != 0 )
        owner->setCurrent( this, normalSelect );
}

void TView::setBounds( const TRect& bounds )
{            
    origin = bounds.a;
    size = bounds.b - bounds.a;
}

void TView::setCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                (curCommandSet != commands ));
    curCommandSet = commands;
}

void TView::setCursor( int x, int y )
{
    cursor.x = x;
    cursor.y = y;
    drawCursor();
}

void TView::setData( void * )
{
}

void TView::setState( ushort aState, Boolean enable )
{
    if( enable == True )
        state |= aState;
    else
        state &= ~aState;

    if( owner == 0 )
        return;

    switch( aState )
        {
        case  sfVisible:
            if( (owner->state & sfExposed) != 0 )
                setState( sfExposed, enable );
            if( enable == True )
                drawShow( 0 );
            else
                drawHide( 0 );
            if( (options & ofSelectable) != 0 )
                owner->resetCurrent();
            break;
        case  sfCursorVis:
        case  sfCursorIns:
            drawCursor();
            break;
        case  sfShadow:
            drawUnderView( True, 0 );
            break;
        case  sfFocused:
            if (owner && owner->canShowCursor())
               // SET: do it only if our owner gives permission
               resetCursor();
            message( owner,
                     evBroadcast,
                     (enable == True) ? cmReceivedFocus : cmReleasedFocus,
                     this
                   );
            break;
        }
}

void TView::show()
{
    if( (state & sfVisible) == 0 )
        setState(sfVisible, True);
}

void TView::showCursor()
{
    setState( sfCursorVis, True );
}

void TView::sizeLimits( TPoint& min, TPoint& max )
{
    min.x = min.y = 0;
    if( owner != 0 )
        max = owner->size;
    else
        max.x = max.y = INT_MAX;
}

TView* TView::TopView()
{
    if( TheTopView != 0 )
        return TheTopView;
    else
        {
        TView* p = this;
        while( p != 0 && !(p->state & sfModal) )
            p = p->owner;
        return p;
        }
}

Boolean TView::valid( ushort )
{
    return True;
}

Boolean TView::containsMouse( TEvent& event )
{
    return Boolean( (state & sfVisible) != 0 &&
                    mouseInView( event.mouse.where )
                  );
}

void TView::shutDown()
{
    hide();
    if( owner != 0 )
        owner->remove( this );
    TObject::shutDown();
}

#if !defined( NO_STREAM )
void TView::write( opstream& os )
{
    ushort saveState =
        state & ~( sfActive | sfSelected | sfFocused | sfExposed );

    os << origin << size << cursor
       << growMode << dragMode << helpCtx
       << saveState << options << eventMask;
}

void *TView::read( ipstream& is )
{
    is >> origin >> size >> cursor
       >> growMode >> dragMode >> helpCtx
       >> state >> options >> eventMask;
    owner = 0;
    next = 0;
    return this;
}

TStreamable *TView::build()
{
    return new TView( streamableInit );
}

TView::TView( StreamableInit )
{
}
#endif // NO_STREAM

uchar TView::mapColor( uchar color )
{
    if( color == 0 )
        return errorAttr;
    TView *cur = this;
    do  {
        TPalette& p = cur->getPalette();
        if( p[0] != 0 )
            {
            if( color > p[0] )
                return errorAttr;
            color = p[color];
            if( color == 0 )
                return errorAttr;
            }
        cur = cur->owner;
        } while( cur != 0 );
    return color;
}

/**[txh]********************************************************************

  Description: 
   This routine enables/disables the screen cursor.
   Two details are important:
   1) If our object is really visible (not under another)
   2) If our state indicates the cursor is visible or not.
   The routine does a search climbing to the owners until the TView that
  have the screen (owner==0) is reached or we determine we are under
  another view and hence the cursor isn't visible.
  SET: I recoded it for clarity.
  
***************************************************************************/

void TView::resetCursor()
{
 int x,y,lookNext=1;
 TView *target=this,*view;

 // If not visible or not focused or cursor not visible (unless never hide)
 // then skip it
 if (((~state) & (sfVisible /*| sfCursorVis*/ | sfFocused))==0 &&
     !(TScreen::getDontMoveHiddenCursor() && ((~state) & sfCursorVis)))
   {
    y = cursor.y;
    x = cursor.x;
    // While the cursor is inside the target
    while ( lookNext &&
            ((x>=0) && (x<target->size.x)) &&
            ((y>=0) && (y<target->size.y)) )
      {
       y += target->origin.y;
       x += target->origin.x;
       if (!target->owner)
         { // Target is the one connected to the screen, set the screen cursor
          TScreen::setCursorPos(x,y);
          if (state & sfCursorVis)
            {
             int curShape=TScreen::cursorLines;
             if (state & sfCursorIns)
                curShape=100*256;
             TScreen::setCursorType(curShape);
            }
          else
             TScreen::setCursorType(0);
          return;
         }
       // Analyze target->owner unless the coordinate is over another object
       // that belongs to the owner.
       lookNext = 0;
       view = target->owner->last;
       do
         {
          view = view->next;
          if (view == target)
            { // Ok x,y is inside target and nobody is over it.
             target = view->owner;
             lookNext = 1;
             break;
            }
         }
       while (!(view->state & sfVisible) ||
              y<view->origin.y ||
              y>=view->origin.y+view->size.y ||
              x<view->origin.x ||
              x>=view->origin.x+view->size.x);
      }
   }
 // Cursor disabled
 TScreen::setCursorType(0);
 return;
}

static Boolean lineExposed(TView *view, int Line, int x1, int x2, TView *target=NULL);

/**[txh]********************************************************************

  Description:
  Finds if the area from x1 to x2 in the indicated line is exposed.
  Note that sometimes the x1 - x2 range can be partially overlapped and we
must split the search in two. In this case the routine calls itself
providing a value for the target TView so we know that's just a continuation
and the initialization is skipped.
  
  Return:
  True if exposed, false if not.
  
***************************************************************************/

static
Boolean lineExposed(TView *view, int line, int x1, int x2, TView *target)
{
 int Xtest,Ytest;

 while (1)
   {
    if (!target)
      {// This is a call to start searching, we must initialize
       target=view;
       // If no owner we are the view attached to the screen -> we are exposed
       if (!view->owner)
          return True;
       // Make coordinates relative to the owner
       line+=view->origin.y;
       x1+=view->origin.x;
       x2+=view->origin.x;
     
       // Apply clipping, and check if the coordinate gets outside
       TRect &clip=view->owner->clip;
       if (line<clip.a.y || line>=clip.b.y)
          return False;
       if (x1<clip.a.x)
          x1=clip.a.x;
       if (x2>clip.b.x)
          x2=clip.b.x;
       if (x1>=x2)
          return False;
     
       // Go to last in the owner's list
       view=view->owner->last;
      }
  
    while (1)
      {
       view=view->next;
       if (view==target)
         {// No other TView is overlapping us
          // If our owner is buffered report exposed to draw in the buffer
          if (view->owner->buffer) return True;
          // If not work with the owner
          view=view->owner;
          target=NULL;
          break;
         }
     
       // If not visible forget it
       if (!(view->state & sfVisible)) continue;
     
       // Check the Y range
       Ytest=view->origin.y;
       if (line<Ytest)  continue;
       Ytest+=view->size.y;
       if (line>=Ytest) continue;
     
       // Check the X range
       Xtest=view->origin.x;
       if (x1>=Xtest)
         {
          Xtest+=view->size.x;
          if (x1>=Xtest) continue;
          // This object overlaps, reduce the x range
          x1=Xtest;
          if (x1<x2) continue;
          // It was reduced to nothing
          return False;
         }
       if (x2<=Xtest) continue;
       // This object overlaps
       Xtest+=view->size.x;
       if (x2<=Xtest)
         {// Reduce the x range
          x2=view->origin.x;
          continue;
         }
       // The object partially overlaps generating two segments
       // So call to analyze x1 to view->origin.x
       if (lineExposed(view,line,x1,view->origin.x,target))
          return True;
       // and then continue with view->origin.x+view->size.x to x2
       x1=Xtest;
      }
   }
}

Boolean TView::exposed()
{
  if (!(state & sfExposed) ||
      size.x<0 || size.y<0) return False;

  // Check each line, if at least one is exposed we are exposed
  int line=0;
  do
    {
     if (lineExposed(this,line,0,size.x))
        return True;
     line++;
    }
  while (line<size.y);

  return False;
}

static
void blitBuffer(TView *view, int line, int xStart, int xEnd, int offset,
                const ushort *buffer, int inShadow)
{
 int count=xEnd-xStart;
 int destOffset=line*view->size.x+xStart;
 int skipOffset=xStart-offset;
 const ushort *toBlit;
 TGroup *group=(TGroup *)view;
 int isScreen=group->buffer==TScreen::screenBuffer;

 if (TDisplay::getDrawingMode()==TDisplay::unicode16)
   {// The display uses Unicode
    toBlit=buffer+skipOffset*2;
    AllocLocalUShort(aux,count*2*2);
    if (inShadow)
      {// is much more efficient to call the OS just once
       memcpy(aux,toBlit,count*2*2);
       int i;
       for (i=0; i<count; i++)
           aux[i*2+1]=shadowAttr;
       toBlit=(const ushort *)aux;
      }
    if (isScreen)
       TScreen::setCharacters(destOffset,(ushort *)toBlit,count);
    else
       memcpy(group->buffer+destOffset*2,toBlit,count*sizeof(ushort)*2);
   }
 else
   {
    toBlit=buffer+skipOffset;
    // Remap characters if needed
    AllocLocalStr(aux,count*sizeof(ushort));
    if (isScreen && TVCodePage::OnTheFlyRemapNeeded())
      {
       memcpy(aux,toBlit,count*sizeof(ushort));
       int i;
       if (inShadow)
         {// Remap and shadow
          for (i=0; i<count; i++)
             {
              uchar *s=((uchar *)aux)+i*2;
              *s=TVCodePage::OnTheFlyRemap(*s);
              s[1]=shadowAttr;
             }
         }
       else
         {// Just remap
          for (i=0; i<count; i++)
             {
              uchar *s=((uchar *)aux)+i*2;
              *s=TVCodePage::OnTheFlyRemap(*s);
             }
         }
       toBlit=(const ushort *)aux;
      }
    else
      {// We don't need to remap, but ...
       if (inShadow)
         {// is much more efficient to call the OS just once
          memcpy(aux,toBlit,count*sizeof(ushort));
          int i;
          for (i=0; i<count; i++)
             {
              uchar *s=((uchar *)aux)+i*2;
              s[1]=shadowAttr;
             }
          toBlit=(const ushort *)aux;
         }
      }
   
    if (isScreen)
       TScreen::setCharacters(destOffset,(ushort *)toBlit,count);
    else
       memcpy(group->buffer+destOffset,toBlit,count*sizeof(ushort));
   }
}

#ifdef TVCompf_MinGW
// SET: Not sure why Vadim wanted it
#define HideMouse()
#define ShowMouse()
#else
// DOS, Linux and Borland (SAA port)
#define HideMouse()  TMouse::hide()
#define ShowMouse()  TMouse::show()
#endif

static
void WriteView(int xStart, int line, int xEnd, const void *buffer,
               TView *view, int offset, int inShadow, TView *target)
{
  int x,y,skipInit=0;

  if (!target)
    {// Initial call so initialize
     // Check line is valid
     if (line<0 || line>=view->size.y) return;
     // Validate x range
     if (xStart<0) xStart=0;
     if (xEnd>view->size.x) xEnd=view->size.x;
     if (xStart>=xEnd) return;
     // Initialize values
     offset=xStart;
     inShadow=0;
     skipInit=0;
    }
  else
     skipInit=1;

  do
    {
     if (skipInit)
        skipInit=0;
     else
       {// Pass to the owner or init if that's the first call
        if (!(view->state & sfVisible) ||
            !view->owner) return;
      
        // Make coordinates relative to the owner
        line+=view->origin.y;
        x=view->origin.x;
        xStart+=x;
        xEnd  +=x;
        offset+=x;
      
        // Apply clipping, and check if the coordinate gets outside
        TRect &clip=view->owner->clip;
        if (line<clip.a.y || line>=clip.b.y) return;
        if (xStart<clip.a.x)
           xStart=clip.a.x;
        if (xEnd>clip.b.x)
           xEnd=clip.b.x;
        if (xStart>=xEnd) return;
       
        target=view;
        view=view->owner->last;
       }

     do
       {
        view=view->next;
        // We are visible go to the owner
        if (view==target) break;
        // Honor the sfVisible bit
        if (!(view->state & sfVisible)) continue;

        // Check the y range
        y=view->origin.y;
        if (line<y) continue;
        y+=view->size.y;
        if (line>=y)
          {// The line is outside, now check for the shadow
           if (!(view->state & sfShadow)) continue;
           y+=shadowSize.y;
           if (line>=y) continue;
           // We are in the shadow line
           x=view->origin.x;
           x+=shadowSize.x;
           if (xStart<x)
             {
              if (xEnd<=x) continue;
              // We are under a shadow. Do the part that isn't under.
              WriteView(xStart,line,x,buffer,view,offset,inShadow,target);
              // Now the rest
              xStart=x;
             }
           x+=view->size.x;
          }
        else
          {// The line is inside, check the X range
           x=view->origin.x;
           if (xStart<x)
             {
              if (xEnd<=x) continue;
              // Do the xStart to view->origin.x part
              WriteView(xStart,line,x,buffer,view,offset,inShadow,target);
              // Now the rest
              xStart=x;
             }
           x+=view->size.x;
           if (xStart<x)
             {
              if (xEnd<=x) return;
              // Overlapped, reduce the size
              xStart=x;
             }
           if (!(view->state & sfShadow)) continue;
           // Now add the shadow
           if (line<view->origin.y+shadowSize.y) continue;
           x+=shadowSize.x;
          }
        // This part deals with the part that can be under the shadow
        if (xStart>=x) continue; // No in shadow
        inShadow++;
        if (xEnd<=x) continue;   // Full in shadow
        // Partially under a shadow, do the shadow part
        WriteView(xStart,line,x,buffer,view,offset,inShadow,target);
        // and now the rest.
        xStart=x;
        inShadow--;
       }
     while (1);

     // We get here if we found a portion that can be exposed and need to
     // check in the owner.
     TGroup *owner=view->owner;
     view=owner;
     // If the owner is unbuffered ...
     if (!owner->buffer)
       { // and locked avoid drawing
        if (owner->lockFlag) return;
        // else go deeper
        continue;
       }
     // If the owner's buffer isn't the screen do the blit
     if (owner->buffer!=TScreen::screenBuffer)
       {
        blitBuffer(view,line,xStart,xEnd,offset,((const ushort *)buffer),inShadow);
        // If locked stop here
        if (owner->lockFlag) return;
        continue;
       }
     // We are here because the owner is buffered and attached to the screen
     if (line!=TEventQueue::curMouse.where.y ||
         xStart>TEventQueue::curMouse.where.x ||
         xEnd<=TEventQueue::curMouse.where.x)
       {// The mouse is not in the draw area
        TMouse::resetDrawCounter();
        blitBuffer(view,line,xStart,xEnd,offset,((const ushort *)buffer),inShadow);
        if (TMouse::getDrawCounter()==0)
          {// There was no mouse event
           if (owner->lockFlag) return;
           continue;
          }
       }
     // The mouse is in the draw area or an event has occoured during
     // the above drawing
     HideMouse();
     blitBuffer(view,line,xStart,xEnd,offset,((const ushort *)buffer),inShadow);
     ShowMouse();
     if (owner->lockFlag) return;
    }
  while (1);
}

#define writeView(b,a,c,B) WriteView(b,a,b+c,B,this,0,0,0)

// That's the way to call the function getting conversion
void TView::writeBuf(int x, int y, int w, int h, TDrawBufferBase& b)
{
 if (b.getType()==TDisplay::getDrawingMode())
   {
    writeNativeBuf(x,y,w,h,b.getBuffer());
    return;
   }
 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// Buffer is unicode and screen codepage
    unsigned elements=w*h;
    AllocLocalStr(dest,elements*2);
    TVCodePage::convertBufferU16_2_CP(dest,b.getBuffer(),elements);
    writeNativeBuf(x,y,w,h,dest);
    return;
   }
 // Buffer is codepage and screen unicode
 unsigned elements=w*h;
 AllocLocalStr(dest,elements*2*2);
 TVCodePage::convertBufferCP_2_U16(dest,(char *)b.getBuffer(),elements);
 writeNativeBuf(x,y,w,h,dest);
}

// Called by old code using codepage encoding
void TView::writeBuf(int x, int y, int w, int h, const void *Buffer)
{
 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// The buffer is in native mode
    writeNativeBuf(x,y,w,h,Buffer);
    return;
   }
 // We have to convert it into an Unicode 16 buffer
 unsigned elements=w*h;
 AllocLocalStr(dest,elements*2*2);
 TVCodePage::convertBufferCP_2_U16(dest,(char *)Buffer,elements);
 writeNativeBuf(x,y,w,h,dest);
}

// Used by new code that uses a buffer according to the mode
void TView::writeNativeBuf(int x, int y, int w, int h, const void *Buffer)
{
 int i=0;
 unsigned wB=w;
 if (TDisplay::getDrawingMode()==TDisplay::unicode16)
    wB*=2;
 uint16 *b=(uint16 *)Buffer;
 while (h--)
   {
    writeView(x,y++,w,b);
    b+=wB;
    i++;
   }
}

void TView::writeLine(int x, int y, int w, int h, TDrawBufferBase& b)
{
 if (b.getType()==TDisplay::getDrawingMode())
   {
    writeNativeLine(x,y,w,h,b.getBuffer());
    return;
   }
 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// Buffer is unicode and screen codepage
    AllocLocalStr(dest,w*2);
    TVCodePage::convertBufferU16_2_CP(dest,b.getBuffer(),w);
    writeNativeLine(x,y,w,h,dest);
    return;
   }
 // Buffer is codepage and screen unicode
 AllocLocalStr(dest,w*2*2);
 TVCodePage::convertBufferCP_2_U16(dest,b.getBuffer(),w);
 writeNativeLine(x,y,w,h,dest);
}

void TView::writeLine(int x, int y, int w, int h, const void *Buffer)
{
 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// The buffer is in native mode
    writeNativeLine(x,y,w,h,Buffer);
    return;
   }
 // We have to convert it into an Unicode 16 buffer
 AllocLocalStr(dest,w*2*2);
 TVCodePage::convertBufferCP_2_U16(dest,Buffer,w);
 writeNativeLine(x,y,w,h,dest);
}

void TView::writeNativeLine(int x, int y, int w, int h, const void *b)
{
  while (h--)
    writeView(x,y++,w,b);
}


void TView::writeChar(int x, int y, char c, uchar color, int count)
{
 if (count<=0)
    return;
 if (TDisplay::getDrawingMode()==TDisplay::unicode16)
   {// Not in native mode
    writeCharU16(x,y,TVCodePage::convertCP_2_U16(c),color,count);
    return;
   }
 // Native mode
 //uint8 cell[2];
 //cell[0]=c;
 //cell[1]=mapColor(color);
 //uint16 cell16=*((uint16 *)cell);
 uint16 cell16=c|((mapColor(color))<<8);

 int i=0;
 AllocLocalUShort(temp,count*2);
 for (i=0; i<count; i++)
     temp[i]=cell16;

 writeView(x,y,count,temp);
}

#if defined(TV_BIG_ENDIAN)
 #define endianCol(letra,color) ((((uint32)letra)<<16) | ((uint32)color))
#else
 #define endianCol(letra,color) ((((uint32)color)<<16) | ((uint32)letra))
#endif

void TView::writeCharU16(int x, int y, unsigned c, unsigned color, int count)
{
 if (count<=0)
    return;
 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// Not in native mode
    writeCharU16(x,y,TVCodePage::convertU16_2_CP(c),color,count);
    return;
   }
 // Native mode
 uint32 cell32=endianCol(c,mapColor(color));
 int i=0;
 uint32 *temp=(uint32 *)alloca(count*4);
 for (i=0; i<count; i++)
     temp[i]=cell32;

 writeView(x,y,count,temp);
}

void TView::writeStr(int x, int y, const char *str, uchar color)
{
 int count=strlen(str),i;
 if (!count)
    return;
 AllocLocalStr(temp,(count+1)*2);

 if (TDisplay::getDrawingMode()==TDisplay::unicode16)
   {// Not in native mode
    TVCodePage::convertStrCP_2_U16((uint16 *)temp,str,count);
    writeStrU16(x,y,(uint16 *)temp,color);
    return;
   }

 color=mapColor(color);
 for (i=0; i<count; i++)
    {
     temp[i*2]=str[i];
     temp[i*2+1]=color;
    }
 writeView(x,y,count,temp);
}

static
int strlenU16(const uint16 *str)
{
 int len=0;
 while (*str++)
    len++;
 return len;
}

void TView::writeStrU16(int x, int y, const uint16 *str, unsigned color)
{
 int count=strlenU16(str),i;
 if (!count)
    return;

 if (TDisplay::getDrawingMode()==TDisplay::codepage)
   {// Not in native mode
    AllocLocalStr(temp,count+1);
    TVCodePage::convertStrU16_2_CP(temp,str,count);
    writeStr(x,y,temp,color);
    return;
   }

 AllocLocalUShort(temp,count*4);
 color=mapColor(color);
 for (i=0; i<count; i++)
    {
     temp[i*2]=str[i];
     temp[i*2+1]=color;
    }
 writeView(x,y,count,temp);
}

