#define Uses_TStreamableClass
#define Uses_TRangeValidator
#include <tv.h>
__link(FilterValidator)

TStreamableClass RRangeValidator( TRangeValidator::name,
                         TRangeValidator::build,
                         __DELTA(TRangeValidator)
                       );

