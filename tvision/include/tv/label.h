/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: added i18n support, T1Label inspired in
TVTools but reworked.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TLabel                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TLabel ) && !defined( TLabel_defined )
#define TLabel_defined

class TRect;
struct TEvent;
class TView;

class CLY_EXPORT TLabel : public TStaticText
{

public:

    TLabel( const TRect& bounds, const char *aText, TView *aLink );
    TLabel( const TRect& bounds, const char *aText, TView *aLink, stTVIntl *aIntlText );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void shutDown();

    TView *link;

    // SET: Now labels propagate the disabled state.
    virtual void setState( ushort aState, Boolean enable );

protected:

    Boolean light;
    void init( TView *aLink );

#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TLabel( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TLabel& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TLabel*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TLabel& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TLabel* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TLabel

#if defined( Uses_T1Label ) && !defined( T1Label_defined )
#define T1Label_defined
class T1Label : public TLabel
{
public:
 T1Label(int x, int y, const char *aText, TView *aLink) :
   TLabel(TRect(x,y,x,y),aText,aLink)
   { growTo(cstrlen(TVIntl::getText(aText,intlText))+1,1); };
};
#endif // Uses_T1Label

