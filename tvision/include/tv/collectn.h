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

#if defined( Uses_TCollection ) && !defined( __TCollection )
#define __TCollection

class CLY_EXPORT TCollection : public virtual TNSCollection
#if !defined( NO_STREAM )
                                                 , public TStreamable
#endif // NO_STREAM
{

public:

    TCollection( ccIndex aLimit, ccIndex aDelta )
      { delta = aDelta; setLimit( aLimit ); }
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

    virtual void *readItem( ipstream& ) = 0;
    virtual void writeItem( void *, opstream& ) = 0;


protected:

    TCollection( StreamableInit );
    virtual void *read( ipstream& );
    virtual void write( opstream& );

public:

    static const char * const name;
#endif // NO_STREAM
};

inline ipstream& operator >> ( ipstream& is, TCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCollection* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TCollection

