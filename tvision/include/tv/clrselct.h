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

#if defined( Uses_TColorSelector ) && !defined( __TColorSelector )
#define __TColorSelector

class TRect;
struct TEvent;

class TColorSelector : public TView
{

public:

    enum ColorSel { csBackground, csForeground };

    TColorSelector( const TRect& Bounds, ColorSel ASelType );
    virtual void draw();
    virtual void handleEvent( TEvent& event );

    static char icon;
    static char oicon;
    static char mark;
    static char omark;

protected:

    uchar color;
    ColorSel selType;

private:

    void colorChanged();

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TColorSelector( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TColorSelector& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TColorSelector*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TColorSelector& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TColorSelector* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TColorSelector

