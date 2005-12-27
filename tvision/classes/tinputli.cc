/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (added insertChar, middle button paste and
other stuff).

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
// Added insertChar member.
#define Uses_ctype
#define Uses_string
#define Uses_stdlib
#define Uses_AllocLocal
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TGKey
#define Uses_TInputLine
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_TValidator
#define Uses_TPalette
#define Uses_TVOSClipboard
#define Uses_TGroup
#include <tv.h>

unsigned TInputLine::defaultModeOptions=0;

#define cpInputLine "\x13\x13\x14\x15"

TInputLine::TInputLine( const TRect& bounds, int aMaxLen, TValidator *aValid ) :
    TView(bounds),
    data( new char[aMaxLen] ),
    maxLen( aMaxLen-1 ),
    curPos( 0 ),
    firstPos( 0 ),
    selStart( 0 ),
    selEnd( 0 ),
    validator( aValid ),
    oldData( new char[aMaxLen] )
{
    state |= sfCursorVis;
    options |= ofSelectable | ofFirstClick;
    *data = EOS;
    modeOptions = defaultModeOptions;
}

void TInputLine::setValidator(TValidator * aValidator)
{
  CLY_destroy(validator);
  validator = aValidator;
}

TInputLine::~TInputLine()
{
    delete[] data;
    delete[] oldData;
    CLY_destroy(validator);
}

Boolean TInputLine::canScroll( int delta )
{
    if( delta < 0 )
        return Boolean( firstPos > 0 );
    else
        if( delta > 0 )
            return Boolean( (int)strlen(data) - firstPos + 2 > size.x );
        else
            return False;
}

unsigned TInputLine::dataSize()
{
    unsigned dSize = 0;

    if (validator)
        dSize = validator->transfer(data, NULL, vtDataSize);
    if (dSize == 0)
        dSize = maxLen + 1;
    return dSize;
}

void TInputLine::draw()
{
    int l, r;
    TDrawBuffer b;

    uchar color = (state & sfFocused) ? getColor( 2 ) : getColor( 1 );

    b.moveChar( 0, ' ', color, size.x );
    AllocLocalStr( buf, size.x );
    strncpy( buf, data+firstPos, size.x - 2 );
    buf[size.x - 2 ] = EOS;
    b.moveStr( 1, buf, color );

    if( canScroll(1) )
        b.moveChar( size.x-1, rightArrow, getColor(4), 1 );
    if( canScroll(-1) )
        b.moveChar( 0, leftArrow, getColor(4), 1 );
    if( state & sfSelected )
        {
        l = selStart - firstPos;
        r = selEnd - firstPos;
        l = max( 0, l );
        r = min( size.x - 2, r );
        if (l <  r)
            b.moveChar( l+1, 0, getColor(3), r - l );
        }
    writeLine( 0, 0, size.x, size.y, b );
    setCursor( curPos-firstPos+1, 0);
}

void TInputLine::getData( void *rec )
{
    if (!validator || !validator->transfer(data, rec, vtGetData))
        memcpy( rec, data, dataSize() );
}

TPalette& TInputLine::getPalette() const
{
    static TPalette palette( cpInputLine, sizeof( cpInputLine )-1 );
    return palette;
}

int TInputLine::mouseDelta( TEvent& event )
{
    TPoint mouse = makeLocal( event.mouse.where );

    if( mouse.x <= 0 )
        return -1;
    else
        if( mouse.x >= size.x - 1 )
            return 1;
        else
            return 0;
}

int TInputLine::mousePos( TEvent& event )
{
    TPoint mouse = makeLocal( event.mouse.where );
    mouse.x = max( mouse.x, 1 );
    int pos = mouse.x + firstPos - 1;
    pos = max( pos, 0 );
    pos = min( (size_t)pos, strlen(data) );
    return pos;
}

void  TInputLine::deleteSelect()
{
    if( selStart < selEnd )
        {
        strcpy( data+selStart, data+selEnd );
        curPos = selStart;
        }
}

/**[txh]********************************************************************

  Description:
  Inserts a character at the cursor position. If the text is currently
selected it's removed. If a validator is defined it's called. This basic
input line have a fixed size and will refuse to insert the character if
there is not enough space, but the virtual resizeData() is called giving
a chance to create variable size input lines.@*
  I (SET) moved it to allow insertions from sources other than the keyboard
emulating it's behavior.
  
  Return: False if the validator canceled the character, otherwise True.
  
***************************************************************************/

