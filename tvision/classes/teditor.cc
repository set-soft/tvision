/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
/*****************************************************************************

    That's a very old port found in RHIDE beta 4. As Robert incorpoarted my
editor we no longer use the editor class. Even in RHIDE beta 5 Robert used a
modified editor. So that's old code no maintained in ages. To give you an
idea I have an editor.cc file that is very different to it dated 06/1996.
    As a user asked about the missing classes like TEditor I taked this old
code and adapted it to compile with the new port (01/1999).

*****************************************************************************/

#define Uses_ctype
#define Uses_TKeys
// SET: Needed for the new keyboard symbols
#define Uses_TKeys_Extended
// SET: Needed to manipulate the keycodes safetly
#define Uses_TGKey
#define Uses_TEditor
#define Uses_TIndicator
#define Uses_TEvent
#define Uses_TScrollBar
#define Uses_TFindDialogRec
#define Uses_TReplaceDialogRec
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_TMacroCollection
#include <tv.h>

inline int isWordChar( int ch )
{ // SET: changed by the uchar version
 return ucisalnum(ch) || ch == '_';
}

/* SET: I changed the symbols here by the new keyboard definitions to make
   it uniform, I could use the original */
const ushort firstKeys[] =
{
    39,
    kbCtA, cmWordLeft,
    kbCtC, cmPageDown,
    kbCtD, cmCharRight,
    kbCtE, cmLineUp,
    kbCtF, cmWordRight,
    kbCtG, cmDelChar,
    kbCtH, cmBackSpace,
    kbCtK, 0xFF02,
    kbCtL, cmSearchAgain,
    kbCtM, cmNewLine,
    kbCtO, cmIndentMode,
    kbCtQ, 0xFF01,
    kbCtR, cmPageUp,
    kbCtS, cmCharLeft,
    kbCtT, cmDelWord,
    kbCtU, cmUndo,
    kbCtV, cmInsMode,
    kbCtX, cmLineDown,
    kbCtY, cmDelLine,
    // SET: The hack used by the class to search a command needs to have
    // first the codes with modifiers (like Control+Left) and then the code
    // without the modifiers (like Left).
    kbCtLeft, cmWordLeft,
    kbCtRight, cmWordRight,
    kbCtPgUp, cmTextStart,
    kbCtPgDn, cmTextEnd,
    kbCtInsert, cmCopy,
    kbCtDelete, cmClear,
    kbShInsert, cmPaste,
    kbShDelete, cmCut,
    kbLeft, cmCharLeft,
    kbRight, cmCharRight,
    kbHome, cmLineStart,
    kbEnd, cmLineEnd,
    kbUp, cmLineUp,
    kbDown, cmLineDown,
    kbPgUp, cmPageUp,
    kbPgDn, cmPageDown,
    kbDelete, cmDelChar,
    kbInsert, cmInsMode,
    kbEnter, cmNewLine,
    kbBackSpace, cmBackSpace
};

/* SET: Here Borland used a bad trick using the ASCII for the ASCII letters
   with the new keyboard handling that isn't true. The ASCII reported is the
   letter, but the keycode have an arbitrary symbol and it could change from
   OS to OS. Currently is the same under DOS and Linux */
const ushort quickKeys[] =
{   8,
    kbA, cmReplace,
    kbC, cmTextEnd,
    kbD, cmLineEnd,
    kbF, cmFind,
    kbH, cmDelStart,
    kbR, cmTextStart,
    kbS, cmLineStart,
    kbY, cmDelEnd
};

const ushort blockKeys[] =
{   5,
    kbB, cmStartSelect,
    kbC, cmPaste,
    kbH, cmHideSelect,
    kbK, cmCopy,
    kbY, cmCut
};

const ushort *keyMap[] = { firstKeys, quickKeys, blockKeys };

ushort defEditorDialog( int, ... );

#define KeyMap ((const ushort*)(keyMap))

const ushort kbModMask=~kbKeyMask;


// SET: Quick hack by Robert, should be adapted as the other. The original was
// assembler, not much faster than it.
// I changed the silly assumptions about the keycodes by masks based on values
// defined in headers.
ushort scanKeyMap( const void *keyMap, int keyCode )
{
  int i=0;
  ushort bx,cx,dx,ax;
  dx = keyCode;
  cx = KeyMap[i++];
  do
  {
    bx = KeyMap[i++];
    ax = KeyMap[i++];
    if ((bx & kbKeyMask) == (dx & kbKeyMask))
    {
      if (!(bx & kbModMask)) return ax;
      // SET: Here I changed the methode. That's tricky so if you don't
      // understand it don't worry. The idea is that: if I press Ctrl+Shift+
      // Left it should match with Control+Left and not Left. And if I press
      // Shift+Left it must match with Left (because nothing is assigned to
      // Shift+Left.
      //if ((bx & kbModMask) == (dx & kbModMask)) return ax;
      if ((bx & kbModMask) & (dx & kbModMask)) return ax;
    }
    cx--;
  } while (cx != 0);
  return 0;
}

