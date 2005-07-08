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
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_opstream ) && !defined( __opstream )
#define __opstream

class TStreamableClass;
class TPWrittenObjects;

class CLY_EXPORT opstream : virtual public pstream
{
public:
    opstream( CLY_streambuf * );
    ~opstream();

    CLY_StreamPosT tellp();
    opstream& seekp( CLY_StreamPosT );
    opstream& seekp( CLY_StreamOffT, CLY_IOSSeekDir );
    opstream& flush();

    void writeByte( uchar );
    void writeBytes( const void *, size_t );
    void writeWord( ushort val ) { writeShort(val); };
    void writeString( const char * );
    void writeString16( const uint16 *str );
    void writeString( const uint16 *str ) { writeString16(str); };
    /* Platform dependent: */
    void writeShort(ushort val);
    void writeInt(uint val);
    void writeLong(ulong val);
    /* The following are platform independent (stores in little endian) */
    void write8(uint8 val) { writeByte(val); };
    void write16(uint16 val);
    void write32(uint32 val);
    void write64(uint64 val);

    opstream& operator << ( char           val ) { writeByte(val); return *this; };
    #ifndef TVComp_BCPP
    opstream& operator << ( signed char    val ) { writeByte(val); return *this; };
    #endif
    opstream& operator << ( unsigned char  val ) { writeByte(val); return *this; };
    opstream& operator << ( signed short   val ) { writeShort(val); return *this; };
    opstream& operator << ( unsigned short val ) { writeShort(val); return *this; };
    opstream& operator << ( signed int     val ) { writeInt(val); return *this; };
    opstream& operator << ( unsigned int   val ) { writeInt(val); return *this; };
    opstream& operator << ( signed long    val ) { writeLong(val); return *this; };
    opstream& operator << ( unsigned long  val ) { writeLong(val); return *this; };
    opstream& operator << ( float          val ) { writeBytes(&val,sizeof(val)); return *this; };
    opstream& operator << ( double         val ) { writeBytes(&val,sizeof(val)); return *this; };
    opstream& operator << ( long double    val ) { writeBytes(&val,sizeof(val)); return *this; };

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

