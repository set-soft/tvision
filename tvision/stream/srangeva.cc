#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TRangeValidator
#include <tv.h>
__link(RFilterValidator)

TStreamableClass RRangeValidator( TRangeValidator::name,
                         TRangeValidator::build,
                         __DELTA(TRangeValidator)
                       );

#endif

