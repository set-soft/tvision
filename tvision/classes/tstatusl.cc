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

#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#define Uses_TKeys
#include <tv.h>

#define cpStatusLine "\x02\x03\x04\x05\x06\x07"

TStatusLine::TStatusLine( const TRect& bounds, TStatusDef& aDefs ) :
    TView( bounds ),
    defs( &aDefs )
{
    options |= ofPreProcess;
    eventMask |= evBroadcast;
    growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
    findItems();
    computeLength();
}

/**[txh]********************************************************************

  Description:
  This routine computes the length of the status line, if that's greater than
the size.x the status line becomes compacted to allow more options.@*
  Added by SET.

***************************************************************************/

void TStatusLine::computeLength()
{
    int l = 0;
    TStatusItem *p;

    if( items != 0 )
        {
        p = items;
        while( p != 0 )
            {
            if( p->text != 0 )
                l += cstrlen(TVIntl::getText(p->text,p->intlText)) + 2;
            p = p->next;
            }
        }
    compactStatus = l > size.x;
}

/**[txh]********************************************************************

  Description:
  Calls TView::changeBounds, additionally re-computes the length of the
line to select the no/compact mode.@*
  Added by SET.

***************************************************************************/

void TStatusLine::changeBounds(const TRect& bounds)
{
 TView::changeBounds(bounds);
 int oldCompact=compactStatus;
 computeLength();
 if (compactStatus!=oldCompact)
    draw();
}

void TStatusLine::disposeItems( TStatusItem *item )
{
    while( item != 0 )
        {
        TStatusItem *T = item;
        item = item->next;
        delete(T);
        }
}

TStatusLine::~TStatusLine(void)
{
    while( defs != 0 )
        {
        TStatusDef *T = defs;
        defs = defs->next;
        disposeItems( T->items );
        delete(T);
        }
}

void TStatusLine::draw()
{
    drawSelect( 0 );
}

void TStatusLine::drawSelect( TStatusItem *selected )
{
    TDrawBuffer b;
    ushort color;
    char hintBuf[256];

    ushort cNormal = getColor(0x0301);
    ushort cSelect = getColor(0x0604);
    ushort cNormDisabled = getColor(0x0202);
    ushort cSelDisabled = getColor(0x0505);
    b.moveChar( 0, ' ', cNormal, size.x );
    TStatusItem *T = items;
    int i = 0, inc = (compactStatus ? 1 : 2); // SET

    while( T != 0 )
        {
        if( T->text != 0 )
            {
            int l = cstrlen( TVIntl::getText(T->text,T->intlText) );
            if( i + l < size.x )
                {
                if( commandEnabled( T->command) )
                    if( T == selected )
                        color = cSelect;
                    else
                        color = cNormal;
                else
                    if( T == selected )
                        color = cSelDisabled;
                    else
                        color = cNormDisabled;

                b.moveChar( i, ' ', color, 1 );
                b.moveCStr( i+1, TVIntl::getText(T->text,T->intlText), color );
                b.moveChar( i+l+1, ' ', color, 1 );
                }
            i += l + inc;
            }
        T = T->next;
        }
    if (size.y == 1)
    {
      if( i < size.x - 2 )
        {
        strcpy( hintBuf, hint( helpCtx ) );
        if( *hintBuf != EOS )
            {
            b.moveStr( i, hintSeparator, cNormal );
            i += 2;
            if( (int32)strlen(hintBuf) + i > size.x )
                hintBuf[size.x-i] = EOS;
            b.moveCStr( i, hintBuf, cNormal );
            }
        }
      writeLine( 0, 0, size.x, 1, b );
    }
    else
    {
      writeLine( 0, 0, size.x, 1, b );
      strcpy( hintBuf, hint( helpCtx ) );
      hintBuf[size.x] = 0;
      b.moveChar(0, ' ', cNormal, size.x);
      b.moveCStr(0, hintBuf, cNormal);
      writeLine( 0, 1, size.x, 1, b );
    }
}

void TStatusLine::findItems()
{
    TStatusDef *p = defs;
    while( p != 0 && ( helpCtx < p->min || helpCtx > p->max ) )
        p = p->next;
    items = ( p == 0 ) ? 0 : p->items;
}

