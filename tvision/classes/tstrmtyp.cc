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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string

#define Uses_TStreamableClass
#define Uses_TStreamableTypes
#include <tv.h>

TStreamableTypes::TStreamableTypes() : TNSSortedCollection( 5, 5 )
{
}

// Avoid replacing new by MSS's macro
#include <tv/no_mss.h>

void *TStreamableTypes::operator new( size_t, void * arena )
{
    return arena;
}

#include <tv/yes_mss.h>

TStreamableTypes::~TStreamableTypes()
{
}

void TStreamableTypes::registerType( const TStreamableClass *d )
{
    insert( (void *)d );
}

const TStreamableClass *TStreamableTypes::lookup( const char *name )
{
    ccIndex loc;
    if( search( (void *)name, loc ) )
        return (TStreamableClass *)at( loc );
    else
        return 0;
}

void *TStreamableTypes::keyOf( void *d )
{
    return (void *)((TStreamableClass *)d)->name;
}

int TStreamableTypes::compare( void *d1, void *d2 )
{
    return strcmp( (char *)d1, (char *)d2 );
}

