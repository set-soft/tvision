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

#if defined( Uses_THistory ) && !defined( __THistory )
#define __THistory

class TRect;
class TInputLine;
struct TEvent;
class THistoryWindow;

class THistory : public TView
{

public:

    THistory( const TRect& bounds, TInputLine *aLink, ushort aHistoryId );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual THistoryWindow *initHistoryWindow( const TRect& bounds );
    virtual void shutDown();

    static char icon[];
    static char oicon[];

protected:

    TInputLine *link;
    ushort historyId;

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    THistory( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, THistory& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, THistory*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, THistory& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, THistory* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_THistory

