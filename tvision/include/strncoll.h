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

#if defined( Uses_TStringCollection ) && !defined( __TStringCollection )
#define __TStringCollection

class TStringCollection : public TSortedCollection
{

public:

    TStringCollection( ccIndex aLimit, ccIndex aDelta );

private:

    virtual int compare( void *key1, void *key2 );
    virtual void freeItem( void *item );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }
    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

protected:

    TStringCollection( StreamableInit ) : TSortedCollection ( streamableInit ) {};

public:

    static const char * const name;
    static TStreamable *build();

    TStringCollection & operator = (const TStringCollection &);
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TStringCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStringCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStringCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStringCollection* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TStringCollection

