/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

SET: I introduced an important change: now buf is a pointer to a filebuf
and not a filebuf. This is much more convenient to make the code
compilable with gcc 3.x without breaking compatibility.

 *
 *
 */

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpbase ) && !defined( __fpbase )
#define __fpbase

class fpbase : virtual public pstream
{

public:

    fpbase();
    fpbase( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
    fpbase( int );
    fpbase( int, char *, int );
    ~fpbase();

    void open( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
    #ifdef CLY_HaveFBAttach
    void attach( int );
    #endif
    void close();
    void setbuf( char *, int );
    CLY_std(filebuf) * rdbuf();

private:

    CLY_std(filebuf) *buf;

};

#endif  // Uses_fpbase

