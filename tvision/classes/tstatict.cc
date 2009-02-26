/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea: added i18n support.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <ctype.h>
#define Uses_string
#define Uses_AllocLocal
#define Uses_TStaticText
#define Uses_TDrawBuffer
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#include <tv.h>

#define cpStaticText "\x06"

TStaticText::TStaticText( const TRect& bounds, const char *aText ) :
    TView( bounds ),
    text( newStr( aText ) ),
    intlText( NULL ),
    noIntl( 0 )
{
}

TStaticText::TStaticText( const TRect& bounds, const char *aText,
                          stTVIntl *aIntlText ) :
    TView( bounds ),
    text( newStr( aText ) ),
    intlText( aIntlText ),
    noIntl( 0 )
{
}

TStaticText::~TStaticText()
{
    DeleteArray((char *)text);
    TVIntl::freeSt( intlText );
}

void TStaticText::draw()
{
    uchar color;
    Boolean center;
    int i, j, l, p, y;
    TDrawBuffer b;
    int maxLen = size.x*size.y;
    // +2 to hold the \x3 center char (SET)
    AllocLocalStr(s,maxLen+2);

    color = getColor(1);
    getText(s, maxLen+1);
    l = strlen(s);
    // Check if the size if bigger than allowed and the extra char isn't the
    // center char (SET)
    if ((l > maxLen) && s[0]!=3)
        {
        l--;
        s[maxLen+1] = 0;
        }
    p = 0;
    y = 0;
    center = False;
    while (y < size.y)
        {
        b.moveChar(0, ' ', color, size.x);
        if (p < l)
            {
            if (s[p] == 3)
                {
                center = True;
                ++p;
                }
            i = p;
            do {
               j = p;
               while ((p < l) && (s[p] == ' ')) 
                   ++p;
               while ((p < l) && (s[p] != ' ') && (s[p] != '\n'))
                   ++p;
               } while ((p < l) && (p < i + size.x) && (s[p] != '\n'));
            if (p > i + size.x)
                {
                if (j > i)
                    p = j;
                else
                    p = i + size.x;
                }
            if (center == True)
               j = (size.x - p + i) / 2 ;
            else 
               j = 0;
            b.moveBuf(j, &s[i], color, (p - i));
            while ((p < l) && (s[p] == ' '))
                p++;
            if ((p < l) && (s[p] == '\n'))
                {
                center = False;
                p++;
                if ((p < l) && (s[p] == 10))
                    p++;
                }
            }
        writeLine(0, y++, size.x, 1, b);
        }
}

TPalette& TStaticText::getPalette() const
{
    static TPalette palette( cpStaticText, sizeof( cpStaticText )-1 );
    return palette;
}

const char *TStaticText::getText()
{
     return noIntl ? text : TVIntl::getText( text, intlText );
}

void TStaticText::getText( char *s, int maxLen )
{
    if( text == 0 )
        *s = EOS;
    else
    {
        const char *iText = getText();
        strncpy( s, iText, maxLen );
        s[maxLen] = 0;
    }
}

#if !defined( NO_STREAM )
void TStaticText::write( opstream& os )
{
    TView::write( os );
    os.writeString( text );
}

void *TStaticText::read( ipstream& is )
{
    TView::read( is );
    text = is.readString();
    intlText = NULL;
    return this;
}

TStreamable *TStaticText::build()
{
    return new TStaticText( streamableInit );
}

TStaticText::TStaticText( StreamableInit ) : TView( streamableInit )
{
}
#endif // NO_STREAM


