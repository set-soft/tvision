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

#define Uses_pstream
#define Uses_fpbase
#include <tv.h>

fpbase::fpbase()
{
    pstream::init( &buf );
}

fpbase::fpbase( const char *name, int omode, int prot )
{
    pstream::init( &buf );
    open( name, omode, prot );
}

fpbase::fpbase( int f ) : buf( f )
{
    pstream::init( &buf );
}

fpbase::fpbase( int f, char *b, int len ) : buf( f, b, len )
{
    pstream::init( &buf );
}

fpbase::~fpbase()
{
}

void fpbase::open( const char *b, int m, int prot )
{
    if( buf.is_open() )
        clear(ios::failbit);        // fail - already open
    else if( buf.open(b, m, prot) )
        clear(ios::goodbit);        // successful open
    else
        clear(ios::badbit);     // open failed
}

void fpbase::attach( int f )
{
    if( buf.is_open() )
        setstate(ios::failbit);
    else if( buf.attach(f) )
        clear(ios::goodbit);
    else
        clear(ios::badbit);
}

void fpbase::close()
{
    if( buf.close() )
        clear(ios::goodbit);
    else
        setstate(ios::failbit);
}

void fpbase::setbuf(char* b, int len)
{
    if( buf.setbuf(b, len) )
        clear(ios::goodbit);
    else
        setstate(ios::failbit);
}

filebuf *fpbase::rdbuf()
{
    return &buf;
}

