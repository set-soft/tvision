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

#if defined( Uses_TDrawBuffer ) && !defined( __TDrawBuffer )
#define __TDrawBuffer

class TDrawBuffer
{

    friend class TView;

public:
    TDrawBuffer() { data2=NULL; }
    ~TDrawBuffer() { if (data2) DeleteArray(data2); }

    void moveChar( unsigned indent, char c, unsigned attr, unsigned count );
    void moveCharU16( unsigned indent, unsigned c, unsigned attr, unsigned count );
    void moveStr( unsigned indent, const char *str, unsigned attrs );
    void moveStrU16( unsigned indent, const uint16 *str, unsigned attrs );
    void moveCStr( unsigned indent, const char *str, unsigned attrs );
    void moveCStrU16( unsigned indent, const uint16 *str, unsigned attrs, unsigned attrs2 );
    void moveBuf( unsigned indent, const void *source,
                  unsigned attr, unsigned count );
    void moveBufU16( unsigned indent, const void *source,
                     unsigned attr, unsigned count );

    void putAttribute( unsigned indent, unsigned attr );
    void putAttributeU16( unsigned indent, unsigned attr );
    void putChar( unsigned indent, unsigned c );
    void putCharU16( unsigned indent, unsigned c );

    Boolean isU16();

protected:

    uint16 data[maxViewWidth];
    uint16 *data2;

    void verifyData2();

};

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

inline void TDrawBuffer::verifyData2()
{
    if( !data2 )
        data2 = new uint16[maxViewWidth];
}

inline Boolean TDrawBuffer::isU16()
{
    return data2!=NULL;
}

inline void TDrawBuffer::putAttribute( unsigned indent, unsigned attr )
{
    hiByte(data[indent]) = (uchar)attr;
}

inline void TDrawBuffer::putAttributeU16( unsigned indent, unsigned attr )
{
    verifyData2();
    hiByte(data[indent])  = (uchar)attr;
    hiByte(data2[indent]) = (uchar)(attr>>8);
}

inline void TDrawBuffer::putChar( unsigned indent, unsigned c )
{
    loByte(data[indent]) = (uchar)c;
}

inline void TDrawBuffer::putCharU16( unsigned indent, unsigned c )
{
    verifyData2();
    loByte(data[indent])  = (uchar)c;
    loByte(data2[indent]) = (uchar)(c>>8);
}

#endif  // Uses_TDrawBuffer

