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

#if defined( Uses_TSortedCollection ) && !defined( __TSortedCollection )
#define __TSortedCollection

class TSortedCollection : public TNSSortedCollection, public TCollection
{

public:

    TSortedCollection( ccIndex aLimit, ccIndex aDelta) :
        TCollection( aLimit, aDelta ) {}

private:

    virtual int compare( void *key1, void *key2 ) = 0;
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }
    virtual void *readItem( ipstream& ) = 0;
    virtual void writeItem( void *, opstream& ) = 0;

protected:

    TSortedCollection( StreamableInit );
    virtual void *read( ipstream& );
    virtual void write( opstream& );

public:

    static const char * const name;
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TSortedCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TSortedCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TSortedCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TSortedCollection* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TSortedCollection

