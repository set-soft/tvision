/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.
Added filelength member by Salvador E. Tropea (SET)
Added fpstream( CLY_OpenModeT omode, const char* name, int prot ) to avoid
the CLY_IOSOut | CLY_IOSBin addition.

 *
 *
 */

#define Uses_fpbase
#define Uses_fpstream
#define Uses_IOS_BIN
#define Uses_filelength
#include <tv.h>

UsingNamespaceStd

fpstream::fpstream()
{
}

fpstream::fpstream( const char* name, CLY_OpenModeT omode, int prot ) :
        fpbase( name, omode | CLY_IOSOut | CLY_IOSBin, prot )
{
}

fpstream::fpstream( CLY_OpenModeT omode, const char* name, int prot ) :
        fpbase( name, omode, prot )
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

CLY_filebuf *fpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void fpstream::open( const char *name, CLY_OpenModeT omode, int prot )
{
    fpbase::open( name, omode | CLY_IOSIn | CLY_IOSOut | CLY_IOSBin, prot );
}


// SET: I added this member because a lot of applications tries to get
// the file descriptor just to find the filelength. This mechanism doesn't
// work with the ISO C++ 1998 standard so here I hide the difference using
// it.
#ifdef CLY_ISOCpp98
long fpstream::filelength()
{
 long basePos=tellp();
 long ret=seekg(0, CLY_IOSEnd).tellg();
 seekg(basePos, CLY_IOSBeg);
 return ret;
}
#else
long fpstream::filelength()
{
 return ::filelength(rdbuf()->fd());
}
#endif

