#ifndef NO_STREAM

#define Uses_TLabel
#define Uses_TStreamableClass
#include <tv.h>
__link( RStaticText )

TStreamableClass RLabel( TLabel::name,
                         TLabel::build,
                         __DELTA(TLabel)
                       );

#endif

