#ifndef NO_STREAM

#define Uses_TScrollBar
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RScrollBar( TScrollBar::name,
                              TScrollBar::build,
                              __DELTA(TScrollBar)
                           );

#endif

