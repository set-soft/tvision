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
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_opstream ) && !defined( __opstream )
#define __opstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H


class TStreamableClass;
class TPWrittenObjects;

class opstream : virtual public pstream
{

public:

    opstream( streambuf * );
    ~opstream();

    streampos tellp();
    opstream& seekp( streampos );
    opstream& seekp( streamoff, ios::seek_dir );
    opstream& flush();

    void writeByte( uchar );
    void writeBytes( const void *, size_t );
    void writeWord( ushort );
    void writeString( const char * );

    friend opstream& operator << ( opstream&, char );
#ifndef __TURBOC__
    friend opstream& operator << ( opstream&, signed char );
#endif
    friend opstream& operator << ( opstream&, unsigned char );
    friend opstream& operator << ( opstream&, signed short );
    friend opstream& operator << ( opstream&, unsigned short );
    friend opstream& operator << ( opstream&, signed int );
    friend opstream& operator << ( opstream&, unsigned int );
    friend opstream& operator << ( opstream&, signed long );
    friend opstream& operator << ( opstream&, unsigned long );
    friend opstream& operator << ( opstream&, float );
    friend opstream& operator << ( opstream&, double );
    friend opstream& operator << ( opstream&, long double );

    friend opstream& operator << ( opstream&, TStreamable& );
    friend opstream& operator << ( opstream&, TStreamable * );

protected:

    opstream();

    void writePrefix( const TStreamable& );
    void writeData( TStreamable& );
    void writeSuffix( const TStreamable& );

    P_id_type find( const void *adr );
    void registerObject( const void *adr );

private:

    TPWrittenObjects *objs;

};

#endif  // Uses_opstream