Boolean TInputLine::insertChar(char value)
{
    saveState();
    if( (state & sfCursorIns) == 0 )
    {
        deleteSelect();
    }
    int l = strlen(data);
    if (((l == maxLen) && ((state & sfCursorIns) == 0)) ||
        ((state & sfCursorIns) && curPos == maxLen))
      resizeData();
    {
      if( (state & sfCursorIns) == 0 )
      {
        if (l < maxLen)
          memmove( data + curPos + 1, data + curPos,
                   strlen(data+curPos)+1 );
      }
      else if (l == curPos)
      {
        data[curPos+1] = 0;
      }
      if ((((state & sfCursorIns) == 0) && (l < maxLen)) ||
          ((state & sfCursorIns) && (curPos < maxLen)))
      {
        if( firstPos > curPos )
            firstPos = curPos;
        data[curPos++] = value;
      }
    }

    return checkValid(False);
}

/**[txh]********************************************************************

  Description:
  Used internally to ensure the cursor is at a visible position, unselect
the text and force a draw.
  
***************************************************************************/

void TInputLine::makeVisible()
{
    if( firstPos > curPos )
        firstPos = curPos;
    int i = curPos - size.x + 2;
    if( firstPos < i )
        firstPos = i;
    drawView();
}

void TInputLine::saveState()
{
    if (validator)
        {
        strcpy(oldData,data);
        oldCurPos = curPos;
        oldFirstPos = firstPos;
        oldSelStart = selStart;
        oldSelEnd = selEnd;
        }
}

void TInputLine::restoreState()
{
    if (validator)
        {
        strcpy(data, oldData);
        curPos = oldCurPos;
        firstPos = oldFirstPos;
        selStart = oldSelStart;
        selEnd = oldSelEnd;
        }
}

Boolean TInputLine::checkValid(Boolean noAutoFill)
{
    int oldLen;
    char *newData;

    if (validator)
        {
        oldLen = (int)strlen(data);
        // SET:  I think TValidator should avoid to exceed maxLen.
        newData = new char[maxLen+2];
        strcpy(newData, data);
        if (!validator->isValidInput(newData, noAutoFill))
            {
                restoreState();
                delete[] newData;
                return False;
            }
        else
            {
            if ((int)strlen(newData) > maxLen)
                // We don't support validators that write more data than the allowed.
                abort();
                //newData[maxLen] = 0;
            strcpy(data,newData);
            // Handle the autofill feature
            if ((curPos >= oldLen) && ((int)strlen(data) > oldLen))
                curPos = (int)strlen(data);
            delete[] newData;
            return True;
            }
        }
    return True;
}



#define adjustSelectBlock() \
          if( curPos < anchor )  \
              {                  \
              selStart = curPos; \
              selEnd = anchor;   \
              }                  \
          else                   \
              {                  \
              selStart = anchor; \
              selEnd = curPos;   \
              }

void  TInputLine::handleEvent( TEvent& event )
{
    ushort key;
    Boolean extendBlock;
    TView::handleEvent(event);

    int delta, anchor=0;
    if( (state & sfSelected) != 0 )
        switch( event.what )
            {
            case  evMouseDown:
                if( event.mouse.buttons == mbMiddleButton &&
                    TVOSClipboard::isAvailable() > 1 )
                   {
                   unsigned size,i;
                   char *p=TVOSClipboard::paste(1,size);
                   if( p )
                     {
                     for (i=0; i<size; i++)
                        {
                        insertChar( p[i] );
                        selStart = selEnd = 0; // Reset the selection or we will delete the last insertion
                        }
                     DeleteArray( p );
                     makeVisible();
                     }
                   }
                else if( canScroll(delta = mouseDelta(event)) )
                    do  {
                        if( canScroll(delta) )
                            {
                            firstPos += delta;
                            drawView();
                            }
                        } while( mouseEvent( event, evMouseAuto ) );
                else if( event.mouse.doubleClick )
                        selectAll(True);
                else
                    {
                    anchor =  mousePos(event);
                    do  {
                        if( event.what == evMouseAuto &&
                            canScroll( delta = mouseDelta(event) )
                          )
                            firstPos += delta;
                        curPos = mousePos(event);
                        adjustSelectBlock()
                        drawView();
                        } while (mouseEvent(event, evMouseMove | evMouseAuto));
                    if( TVOSClipboard::isAvailable() > 1 )
                        TVOSClipboard::copy(1,data+selStart,selEnd-selStart);
                    }
                clearEvent(event);
                break;
            case  evKeyDown:
                key = ctrlToArrow(event.keyDown.keyCode);
                extendBlock = False;
                if( key & kbShiftCode )
                    {
                    ushort keyS = key & (~kbShiftCode);
                    if( keyS == kbHome || keyS == kbLeft || keyS == kbRight || keyS == kbEnd )
                        {
                        if (curPos == selEnd)
                            anchor = selStart;
                        else
                            anchor = selEnd;
                        key = keyS;
                        extendBlock = True;
                        }
                    }
                        
                switch( key )
                    {
                    case kbLeft:
                        if( curPos > 0 )
                            curPos--;
                        break;
                    case kbRight:
                        if( curPos < (int)strlen(data) )
                            curPos++;
                        break;
                    case kbHome:
                        curPos =  0;
                        break;
                    case kbEnd:
                        curPos = strlen(data);
                        break;
                    case kbBackSpace:
                        if( curPos > 0 )
                            {
                            saveState();
                            strcpy( data+curPos-1, data+curPos );
                            curPos--;
                            if( firstPos > 0 )
                                firstPos--;
                            checkValid(True);
                            }
                        break;
                    case kbDelete:
                        saveState();
                        if( selStart == selEnd )
                            if( curPos < (int)strlen(data) )
                                {
                                selStart = curPos;
                                selEnd = curPos + 1;
                                }
                        deleteSelect();
                        checkValid(True);
                        break;
                    case kbInsert:
                        setState(sfCursorIns, Boolean(!(state & sfCursorIns)));
                        break;
                    case kbCtY:
                        *data = EOS;
                        curPos = 0;
                        break;
                    // Let them pass even if these contains a strange ASCII (SET)
                    case kbEnter:
                    case kbTab:
                         return;
                    default:
                        if( event.keyDown.charScan.charCode >= ' ' )
                          {
                          if( !insertChar( event.keyDown.charScan.charCode ) )
                            {
                            clearEvent(event);
                            break;
                            }
                          }
                        else
                          {
                             return;
                          }
                    }
                    if (extendBlock)
                        {
                        adjustSelectBlock()
                        }
                    else
                        {
                        selStart = 0;
                        selEnd = 0;
                        }
                    makeVisible();
                    clearEvent( event );
                    break;
            }
}

