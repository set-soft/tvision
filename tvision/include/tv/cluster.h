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
/*      class TCluster                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TCluster ) && !defined( __TCluster )
#define __TCluster

class TRect;
class TSItem;
struct TEvent;
class TPoint;
class TStringCollection;
class TStringCollectionCIntl;

class CLY_EXPORT TCluster : public TView
{

public:

    TCluster( const TRect& bounds, TSItem *aStrings );
    ~TCluster();

    virtual uint32 dataSize();
    void drawBox( const char *icon, char marker );
    virtual void getData( void *rec );
    ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual Boolean mark( int item );
    virtual void press( int item );
    virtual void movedTo( int item );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );
    // SET: see tcluster.cc for more information
    unsigned getExtraOptions() { return extraOptions; }
    void setExtraOptions(unsigned ops) { extraOptions=ops; }
    // SET: for i18n support
    const char *getItemText( ccIndex item );

protected:
    static unsigned extraOptions;

    uint32 value;
    int sel;
    // SET: i18n cache
    TStringCollectionCIntl *intlStrings;
    TStringCollection *strings;

private:

    int column( int item );
    int findSel( TPoint p );
    int row( int item );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TCluster( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TCluster& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCluster*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCluster& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCluster* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TCluster

