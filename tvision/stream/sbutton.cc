#define Uses_TButton
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RButton( TButton::name,
                          TButton::build,
                          __DELTA(TButton)
                        );

