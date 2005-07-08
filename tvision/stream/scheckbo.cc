#ifndef NO_STREAM

#define Uses_TCheckBoxes
#define Uses_TStreamableClass
#include <tv.h>

__link( RCluster )
TStreamableClass CLY_EXPORT RCheckBoxes( TCheckBoxes::name,
                              TCheckBoxes::build,
                              __DELTA(TCheckBoxes)
                            );
TStreamableClass CLY_EXPORT RCheckBoxes32( TCheckBoxes32::name,
                                TCheckBoxes32::build,
                                __DELTA(TCheckBoxes32)
                              );

#endif