#define cpEditor    "\x06\x07"

TEditor::TEditor( const TRect& bounds,
                  TScrollBar *aHScrollBar,
                  TScrollBar *aVScrollBar,
                  TIndicator *aIndicator,
		  uint32 aBufSize ) :
    TView( bounds ),
    hScrollBar( aHScrollBar ),
    vScrollBar( aVScrollBar ),
    indicator( aIndicator ),
    bufSize( aBufSize ),
    canUndo( True ),
    selecting( False ),
    overwrite( False ),
    autoIndent( False ) ,
    lockCount( 0 ),
    keyState( 0 )
{
    growMode = gfGrowHiX | gfGrowHiY;
    options |= ofSelectable;
    eventMask = evMouseDown | evKeyDown | evCommand | evBroadcast;
    showCursor();
    initBuffer();
    if( buffer != 0 )
        isValid = True;
    else
    {
        editorDialog( edOutOfMemory );
        bufSize = 0;
        isValid = False;
    }
    setBufLen(0);
}

TEditor::~TEditor()
{
}

void TEditor::shutDown()
{
    doneBuffer();
    TView::shutDown();
}

void TEditor::changeBounds( const TRect& bounds )
{
    setBounds(bounds);
    delta.x = max(0, min(delta.x, limit.x - size.x));
    delta.y = max(0, min(delta.y, limit.y - size.y));
    update(ufView);
}

/* SET: Note that Robert added a tabSize member to the class */
int TEditor::charPos( uint32 p, uint32 target )
{
    int pos = 0;
    while( p < target )
    {
        if( bufChar(p) == '\x9' )
#if 0
            pos |= 7;
#else
	    pos += tabSize - (pos % tabSize) -1;
#endif
        pos++;
        p++;
    }
    return pos;
}

uint32 TEditor::charPtr( uint32 p, int target )
{
  int pos = 0;
  while( (pos < target) && (p < bufLen) && (bufChar(p) != '\x0D') && (bufChar(p) != '\x0A') )
    {
    if( bufChar(p) == '\x09' )
#if 0
            pos |= 7;
#else
	    pos += tabSize - (pos % tabSize) -1;
#endif
    pos++;
    p++;
    }
  if( pos > target )
    p--;
  return p;
}

Boolean TEditor::clipCopy()
{
    Boolean res = False;
    if( (clipboard != 0) && (clipboard != this) )
        {
        res = clipboard->insertFrom(this);
        selecting = False;
        update(ufUpdate);
        }
    return res;
}

void TEditor::clipCut()
{
    if( clipCopy() == True )
        deleteSelect();
}

void TEditor::clipPaste()
{
    if( (clipboard != 0) && (clipboard != this) )
        insertFrom(clipboard);
}


void TEditor::convertEvent( TEvent& event )
{
    if( event.what == evKeyDown )
        {
        /* SET: Here we had another dirty code. The code assumed some special
           values for (A) the modifiers and (B) the keycodes, that's really
           bad coding style
	if ( (shiftkeys() & 0x03) != 0 &&
            event.keyDown.charScan.scanCode >= 0x47 &&
            event.keyDown.charScan.scanCode <= 0x51
          )
            event.keyDown.charScan.charCode = 0;*/

        // SET: More assumptions were here
        ushort key = event.keyDown.keyCode;
        if( keyState != 0 )
            {
            if( key>=kbCtA && key<=kbCtZ )
                key+=kbA-kbCtA;
            if( key>=kbShA && key<=kbShA )
                key+=kbA-kbShA;
            }
        key = scanKeyMap(keyMap[keyState], key);
        keyState = 0;
        if( key != 0 )
            {
            if( (key & 0xFF00) == 0xFF00 )
                {
                keyState = (key & 0xFF);
                clearEvent(event);
                }
            else
                {
                event.what = evCommand;
                event.message.command = key;
                }
            }
        }
}

Boolean TEditor::cursorVisible()
{
  return Boolean((curPos.y >= delta.y) && (curPos.y < delta.y + size.y));
}

