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
/*   class ifpstream                                                       */
/*                                                                         */
/*   Base class for reading streamable objects from file streams           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ifpstream ) && !defined( __ifpstream )
#define __ifpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class ifpstream : public fpbase, public ipstream
{

public:

    ifpstream();
    ifpstream( const char *,
                      CLY_OpenModeT = ios::in,
                      int = CLY_FBOpenProtDef
                    );
    ifpstream( int );
    ifpstream( int, char *, int );
    ~ifpstream();

    filebuf * rdbuf();
    void open( const char *,
                      CLY_OpenModeT = ios::in,
                      int = CLY_FBOpenProtDef
                    );

};

#endif  // Uses_ifpstream

