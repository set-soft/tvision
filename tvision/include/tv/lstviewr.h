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

#if defined( Uses_TListViewer ) && !defined( __TListViewer )
#define __TListViewer

class TRect;
class TScrollBar;
struct TEvent;

class TListViewer : public TView
{

public:

    TListViewer( const TRect& bounds,
		 ushort aNumCols,
		 TScrollBar *aHScrollBar,
		 TScrollBar *aVScrollBar
	       );

    virtual void changeBounds( const TRect& bounds );
    virtual void draw();
    virtual void focusItem( ccIndex item );
    virtual void focusItemCentered( ccIndex item );
    virtual TPalette& getPalette() const;
    virtual void getText( char *dest, ccIndex item, short maxLen );
    virtual Boolean isSelected( ccIndex item );
    virtual void handleEvent( TEvent& event );
    virtual void selectItem( ccIndex item );
    void setRange( ccIndex aRange );
    virtual void setState( ushort aState, Boolean enable );

    virtual void focusItemNum( ccIndex item );
    virtual void shutDown();

    TScrollBar *hScrollBar;
    TScrollBar *vScrollBar;
    short numCols;
    ccIndex topItem;
    ccIndex focused;
    ccIndex range;
    Boolean handleSpace;

    static uchar columnSeparator;
    static uchar ocolumnSeparator;

    // SET: see tlistvie.cc for more information
    unsigned getExtraOptions() { return extraOptions; }
    void setExtraOptions(unsigned ops) { extraOptions=ops; }

protected:
    // SET: extra options ored by default to all objects of this class.
    static unsigned extraOptions;
    // SET: neede to change the scroll bars too.
    void setNumCols(int aNumCols);
    // SET: Used to center the focused item.
    Boolean center;

#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
	{ return name; }

protected:

    TListViewer( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TListViewer& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TListViewer*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TListViewer& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TListViewer* cl )
   { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TListViewer

