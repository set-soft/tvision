#define Uses_TListBox
#define Uses_TStreamableClass
#include <tv.h>
__link( RListViewer )

TStreamableClass RListBox( TListBox::name,
                           TListBox::build,
                           __DELTA(TListBox)
                         );

