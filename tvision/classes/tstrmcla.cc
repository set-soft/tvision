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

#define Uses_TStreamableClass
#define Uses_pstream
#include <tv.h>

TStreamableClass::TStreamableClass( const char *n, BUILDER b, int d ) :
    name( n ),
    build( b ),
    delta( d )
{
    pstream::initTypes();
    pstream::registerType( this );
}

