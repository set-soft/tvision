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

#define Uses_fpbase
#define Uses_ofpstream
#include <tv.h>

ofpstream::ofpstream()
{
}

ofpstream::ofpstream( const char* name, int omode, int prot ) :
        fpbase( name, omode | ios::out | ios::bin, prot )
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
    fpbase::open( name, omode | ios::out | ios::bin, prot );
}

