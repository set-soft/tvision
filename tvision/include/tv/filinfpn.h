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

#if defined( Uses_TFileInfoPane ) && !defined( __TFileInfoPane )
#define __TFileInfoPane

class TRect;
struct TEvent;

class TFileInfoPane : public TView
{

public:

    TFileInfoPane( const TRect& bounds );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );

private:

    TSearchRec file_block;

    static const char * const months[13];
    static const char * pmText;
    static const char * amText;
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TFileInfoPane( StreamableInit ) : TView ( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFileInfoPane& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileInfoPane*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileInfoPane& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileInfoPane* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TFileInfoPane

