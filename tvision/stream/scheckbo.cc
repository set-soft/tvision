#define Uses_TCheckBoxes
#define Uses_TStreamableClass
#include <tv.h>

__link( RCluster )
TStreamableClass RCheckBoxes( TCheckBoxes::name,
                              TCheckBoxes::build,
                              __DELTA(TCheckBoxes)
                            );

