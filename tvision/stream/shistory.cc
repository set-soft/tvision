#ifndef NO_STREAM

#define Uses_THistory
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )
__link( RInputLine )

TStreamableClass RHistory( THistory::name,
                           THistory::build,
                           __DELTA(THistory)
                         );

#endif

