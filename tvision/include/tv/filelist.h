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

#if defined( Uses_TFileList ) && !defined( __TFileList )
#define __TFileList

class TRect;
class TScrollBar;
struct TEvent;

class TFileList : public TSortedListBox
{

public:

    TFileList( const TRect& bounds,
	       TScrollBar *aScrollBar
	     );
    ~TFileList();

    virtual void focusItem( ccIndex item );
    virtual void getText( char *dest, ccIndex item, short maxLen );
    virtual void selectItem( ccIndex item );
    virtual void handleEvent( TEvent &event );
    void newList( TFileCollection *aList );
    void readDirectory( const char *dir, const char *wildCard );
    void readDirectory( const char *wildCard );

    virtual uint32 dataSize();
    virtual void getData( void *rec );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );

    TFileCollection *list();

private:

    virtual void *getKey( const char *s );

//    static const char * tooManyFiles;
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TFileList( StreamableInit ) : TSortedListBox ( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFileList& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileList*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileList& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileList* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM


inline void TFileList::newList( TFileCollection *f )
{
    TSortedListBox::newList( f );
}


inline TFileCollection *TFileList::list()
{
    return (TFileCollection *)TSortedListBox::list();
}

#endif  // Uses_TFileList

