#ifndef NO_STREAM

#define Uses_TCalcDisplay
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RCalcDisplay( TCalcDisplay::name,
                               TCalcDisplay::build,
                               __DELTA(TCalcDisplay)
                             );

#endif

