#define Uses_TStreamableClass
#define Uses_TMemo
#include <tv.h>
__link( REditor )

TStreamableClass RMemo( TMemo::name,
                        TMemo::build,
                        __DELTA(TMemo)
                      );

