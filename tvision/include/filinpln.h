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

#if defined( Uses_TFileInputLine ) && !defined( __TFileInputLine )
#define __TFileInputLine

class TRect;
struct TEvent;

class TFileInputLine : public TInputLine
{

public:

    TFileInputLine( const TRect& bounds, short aMaxLen );

    virtual void handleEvent( TEvent& event );
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
	{ return name; }

protected:

    TFileInputLine( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFileInputLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileInputLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileInputLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileInputLine* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TFileInputLine

