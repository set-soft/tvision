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

#if defined( Uses_TStrListMaker ) && !defined( __TStrListMaker )
#define __TStrListMaker

class TStrListMaker : public TObject
#if !defined( NO_STREAM )
                                     , public TStreamable
#endif // NO_STREAM
{

public:

    TStrListMaker( ushort aStrSize, ushort aIndexSize );
    ~TStrListMaker();

    void put( ushort key, char *str );

private:

    ushort strPos;
    ushort strSize;
    char *strings;
    ushort indexPos;
    ushort indexSize;
    TStrIndexRec *index;
    TStrIndexRec cur;
    void closeCurrent();
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return TStringList::name; }

protected:

    TStrListMaker( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& ) { return 0; }

public:

    static TStreamable *build();
#endif // NO_STREAM
};                    

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TStrListMaker& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStrListMaker*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStrListMaker& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStrListMaker* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM


#endif  // Uses_TStrListMaker

