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
 * Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
 * Andris Pavenis.
 * Modified by Mike Gorchak <mike@malva.ua> to report free memory on QNX.
 */
#include <tv/configtv.h>

// SET: moved the standard headers before tv.h
#define Uses_string
#define Uses_stdlib
#define Uses_ctype
#define Uses_time
#define Uses_iomanip
#if defined(TVOSf_QNXRtP) || defined(TVOSf_QNX4)
 #define Uses_stdio
 #define Uses_sys_stat
#endif // TVOSf_QNXRtP

#define Uses_TRect
#define Uses_TView
#define Uses_TDrawBuffer
#include <tv.h>

#ifdef TVOSf_QNX4
 #include <sys/osinfo.h>
#endif // TVOSf_QNX4

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
 #if defined(TVOSf_QNXRtP)
   struct stat st;
   long rval=0;

   if (stat("/proc", &st)==0)
   {
      rval=st.st_size;
   }

   if (rval>1024UL*512UL) // one half megabyte !
   {
      if (rval>1024UL*1024UL*32UL) // if above 32Mb free
      {
         sprintf(heapStr, "%10dMb", rval/1024UL/1024UL);
      }
      else
      {
         sprintf(heapStr, "%10dKb", rval/1024UL);
      }
   }
   else
   {
      sprintf(heapStr, "%12d", rval);
   }

   return rval;
 #elif defined(TVOSf_QNX4)

   _osinfo CurrInfo;
   unsigned long rval;
   
   qnx_osinfo(0, &CurrInfo);
   rval=CurrInfo.freepmem;

   if (rval>1024UL*512UL) // one half megabyte !
   {
      if (rval>1024UL*1024UL*32UL) // if above 32Mb free
      {
         sprintf(heapStr, "%10dMb", rval/1024UL/1024UL);
      }
      else
      {
         sprintf(heapStr, "%10dKb", rval/1024UL);
      }
   }
   else
   {
      sprintf(heapStr, "%12d", rval);
   }

   return rval;

 #else
	/* SS: changed */
	strcpy(heapStr, "Hello world!");
	return -1;
 #endif // TVOSf_QNXRtP
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
