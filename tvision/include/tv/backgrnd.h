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

#if defined( Uses_TBackground ) && !defined( __TBackground )
#define __TBackground
    
class TRect;

class TBackground : public TView
{

public:

    TBackground( const TRect& bounds, char aPattern );
    virtual void draw();
    virtual TPalette& getPalette() const;
    void changePattern(char newP) { pattern=newP; draw(); }
    char getPattern(void) { return pattern; }

protected:
    char pattern;

#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TBackground( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TBackground& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TBackground*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TBackground& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TBackground* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TBackground

