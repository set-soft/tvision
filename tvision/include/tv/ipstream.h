/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Robert H”hne to be used for RHIDE.
 *
 * Modified by Jose Angel Sanchez Caso (JASC) to have machine endian and
 *  integer size compatibility.
 *
 * Added functions to rework the endian stuff by SET: readShort, readInt,
 * readLong, read8, read16, read32 and read64.
 *
 * ------------------------------------------------------------------------*
 *                                                                         *
 *   class ipstream                                                        *
 *                                                                         *
 *   Base class for reading streamable objects                             *
 *                                                                         *
 * ------------------------------------------------------------------------*/

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
    ushort readWord() { return readShort(); };
    void  readBytes( void *, size_t );
    char * readString();
    char * readString( char *, unsigned );
    /* Platform dependent: */
    ushort readShort();
    uint   readInt();
    ulong  readLong();
    /* The following are platform independent (stores in little endian) */
    uint8  read8() { return readByte(); };
    uint16 read16();
    uint32 read32();
    uint64 read64();

    #ifndef TVComp_BCPP
    ipstream& operator >> (signed char    &ch ) {ch=readByte();  return (*this);}
    #endif
    ipstream& operator >> (char           &ch ) {ch=readByte();  return (*this);}
    ipstream& operator >> (unsigned char  &ch ) {ch=readByte();  return (*this);}
    ipstream& operator >> (signed short   &sh ) {sh=readShort();  return (*this);}
    ipstream& operator >> (unsigned short &sh ) {sh=readShort();  return (*this);}
    ipstream& operator >> (signed int     &i  ) { i=readInt(); return (*this);}
    ipstream& operator >> (unsigned int   &i  ) { i=readInt(); return (*this);}
    ipstream& operator >> (signed long    &l  ) { l=readLong(); return (*this);}
    ipstream& operator >> (unsigned long  &l  ) { l=readLong(); return (*this);}
    ipstream& operator >> (float          &f  ) { readBytes( &f, sizeof(f) ); return (*this);}
    ipstream& operator >> (double         &d  ) { readBytes( &d, sizeof(d) ); return (*this);}

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