void TEditor::deleteRange( uint32 startPtr,
			   uint32 endPtr,
                           Boolean delSelect
                         )
{
    if( hasSelection() == True && delSelect == True )
        deleteSelect();
    else
        {
        setSelect(curPtr, endPtr, True);
        deleteSelect();
        setSelect(startPtr, curPtr, False);
        deleteSelect();
        }
}

void TEditor::deleteSelect()
{
    insertText( 0, 0, False );
}

void TEditor::doneBuffer()
{
    delete buffer;
}

void TEditor::doSearchReplace()
{
    int i;
    do  {
        i = cmCancel;
        if( search(findStr, editorFlags) == False )
            {
            if( (editorFlags & (efReplaceAll | efDoReplace)) !=
                (efReplaceAll | efDoReplace) )
                    editorDialog( edSearchFailed );
            }
        else
            if( (editorFlags & efDoReplace) != 0 )
                {
                i = cmYes;
                if( (editorFlags & efPromptOnReplace) != 0 )
                    {
                    TPoint c = makeGlobal( cursor );
                    i = editorDialog( edReplacePrompt, &c );
                    }
                if( i == cmYes )
                    {
                    lock();
                    insertText( replaceStr, strlen(replaceStr), False);
                    trackCursor(False);
                    unlock();
                    }
                }
        } while( i != cmCancel && (editorFlags & efReplaceAll) != 0 );
}

void TEditor::doUpdate()
{
    if( updateFlags != 0 )
        {
        setCursor(curPos.x - delta.x, curPos.y - delta.y);
        if( (updateFlags & ufView) != 0 )
            drawView();
        else
            if( (updateFlags & ufLine) != 0 )
                drawLines( curPos.y-delta.y, 1, lineStart(curPtr) );
        if( hScrollBar != 0 )
            hScrollBar->setParams(delta.x, 0, limit.x - size.x, size.x / 2, 1);
        if( vScrollBar != 0 )
            vScrollBar->setParams(delta.y, 0, limit.y - size.y, size.y - 1, 1);
        if( indicator != 0 )
            indicator->setValue(curPos, modified);
        if( (state & sfActive) != 0 )
            updateCommands();
        updateFlags = 0;
        }
}

void TEditor::draw()
{
    if( drawLine != delta.y )
        {
        drawPtr = lineMove( drawPtr, delta.y - drawLine );
        drawLine = delta.y;
        }
    drawLines( 0, size.y, drawPtr );
}

void TEditor::drawLines( int y, int count, uint32 linePtr )
{
    ushort color = getColor(0x0201);
    while( count-- > 0 )
        {
        ushort b[maxLineLength];
        formatLine( b, linePtr, delta.x+size.x, color );
        writeBuf(0, y, size.x, 1, &b[delta.x]);
        linePtr = nextLine(linePtr);
        y++;
        }
}

void TEditor::find()
{
    TFindDialogRec findRec( findStr, editorFlags );
    if( editorDialog( edFind, &findRec ) != cmCancel )
        {
        strcpy( findStr, findRec.find );
        editorFlags = findRec.options & ~efDoReplace;
        doSearchReplace();
        }
}

uint32 TEditor::getMousePtr( TPoint m )
{
    TPoint mouse = makeLocal( m );
    mouse.x = max(0, min(mouse.x, size.x - 1));
    mouse.y = max(0, min(mouse.y, size.y - 1));
    return charPtr(lineMove(drawPtr, mouse.y + delta.y - drawLine),
        mouse.x + delta.x);
}

TPalette& TEditor::getPalette() const
{
    static TPalette palette( cpEditor, sizeof( cpEditor )-1 );
    return palette;
}


void TEditor::checkScrollBar( const TEvent& event,
                              TScrollBar *p,
                              int& d
                            )
{
    if( (event.message.infoPtr == p) && (p->value != d) )
        {
        d = p->value;
        update( ufView );
        }
}

