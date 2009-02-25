/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to add functionality.

 *
 *
 */

#if defined( Uses_TListBox ) && !defined( __TListBox )
#define __TListBox

class TRect;
class TScrollBar;
class TCollection;

#pragma pack(1)
struct TListBoxRec
{
    TCollection *items   CLY_Packed;
    ccIndex selection    CLY_Packed;
};
#pragma pack()

class TListBox : public TListViewer
{

public:

    TListBox( const TRect& bounds, ushort aNumCols, TScrollBar *aScrollBar );
    TListBox( const TRect& bounds, ushort aNumCols, TScrollBar *aHScrollBar,
              TScrollBar *aVScrollBar, Boolean aCenterOps = False );
    ~TListBox();

    virtual uint32 dataSize();
    virtual void getData( void *rec );
    virtual void getText( char *dest, ccIndex item, short maxLen );
    virtual void newList( TCollection *aList );
    virtual void setData( void *rec );
    // SET: You not always want to destroy the items.
    virtual void newList( TCollection *aList, Boolean destroyItems );
    virtual void setData( void *rec, Boolean destroyItems );

    // SET: Center the list in setData
    Boolean center;

    TCollection *list();

protected:
    TCollection *items;

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TListBox( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TListBox* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

inline TCollection *TListBox::list()
{
    return items;
}

#endif  // Uses_TListBox

