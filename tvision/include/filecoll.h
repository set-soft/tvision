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

#if defined( Uses_TFileCollection ) && !defined( __TFileCollection )
#define __TFileCollection

class TSearchRec;

class TFileCollection: public TSortedCollection
{

public:

    TFileCollection( ccIndex aLimit, ccIndex aDelta) :
	TSortedCollection( aLimit, aDelta ) {}

    TSearchRec *at( ccIndex index )
	{ return (TSearchRec *)TSortedCollection::at( index ); }

    void remove( TSearchRec *item )
	{ TSortedCollection::remove( item ); }
    void free( TSearchRec *item )
	{ TSortedCollection::free( item ); }
    void atPut( ccIndex index, TSearchRec *item )
	{ TSortedCollection::atPut( index, item ); }

    TSearchRec *firstThat( ccTestFunc Test, void *arg );
    TSearchRec *lastThat( ccTestFunc Test, void *arg );

private:

    virtual void freeItem( void *item )
	{ delete (TSearchRec *)item; }

    virtual int compare( void *key1, void *key2 );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

protected:

    TFileCollection( StreamableInit ) : TSortedCollection ( streamableInit ) {}

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFileCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileCollection* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

inline TSearchRec *TFileCollection::firstThat( ccTestFunc func, void *arg )
{
    return (TSearchRec *)TSortedCollection::firstThat( ccTestFunc(func), arg );
}

inline TSearchRec *TFileCollection::lastThat( ccTestFunc func, void *arg )
{
    return (TSearchRec *)TSortedCollection::lastThat( ccTestFunc(func), arg );
}

#endif  // Uses_TFileCollection

