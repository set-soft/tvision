/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

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

class TStreamableTypes;

class CLY_EXPORT pstream
{
    friend class TStreamableTypes;

public:
    enum StreamableError { peNotRegistered, peInvalidType };
    enum PointerTypes { ptNull, ptIndexed, ptObject };

    pstream(CLY_streambuf *);
    virtual ~pstream();

    int rdstate() const;
    int eof() const;
    int fail() const;
    int bad() const;
    int good() const;
    void clear( int = 0 );
    operator void *() const;
    int operator ! () const;

    CLY_streambuf *rdbuf() const;

    static void initTypes();
    // SET: called atexit to kill the collection
    static void deInitTypes();

    void error( StreamableError );
    void error( StreamableError, const TStreamable& );
    static void registerType( TStreamableClass *ts );

protected:
    pstream();

    CLY_streambuf *bp;
    int state;

    void init(CLY_streambuf *);
    void setstate( int );

    static TStreamableTypes * types;
};

#endif  // Uses_pstream

