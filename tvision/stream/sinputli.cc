#ifndef NO_STREAM

#define Uses_TInputLine
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RInputLine( TInputLine::name,
                             TInputLine::build,
                             __DELTA(TInputLine)
                           );

#endif

