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

#define Uses_string
#define Uses_fpbase
#define Uses_ofpstream
#define Uses_IOS_BIN
#include <tv.h>

ofpstream::ofpstream()
{
}

ofpstream::ofpstream( const char* name, int omode, int prot ) :
        fpbase( name, omode | ios::out | IOS_BIN, prot )
{
}

ofpstream::ofpstream( int f ) : fpbase( f )
{
}

ofpstream::ofpstream(int f, char* b, int len) : fpbase(f, b, len)
{
}

ofpstream::~ofpstream()
{
}

filebuf *ofpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void ofpstream::open( const char *name, int omode, int prot )
{
    fpbase::open( name, omode | ios::out | IOS_BIN, prot );
}

