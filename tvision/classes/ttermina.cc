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

#define Uses_TTextDevice
#define Uses_TTerminal
#define Uses_otstream
#include <tv.h>

#include <string.h>

TTerminal::TTerminal( const TRect& bounds,
                      TScrollBar *aHScrollBar,
                      TScrollBar *aVScrollBar,
                      uint32 aBufSize ) :
    TTextDevice(bounds, aHScrollBar, aVScrollBar),
    queFront( 0 ),
    queBack( 0 )
{
    growMode = gfGrowHiX + gfGrowHiY;
    bufSize = aBufSize;
    buffer = new char[ bufSize ];
    setLimit( 0, 1 );
    setCursor( 0, 0 );
    showCursor();
}


TTerminal::~TTerminal()
{
    delete buffer;
}

void TTerminal::bufDec( uint32& val )
{
    if (val == 0)
        val = bufSize - 1;
    else
        val--;
}

void TTerminal::bufInc( uint32& val )
{
    if( ++val >= bufSize )
        val = 0;
}

Boolean TTerminal::canInsert( uint32 amount )
{
    int32 T = (queFront < queBack) ?
        ( queFront +  amount ) :
        ( int32(queFront) - bufSize + amount);   // cast needed so we get
                                                // signed comparison
    return Boolean( (int32)queBack > T );
}

void TTerminal::draw()
{
    short  i;
    uint32 begLine, endLine;
    char s[256];
    int32 bottomLine;

    bottomLine = size.y + delta.y;
    if( limit.y > bottomLine )
        {
        endLine = prevLines( queFront, limit.y - bottomLine );
        bufDec( endLine );
        }
    else
        endLine = queFront;

    if( limit.y > size.y )
        i = size.y - 1;
    else
        {
        for( i = limit.y; i <= size.y - 1; i++ )
            writeChar(0, i, ' ', 1, size.x);
        i =  limit.y -  1;
        }

    for( ; i >= 0; i-- )
        {
        begLine = prevLines(endLine, 1);
        if (endLine >= begLine)
            {
            int T = int( endLine - begLine );
            memcpy( s, &buffer[begLine], T );
            s[T] = EOS;
            }
        else
            {
            int T = int( bufSize - begLine);
            memcpy( s, &buffer[begLine], T );
            memcpy( s+T, buffer, endLine );
            s[T+endLine] = EOS;
            }
        if( delta.x >= (int32)strlen(s) )
            *s = EOS;
        else
            strcpy( s, &s[delta.x] );

        writeStr( 0, i, s, 1 );
        writeChar( strlen(s), i, ' ', 1, size.x );
        endLine = begLine;
        bufDec( endLine );
        }
}

uint32 TTerminal::nextLine( uint32 pos )
{
    if( pos != queFront )
        {
        while( buffer[pos] != '\n' && pos != queFront )
            bufInc(pos);
        if( pos != queFront )
            bufInc( pos );
        }
    return pos;
}

streamsize TTerminal::do_sputn( const char *s, int count )
{
    ushort screenLines = limit.y;
    int32 i;
    for( i = 0; i < count; i++ )
        if( s[i] == '\n' )
            screenLines++;

    while( !canInsert( count ) )
        {
        queBack = nextLine( queBack );
        screenLines--;
        }

    if( queFront + count >= bufSize )
        {
        i = bufSize - queFront;
        memcpy( &buffer[queFront], s, i );
        memcpy( buffer, &s[i], count - i );
        queFront = count - i;
        }
    else
        {
        memcpy( &buffer[queFront], s, count );
        queFront += count;
        }

    setLimit( limit.x, screenLines );
    scrollTo( 0, screenLines + 1 );
    i = prevLines( queFront, 1 );
    if( i <= (int32)queFront )
        i = queFront - i;
    else
        i = bufSize - (i - queFront);
    setCursor( i, screenLines - delta.y - 1 );
    drawView();
    return count;
}

Boolean TTerminal::queEmpty()
{
    return Boolean( queBack == queFront );
}

otstream::otstream( TTerminal *tt )
{
    ios::init( tt );
}

#define incdi \
  pos++; \
  if (pos>bufSize) pos = bufSize

#define decdi \
  if (!pos) pos = bufSize; \
  pos --

#define LineSeparator 10

uint32 TTerminal::prevLines(uint32 pos, uint32 Lines)
{
  uint32 count;
  if (Lines == 0)
  {
    incdi;
    incdi;
    return pos;
  }
  if (queBack==pos) return pos;
  decdi;
  while (1)
  {
    if (pos>queBack) count = pos-queBack;
    else count = pos;
    count++;
    while (buffer[pos] != '\n' && count)
    {
      pos--;
      count--;
    }
    if (buffer[pos+1] == '\n')
    {
      Lines--;
      if (Lines == 0)
      {
        incdi;
        incdi;
        return pos;
      }
    }
    if ((pos+1) == queBack) return pos+1;
    pos = bufSize-1;
  }
}