void TEditor::handleEvent( TEvent& event )
{
    TView::handleEvent( event );
    //if (macros) macros->handleEvent(event,this);
    convertEvent( event );
    Boolean centerCursor = Boolean(!cursorVisible());
    uchar selectMode = 0;
    // SET: This way of getting the shift state is deprecated in our port, by
    // now works.
    if( selecting == True || (TGKey::getShiftState() & (kbRightShiftDown|kbLeftShiftDown)) != 0 )
        selectMode = smExtend;

    switch( event.what )
        {

        case evMouseDown:
            if( event.mouse.doubleClick == True )
                selectMode |= smDouble;

            do  {
                lock();
                if( event.what == evMouseAuto )
                    {
                    TPoint mouse = makeLocal( event.mouse.where );
                    TPoint d = delta;
                    if( mouse.x < 0 )
                        d.x--;
                    if( mouse.x >= size.x )
                        d.x++;
                    if( mouse.y < 0 )
                        d.y--;
                    if( mouse.y >= size.y )
                        d.y++;
                    scrollTo(d.x, d.y);
                    }
                setCurPtr(getMousePtr(event.mouse.where), selectMode);
                selectMode |= smExtend;
                unlock();
                } while( mouseEvent(event, evMouseMove + evMouseAuto) );
            break;

        case evKeyDown:
            if( event.keyDown.charScan.charCode == 9 ||
                ( event.keyDown.charScan.charCode >= 32 && event.keyDown.charScan.charCode < 255 ) )
                    {
                    lock();
                    if( overwrite == True && hasSelection() == False )
                        if( curPtr != lineEnd(curPtr) )
                            selEnd = nextChar(curPtr);
                    insertText( &event.keyDown.charScan.charCode, 1, False);
                    trackCursor(centerCursor);
                    unlock();
                    }
            else
                return;
            break;

        case evCommand:
            switch( event.message.command )
                {
		case cmInsertText:
		    insertText(event.message.infoPtr,strlen((char *)event.message.infoPtr),False);
                    break;
                case cmFind:
                    find();
                    break;
                case cmReplace:
                    replace();
                    break;
                case cmSearchAgain:
                    doSearchReplace();
                    break;
                default:
                    lock();
                    switch( event.message.command )
                        {
                        case cmCut:
                            clipCut();
                            break;
                        case cmCopy:
                            clipCopy();
                            break;
                        case cmPaste:
                            clipPaste();
                            break;
                        case cmUndo:
                            undo();
                            break;
                        case cmClear:
                            deleteSelect();
                            break;
                        case cmCharLeft:
                            setCurPtr(prevChar(curPtr), selectMode);
                            break;
                        case cmCharRight:
                            setCurPtr(nextChar(curPtr), selectMode);
                            break;
                        case cmWordLeft:
                            setCurPtr(prevWord(curPtr), selectMode);
                            break;
                        case cmWordRight:
                            setCurPtr(nextWord(curPtr), selectMode);
                            break;
                        case cmLineStart:
                            setCurPtr(lineStart(curPtr), selectMode);
                            break;
                        case cmLineEnd:
                            setCurPtr(lineEnd(curPtr), selectMode);
                            break;
                        case cmLineUp:
                            setCurPtr(lineMove(curPtr, -1), selectMode);
                            break;
                        case cmLineDown:
                            setCurPtr(lineMove(curPtr, 1), selectMode);
                            break;
                        case cmPageUp:
                            setCurPtr(lineMove(curPtr, -(size.y-1)), selectMode);
                            break;
                        case cmPageDown:
                            setCurPtr(lineMove(curPtr, size.y-1), selectMode);
                            break;
                        case cmTextStart:
                            setCurPtr(0, selectMode);
                            break;
                        case cmTextEnd:
                            setCurPtr(bufLen, selectMode);
                            break;
                        case cmNewLine:
                            newLine();
                            break;
                        case cmBackSpace:
                            deleteRange(prevChar(curPtr), curPtr, True);
                            break;
                        case cmDelChar:
                            deleteRange(curPtr, nextChar(curPtr), True);
                            break;
                        case cmDelWord:
                            deleteRange(curPtr, nextWord(curPtr), False);
                            break;
                        case cmDelStart:
                            deleteRange(lineStart(curPtr), curPtr, False);
                            break;
                        case cmDelEnd:
                            deleteRange(curPtr, lineEnd(curPtr), False);
                            break;
                        case cmDelLine:
                            deleteRange(lineStart(curPtr), nextLine(curPtr), False);
                            break;
                        case cmInsMode:
                            toggleInsMode();
                            break;
                        case cmStartSelect:
                            startSelect();
                            break;
                        case cmHideSelect:
                            hideSelect();
                            break;
                        case cmIndentMode:
                            autoIndent = Boolean(!autoIndent);
                            break;
                        default:
                            unlock();
                            return;
                        }
                    trackCursor(centerCursor);
                    unlock();
                    break;
                }

        case evBroadcast:
            switch( event.message.command )
                {
                case cmScrollBarChanged:
                    checkScrollBar( event, hScrollBar, delta.x );
                    checkScrollBar( event, vScrollBar, delta.y );
                    break;
                default:
                    return;
                }
        }
    clearEvent(event);
}


