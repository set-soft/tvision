/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

 *
 *
 */

#define Uses_string
#define Uses_fpbase
#define Uses_ofpstream
#define Uses_IOS_BIN
#include <tv.h>

UsingNamespaceStd

ofpstream::ofpstream()
{
}

ofpstream::ofpstream( const char* name, CLY_OpenModeT omode, int prot ) :
        fpbase( name, omode | CLY_IOSOut | CLY_IOSBin, prot )
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

CLY_filebuf *ofpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void ofpstream::open( const char *name, CLY_OpenModeT omode, int prot )
{
    fpbase::open( name, omode | CLY_IOSOut | CLY_IOSBin, prot );
}

