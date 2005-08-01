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

class CLY_EXPORT TDrawBuffer
{

    friend class TView;

public:

    void moveChar( unsigned indent, char c, unsigned attr, unsigned count );
    void moveStr( unsigned indent, const char *str, unsigned attrs );
    void moveCStr( unsigned indent, const char *str, unsigned attrs );
    void moveBuf( unsigned indent, const void *source,
                  unsigned attr, unsigned count );

    void putAttribute( unsigned indent, unsigned attr );
    void putChar( unsigned indent, unsigned c );

protected:

    ushort data[maxViewWidth];

};

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

inline void TDrawBuffer::putAttribute( unsigned indent, unsigned attr )
{
    hiByte(data[indent]) = (uchar)attr;
}

inline void TDrawBuffer::putChar( unsigned indent, unsigned c )
{
    loByte(data[indent]) = (uchar)c;
}

#endif  // Uses_TDrawBuffer