// TEDITOR2.CC

// inline int isWordChar( int ch )
// {
//     return isalnum(ch) || ch == '_';
// }

int countLines( void *buf, uint32 count )
{
  int ret=0,i=0;
  while (count--) if (((const char *)(buf))[i++] == 0x0a) ret++;
  return ret;
}

#define Block ((const char *)(block))

uint32 scan( const void *block, uint32 size, const char *str )
{
    if (!size) return (UINT_MAX);
    uint32 ret=0;
    while (size--)
    {
      if (Block[ret] == str[0])
      {
	uint32 i=0;
        do
        {
	  i++;
	  if (!str[i]) return (ret);
	  if (size<i) return (UINT_MAX);
	} while (Block[ret+i] == str[i]);
      }
      ret++;
    }
    return (UINT_MAX);
}

uint32 iScan( const void *block, uint32 size, const char *str )
{
    if (!size) return (UINT_MAX);
    uint32 ret=0;
    while (size--)
    {
      if (toupper(Block[ret]) == toupper(str[0]))
      {
	uint32 i=0;
        do
        {
	  i++;
	  if (!str[i]) return (ret);
	  if (size<i) return (UINT_MAX);
	} while (toupper(Block[ret+i]) == toupper(str[i]));
      }
      ret++;
    }
    return (UINT_MAX);
}

Boolean TEditor::hasSelection()
{
    return Boolean(selStart != selEnd);
}

void TEditor::hideSelect()
{
    selecting = False;
    setSelect(curPtr, curPtr, False);
}

void TEditor::initBuffer()
{
    buffer = new char[bufSize];
}

Boolean TEditor::insertBuffer( char *p,
			       uint32 offset,
			       uint32 length,
                               Boolean allowUndo,
                               Boolean selectText
                             )
{
    selecting = False;
    uint32 selLen = selEnd - selStart;
    if( selLen == 0 && length == 0 )
        return True;

    uint32 delLen = 0;
    if( allowUndo == True )
        {
        if( curPtr == selStart )
            delLen = selLen;
        else
            if( selLen > insCount )
                delLen = selLen - insCount;
        }

    uint32 newSize = uint32(bufLen + delCount - selLen + delLen) + length;

    if( newSize > bufLen + delCount )
	if( setBufSize((uint32)(newSize)) == False )
            {
            editorDialog( edOutOfMemory );
            return False;
            }

    uint32 selLines = countLines( &buffer[bufPtr(selStart)], selLen );
    if( curPtr == selEnd )
        {
        if( allowUndo == True )
            {
            if( delLen > 0 )
                memmove( 
                         &buffer[curPtr + gapLen - delCount - delLen],
                         &buffer[selStart],
                         delLen
                       );
            insCount -= selLen - delLen;
            }
        curPtr = selStart;
        curPos.y -= selLines;
        }
    if( delta.y > curPos.y )
        {
        delta.y -= selLines;
        if( delta.y < curPos.y )
            delta.y = curPos.y;
        }

    if( length > 0 )
        memmove(
                &buffer[curPtr],
                &p[offset],
                length
               );

    uint32 lines = countLines( &buffer[curPtr], length );
    curPtr += length;
    curPos.y += lines;
    drawLine = curPos.y;
    drawPtr = lineStart(curPtr);
    curPos.x = charPos(drawPtr, curPtr);
    if( selectText == False )
        selStart = curPtr;
    selEnd = curPtr;
    bufLen += length - selLen;
    gapLen -= length - selLen;
    if( allowUndo == True )
        {
        delCount += delLen;
        insCount += length;
        }
    limit.y += lines - selLines;
    delta.y = max(0, min(delta.y, limit.y - size.y));
    if( isClipboard() == False )
        modified = True;
    setBufSize(bufLen + delCount);
    if( selLines == 0 && lines == 0 )
        update(ufLine);
    else
        update(ufView);
    return True;
}

Boolean TEditor::insertFrom( TEditor *editor )
{
    return insertBuffer( editor->buffer,
                         editor->bufPtr(editor->selStart),
                         editor->selEnd - editor->selStart,
                         canUndo,
                         isClipboard()
                        );
}

Boolean TEditor::insertText( const void *text, uint32 length, Boolean selectText )
{
  return insertBuffer( (char *)text, 0, length, canUndo, selectText);
}

