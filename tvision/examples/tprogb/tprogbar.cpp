#define Uses_stdlib
#define Uses_ctype
#define Uses_unistd

#define Uses_string
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TEventQueue
#define Uses_TRect
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_TDialog
#define Uses_TButton
#define Uses_StaticText
#define Uses_TSItem
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TDrawBuffer
#define Uses_TStreamableClass

#include <tv.h>
__link( RView )
__link( RDialog )
__link( RButton )

#include "tprogbar.h"

const int sampleIterations = 21 ;


TProgressBar::TProgressBar(const TRect& r, unsigned long iters ) : TView ( r )
{
	 options |= ofSelectable;
	 eventMask = (evKeyboard | evBroadcast);
	 maxWidth   = DISPLAYLEN ;//- 1 ;  // newWidth;         // maximum width of thermometer bar
	 curWidth   = 0;                    // current width of percentage bar
	 oldWidth   = 0;                    // old width of percentage bar
	 chPercent  = 100.0/maxWidth;       // percent per character
	 maxIter    = iters ;               // maximum iteration
	 curIter    = 0;                    // current iteration
	 oldPercent = 0;                    // old percentage
    curPercent = 0;                    // current percentage
    backChar   = '°';                  // background character
    percChar   = '²';                  // foreground character
    bar = new char[ maxWidth + 1];
    memset( bar, backChar, maxWidth );
	 bar[ maxWidth ] = '\0';
}

TProgressBar::~TProgressBar()
{
	 delete bar ;
}
 
void TProgressBar::handleEvent(TEvent& event)
{
    TView::handleEvent(event);
 
    switch(event.what)
    {
    case evKeyboard:
		  mainProcess() ;
		  break;
	 case evBroadcast:
		  if(event.message.command == cmOK)
				mainProcess() ;
		  break;
	 }
	 clearEvent(event);
}
 
void TProgressBar::draw()
{
    char color = getColor(1);
    TDrawBuffer nbuf;

	 nbuf.moveChar(0,' ',color,size.x);

	 nbuf.moveStr( 0, bar, color ) ;

    writeLine(0, 0, size.x, 1, nbuf);
}

void TProgressBar::mainProcess( void )
{
	 unsigned long cnt = 0 ;
	 for( int x = 0; x < sampleIterations; x++ )
	 {
		  setCurIter( ++cnt );         // set the current iteration count & update
		  usleep(500000);
	 }
	 usleep(500000);
	 message(owner,evCommand,cmOK,this);// close dialog box
}

void TProgressBar::calcPercent ( )
{
	 unsigned int percent;
	 unsigned int width;
    unsigned i;
 
    // calculate the new percentage
    percent = (int) ( ( (double)curIter / (double)maxIter ) * (double)100 );
 
    // percentage change?
    if ( percent != curPercent )
    {
        oldPercent = curPercent;       // save current percentage
        curPercent = percent;          // save new percentage
        width = (int)(curPercent / chPercent);// calculate percentage bar width
 
        // width change?
        if ( width != curWidth )
		  {
            oldWidth = curWidth;       // save the current width
				curWidth = width;          // save new width
 
            // update the bar string
				if ( oldWidth < curWidth )
            {
                for ( i = oldWidth; i < curWidth; i++ )
                {
                    bar[i] = percChar;
                }
            }
            else
            {
                for ( i = curWidth; i < oldWidth; i++ )
					 {
                    bar[i] = backChar;
                }
				}
        }
    }
}
 
// terminate the thermometer bar display
void TProgressBar::term ( )
{
}
 
// return the maximum iteration
unsigned long TProgressBar::getMaxIter ( )
{
    return ( maxIter );
}
 
// return the current iteration
unsigned long TProgressBar::getCurIter ( )
{
	 return ( curIter );
}

// set a new maximum iteration & update display
void TProgressBar::setMaxIter ( unsigned long newMax )
{
    unsigned long tmp = maxIter;
    maxIter = newMax;
	 memset( bar, backChar, maxWidth );
	 curWidth   = 0;                    // current width of percentage bar
	 oldWidth   = 0;                    // old width of percentage bar
	 curIter    = 0;                    // current iteration
	 oldPercent = 0;                    // old percentage
    curPercent = 0;                    // current percentage
	 if ( tmp )                // since it starts with 0, only update if changing
    {
		  drawView();                       // update the thermometer bar display
    }
}
 
// set a new current iteration & update display
void TProgressBar::setCurIter ( unsigned long newCur )
{
    curIter = newCur;
 
    calcPercent();
 
	 // width change?
	 if ( curPercent != oldPercent )
	 {
		  drawView();                       // paint the thermometer bar
	 }
}
