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

#if defined( Uses_TIndicator ) && !defined( __TIndicator )
#define __TIndicator

class TRect;
class TPoint;

class TIndicator : public TView
{

public:

    TIndicator( const TRect& );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void setState( ushort, Boolean );
    void setValue( const TPoint&, Boolean );

    static char dragFrame;
    static char normalFrame;
    static char modifiedStar;
    static char odragFrame;
    static char onormalFrame;
    static char omodifiedStar;

protected:

    TPoint location;
    Boolean modified;

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TIndicator( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TIndicator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TIndicator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TIndicator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TIndicator* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TIndicator