void TInputLine::selectAll( Boolean enable )
{
    selStart = 0;
    if( enable )
        curPos = selEnd = strlen(data);
    else
        curPos = selEnd = 0;
    firstPos = max( 0, curPos-size.x+2 );
    if( TVOSClipboard::isAvailable() > 1 )
        TVOSClipboard::copy(1,data+selStart,selEnd-selStart);
    drawView();
}

void TInputLine::setData( void *rec )
{
    if (!validator || !validator->transfer(data,rec,vtSetData))
        {
        // The following is invalid is someboy calls setData("string")
        // memcpy( data, rec, dataSize()-1 );
        strncpy( data, (const char*)rec, dataSize()-1 );
        data[dataSize()-1] = EOS; // strncpy doesn't make it if strlen(rec)>dataSize()-1
        }
    selectAll( True );
}

void TInputLine::setState( ushort aState, Boolean enable )
{
    if (validator &&                           // We have a validator
        (modeOptions & ilValidatorBlocks)  &&  // We want to block if invalid
        owner && (owner->state & sfActive) &&  // The owner is visible
        aState==sfFocused && enable==False)    // We are losing the focus
      {
       TValidator *v=validator;
       validator=NULL;             // Avoid nested tests
       Boolean ret=v->validate(data); // Check if we have valid data
       validator=v;
       if (!ret)                   // If not refuse the focus change
          return;
      }
    TView::setState( aState, enable );
    if( aState == sfSelected ||
        ( aState == sfActive && (state & sfSelected) != 0 )
      )
        selectAll( enable );
}

#if !defined( NO_STREAM )
void TInputLine::write( opstream& os )
{
    TView::write( os );
    os << maxLen << curPos << firstPos
       << selStart << selEnd;
    os.writeString( data);
    os << validator;
}

void *TInputLine::read( ipstream& is )
{
    TView::read( is );
    is >> maxLen >> curPos >> firstPos
       >> selStart >> selEnd;
    data = new char[maxLen + 1];
    oldData = new char[maxLen + 1];
    is.readString(data, maxLen+1);
    state |= sfCursorVis;
    is >> validator;
    // BC++ TV 2.0 options |= ofSelectable | ofFirstClick;
    return this;
}

TStreamable *TInputLine::build()
{
    return new TInputLine( streamableInit );
}

TInputLine::TInputLine( StreamableInit ) : TView( streamableInit ),
  validator(NULL)
{
}
#endif // NO_STREAM

Boolean TInputLine::valid(ushort cmd)
{
    if (validator)
        {
        if (cmd == cmValid)
            return Boolean(validator->status == vsOk);
        else if (cmd != cmCancel)
            if (!validator->validate(data))
                {
                owner->current = 0;
                select();
                return False;
                }
        }
    return True;
}