Boolean TEditor::isClipboard()
{
    return Boolean(clipboard == this);
}

uint32 TEditor::lineMove( uint32 p, int count )
{
    uint32 i = p;
    p = lineStart(p);
    int pos = charPos(p, i);
    while( count != 0 )
        {
        i = p;
        if( count < 0 )
            {
            p = prevLine(p);
            count++;
            }
        else
            {
            p = nextLine(p);
            count--;
            }
        }
    if( p != i )
        p = charPtr(p, pos);
    return p;
}

void TEditor::lock()
{
    lockCount++;
}

void TEditor::newLine()
{
    uint32 p = lineStart(curPtr);
    uint32 i = p;
    while( i < curPtr &&
           ( (buffer[i] == ' ') || (buffer[i] == '\x9'))
         )
         i++;
    insertText(CLY_crlf,CLY_LenEOL,False);
    if( autoIndent == True )
        insertText( &buffer[p], i - p, False);
}

uint32 TEditor::nextLine( uint32 p )
{
    return nextChar(lineEnd(p));
}

uint32 TEditor::nextWord( uint32 p )
{
   if (isWordChar(bufChar(p)))
      while (p < bufLen && isWordChar(bufChar(p)))
         p = nextChar(p);
   else if (p < bufLen)
      p = nextChar(p);
   while (p < bufLen && !isWordChar(bufChar(p)))
      p = nextChar(p);
   return p;
}

uint32 TEditor::prevLine( uint32 p )
{
  return lineStart(prevChar(p));
}

uint32 TEditor::prevWord( uint32 p )
{
    while( p > 0 && isWordChar(bufChar(prevChar(p))) == 0 )
        p = prevChar(p);
    while( p > 0 && isWordChar(bufChar(prevChar(p))) != 0 )
        p = prevChar(p);
    return p;
}

void TEditor::replace()
{
    TReplaceDialogRec replaceRec( findStr, replaceStr, editorFlags );
    if( editorDialog( edReplace, &replaceRec ) != cmCancel )
        {
        strcpy( findStr, replaceRec.find );
        strcpy( replaceStr, replaceRec.replace );
        editorFlags = replaceRec.options | efDoReplace;
        doSearchReplace();
        }

}

void TEditor::scrollTo( int x, int y )
{
    x = max(0, min(x, limit.x - size.x));
    y = max(0, min(y, limit.y - size.y));
    if( x != delta.x || y != delta.y )
        {
        delta.x = x;
        delta.y = y;
        update(ufView);
        }
}

Boolean TEditor::search( const char *findStr, ushort opts )
{
    uint32 pos = curPtr;
    uint32 i;
    do  {
        if( (opts & efCaseSensitive) != 0 )
            i = scan( &buffer[bufPtr(pos)], bufLen - pos, findStr);
        else
            i = iScan( &buffer[bufPtr(pos)], bufLen - pos, findStr);

        if( i != sfSearchFailed )
            {
            i += pos;
            if( (opts & efWholeWordsOnly) == 0 ||
                !(
                    ( i != 0 && isWordChar(bufChar(i - 1)) != 0 ) ||
                    ( i + strlen(findStr) != bufLen &&
                        isWordChar(bufChar(i + strlen(findStr)))
                    )
                 ))
                {
                lock();
                setSelect(i, i + strlen(findStr), False);
                trackCursor(Boolean(!cursorVisible()));
                unlock();
                return True;
                }
            else
                pos = i + 1;
            }
        } while( i != sfSearchFailed );
    return False;
}

void TEditor::setBufLen( uint32 length )
{
    bufLen = length;
    gapLen = bufSize - length;
    selStart = 0;
    selEnd = 0;
    curPtr = 0;
    delta.x = 0;
    delta.y = 0;
    curPos = delta;
    limit.x = maxLineLength;
    limit.y = countLines( &buffer[gapLen], bufLen ) + 1;
    drawLine = 0;
    drawPtr = 0;
    delCount = 0;
    insCount = 0;
    modified = False;
    update(ufView);
}

Boolean TEditor::setBufSize( uint32 newSize )
{
    return Boolean(newSize <= bufSize);
}

void TEditor::setCmdState( ushort command, Boolean enable )
{
    TCommandSet s;
    s += command;
    if( enable == True && (state & sfActive) != 0 )
        enableCommands(s);
    else
        disableCommands(s);
}

