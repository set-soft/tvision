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

#define Uses_fpbase
#define Uses_fpstream
#define Uses_IOS_BIN
#include <tv.h>

fpstream::fpstream()
{
}

fpstream::fpstream( const char* name, int omode, int prot ) :
        fpbase( name, omode | ios::out | IOS_BIN, prot )
{
}

fpstream::fpstream( int f ) : fpbase( f )
{
}

fpstream::fpstream(int f, char* b, int len) : fpbase(f, b, len)
{
}

fpstream::~fpstream()
{
}

filebuf *fpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void fpstream::open( const char *name, int omode, int prot )
{
    fpbase::open( name, omode | ios::in | ios::out | IOS_BIN, prot );
}

