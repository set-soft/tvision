#define Uses_TMonoSelector
#define Uses_TStreamableClass
#include <tv.h>
__link( RCluster )

TStreamableClass RMonoSelector( TMonoSelector::name,
                                TMonoSelector::build,
                                __DELTA(TMonoSelector)
                              );

