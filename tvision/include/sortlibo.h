/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H�hne to be used for RHIDE.

 *
 *
 */

#if defined( Uses_TSortedListBox ) && !defined( __TSortedListBox )
#define __TSortedListBox

class TRect;
class TScrollBar;
class TEvent;
class TSortedCollection;

class TSortedListBox: public TListBox
{

public:

    TSortedListBox( const TRect& bounds,
		    ushort aNumCols,
		    TScrollBar *aScrollBar
		  );

    virtual void handleEvent( TEvent& event );
    void newList( TSortedCollection *aList );

    TSortedCollection *list();

protected:

    uchar shiftState;

private:

    virtual void *getKey( const char *s );

    ushort searchPos;
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TSortedListBox( StreamableInit ) : TListBox ( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

inline ipstream& operator >> ( ipstream& is, TSortedListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TSortedListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TSortedListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TSortedListBox* cl )
    { return os << (TStreamable *)cl; }

inline TSortedCollection *TSortedListBox::list()
{
    return (TSortedCollection *)TListBox::list();
}

#endif  // Uses_TSortedListBox

