#define Uses_TStreamableClass
#define Uses_TFileEditor
#include <tv.h>
__link( REditor )

TStreamableClass RFileEditor( TFileEditor::name,
                              TFileEditor::build,
                              __DELTA(TFileEditor)
                            );

