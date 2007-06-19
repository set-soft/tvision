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
/*   class fpstream                                                        */
/*                                                                         */
/*   Base class for reading and writing streamable objects to              */
/*   bidirectional file streams                                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpstream ) && !defined( __fpstream )
#define __fpstream

class CLY_EXPORT fpstream : public fpbase, public iopstream
{
public:
    fpstream();
    fpstream( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
    fpstream( CLY_OpenModeT, const char* , int = CLY_FBOpenProtDef );
    fpstream( int );
    fpstream( int, char *, int );
    ~fpstream();
    long filelength();

    CLY_filebuf *rdbuf();
    void open( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
};

#endif  // Uses_fpstream