TPalette& TStatusLine::getPalette() const
{
    static TPalette palette( cpStatusLine, sizeof( cpStatusLine )-1 );
    return palette;
}

TStatusItem *TStatusLine::itemMouseIsIn( TPoint mouse )
{
    if( mouse.y !=  0 )
        return 0;

    int i, inc = (compactStatus ? 1 : 2); // SET
    TStatusItem *T;

    for( i = 0, T = items; T != 0; T = T->next)
        {
        if( T->text != 0 )
            {
            int k = i + cstrlen(TVIntl::getText(T->text,T->intlText)) + inc;
            if( mouse.x >= i && mouse. x < k )
                return T;
            i = k;
            }
        }
    return 0;
}

void TStatusLine::handleEvent( TEvent& event )
{
    TView::handleEvent(event);

    switch (event.what)
        {
        case  evMouseDown:
            {
            TStatusItem *T = 0;

            do  {
                TPoint mouse = makeLocal( event.mouse.where );
                if( T != itemMouseIsIn(mouse) )
                    drawSelect( T = itemMouseIsIn(mouse) );
                } while( mouseEvent( event, evMouseMove ) );

            if( T != 0 && commandEnabled(T->command) )
                {
                event.what = evCommand;
                event.message.command = T->command;
                event.message.infoPtr = 0;
                putEvent(event);
                }
            clearEvent(event);
            drawView();
            break;
            }
        case evKeyDown:
            {
            for( TStatusItem *T = items; T != 0; T = T->next )
                {
                if( T->keyCode != kbNoKey &&
                    event.keyDown.keyCode ==  T->keyCode &&
                    commandEnabled(T->command))
                    {
                    event.what = evCommand;
                    event.message.command = T->command;
                    event.message.infoPtr = 0;
                    return;
                    }
            }
            break;
            }
        case evBroadcast:
            if( event.message.command == cmCommandSetChanged )
                drawView();
            break;
        }
}

const char* TStatusLine::hint( ushort )
{
    return "";
}

void TStatusLine::update()
{
    TView *p = TopView();
    ushort h = ( p != 0 ) ? p->getHelpCtx() : hcNoContext;
    if( helpCtx != h )
        {
        helpCtx = h;
        findItems();
        drawView();
        }
}

#if !defined( NO_STREAM )
void TStatusLine::writeItems( opstream& os, TStatusItem *ts )
{
    int count = 0;
    for( TStatusItem *t = ts; t != 0; t = t->next )
        count++;
    os << count;
    for( ; ts != 0; ts = ts->next )
        {
        os.writeString( ts->text );
        os << ts->keyCode << ts->command;
        }
}

void TStatusLine::writeDefs( opstream& os, TStatusDef *td )
{
    int count = 0;
    for( TStatusDef *t = td; t != 0; t = t->next )
        count++;
    os << count;
    for( ; td != 0; td = td->next )
        {
        os << td->min << td->max;
        writeItems( os, td->items );
        }
}

void TStatusLine::write( opstream& os )
{
    TView::write( os );
    writeDefs( os, defs );
}

TStatusItem *TStatusLine::readItems( ipstream& is )
{
    TStatusItem *cur = 0;
    TStatusItem *first;
    TStatusItem **last = &first;
    int count;
    is >> count;
    while( count-- > 0 )
        {
        const char *t = is.readString();
        ushort key, cmd;
        is >> key >> cmd;
        cur = new TStatusItem( t, key, cmd );
        *last = cur;
        last = &(cur->next);
        }
    *last = 0;
    return first;
}

TStatusDef *TStatusLine::readDefs( ipstream& is )
{
    TStatusDef *cur = 0;
    TStatusDef *first;
    TStatusDef **last = &first;
    int count;
    is >> count;
    while( count-- > 0 )
        {
        ushort min, max;
        is >> min >> max;
        cur = new TStatusDef( min, max, readItems( is ) );
        *last = cur;
        last = &(cur->next);
        }
    *last = 0;
    return first;
}

void *TStatusLine::read( ipstream& is )
{   
    TView::read( is );
    defs = readDefs( is );
    findItems();
    return this;
}

TStreamable *TStatusLine::build()
{
    return new TStatusLine( streamableInit );
}

TStatusLine::TStatusLine( StreamableInit ) : TView( streamableInit )
{
}
#endif // NO_STREAM


