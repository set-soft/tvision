/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified for i18n support by Salvador Eduardo Tropea.

 *
 *
 */

#if defined( Uses_TWindow ) && !defined( __TWindow )
#define __TWindow

class TFrame;
class TRect;
class TPoint;
struct TEvent;
class TFrame;
class TScrollBar;

class CLY_EXPORT TWindowInit
{

public:

    TWindowInit( TFrame *(*cFrame)( TRect ) );

protected:

    TFrame *(*createFrame)( TRect );

};

/* ---------------------------------------------------------------------- */
/*      class TWindow                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = Scroller normal text                                        */
/*        7 = Scroller selected text                                      */
/*        8 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

class CLY_EXPORT TWindow: public TGroup, public virtual TWindowInit
{

public:

    TWindow( const TRect& bounds,
	     const char *aTitle,
	     short aNumber
	   );
    ~TWindow();

    virtual void close();
    virtual TPalette& getPalette() const;
    virtual const char *getTitle( short maxSize );
    virtual void handleEvent( TEvent& event );
    static TFrame *initFrame( TRect );
    virtual void setState( ushort aState, Boolean enable );
    virtual void sizeLimits( TPoint& min, TPoint& max );
    TScrollBar *standardScrollBar( ushort aOptions );
    virtual void zoom();
    virtual void shutDown();

    uchar flags;
    TRect zoomRect;
    short number;
    short palette;
    TFrame *frame;
    const char *title;
    stTVIntl   *intlTitle;
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
	{ return name; }

protected:

    TWindow( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TWindow& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TWindow*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TWindow& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TWindow* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TWindow

