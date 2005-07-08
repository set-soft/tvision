#ifndef NO_STREAM

#define Uses_TColorDialog
#define Uses_TStreamableClass
#include <tv.h>
__link( RLabel )
__link( RButton )
__link( RDialog )
__link( RColorSelector )
__link( RMonoSelector )
__link( RColorGroupList )
__link( RColorDisplay )
__link( RColorItemList )
__link( RScrollBar )

TStreamableClass CLY_EXPORT RColorDialog( TColorDialog::name,
                               TColorDialog::build,
                               __DELTA(TColorDialog)
                             );

#endif

