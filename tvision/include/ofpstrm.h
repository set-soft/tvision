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
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ofpstream ) && !defined( __ofpstream )
#define __ofpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class ofpstream : public fpbase, public opstream
{

public:

    ofpstream();
    ofpstream( const char *,
                      int = ios::out,
                      int = filebuf::openprot
                    );
    ofpstream( int );
    ofpstream( int, char *, int );
    ~ofpstream();

    filebuf * rdbuf();
    void open( const char *,
                      int = ios::out,
                      int = filebuf::openprot
                    );

};

#endif  // Uses_ofpstream

