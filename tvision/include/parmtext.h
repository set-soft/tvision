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

/* ---------------------------------------------------------------------- */
/*      class TParamText                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TParamText ) && !defined( __TParamText )
#define __TParamText

class TRect;

class TParamText : public TStaticText
{

public:
    TParamText( const TRect& bounds, const char *aText, int aParamCount );

    virtual uint32 dataSize();
    virtual void getText( char *buf, int maxLen );
    virtual void setData( void *rec );

protected:

    short paramCount;
    void *paramList;
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TParamText( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TParamText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TParamText*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TParamText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TParamText* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TParamText

