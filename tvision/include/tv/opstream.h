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
    void writeWord( ushort val ) { writeShort(val); };
    void writeString( const char * );
    /* Platform dependent: */
    void writeShort(ushort val);
    void writeInt(uint val);
    void writeLong(ulong val);
    /* The following are platform independent (stores in little endian) */
    void write8(uint8 val) { writeByte(); };
    void write16(uint16 val);
    void write32(uint32 val);
    void write64(uint64 val);

    opstream& operator << ( opstream&, char           val ) { ps.writeByte(val); return ps; };
    #ifndef __TURBOC__
    opstream& operator << ( opstream&, signed char    val ) { ps.writeByte(val); return ps; };
    #endif
    opstream& operator << ( opstream&, unsigned char  val ) { ps.writeByte(val); return ps; };
    opstream& operator << ( opstream&, signed short   val ) { ps.writeShort(val); return ps; };
    opstream& operator << ( opstream&, unsigned short val ) { ps.writeShort(val); return ps; };
    opstream& operator << ( opstream&, signed int     val ) { ps.writeInt(val); return ps; };
    opstream& operator << ( opstream&, unsigned int   val ) { ps.writeInt(val); return ps; };
    opstream& operator << ( opstream&, signed long    val ) { ps.writeLong(val); return ps; };
    opstream& operator << ( opstream&, unsigned long  val ) { ps.writeLong(val); return ps; };
    opstream& operator << ( opstream&, float          val ) { ps.writeBytes(&val,sizeof(val)); return ps; };
    opstream& operator << ( opstream&, double         val ) { ps.writeBytes(&val,sizeof(val)); return ps; };
    opstream& operator << ( opstream&, long double    val ) { ps.writeBytes(&val,sizeof(val)); return ps; };

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

