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

#if defined( Uses_TDirCollection ) && !defined( __TDirCollection )
#define __TDirCollection
// Avoid replacing free by MSS's macro
#include <tv/no_mss.h>

class TDirEntry;

class TDirCollection : public TCollection
{

public:

    TDirCollection( ccIndex aLimit, ccIndex aDelta) :
	TCollection( aLimit, aDelta ) {}

    TDirEntry *at( ccIndex index )
	{ return (TDirEntry *)TCollection::at( index );}

    void remove( TDirEntry *item )
	{ TCollection::remove( item ); }
    void free( TDirEntry *item )
	{ TCollection::free( item ); }
    void atPut( ccIndex index, TDirEntry *item )
	{ TCollection::atPut( index, item ); }

    TDirEntry *firstThat( ccTestFunc Test, void *arg );
    TDirEntry *lastThat( ccTestFunc Test, void *arg );

private:

    virtual void freeItem( void *item )
	{ delete (TDirEntry *)item; }
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }
    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

protected:

    TDirCollection( StreamableInit ) : TCollection ( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

#include <tv/yes_mss.h>

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TDirCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDirCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDirCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDirCollection* cl )
    { return os << (TStreamable *)cl; }
#endif

inline TDirEntry *TDirCollection::firstThat( ccTestFunc func, void *arg )
{
    return (TDirEntry *)TCollection::firstThat( ccTestFunc(func), arg );
}

inline TDirEntry *TDirCollection::lastThat( ccTestFunc func, void *arg )
{
    return (TDirEntry *)TCollection::lastThat( ccTestFunc(func), arg );
}

#endif  // Uses_TDirCollection

