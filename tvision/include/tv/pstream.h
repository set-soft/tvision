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
/*   class pstream                                                         */
/*                                                                         */
/*   Base class for handling streamable objects.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_pstream ) && !defined( __pstream )
#define __pstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class TStreamableTypes;

class pstream
{

    friend class TStreamableTypes;

public:

    enum StreamableError { peNotRegistered, peInvalidType };
    enum PointerTypes { ptNull, ptIndexed, ptObject };

    pstream( streambuf * );
    virtual ~pstream();

    int rdstate() const;
    int eof() const;
    int fail() const;
    int bad() const;
    int good() const;
    void clear( int = 0 );
    operator void *() const;
    int operator ! () const;

    streambuf * rdbuf() const;

    static void initTypes();
    // SET: called atexit to kill the collection
    static void deInitTypes();

    void error( StreamableError );
    void error( StreamableError, const TStreamable& );
    static void registerType( TStreamableClass *ts );

protected:

    pstream();

    streambuf *bp;
    int state;

    void init( streambuf * );
    void setstate( int );

    static TStreamableTypes * types;

};

#endif  // Uses_pstream

