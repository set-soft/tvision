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

#if defined( Uses_TColorGroupList ) && !defined( __TColorGroupList )
#define __TColorGroupList

class TRect;
class TScrollBar;
class TColorGroup;
class TColorItem;

class TColorGroupList : public TListViewer
{

public:

    TColorGroupList( const TRect& bounds,
                     TScrollBar *aScrollBar,
                     TColorGroup *aGroups
                   );
    virtual ~TColorGroupList();
    virtual void focusItem( ccIndex item );
    virtual void getText( char *dest, ccIndex item, short maxLen );

protected:

    TColorGroup *groups;
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }
    static void writeItems( opstream&, TColorItem * );
    static void writeGroups( opstream&, TColorGroup * );
    static TColorItem *readItems( ipstream& );
    static TColorGroup *readGroups( ipstream& );

protected:

    TColorGroupList( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TColorGroupList& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TColorGroupList*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TColorGroupList& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TColorGroupList* cl )
    { return os << (TStreamable *)cl; }
#endif

#endif  // Uses_TColorGroupList

