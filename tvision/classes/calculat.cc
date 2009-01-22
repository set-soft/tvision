/*------------------------------------------------------------*/
/*                                                            */
/*   Turbo Vision 1.0                                         */
/*   Copyright (c) 1991 by Borland International              */
/*                                                            */
/*   Calc.cpp:  TCalculator constructor                       */
/*                                                            */
/*------------------------------------------------------------*/

/*
  Modified by Salvador Eduardo Tropea <salvador@inti.gov.ar>
  <set@ieee.org> <set@computer.org>
  I ported it from TV 1.03.
 */

#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TRect
#define Uses_TEvent
#define Uses_TButton
#define Uses_TStreamableClass
#define Uses_TStreamable
#define Uses_TView
#define Uses_TDialog
#define Uses_TCalculator
#include <tv.h>

// SET: Moved to the class, made ASCII
const char *TCalculator::keyChar[20] =
    {    "C",   "<-",    "%",   "+-",
         "7",    "8",    "9",    "/",
         "4",    "5",    "6",    "*",
         "1",    "2",    "3",    "-",
         "0",    ".",    "=",    "+"
    };

TCalculator::TCalculator() :
    // SET: The display size must be satisfied
    TWindowInit( &TCalculator::initFrame ),
    TDialog( TRect(5, 3, 5+6+DISPLAYLEN, 18), __("Pocket Calculator") )
{
    TView *tv;
    TRect r;

    options |= ofFirstClick;

    // SET: enlarged buttons
    for(int i = 0; i <= 19; i++)
        {
        int x = (i%4)*6+3;
        int y = (i/4)*2+4;
        r = TRect( x, y, x+6, y+2 );

        tv = new TButton( r, keyChar[i], cmCalcButton+i, bfNormal | bfBroadcast );
        tv->options &= ~ofSelectable;
        insert( tv );
        }
    r = TRect( 3, 2, 3+DISPLAYLEN, 3 ); // SET, that's checked in setDisplay
    insert( new TCalcDisplay(r) );
}

#if !defined( NO_STREAM )
TStreamable *TCalculator::build()
{
    return new TCalculator( streamableInit );
}
#endif

