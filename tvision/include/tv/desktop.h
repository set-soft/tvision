/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea, tile, options, cursor behavior, etc.

 *
 *
 */

#if defined( Uses_TDeskTop )  && !defined( __TDeskTop )
#define __TDeskTop

// SET: To set the tile partition priority
const unsigned dsktTileVertical=1, dsktTileHorizontal=0;

class TBackground;
class TRect;
struct TEvent;

class TDeskInit
{
public:
    TDeskInit( TBackground *(*cBackground)( TRect ) );

protected:
    TBackground *(*createBackground)( TRect );
};

class CLY_EXPORT TDeskTop : public TGroup, public virtual TDeskInit
{

public:

    TDeskTop( const TRect& );

    void cascade( const TRect& );
    virtual void handleEvent( TEvent& );
    static TBackground *initBackground( TRect );
    void tile( const TRect& );
    virtual void tileError();
    virtual void shutDown();
    TBackground *getBackground(void) { return background; }
    // SET: Added to setup the tileable options
    unsigned getOptions() { return flagsOptions; }
    void setOptions(unsigned aFlags) { flagsOptions=aFlags; }
    virtual Boolean canShowCursor();
    // SET: Added to help Braille Terminals
    virtual ushort execView( TView *p );

    // SET: Made non-constant and added original
    static char defaultBkgrnd;
    static char odefaultBkgrnd;

protected:

    TBackground *background;
    unsigned flagsOptions;

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TDeskTop( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TDeskTop& cl )
    { return is >> (TStreamable&)(TGroup&)cl; }
inline ipstream& operator >> ( ipstream& is, TDeskTop*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDeskTop& cl )
    { return os << (TStreamable&)(TGroup&)cl; }
inline opstream& operator << ( opstream& os, TDeskTop* cl )
    { return os << (TStreamable *)(TGroup *)cl; }
#endif // NO_STREAM

#endif

