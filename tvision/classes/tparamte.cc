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
#include <stdio.h>

#define Uses_TParamText
#include <tv.h>

TParamText::TParamText( const TRect& bounds,
                        const char *aText,
                        int aParamCount ) :
    TStaticText(bounds, aText),
    paramCount( aParamCount ),
    paramList( 0 )
{
}

uint32 TParamText::dataSize()
{
    return paramCount * sizeof(long);
}

void TParamText::getText( char *s, int /* maxLen */)
{
    if( text == 0 )
        *s = EOS;
    else
        vsprintf( s, text, paramList );
}

void TParamText::setData( void *rec )
{
    paramList = &rec;
}

#if !defined( NO_STREAM )
void TParamText::write( opstream& os )
{
    TStaticText::write( os );
    os << paramCount;
}

void *TParamText::read( ipstream& is )
{
    TStaticText::read( is );
    is >> paramCount;
    paramList = 0;
    return this;
}

TStreamable *TParamText::build()
{
    return new TParamText( streamableInit );
}

TParamText::TParamText( StreamableInit ) : TStaticText( streamableInit )
{
}
#endif // NO_STREAM


