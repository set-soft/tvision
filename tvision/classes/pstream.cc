/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdlib.h>

#define Uses_TStreamableTypes
#define Uses_TStreamableClass
#define Uses_TStreamable
#define Uses_pstream
#include <tv.h>

UsingNamespaceStd

pstream::pstream( CLY_streambuf *sb )
{
    init( sb );
}

pstream::~pstream()
{
}

void pstream::deInitTypes(void)
{
 if (types)
    delete types;
}

// The following call to new is overloaded, avoid
// using MSS for it.
#include <tv/no_mss.h>

void pstream::initTypes()
{
 if (!types)
   {
    types=new TStreamableTypes;
    atexit(pstream::deInitTypes);
   }
}

#include <tv/yes_mss.h>

int pstream::rdstate() const
{
    return state;
}

int pstream::eof() const
{
    return state & CLY_IOSEOFBit;
}

int pstream::fail() const
{
    return state & (CLY_IOSFailBit | CLY_IOSBadBit);
}

int pstream::bad() const
{
    return state & (CLY_IOSBadBit);
}

int pstream::good() const
{
    return state == 0;
}

void pstream::clear( int i )
{
    state = (i & 0xFF);
}

void pstream::registerType( TStreamableClass *ts )
{ 
    types->registerType( ts ); 
}

pstream::operator void *() const
{
    return fail() ? 0 : (void *)this;
}

int pstream::operator! () const
{
    return fail();
}

CLY_streambuf * pstream::rdbuf() const
{
    return bp;
}

pstream::pstream()
{
}

void pstream::error( StreamableError )
{
    abort();
}

void pstream::error( StreamableError, const TStreamable& )
{
    abort();
}

void pstream::init( CLY_streambuf *sbp )
{
    state = 0;
    bp = sbp;
}

void pstream::setstate( int b )
{
    state |= (b&0xFF);
}

TStreamableTypes * pstream::types;
