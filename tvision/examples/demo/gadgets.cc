/*-------------------------------------------------------------------*/
/*                                                                   */
/*   Turbo Vision Demo                                               */
/*                                                                   */
/*   Gadgets.cpp:  Gadgets for the Turbo Vision Demo.  Includes a    */
/*        heap view and a clock view which display the clock at the  */
/*        right end of the menu bar and the current heap space at    */
/*        the right end of the status line.                          */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
/*
 * Modified by Sergio Sigala <ssigala@globalnet.it>
 */

// SET: moved the standard headers before tv.h
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define Uses_TRect
#define Uses_TView
#define Uses_TDrawBuffer
#include <tv.h>

#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <iomanip.h>

#include "gadgets.h"

//extern "C" unsigned long farcoreleft( void );

//
// ------------- Heap Viewer functions
//

THeapView::THeapView(TRect& r) : TView( r )
{
    oldMem = 0;
    newMem = heapSize();

	/* SS: now resizing under X works well */
	growMode = gfGrowLoX | gfGrowLoY | gfGrowHiX | gfGrowHiY;
}


void THeapView::draw()
{
    TDrawBuffer buf;
    char c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, heapStr, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}


void THeapView::update()
{
    if( (newMem = heapSize()) != oldMem )
        {
        oldMem = newMem;
        drawView();
        }
}


long THeapView::heapSize()
{
	/* SS: changed */
#if 0
//#if !defined( __DPMI32__ )
//    long total = farcoreleft();
//#else
    long total = 0;
//#endif

#if !defined( __DPMI16__ ) && !defined( __DPMI32__ )
    struct farheapinfo heap;
#endif

    ostrstream totalStr( heapStr, sizeof heapStr);

//#if defined( __DPMI32__ )
//    switch( _HEAPEMPTY )
//#else
    switch( heapcheck() )
//#endif
        {
        case _HEAPEMPTY:
            strcpy(heapStr, "     No heap");
            total = -1;
            break;

        case _HEAPCORRUPT:
            strcpy(heapStr, "Heap corrupt");
            total = -2;
            break;

        case _HEAPOK:
#if !defined( __DPMI16__ ) && !defined( __DPMI32__ )
            heap.ptr = NULL;
            while(farheapwalk(&heap) != _HEAPEND)
                if(!heap.in_use)
                    total += heap.size;
#endif
            totalStr << setw(12) << total << ends;
            break;
        }
    return(total);
#endif
	strcpy(heapStr, "Hello world!");
	return -1;
}


//
// -------------- Clock Viewer functions
//

TClockView::TClockView( TRect& r ) : TView( r )
{
    strcpy(lastTime, "        ");
    strcpy(curTime, "        ");

	/* SS: now resizing under X works well */
	growMode = gfGrowLoX | gfGrowHiX;
}


void TClockView::draw()
{
    TDrawBuffer buf;
    char c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, curTime, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}


void TClockView::update()
{
    time_t t = time(0);
    char *date = ctime(&t);

    date[19] = '\0';
    strcpy(curTime, &date[11]);        /* Extract time. */

    if( strcmp(lastTime, curTime) )
        {
        drawView();
        strcpy(lastTime, curTime);
        }
}
