/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to compile for 64 bits architectures.

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

#define __DELTA( d ) ((uipointer)((TStreamable *)(d *)1)-1)

class CLY_EXPORT TStreamableClass
{

    friend class TStreamableTypes;
    friend class opstream;
    friend class ipstream;

public:

    TStreamableClass( const char *n, BUILDER b, int d );

private:

    const char *name;
    BUILDER build;
    int delta;

};

#endif  // Uses_TStreamableClass