void TEditor::setCurPtr( uint32 p, uchar selectMode )
{
    uint32 anchor;
    if( (selectMode & smExtend) == 0 )
        anchor = p;
    else if( curPtr == selStart )
        anchor = selEnd;
    else
        anchor = selStart;

    if( p < anchor )
        {
        if( (selectMode & smDouble) != 0 )
            {
            p = prevLine(nextLine(p));
            anchor = nextLine(prevLine(anchor));
            }
        setSelect(p, anchor, True);
        }
    else
        {
        if( (selectMode & smDouble) != 0 )
            {
            p = nextLine(p);
            anchor = prevLine(nextLine(anchor));
            }
        setSelect(anchor, p, False);
        }
}

void TEditor::setSelect( uint32 newStart, uint32 newEnd, Boolean curStart )
{
    uint32 p;
    if( curStart != 0 )
        p = newStart;
    else
        p = newEnd;

    uchar flags = ufUpdate;

    if( newStart != selStart || newEnd != selEnd )
        if( newStart != newEnd || selStart != selEnd )
            flags = ufView;

    if( p != curPtr )
        {
        if( p > curPtr )
            {
	    uint32 l = p - curPtr;
            memmove( &buffer[curPtr], &buffer[curPtr + gapLen], l);
            curPos.y += countLines(&buffer[curPtr], l);
            curPtr = p;
            }
        else
            {
	    uint32 l = curPtr - p;
            curPtr = p;
            curPos.y -= countLines(&buffer[curPtr], l);
            memmove( &buffer[curPtr + gapLen], &buffer[curPtr], l);
            }
        drawLine = curPos.y;
        drawPtr = lineStart(p);
        curPos.x = charPos(drawPtr, p);
        delCount = 0;
        insCount = 0;
        setBufSize(bufLen);
    }
    selStart = newStart;
    selEnd = newEnd;
    update(flags);
}

void TEditor::setState( ushort aState, Boolean enable )
{
    TView::setState(aState, enable);
    switch( aState )
        {
        case sfActive:
            if( hScrollBar != 0 )
                hScrollBar->setState(sfVisible, enable);
            if( vScrollBar != 0 )
                vScrollBar->setState(sfVisible, enable);
            if( indicator != 0 )
                indicator->setState(sfVisible, enable);
            updateCommands();
            break;

        case sfExposed:
            if( enable == True )
                unlock();
        }
}

void TEditor::startSelect()
{
    hideSelect();
    selecting = True;
}

void TEditor::toggleInsMode()
{
    overwrite = Boolean(!overwrite);
    setState(sfCursorIns, Boolean(!getState(sfCursorIns)));
}

void TEditor::trackCursor( Boolean center )
{
    if( center == True )
        scrollTo( curPos.x - size.x + 1, curPos.y - size.y / 2);
    else
        scrollTo( max(curPos.x - size.x + 1, min(delta.x, curPos.x)),
                  max(curPos.y - size.y + 1, min(delta.y, curPos.y)));
}

void TEditor::undo()
{
    if( delCount != 0 || insCount != 0 )
        {
        selStart = curPtr - insCount;
        selEnd = curPtr;
	uint32 length = delCount;
        delCount = 0;
        insCount = 0;
        insertBuffer(buffer, curPtr + gapLen - length, length, False, True);
        }
}

void TEditor::unlock()
{
    if( lockCount > 0 )
        {
        lockCount--;
        if( lockCount == 0 )
            doUpdate();
        }
}

void TEditor::update( uchar aFlags )
{
    updateFlags |= aFlags;
    if( lockCount == 0 )
        doUpdate();
}

void TEditor::updateCommands()
{
    setCmdState( cmUndo, Boolean( delCount != 0 || insCount != 0 ) );
    if( isClipboard() == False )
        {
        setCmdState(cmCut, hasSelection());
        setCmdState(cmCopy, hasSelection());
        setCmdState(cmPaste,
                    Boolean(clipboard != 0 && (clipboard->hasSelection())) );
        }
    setCmdState(cmClear, hasSelection());
    setCmdState(cmFind, True);
    setCmdState(cmReplace, True);
    setCmdState(cmSearchAgain, True);
}

Boolean TEditor::valid( ushort )
{
  return isValid;
}

#ifndef NO_STREAM

void TEditor::write( opstream& os )
{
    TView::write( os );
    os << hScrollBar << vScrollBar << indicator
       << bufSize << (short)canUndo;
}

