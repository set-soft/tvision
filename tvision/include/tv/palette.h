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

#if defined( Uses_TPalette ) && !defined( __TPalette )
#define __TPalette

class CLY_EXPORT TPalette
{

public:

    TPalette( const char *, ushort );
    TPalette( const TPalette& );
    ~TPalette();

    TPalette& operator = ( const TPalette& );

    uchar& operator[]( int index) const
    {
        return data[index];
    }

    uchar *data;

};

#endif  // Uses_TPalette

