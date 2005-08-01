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
/*      class TStatusLine                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TStatusLine ) && !defined( __TStatusLine )
#define __TStatusLine

class TRect;
struct TEvent;
class TPoint;

class CLY_EXPORT TStatusLine : public TView
{

public:

    TStatusLine( const TRect& bounds, TStatusDef& aDefs );
    ~TStatusLine();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual const char* hint( ushort aHelpCtx );
    void update();
    void computeLength(); // SET: see compactStatus
    virtual void changeBounds(const TRect& bounds);

    static char hintSeparator[];
    static char ohintSeparator[];

    // SET: Look the comments in TMenuView, same purpose
    int compactStatus;

protected:

    TStatusItem *items;
    TStatusDef *defs;

private:

    void drawSelect( TStatusItem *selected );
    void findItems();
    TStatusItem *itemMouseIsIn( TPoint );
    void disposeItems( TStatusItem *item );

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

    static void writeItems( opstream&, TStatusItem * );
    static void writeDefs( opstream&, TStatusDef * );
    static TStatusItem *readItems( ipstream& );
    static TStatusDef *readDefs( ipstream& );


protected:

    TStatusLine( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );
 
public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TStatusLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStatusLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStatusLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStatusLine* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TStatusLine

