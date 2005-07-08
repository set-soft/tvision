#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TCluster
#include <tv.h>
__link( RView )
__link( RStringCollection )

TStreamableClass CLY_EXPORT RCluster( TCluster::name,
                           TCluster::build,
                           __DELTA(TCluster)
                         );

#endif

