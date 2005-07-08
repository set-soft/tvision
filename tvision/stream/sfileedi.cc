#ifndef NO_STREAM

#define Uses_TStreamableClass
#define Uses_TFileEditor
#include <tv.h>
__link( REditor )

TStreamableClass CLY_EXPORT RFileEditor( TFileEditor::name,
                              TFileEditor::build,
                              __DELTA(TFileEditor)
                            );

#endif

