/*=========================================================================
   This TProgressBar class expands on Mr. Perez's upload TPROGB.ZIP. This
alternative lets you include the class definition without having to 
modify it for each main process. The dialog box that contructs TProgressBar
will be responsible for updating it. See the example in EXAMPLE.CPP. You'll
also find instructions on how to include the source in your TV.LIB and simply
#define Uses_TProgressBar before #include <tv.h>.

By: Barnaby W. Falls
CIS: 70662,1523
-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define Uses_string

#define Uses_TProgressBar
#define Uses_TView
#define Uses_TRect
#define Uses_TGroup
#define Uses_TDrawBuffer
#define Uses_TStreamableClass
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#include <tv.h>

#include "tprogbar.h"

__link( RView )

/* Registration object */
TStreamableClass RProgressBar( TProgressBar::name,
                              TProgressBar::build,
                              __DELTA(TProgressBar)
                            );

#define cpProgressBar "\x04"
/*                       ³
                         ÀÄÄ Progress Bar Attrib Pair
     cpProgressBar maps to TProgram::appPalette's index 4 which is, by
default, used for the ScrollBar Page. cpProgressBar represents the normal
progress bar color. In the constructor the foreground and background
attributes are swapped to form the highlight color. Thus the highlight will
always be the inverse of the bar color.
*/

const char * const TProgressBar::name = "TProgressBar";

TProgressBar::TProgressBar(const TRect& bounds, unsigned long aTotal, char abackChar) :
   TView(bounds)
{
         backChar = abackChar;
         total = aTotal;
         numOffset = (size.x/2)-3;
         bar = new char[size.x+1];
         memset(bar,backChar,size.x);
         bar[size.x] = '\0';
         charValue = (double)100/(double)size.x;
         progress =
         curPercent =
         curWidth = 0;
}

TProgressBar::~TProgressBar(){
   delete bar;
}

void TProgressBar::draw() {
   char string[4];
   sprintf(string,"%d",curPercent);
   string[3] = '\0';
   if(curPercent<10) {
      string[2] = string[0];
      string[1] = string[0] = ' ';
      }
   else if(curPercent<100 && curPercent>9) {
      string[2] = string[1];
      string[1] = string[0];
      string[0] = ' ';
      }
   TDrawBuffer nbuf;
   uchar colorNormal, colorHiLite;
   colorNormal = getColor(1);
   uchar fore = colorNormal >>4;                    // >>4 is same as /16
   colorHiLite = fore+((colorNormal-(fore<<4))<<4); // <<4 is same as *16
   nbuf.moveChar(0,backChar,colorNormal,size.x);
   nbuf.moveStr(numOffset,string,colorNormal);
   nbuf.moveStr(numOffset+3," %",colorNormal);
   unsigned i;
   for(i=0;i<curWidth;i++)
      nbuf.putAttribute(i,colorHiLite);
   writeLine(0, 0, size.x, 1, nbuf);
}


TPalette& TProgressBar::getPalette() const
{
   static TPalette palette( cpProgressBar, sizeof( cpProgressBar )-1 );
   return palette;
}


void TProgressBar::update(unsigned long aProgress) {
   progress = aProgress;
   calcPercent();
   drawView();
}

void TProgressBar::calcPercent() {
   unsigned int percent;
   unsigned int width;

   // calculate the new percentage
   percent = (int) ( ((double)progress/(double)total) * (double)100 );

   // percentage change?
   if(percent!=curPercent) {
      curPercent = percent;          // save new percentage
      width = (int)((double)curPercent/charValue);// calculate percentage bar width

      // width change?
      if(width!=curWidth) {
	 curWidth = width;          // save new width
        }
    }
}

// return the maximum iteration
unsigned long TProgressBar::getTotal() {
   return total;
}

// return the current iteration
unsigned long TProgressBar::getProgress() {
   return progress;
}

// set a new maximum iteration & update display
void TProgressBar::setTotal(unsigned long newTotal)
{
   unsigned long tmp = total;
   total = newTotal;
   memset(bar,backChar,size.x);
   curWidth   = 0;                    // current width of percentage bar
   progress   = 0;                    // current iteration
   curPercent = 0;                    // current percentage
   if(tmp)                // since it starts with 0, only update if changing
      drawView();                       // update the thermometer bar display
}

// set a new current iteration & update display
void TProgressBar::setProgress(unsigned long newProgress) {
   progress = newProgress;
   calcPercent();
   drawView();                       // paint the thermometer bar
}

void TProgressBar::write( opstream& os )
{
    TView::write( os );
    os.writeString( bar );
    os << backChar << total << progress << dispLen <<
          curPercent << curWidth << numOffset << charValue;
}

void *TProgressBar::read( ipstream& is )
{
    TView::read( is );
    bar = is.readString();
    is >> backChar >> total >> progress >> dispLen >>
          curPercent >> curWidth >> numOffset >> charValue;
    return this;
}

TStreamable *TProgressBar::build()
{
    return new TProgressBar( streamableInit );
}

