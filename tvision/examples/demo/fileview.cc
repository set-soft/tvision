/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision FileViewer Demo Support File             */
/*                                                         */
/*---------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
/*
 * Modified by Sergio Sigala <ssigala@globalnet.it>
 * Modified by Salvador E. Tropea <set@ieee.org>
 */

// SET: moved the standard headers before tv.h
#define Uses_stdio
#define Uses_string
#define Uses_stdlib
#define Uses_ctype
#define Uses_IfStreamGetLine
#define Uses_fstream

#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TScroller
#define Uses_TDrawBuffer
#define Uses_TRect
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TStreamableClass
#define Uses_TEditor          // JASC, cmOpen
#include <tv.h>
__link(RScroller)
__link(RScrollBar)

#include "tvcmds.h"
#include "fileview.h"

UsingNamespaceStd

const char * const TFileViewer::name = "TFileViewer";

TFileViewer::TFileViewer( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar,
                          const char *aFileName) :
    TScroller( bounds, aHScrollBar, aVScrollBar )
{
    growMode = gfGrowHiX | gfGrowHiY;
    isValid = True;
    fileName = 0;
    readFile( aFileName );
    delta.x=delta.y=0;
}

TFileViewer::~TFileViewer()
{
     delete [] fileName;
     CLY_destroy(fileLines);
}

void TFileViewer::draw()
{
    char *p;

    ushort c =  getColor(0x0301);
    for( short i = 0; i < size.y; i++ )
        {
        TDrawBuffer b;
        b.moveChar( 0, ' ', c, (short)size.x );

        if( delta.y + i < fileLines->getCount() )
            {
            char s[maxLineLength+1];
            p = (char *)( fileLines->at(delta.y+i) );
            if( p == 0 || (int)strlen(p) < delta.x )
                s[0] = EOS;
            else
            {
                strncpy( s, p+delta.x, size.x );
                if( (int)strlen( p + delta.x ) > size.x )
                    s[size.x] = EOS;
                }
            b.moveStr( 0, s, c );
            }
        writeBuf( 0, i, (short)size.x, 1, b );
        }
}

void TFileViewer::scrollDraw()
{
    TScroller::scrollDraw();
    draw();
}

void TFileViewer::readFile( const char *fName )
{
    delete fileName;

    limit.x = 0;
    fileName = newStr( fName );
    fileLines = new TLineCollection(5, 5);
    ifstream fileToView( fName );
    if( !fileToView )
        {
        messageBox( "Invalid drive or directory", mfError | mfOKButton );
        isValid = False;
        }
    else
        {
        char line[maxLineLength+1];
        // SET: Changed to use getline as suggested by Andris.
        // New standards makes the use of get incorrect.
        while( !lowMemory() &&
               !fileToView.eof() &&
               IfStreamGetLine(fileToView,line,sizeof line)
             )
            {
            limit.x = max( limit.x, (int)strlen( line ) );
            fileLines->insert( newStr( line ) );
            }
        isValid = True;
        }
    limit.y = fileLines->getCount();
}

void TFileViewer::setState( ushort aState, Boolean enable )
{
    TScroller::setState( aState, enable );
    if( enable && (aState & sfExposed) )
        setLimit( limit.x, limit.y );
}

Boolean TFileViewer::valid( ushort )
{
    return isValid;
}

void *TFileViewer::read(ipstream& is)
{
    char *fName;

    TScroller::read(is);
    fName = is.readString();
    fileName = 0;
    readFile(fName);
    delete fName; 
    return this;
}

void TFileViewer::write(opstream& os)
{
    TScroller::write(os);
    os.writeString(fileName);
}

TStreamable *TFileViewer::build()
{
    return new TFileViewer( streamableInit );
}


TStreamableClass RFileView( TFileViewer::name,
                            TFileViewer::build,
                              __DELTA(TFileViewer)
                          );



static short winNumber = 0;

TFileWindow::TFileWindow( const char *fileName ) :
    TWindowInit( &TFileWindow::initFrame ),
    TWindow( TProgram::deskTop->getExtent(), fileName, winNumber++ )
{
    options |= ofTileable;
    TRect r( getExtent() );
    r.grow(-1, -1);
    insert(new TFileViewer( r,
                            standardScrollBar(sbHorizontal | sbHandleKeyboard),
                            standardScrollBar(sbVertical | sbHandleKeyboard),
                            fileName) );
}
