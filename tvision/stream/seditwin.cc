#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TEditWindow
#include <tv.h>
__link( RWindow )
__link( RFileEditor )

TStreamableClass CLY_EXPORT REditWindow( TEditWindow::name,
                              TEditWindow::build,
                              __DELTA(TEditWindow)
                            );

#endif

