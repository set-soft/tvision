#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TIndicator
#include <tv.h>
__link( RView )

TStreamableClass RIndicator( TIndicator::name,
                             TIndicator::build,
                             __DELTA(TIndicator)
                           );

#endif

