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

#if defined( Uses_TResourceCollection ) && !defined( __TResourceCollection )
#define __TResourceCollection

class TResourceCollection: public TStringCollection
{

public:

    TResourceCollection( ccIndex aLimit, ccIndex aDelta );

    virtual void *keyOf( void *item );

private:

    virtual void freeItem( void *item );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }
    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

public:

    static const char * const name;
    static TStreamable *build();
    TResourceCollection( StreamableInit) : TStringCollection( streamableInit ) {};
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TResourceCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TResourceCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TResourceCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TResourceCollection* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TResourceCollection

