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
#include <stdlib.h>
#include <time.h>
#define Uses_string
#include <stdio.h>

#define Uses_TFileInputLine
#define Uses_TEvent
#define Uses_TSearchRec
#define Uses_TFileInfoPane
#define Uses_TDrawBuffer
#define Uses_TFileDialog
#define Uses_TPalette
#include <tv.h>

#define cpInfoPane "\x1E"

TFileInfoPane::TFileInfoPane( const TRect& bounds ) :
    TView(bounds)
{
    eventMask |= evBroadcast;
    *(file_block.name) = EOS; // SET: We must have something in case there are no matches
}

const char * const TFileInfoPane::months[] =
    {
    "",__("Jan"),__("Feb"),__("Mar"),__("Apr"),__("May"),__("Jun"),
    __("Jul"),__("Aug"),__("Sep"),__("Oct"),__("Nov"),__("Dec")
    };

void TFileInfoPane::draw()
{
    Boolean PM;
    TDrawBuffer b;
    ushort  color;
#if 1
    struct tm *time;
#else
    ftime *time;
#endif
    char path[PATH_MAX];

    strcpy( path, ((TFileDialog *)owner)->directory );
    strcat( path, ((TFileDialog *)owner)->wildCard );
    CLY_fexpand( path );

    color = getColor(0x01);
    b.moveChar( 0, ' ', color, size.x );
    b.moveStr( 1, path, color );
    writeLine( 0, 0, size.x, 1, b );

    b.moveChar( 0, ' ', color, size.x );
    b.moveStr( 1, file_block.name, color );

    writeLine( 0, 1, size.x, 1, b);
    b.moveChar( 0, ' ', color, size.x );

    if ( *(file_block.name) != EOS )
    {

      char buf[10];
      sprintf(buf,"%ld",(long)file_block.size);
      b.moveStr( 14, buf, color );

      time = localtime(&file_block.time);
      if (time)
        {// SET: I don't know how many libc in the world behaves in this
         // stupid way, but Mingw32 980701-4 does it for some crazy dates.
         b.moveStr( 25, _(months[time->tm_mon+1]), color );
         sprintf(buf,"%02d",time->tm_mday);
         b.moveStr( 29, buf, color );
   
         b.putChar( 31, ',' );
         sprintf(buf,"%d",time->tm_year+1900);
         b.moveStr( 32, buf, color );
   
         PM = Boolean(time->tm_hour >= 12 );
         time->tm_hour %= 12;
   
         if ( time->tm_hour == 0 )
           time->tm_hour = 12;
         sprintf(buf,"%02d",time->tm_hour);
         b.moveStr( 38, buf, color );
         b.putChar( 40, ':' );
         sprintf(buf,"%02d",time->tm_min);
         b.moveStr( 41, buf, color );
   
         if ( PM )
           b.moveStr( 43, pmText, color );
         else
           b.moveStr( 43, amText, color );
        }
    }
    writeLine(0, 2, size.x, 1, b );
    b.moveChar( 0, ' ', color, size. x);
    writeLine( 0, 3, size.x, size.y-3, b);
}

TPalette& TFileInfoPane::getPalette() const
{
    static TPalette palette( cpInfoPane, sizeof( cpInfoPane )-1 );
    return palette;
}

void TFileInfoPane::handleEvent( TEvent& event )
{
    TView::handleEvent(event);
    if( event.what == evBroadcast && event.message.command == cmFileFocused )
        {
        file_block = *((TSearchRec *)(event.message.infoPtr));
        drawView();
        }
}

#if !defined( NO_STREAM )
TStreamable *TFileInfoPane::build()
{
    return new TFileInfoPane( streamableInit );
}
#endif // NO_STREAM

