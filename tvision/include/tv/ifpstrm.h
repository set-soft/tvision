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

class CLY_EXPORT ifpstream : public fpbase, public ipstream
{
public:
    ifpstream();
    ifpstream( const char *,
                      CLY_OpenModeT = CLY_IOSIn,
                      int = CLY_FBOpenProtDef
                    );
    ifpstream( int );
    ifpstream( int, char *, int );
    ~ifpstream();

    CLY_filebuf *rdbuf();
    void open( const char *,
                      CLY_OpenModeT = CLY_IOSIn,
                      int = CLY_FBOpenProtDef
                    );
};

#endif  // Uses_ifpstream

