#define Uses_TStreamableClass
#define Uses_TFilterValidator
#include <tv.h>

TStreamableClass RFilterValidator( TFilterValidator::name,
                         TFilterValidator::build,
                         __DELTA(TFilterValidator)
                       );

