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
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string
#define Uses_stdio
#define Uses_AllocLocal
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

void TView::putInFrontOf( TView *Target )
{
    TView *p, *lastView;

    if( owner != 0 && Target != this && Target != nextView() &&
         ( Target == 0 || Target->owner == owner)
      )
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

void TView::resetCursor()
{
  int ax,cx,dx;
  TView *self=this,*view;
  if ((~state) & (sfVisible /*| sfCursorVis*/ | sfFocused)) goto lab4;
  if (TScreen::getDontMoveHiddenCursor() && ((~state) & sfCursorVis)) goto lab4;
  ax = cursor.y;
  dx = cursor.x;
lab1:
  if ( (ax<0) || (ax>=self->size.y) ) goto lab4;
  if ( (dx<0) || (dx>=self->size.x) ) goto lab4;
  ax += self->origin.y;
  dx += self->origin.x;
  if (!self->owner) goto lab5;
  view = self->owner->last;
lab2:
  view = view->next;
  if (view == self)
  {
    self = view->owner;
    goto lab1;
  }
  if (!(view->state & sfVisible)) goto lab2;
  if (ax<view->origin.y) goto lab2;
  if (ax>=view->origin.y+view->size.y) goto lab2;
  if (dx<view->origin.x) goto lab2;
  if (dx>=view->origin.x+view->size.x) goto lab2;
lab4:
  // Cursor disabled
  TScreen::setCursorType(0);
  return;

lab5:
  TScreen::setCursorPos(dx,ax);
  if (state & sfCursorVis)
    {
     cx=TScreen::cursorLines;
     if (state & sfCursorIns)
        cx=100*256;
     TScreen::setCursorType(cx);
    }
  else
     TScreen::setCursorType(0);
}

#define VIEW ((TGroup *)(view))

static TView *target;

enum call_lab {lab_10,lab_11,lab_20};
Boolean call(call_lab LAB,TView * &view,int &ax,int &bx,int &cx,int &si);

Boolean call(call_lab LAB,TView * &view,int &ax,int &bx,int &cx,int &si)
{
  TView *retttarget,*rettview;
  int rettax,rettcx,rettsi;
  Boolean flag;
  switch (LAB)
  {
    case lab_10 : goto lab10;
    case lab_11 : goto lab11;
    case lab_20 : goto lab20;
  }
  return False;
lab10:
  view = view->owner;
  if (VIEW->buffer) return True;
lab11:
  target = view;
  ax += view->origin.y;
  si = view->origin.x;
  bx += si;
  cx += si;
  view = view->owner;
  if (!view) return True;
  if (ax<VIEW->clip.a.y) return False;
  if (ax>=VIEW->clip.b.y) return False;
  if (bx<VIEW->clip.a.x) bx = VIEW->clip.a.x;
  if (cx>VIEW->clip.b.x) cx = VIEW->clip.b.x;
  if (bx>=cx) return False;
  view = VIEW->last;
lab20:
  view = view->next;
  if (view == target) goto lab10;
  if (!(view->state & sfVisible)) goto lab20;
  si = view->origin.y;
  if (ax<si) goto lab20;
  si += view->size.y;
  if (ax>=si) goto lab20;
  si = view->origin.x;
  if (bx<si) goto lab22;
  si += view->size.x;
  if (bx>=si) goto lab20;
  bx = si;
  if (bx<cx) goto lab20;
  return False;
lab22:
  if (cx<=si) goto lab20;
  si += view->size.x;
  if (cx>si) goto lab23;
  cx = view->origin.x;
  goto lab20;
lab23:
  retttarget = target;
  rettview = view;
  rettsi = si;
  rettcx = cx;
  rettax = ax;
  cx = view->origin.x;
  flag = call(lab_20,view,ax,bx,cx,si);
  ax = rettax;
  cx = rettcx;
  bx = rettsi;
  view = rettview;
  target = retttarget;
  if (!flag) goto lab20;
  return True;
}

Boolean TView::exposed()
{
  int ax,bx,cx,si;
  int rettax;
  Boolean flag;
  TView *view = this;
  if (!(state & sfExposed)) return False;
  ax = 0;
  if (size.x<ax || size.y<ax) return False;
lab1:
  bx = 0;
  cx = view->size.x;
  rettax = ax;
  flag = call(lab_11,view,ax,bx,cx,si);
  ax = rettax;
  if (flag) return True;
  view = this;
  ax++;
  if (ax<view->size.y) goto lab1;
  return False;
}

extern TPoint shadowSize;
extern uchar shadowAttr;

static int offset;
static int y_pos,x_pos_start,x_pos_end,in_shadow;
const void * _Buffer;
TView *_view;

void _call(int LAB);

void call30(int x)
{
  TView *retttarget = target;
  int rettoffset = offset;
  TView *rettview = _view;
  int rettdx = in_shadow;
  int rettcx = x_pos_end;
  int rettax = y_pos;
  x_pos_end = x;
  _call(20);
  y_pos = rettax;
  x_pos_end = rettcx;
  in_shadow = rettdx;
  _view = rettview;
  offset = rettoffset;
  target = retttarget;
  x_pos_start = x;
}

void call50()
{
  int count = x_pos_end - x_pos_start;
  int buf_offset = y_pos * _view->size.x + x_pos_start;
  int skip_offset = x_pos_start - offset;
  const ushort *toBlit=((const ushort *)_Buffer)+skip_offset;
  int isScreen=((TGroup *)(_view))->buffer==TScreen::screenBuffer;

  // Remap characters if needed
  AllocLocalStr(aux,count*sizeof(ushort));
  if (isScreen && TVCodePage::OnTheFlyRemapNeeded())
    {
     memcpy(aux,toBlit,count*sizeof(ushort));
     int i;
     if (in_shadow)
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
     if (in_shadow)
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
     TScreen::setCharacters(buf_offset,(ushort *)toBlit,count);
  else
     memcpy(((TGroup *)(_view))->buffer+buf_offset,toBlit,count*sizeof(ushort));
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

void _call(int LAB)
{
  int x;
  switch (LAB)
  {
    case 00 : goto lab00;
    case 20 : goto lab20;
  }
  return;
lab00:
  offset = x_pos_start;
  x_pos_end += x_pos_start;
  in_shadow = 0;
  if (y_pos < 0 || y_pos >= _view->size.y) return;
  x_pos_start = max(x_pos_start,0);
  x_pos_end = min(x_pos_end,_view->size.x);
  if (x_pos_start >= x_pos_end) return;
lab10:
  if ( !( _view->state & sfVisible) ) return;
  if (!_view->owner) return;
  target = _view;
  y_pos += _view->origin.y;
  x = _view->origin.x;
  x_pos_start += x;
  x_pos_end += x;
  offset += x;
  if (y_pos < _view->owner->clip.a.y) return;
  if (y_pos >= _view->owner->clip.b.y) return;
  x_pos_start = max(x_pos_start,_view->owner->clip.a.x);
  x_pos_end = min(x_pos_end,_view->owner->clip.b.x);
  if (x_pos_start >= x_pos_end) return;
  _view = _view->owner->last;
lab20:
  do
  {
    _view = _view->next;
    if (_view == target) break;
    if (!(_view->state & sfVisible)) continue;
    {
      int y = _view->origin.y;
      if (y_pos < y) continue;
      y += _view->size.y;
      if (y_pos < y) goto lab23;
      if (!(_view->state & sfShadow)) continue;
      y += shadowSize.y;
      if (y_pos >= y) continue;
    }
    x = _view->origin.x;
    x += shadowSize.x;
    if (x_pos_start >= x) goto lab22;
    if (x_pos_end <= x) continue;
    call30(x);
  lab22:
    x += _view->size.x;
    goto lab26;
  lab23:
    x = _view->origin.x;
    if (x_pos_start < x)
    {
      if (x_pos_end <= x) continue;
      call30(x);
    }
    x += _view->size.x;
    if (x_pos_start < x)
    {
      if (x_pos_end <= x) return;
      x_pos_start = x;
    }
    if (!(_view->state & sfShadow)) continue;
    if (y_pos < _view->origin.y+shadowSize.y) continue;
    x += shadowSize.x;
  lab26:
    if (x_pos_start >= x) continue;
    in_shadow++;
    if (x_pos_end <= x) continue;
    call30(x);
    in_shadow--;
  } while (1);

  _view = _view->owner;
  if (!((TGroup *)(_view))->buffer)
  {
    if (((TGroup *)(_view))->lockFlag) return;
    goto lab10;
  }
  if ((((TGroup *)(_view))->buffer) != TScreen::screenBuffer)
  {
    call50();
    if (((TGroup *)(_view))->lockFlag) return;
    goto lab10;
  }
  if (y_pos!=TEventQueue::curMouse.where.y ||
      x_pos_start>TEventQueue::curMouse.where.x ||
      x_pos_end<=TEventQueue::curMouse.where.x)
  // the mouse is not in the draw area
  {
    TMouse::resetDrawCounter();
    call50();
    if (TMouse::getDrawCounter()==0)
    {
      // there was no mouse event
      if (((TGroup *)(_view))->lockFlag) return;
      goto lab10;
    }
  }
  // the mouse is in the draw area or an event has occoured during
  // the above drawing
  HideMouse();
  call50();
  ShowMouse();
  if (((TGroup *)(_view))->lockFlag) return;
  goto lab10;
}

struct MYARGS {
  ushort ax;
  ushort bx;
  ushort cx;
  void * Buffer;
  TView * view;
};

#define WRITEVIEW(b,a,c,B)\
do\
{\
  x_pos_start = b;\
  y_pos = a;\
  x_pos_end = c;\
  _Buffer = B;\
  _view = this;\
  _call(00);\
} while (0)

void TView::writeView(write_args wa)
{
  MYARGS * WA = (MYARGS *)&wa;
  x_pos_start = WA->bx;
  y_pos = WA->ax;
  x_pos_end = WA->cx;
  _Buffer = WA->Buffer;
  _view = WA->view;
  _call(00);
}

void TView::writeBuf(short x,short y,short w,short h,const void * Buffer)
{
  int i=0;
  while (h--)
  {
    WRITEVIEW(x,y++,w,((ushort *)(Buffer))+w*i);
    i++;
  }
}

void TView::writeChar( short x, short y, char c, uchar color, short count )
{
  ushort colo = (mapColor(color) << 8) | (uchar)c;
  int i=0;
  if (count<=0) return;
  AllocLocalUShort(temp,count*sizeof(ushort));
  for (i=0;i<count;i++) temp[i]=colo;
  WRITEVIEW(x,y,count,temp);
}

void TView::writeLine( short x, short y, short w, short h, const void *Buffer )
{
  while (h--) WRITEVIEW(x,y++,w,((ushort *)(Buffer)));
}

void TView::writeStr( short x, short y, const char *str, uchar color )
{
  ushort count = strlen(str),i;
  if (!count) return;
  AllocLocalUShort(temp,count*sizeof(ushort));
  color = mapColor(color);
  for (i=0;i<count;i++) temp[i] = (color << 8) | (uchar)str[i];
  WRITEVIEW(x,y,count,temp);
}

