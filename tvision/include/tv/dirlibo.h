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

#if defined( Uses_TDirListBox ) && !defined( __TDirListBox )
#define __TDirListBox

class TRect;
class TScrollBar;
struct TEvent;
class TDirCollection;

#include <limits.h>

class TDirListBox : public TListBox
{

public:

    TDirListBox( const TRect& bounds, TScrollBar *aVScrollBar,
                 TScrollBar *aHScrollBar = NULL );
    ~TDirListBox();

    virtual void getText( char *, ccIndex, short );
    virtual void handleEvent( TEvent& );
    virtual Boolean isSelected( ccIndex );
    void newDirectory( const char * );
    virtual void setState( ushort aState, Boolean enable );
    virtual void draw();
    // SET: Now we use the cursor to help in incremental searches
    void updateCursorPos();

    TDirCollection *list();

    static char pathDir[];
    static char firstDir[];
    static char middleDir[];
    static char lastDir[];
    static char graphics[];
    static char opathDir[];
    static char ofirstDir[];
    static char omiddleDir[];
    static char olastDir[];
    static char ographics[];

private:

    void showDrives( TDirCollection * );
    void showDirs( TDirCollection * );

    char dir[PATH_MAX];
    ushort cur;

    // SET: Incremental search
    char incremental[PATH_MAX];
    int  incPos;

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TDirListBox( StreamableInit ): TListBox( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TDirListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDirListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDirListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDirListBox* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

inline TDirCollection *TDirListBox::list()
{
    return (TDirCollection *)TListBox::list();
}

#endif  // Uses_TDirListBox

