/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added TRadioButtons32 and code page stuff by Salvador Eduardo Tropea.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TRadioButtons                                               */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */


#if defined( Uses_TRadioButtons ) && !defined( __TRadioButtons )
#define __TRadioButtons

class TRect;
class TSItem;

class TRadioButtons : public TCluster
{

public:

    TRadioButtons( const TRect& bounds, TSItem *aStrings ) :
      TCluster( bounds, aStrings ) { enableMasks=NULL; }

    virtual void draw();
    virtual Boolean mark( int item );
    virtual void movedTo( int item );
    virtual void press( int item );
    virtual void setData( void *rec );

    // SET: Used to enable/disable other TViews according to current state.
    // Upto 32 can be controled.
    void setEnableMask( uint32 *masks, TView **views, int cViews )
    {
     enableMasks = masks;
     enableViews = views;
     enableCViews = cViews;
    }

    static char button[];
    static char obutton[];
    static char check;
    static char ocheck;

protected:

    uint32 *enableMasks;
    TView  **enableViews;
    int enableCViews;
    void evaluateMasks();

private:

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TRadioButtons( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

class TRadioButtons32 : public TRadioButtons
{
public:
 TRadioButtons32(const TRect& bounds,TSItem *aStrings) :
   TRadioButtons(bounds,aStrings) {};
 virtual uint32 dataSize();
private:
#if !defined( NO_STREAM )
 virtual const char *streamableName() const { return name; }
protected:
 TRadioButtons32(StreamableInit);
public:
 static const char * const name;
 static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TRadioButtons& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TRadioButtons*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TRadioButtons& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TRadioButtons* cl )
    { return os << (TStreamable *)cl; }

inline ipstream& operator >> ( ipstream& is, TRadioButtons32& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TRadioButtons32*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TRadioButtons32& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TRadioButtons32* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TRadioButtons

