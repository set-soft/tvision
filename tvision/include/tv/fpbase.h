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
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpbase ) && !defined( __fpbase )
#define __fpbase

#if !defined( __FSTREAM_H )
#include <fstream.h>
#endif  // __FSTREAM_H

class fpbase : virtual public pstream
{

public:

    fpbase();
    fpbase( const char *, int, int = FILEBUF_OPENPROT );
    fpbase( int );
    fpbase( int, char *, int );
    ~fpbase();

    void open( const char *, int, int = FILEBUF_OPENPROT );
    #ifndef TVComp_BCPP
    void attach( int );
    #endif
    void close();
    void setbuf( char *, int );
    filebuf * rdbuf();

private:

    filebuf buf;

};

#endif  // Uses_fpbase

