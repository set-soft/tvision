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
#define Uses_ifpstream
#include <tv.h>

ifpstream::ifpstream()
{
}

ifpstream::ifpstream( const char* name, int omode, int prot ) :
        fpbase( name, omode | ios::in | IOS_BIN, prot )
{
}

ifpstream::ifpstream( int f ) : fpbase( f )
{
}

ifpstream::ifpstream(int f, char* b, int len) : fpbase(f, b, len)
{
}

ifpstream::~ifpstream()
{
}

filebuf *ifpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void ifpstream::open( const char *name, int omode, int prot )
{
    fpbase::open( name, omode | ios::in | IOS_BIN, prot );
}

