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

#if defined( Uses_TGroup ) && !defined( __TGroup )
#define __TGroup

class CLY_EXPORT TGroup : public TView
{

public:

    friend void genRefs();

    TGroup( const TRect& bounds );
    ~TGroup();

    virtual void shutDown();

    ushort execView( TView *p );
    virtual ushort execute();

    void insertView( TView *p, TView *Target );
    void remove( TView *p );
    void removeView( TView *p );
    void resetCurrent();
    void setCurrent( TView *p, selectMode mode );
    void selectNext( Boolean forwards );
    TView *firstThat( Boolean (*func)( TView *, void * ), void *args );
    void forEach( void (*func)( TView *, void * ), void *args );
    void insert( TView *p );
    void insertBefore( TView *p, TView *Target );
    TView *current;
    TView *at( short index );
    TView *firstMatch( ushort aState, ushort aOptions );
    short indexOf( TView *p );
    Boolean matches( TView *p );
    TView *first();

    virtual void setState( ushort aState, Boolean enable );

    virtual void handleEvent( TEvent& event );

    void drawSubViews( TView *p, TView *bottom );

    virtual void changeBounds( const TRect& bounds );

    virtual uint32 dataSize();
    virtual void getData( void *rec );
    virtual void setData( void *rec );

    virtual void draw();
    void redraw();
    void CLY_Redraw();
    void lock();
    void unlock();
    virtual void resetCursor();
    // SET: A view should ask your owner (a group) if that's right
    // time to show the cursor or we are locked.
    virtual Boolean canShowCursor();

    virtual void endModal( ushort command );

    virtual void eventError( TEvent& event );

    virtual ushort getHelpCtx();

    virtual Boolean valid( ushort command );

    void freeBuffer();
    void getBuffer();

    TView *last;

    TRect clip;
    phaseType phase;

    ushort *buffer;
    uchar lockFlag;
    ushort endState;

private:

    Boolean invalid( TView *p, ushort command );
    void focusView( TView *p, Boolean enable );
    void selectView( TView *p, Boolean enable );
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:

    TGroup( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TGroup& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TGroup*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TGroup& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TGroup* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TGroup

