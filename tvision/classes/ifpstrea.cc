/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

 *
 *
 */

#define Uses_fpbase
#define Uses_ifpstream
#define Uses_IOS_BIN
#include <tv.h>

UsingNamespaceStd

ifpstream::ifpstream()
{
}

ifpstream::ifpstream( const char* name, CLY_OpenModeT omode, int prot ) :
        fpbase( name, omode | CLY_IOSIn | CLY_IOSBin, prot )
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

CLY_filebuf *ifpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void ifpstream::open( const char *name, CLY_OpenModeT omode, int prot )
{
    fpbase::open( name, omode | CLY_IOSIn | CLY_IOSBin, prot );
}


