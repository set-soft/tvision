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

#if defined( Uses_TMonoSelector ) && !defined( __TMonoSelector )
#define __TMonoSelector

class TRect;
struct TEvent;

class TMonoSelector : public TCluster
{

public:

    TMonoSelector( const TRect& bounds );
    virtual void draw();
    virtual void handleEvent( TEvent& event );
    virtual Boolean mark( int item );
    void newColor();
    virtual void press( int item );
    void movedTo( int item );

    static char button[];
    static char obutton[];

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TMonoSelector( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TMonoSelector& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMonoSelector*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMonoSelector& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMonoSelector* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TMonoSelector

