/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
/*****************************************************************************

  Be careful, I think we never really tested this class. In fact I modified
the size of the memo structure because the old code uses 4Gb ;-)

*****************************************************************************/

#define Uses_string

#define Uses_TKeys
#define Uses_TEditor
#define Uses_TMemo
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#define cpMemo      "\x1A\x1B"

TMemo::TMemo( const TRect& bounds,
                  TScrollBar *aHScrollBar,
                  TScrollBar *aVScrollBar,
                  TIndicator *aIndicator,
                  uint32 aBufSize ) :
    TEditor( bounds, aHScrollBar, aVScrollBar, aIndicator, aBufSize )
{
}

uint32 TMemo::dataSize()
{
    return bufSize + sizeof( ushort );
}

void TMemo::getData( void *rec )
{
    TMemoData *data = (TMemoData *)rec;

    data->length = bufLen;
    memcpy(data->buffer, buffer, curPtr);
    memcpy(&data->buffer[curPtr], buffer+(curPtr + gapLen), bufLen - curPtr);
    memset(&data->buffer[bufLen], 0, bufSize - bufLen);
}

void TMemo::setData( void *rec )
{
    TMemoData *data = (TMemoData *)rec;

    memcpy(&buffer[bufSize - data->length], data->buffer, data->length);
    setBufLen(data->length);
}
 
TPalette& TMemo::getPalette() const
{
    static TPalette palette( cpMemo, sizeof( cpMemo )-1 );
    return palette;
}

void TMemo::handleEvent( TEvent& event )
{
    if( event.what != evKeyDown || event.keyDown.keyCode != kbTab )
        TEditor::handleEvent(event);
}

#ifndef NO_STREAM

void TMemo::write( opstream& os )
{
    TEditor::write( os );
    os << bufLen;
    os.writeBytes( buffer, curPtr );
    os.writeBytes( buffer + gapLen, bufLen - curPtr );
}

void *TMemo::read( ipstream& is )
{
    TEditor::read( is );
    uint32 length;
    is >> length;
    if( isValid )
        {
        is.readBytes( buffer + bufSize - length, length );
        setBufLen( length );
        }
    else
        is.seekg( is.tellg() + CLY_StreamPosT(length) );
    return this;
}

TStreamable *TMemo::build()
{
    return new TMemo( streamableInit );
}

TMemo::TMemo( StreamableInit ) : TEditor( streamableInit )
{
}

#endif // #ifndef NO_STREAM
