#ifndef NO_STREAM

#define Uses_TStaticText
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass CLY_EXPORT RStaticText( TStaticText::name,
                              TStaticText::build,
                              __DELTA(TStaticText)
                            );

#endif

