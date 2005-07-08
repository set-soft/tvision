#ifndef NO_STREAM

#define Uses_TParamText
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )
__link( RStaticText )

TStreamableClass CLY_EXPORT RParamText( TParamText::name,
                              TParamText::build,
                              __DELTA(TParamText)
                            );

#endif

