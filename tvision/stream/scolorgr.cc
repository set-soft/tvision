#ifndef NO_STREAM

#define Uses_TColorGroupList
#define Uses_TStreamableClass
#include <tv.h>
__link( RListViewer )

TStreamableClass RColorGroupList( TColorGroupList::name,
                                  TColorGroupList::build,
                                  __DELTA(TColorGroupList)
                                );

#endif

