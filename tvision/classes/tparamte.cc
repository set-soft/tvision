/*------------------------------------------------------------*/
/* filename -       tparamte.cc                               */
/*                                                            */
/* function(s)                                                */
/*                  TParamText member functions               */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Salvador E. Tropea (SET) for RHTVision port.
 *
 * Big Warning!!!!! This code uses a 256 bytes buffer and doesn't check
 * for overflow.
 * I added:
 * 1) The limit is now a constant defined in tparamTextMaxLen, so you can
 * adjust it.
 * 2) If SAFE_CODE is defined (default) the setText() member checks the
 * lenght of the string. I use a slow trick, but is better than corrupting
 * memory.
 *
 * Portability note:
 * getText( char *s, int maxLen ) and BTV uses: getText( char *s )
 *   That's because Robert modified TStaticText, I think that's good, asking
 * to write in a string without knowing the length is a very bad idea.
 *
 */
#include <stdio.h>
#include <stdarg.h>
#define Uses_string

#define Uses_TParamText
#include <tv.h>


TParamText::TParamText( const TRect& bounds ) :
    TStaticText(bounds, 0 ),
    str( new char [tparamTextMaxLen] )
{
    str[0] = EOS;
}

TParamText::~TParamText()
{
    delete str;
}

// , int maxLen ) isn't part of TV 2.0
void TParamText::getText( char *s, int maxLen )
{
    if( str == 0 )
        *s = EOS;
    else
    {
        strncpy( s, str, maxLen );
        s[maxLen] = 0;
    }
}

int TParamText::getTextLen()
{
    return (str != 0) ? strlen( str ) : 0;
}

void TParamText::setText( char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    #ifdef SAFE_CODE
    // Slow but we can check the overflow
    FILE *f=fopen("/dev/null","wb");
    int len=vfprintf(f,fmt,ap);
    fclose(f);
    if (len>=tparamTextMaxLen)
      {
       *str=EOS;
       return;
      }
    #endif
    vsprintf( str, fmt, ap );
    va_end( ap );

    drawView();
}

#if !defined(NO_STREAMABLE)

TStreamable *TParamText::build()
{
    return new TParamText( streamableInit );
}

TParamText::TParamText( StreamableInit ) : TStaticText( streamableInit )
{
}

#endif
