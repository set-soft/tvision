/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

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
                      CLY_OpenModeT = ios::out,
                      int = CLY_FBOpenProtDef
                    );
    ofpstream( int );
    ofpstream( int, char *, int );
    ~ofpstream();

    filebuf * rdbuf();
    void open( const char *,
                      CLY_OpenModeT = ios::out,
                      int = CLY_FBOpenProtDef
                    );

};

#endif  // Uses_ofpstream

