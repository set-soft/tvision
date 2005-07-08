#ifndef NO_STREAM

#define Uses_TFileInputLine
#define Uses_TStreamableClass
#include <tv.h>
__link( RInputLine )

TStreamableClass CLY_EXPORT RFileInputLine( TFileInputLine::name,
                                 TFileInputLine::build,
                                __DELTA(TFileInputLine)
                               );

#endif

