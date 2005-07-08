#ifndef NO_STREAM

#define Uses_TColorDisplay
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass CLY_EXPORT RColorDisplay( TColorDisplay::name,
                                TColorDisplay::build,
                                __DELTA(TColorDisplay)
                              );

#endif

