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
/*   class iopstream                                                       */
/*                                                                         */
/*   Base class for reading and writing streamable objects                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_iopstream ) && !defined( __iopstream )
#define __iopstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class iopstream : public ipstream, public opstream
{

public:

    iopstream( streambuf * );
    ~iopstream();

protected:

    iopstream();

};

#endif  // Uses_iopstream

