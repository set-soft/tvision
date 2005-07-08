#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TIndicator
#include <tv.h>
__link( RView )

TStreamableClass CLY_EXPORT RIndicator( TIndicator::name,
                             TIndicator::build,
                             __DELTA(TIndicator)
                           );

#endif

