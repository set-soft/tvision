#ifndef NO_STREAM

#define Uses_TRadioButtons
#define Uses_TStreamableClass
#include <tv.h>

__link( RCluster )
TStreamableClass RRadioButtons( TRadioButtons::name,
                                TRadioButtons::build,
                                __DELTA(TRadioButtons)
                              );

#endif
