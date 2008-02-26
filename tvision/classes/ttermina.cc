/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <tv/configtv.h>

#define Uses_string

#define Uses_TTextDevice
#define Uses_TTerminal
#define Uses_otstream
#include <tv.h>

UsingNamespaceStd

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

Boolean TTerminal::canInsert( uint32 amount )
{
    int32 T = (queFront < queBack) ?
        ( queFront +  amount ) :
        ( int32(queFront) - bufSize + amount);   // cast needed so we get
                                                // signed comparison
    return Boolean( (int32)queBack > T );
}

const int MaxLineLen=256;

void TTerminal::draw()
{
    short  i;
    uint32 begLine, endLine;
    char s[MaxLineLen];
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
           if (T>=MaxLineLen)
              T=MaxLineLen-1;
           memcpy( s, &buffer[begLine], T );
           s[T] = EOS;
          }
        else
          {
           int T=int( bufSize - begLine);
           if (T>=MaxLineLen)
              T=MaxLineLen-1;
           memcpy( s, &buffer[begLine], T );
           int T2=endLine;
           if (T+T2>=MaxLineLen)
              T2=MaxLineLen-T-1;
           memcpy( s+T, buffer, T2 );
           s[T+T2] = EOS;
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

int TTerminal::do_sputn( const char *s, int count )
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

// The SSC code doesn't support as much as needed for it.
#ifndef HAVE_SSC
otstream::otstream( TTerminal *tt )
    #ifdef CLY_ISOCpp98
    // SET: Andris used it. It looks like the new standard doesn't have a
    // default constructor
    : std::ostream (tt)
    #endif
{
    ios::init( tt );
}
// HAVE_SSC
#endif

// SET: It was really broken, but as we never used it...
// I rewrote it. I also used bufInc and bufDec and made it inline. (instead
// of the old macros).
uint32 TTerminal::prevLines(uint32 posStart, uint32 Lines)
{
 uint32 pos=posStart;

 // If that's the start just return, we can't go back. Remmember that's a
 // circular buffer.
 if (!Lines || pos==queBack)
    return pos;

 // go back 1 character, is to skip the EOL
 bufDec(pos);

 if (pos<queBack)
   { // Scan the first half of the buffer
    do
      {
       if (buffer[pos]=='\n')
         {
          Lines--;
          if (!Lines)
            {
             bufInc(pos); // Because we are over the \n
             return pos;
            }
         }
      }
    while (pos--);
    // Not here, go to the end
    pos=bufSize-1;
   }

 // Scan the second half (first from the point of view of the text)
 do
   {
    if (buffer[pos]=='\n')
      {
       Lines--;
       if (!Lines)
         {
          bufInc(pos);
          return pos;
         }
      }
   }
 while (--pos>=queBack);

 return queBack;
}

