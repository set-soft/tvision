/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableClass                                                */
/*                                                                         */
/*   Used internally by TStreamableTypes and pstream.                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TStreamableClass ) && !defined( __TStreamableClass )
#define __TStreamableClass

#include <limits.h>

const P_id_type P_id_notFound = UINT_MAX;

typedef TStreamable *(*BUILDER)();
#define __DELTA( d ) (FP_OFF((TStreamable *)(d *)1)-1)

class TStreamableClass
{

    friend TStreamableTypes;
    friend opstream;
    friend ipstream;

public:

    TStreamableClass( const char *n, BUILDER b, int d );

private:

    const char *name;
    BUILDER build;
    int delta;

};

#endif  // Uses_TStreamableClass

