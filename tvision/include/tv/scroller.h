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

/* ---------------------------------------------------------------------- */
/*      class TScroller                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*      1 = Normal text                                                   */
/*      2 = Selected text                                                 */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TScroller ) && !defined( __TScroller )
#define __TScroller

class TRect;
class TScrollBar;
struct TEvent;

class CLY_EXPORT TScroller : public TView
{

public:

    TScroller( const TRect& bounds,
               TScrollBar *aHScrollBar,
               TScrollBar *aVScrollBar
             );

    virtual void changeBounds( const TRect& bounds );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void scrollDraw();
    void scrollTo( int32 x, int32 y );
    void setLimit( int32 x, int32 y );
    virtual void setState( ushort aState, Boolean enable );
    void checkDraw();
    virtual void shutDown();

    // How many lines we will scroll when the mouse wheel is used.
    static int defaultWheelStep;
    int wheelStep;

protected:

    uchar drawLock;
    Boolean drawFlag;
    TScrollBar *hScrollBar;
    TScrollBar *vScrollBar;
    TPoint delta;
    TPoint limit;

private:

    void showSBar( TScrollBar *sBar );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TScroller( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TScroller& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TScroller*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TScroller& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TScroller* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TScroller

