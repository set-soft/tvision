/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: added horizontal scrollbar.

 *
 *
 */

#if defined( Uses_TColorItemList ) && !defined( __TColorItemList )
#define __TColorItemList

class TRect;
class TScrollBar;
class TColorItem;
struct TEvent;

class TColorItemList : public TListViewer
{

public:

    TColorItemList( const TRect& bounds,
                    TScrollBar *aScrollBar,
                    TColorItem *aItems,
                    TScrollBar *aHScrollBar=0
                  );
    virtual void focusItem( ccIndex item );
    virtual void getText( char *dest, ccIndex item, short maxLen );
    virtual void handleEvent( TEvent& event );

protected:

    TColorItem *items;
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TColorItemList( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TColorItemList& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TColorItemList*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TColorItemList& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TColorItemList* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TColorItemList

