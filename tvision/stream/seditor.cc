#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TEditor
#include <tv.h>
__link( RView )
__link( RScrollBar )
__link( RIndicator )

TStreamableClass CLY_EXPORT REditor( TEditor::name,
                          TEditor::build,
                          __DELTA(TEditor)
                        );

#endif