void *TEditor::read( ipstream& is )
{
    TView::read( is );
    short temp;
    is >> hScrollBar >> vScrollBar >> indicator
       >> bufSize >> temp;
    canUndo = Boolean(temp);
    selecting = False;
    overwrite = False;
    autoIndent = False;
    lockCount = 0;
    keyState = 0;
    initBuffer();
    if( buffer != 0 )
        isValid = True;
    else
        {
        TEditor::editorDialog( edOutOfMemory, 0 );
        bufSize = 0;
        }
    lockCount = 0;
    lock();
    setBufLen( 0 );
    return this;
}

TStreamable *TEditor::build()
{
    return new TEditor( streamableInit );
}

TEditor::TEditor( StreamableInit ) : TView( streamableInit )
{
}

#endif

// SET: The following routines were assembler in the original TVision, Robert
// did just a quick hack.
// Notes: changed 0xd by '\r' and 0xa by '\n'. Seems to work with EOL=\n
// with only one patch.
// EDITS.CC

char TEditor::bufChar( uint32 p )
{
  if (p>=curPtr) p+=gapLen;
  return buffer[p];
}

uint32 TEditor::bufPtr(uint32 p)
{
  if (p<curPtr) return p;
  return (p+gapLen);
}

uint32 TEditor::lineEnd(uint32 p)
{
#if 1
  int32 di = p,cx,bx;
  bx = 0;
  cx = curPtr-di;
  if (cx<=0) goto lab1;
//  di += bx;
  while (cx--)
  {
    if (buffer[di++] == '\r') goto lab2;
    if (buffer[di-1] == '\n') goto lab2;
  }
  di = curPtr;
lab1:
  cx = bufLen;
  cx -= di;
  if (!cx) return di;
  bx += gapLen;
  di += bx;
  while (cx--)
  {
    if (buffer[di++] == '\r') goto lab2;
    if (buffer[di-1] == '\n') goto lab2;
  }
  goto lab3;
lab2:
  di--;
lab3:
  di-=bx;
  return di;
#else
  if (p == bufLen) return p;
  char c=bufChar(p);
  while (c != '\r' && c != '\n')
  {
    p++;
    if (p == bufLen) return p;
    c = bufChar(p);
  };
  return p;
#endif
}

uint32 TEditor::lineStart(uint32 p)
{
  int32 di = p,cx,bx;
  bx = 0;
  cx = di;
  cx -= curPtr;
  if (cx<=0) goto lab1;
  bx += gapLen;
  di += bx;
  di--;
  while (cx--)
  {
    if (buffer[di--] == '\r') goto lab2;
    if (buffer[di+1] == '\n') goto lab2;
  }
  bx -= gapLen;
  di = curPtr;
lab1:
  cx = di;
  if (!cx) goto lab4;
  di += bx;
  di--;
  while (cx--)
  {
    if (buffer[di--] == '\r') goto lab2;
    if (buffer[di+1] == '\n') goto lab2;
  }
  goto lab3;
lab2:
  di++;
  di++;
  di -= bx;
  if ((uint32)di == curPtr) goto lab4;
  if ((uint32)di == bufLen) goto lab4;
  // SET: When lines end only with \n it fails
  #ifdef CLY_UseCrLf
  if (buffer[di+bx] != '\n') goto lab4;
  di++;
  #endif
  goto lab4;
lab3:
  di = 0;
lab4:
  return di;
}

uint32 TEditor::nextChar(uint32 p)
{
 if (p==bufLen) return p;
 p++;
 if (p==bufLen) return p;
 #ifdef CLY_UseCrLf
 uint32 gl=0;
 if (p>=curPtr) gl=gapLen;
 if (buffer[gl+p]=='\n' && buffer[gl+p-1]=='\r') return (p+1);
 #endif
 return p;
}

uint32 TEditor::prevChar(uint32 p)
{
 if (!p) return p;
 p--;
 if (!p) return p;
 #ifdef CLY_UseCrLf
 uint32 gl=0;
 if (p>=curPtr) gl=gapLen;
 if (buffer[gl+p]=='\n' && buffer[gl+p-1]=='\r') return (p-1);
 #endif
 return p;
}

// SET: Static members.
// EDITSTAT.CC

ushort defEditorDialog( int, ... )
{
 return cmCancel;
}

TEditorDialog TEditor::editorDialog = defEditorDialog;
ushort TEditor::editorFlags = efBackupFiles | efPromptOnReplace;
char TEditor::findStr[maxFindStrLen] = "";
char TEditor::replaceStr[maxReplaceStrLen] = "";
TEditor *TEditor::clipboard = 0;
uint32 TEditor::tabSize = 8;
