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
/*   class fpstream                                                        */
/*                                                                         */
/*   Base class for reading and writing streamable objects to              */
/*   bidirectional file streams                                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpstream ) && !defined( __fpstream )
#define __fpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class fpstream : public fpbase, public iopstream
{

public:

    fpstream();
    fpstream( const char *, int, int = FILEBUF_OPENPROT );
    fpstream( int );
    fpstream( int, char *, int );
    ~fpstream();

    filebuf * rdbuf();
    void open( const char *, int, int = FILEBUF_OPENPROT );

};


#endif  // Uses_fpstream

