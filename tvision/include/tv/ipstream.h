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
/*   class ipstream                                                        */
/*                                                                         */
/*   Base class for reading streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ipstream ) && !defined( __ipstream )
#define __ipstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

class TStreamableClass;
class TPReadObjects;

class ipstream : virtual public pstream
{

public:

    ipstream( streambuf * );
    ~ipstream();

    streampos tellg();
    ipstream& seekg( streampos );
    ipstream& seekg( streamoff, ios::seek_dir );

    uchar readByte();
    void readBytes( void *, size_t );
    ushort readWord();
    char * readString();
    char * readString( char *, unsigned );

    friend ipstream& operator >> ( ipstream&, char& );
#ifndef __TURBOC__
    friend ipstream& operator >> ( ipstream&, signed char& );
#endif
    friend ipstream& operator >> ( ipstream&, unsigned char& );
    friend ipstream& operator >> ( ipstream&, signed short& );
    friend ipstream& operator >> ( ipstream&, unsigned short& );
    friend ipstream& operator >> ( ipstream&, signed int& );
    friend ipstream& operator >> ( ipstream&, unsigned int& );
    friend ipstream& operator >> ( ipstream&, signed long& );
    friend ipstream& operator >> ( ipstream&, unsigned long& );
    friend ipstream& operator >> ( ipstream&, float& );
    friend ipstream& operator >> ( ipstream&, double& );
    friend ipstream& operator >> ( ipstream&, long double& );

    friend ipstream& operator >> ( ipstream&, TStreamable& );
    friend ipstream& operator >> ( ipstream&, void *& );

protected:

    ipstream();

    const TStreamableClass * readPrefix();
    void * readData( const TStreamableClass *,
                                       TStreamable * );
    void readSuffix();

    const void * find( P_id_type );
    void registerObject( const void *adr );

private:

    TPReadObjects *objs;

};

#endif  // Uses_ipstream

