#ifndef NO_STREAM

#define Uses_TFileDialog
#define Uses_TStreamableClass
#include <tv.h>
__link( RDialog )
__link( RFileInputLine )
__link( RFileList )

TStreamableClass CLY_EXPORT RFileDialog( TFileDialog::name,
                              TFileDialog::build,
                              __DELTA(TFileDialog)
                            );

#endif

