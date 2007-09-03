/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
 *
 *
 */

#define Uses_string //memcpy()
#define Uses_TPalette
#include <tv.h>

TPalette::TPalette( const char* d, ushort len ) :
    data( new uchar[ len+1 ] )
{
    data[0] = len;
    memcpy( data+1, d, len );
}

TPalette::TPalette( const TPalette& tp ) :
    data( new uchar[ tp.data[0]+1 ] )
{
    memcpy( data, tp.data, tp.data[0]+1 );
}

TPalette::~TPalette()
{
    delete[] data;
}

TPalette& TPalette::operator = ( const TPalette& tp )
{
    if( data != tp.data )
        {
        if( data[0] != tp.data[0] )
            {
            delete[] data;
            data = new uchar[ tp.data[0]+1 ];
            data[0] = tp.data[0];
            }
        memcpy( data+1, tp.data+1, data[0] );
        }
    return *this;
}


