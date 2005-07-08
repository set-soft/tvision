#ifndef NO_STREAM

#define Uses_TWindow
#define Uses_TStreamableClass
#include <tv.h>
__link( RGroup )
__link( RFrame )

TStreamableClass CLY_EXPORT RWindow( TWindow::name,
                          TWindow::build,
                          __DELTA(TWindow)
                        );

#endif

