/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added TCheckBoxes32 and code page stuff by Salvador Eduardo Tropea.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      TCheckBoxes                                                       */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TCheckBoxes ) && !defined( __TCheckBoxes )
#define __TCheckBoxes

class TRect;
class TSItem;

class CLY_EXPORT TCheckBoxes : public TCluster
{

public:

    TCheckBoxes( const TRect& bounds, TSItem *aStrings) :
      TCluster( bounds, aStrings ) {};

    virtual void draw();
    
    virtual Boolean mark( int item );
    virtual void press( int item );

    static char button[];
    static char obutton[];

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TCheckBoxes( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

// SET: That's a 32 bits version, the only difference is that it uses a
// 32 bits value for set/getData

class TCheckBoxes32 : public TCheckBoxes
{
public:
 TCheckBoxes32(const TRect& bounds,TSItem *aStrings) :
   TCheckBoxes(bounds,aStrings) {};
 virtual uint32 dataSize();
private:
#if !defined( NO_STREAM )
 virtual const char *streamableName() const { return name; }
protected:
 TCheckBoxes32(StreamableInit);
public:
 static const char * const name;
 static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TCheckBoxes& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCheckBoxes*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCheckBoxes& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCheckBoxes* cl )
    { return os << (TStreamable *)cl; }

inline ipstream& operator >> ( ipstream& is, TCheckBoxes32& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCheckBoxes32*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCheckBoxes32& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCheckBoxes32* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TCheckBoxes

