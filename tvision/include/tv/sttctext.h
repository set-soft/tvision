/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: added i18n support, added T1StaticText.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TStaticText                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TStaticText ) && !defined( TStaticText_defined )
#define TStaticText_defined

class TRect;

class CLY_EXPORT TStaticText : public TView
{

public:

    TStaticText( const TRect& bounds, const char *aText );
    TStaticText( const TRect& bounds, const char *aText, stTVIntl *aIntlText );
    ~TStaticText();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void getText( char *buf, int maxLen );
    const char *getText();

protected:

    const char *text;
    stTVIntl   *intlText;
    char        noIntl;
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TStaticText( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TStaticText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStaticText*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStaticText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStaticText* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TStaticText

#if defined( Uses_T1StaticText ) && !defined( T1StaticText_defined )
#define T1StaticText_defined
// This is based on TVTools idea, but I think is better to implement it
// in this way and not like a macro.
// I was forced to use growTo for the i18n stuff and the fact that
// base constructors must be called first and that the order in which
// gcc evaluated the arguments is random when optimizing.
class T1StaticText : public TStaticText
{
public:
 T1StaticText(int x, int y, const char *aText) :
   TStaticText(TRect(x,y,x,y),aText)
   { growTo(cstrlen(TVIntl::getText(aText,intlText))+1,1); };
}
#endif // Uses_T1StaticText

