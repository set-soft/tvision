#ifndef NO_STREAM

#define Uses_TColorItemList
#define Uses_TStreamableClass
#include <tv.h>
__link( RListViewer )

TStreamableClass RColorItemList( TColorItemList::name,
                                 TColorItemList::build,
                                 __DELTA(TColorItemList)
                               );

#endif

