#ifndef NO_STREAM

#define Uses_TDialog
#define Uses_TStreamableClass
#include <tv.h>

__link( RWindow )

TStreamableClass RDialog( TDialog::name,
                          TDialog::build,
                          __DELTA(TDialog)
                        );

#endif

