#ifndef NO_STREAM

#define Uses_TMonoSelector
#define Uses_TStreamableClass
#include <tv.h>
__link( RCluster )

TStreamableClass CLY_EXPORT RMonoSelector( TMonoSelector::name,
                                TMonoSelector::build,
                                __DELTA(TMonoSelector)
                              );

#endif

