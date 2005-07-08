#ifndef NO_STREAM

#define Uses_TScroller
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass CLY_EXPORT RScroller( TScroller::name,
                             TScroller::build,
                             __DELTA(TScroller)
                          );

#endif

