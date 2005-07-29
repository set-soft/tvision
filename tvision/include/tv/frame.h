/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TFrame                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive frame                                               */
/*        2 = Passive title                                               */
/*        3 = Active frame                                                */
/*        4 = Active title                                                */
/*        5 = Icons                                                       */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TFrame ) && !defined( __TFrame )
#define __TFrame

class TRect;
struct TEvent;
class TDrawBuffer;

class TFrame : public TView
{

public:

    TFrame( const TRect& bounds );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void setState( ushort aState, Boolean enable );

    // SET: Used to disable icon animation
    static Boolean doAnimation;
    
    static char frameChars[33];
    static char closeIcon[];
    static char zoomIcon[];
    static char unZoomIcon[];
    static char dragIcon[];
    static char animIcon[];
    static char oframeChars[33];
    static char ocloseIcon[];
    static char ozoomIcon[];
    static char ounZoomIcon[];
    static char odragIcon[];
    static char oanimIcon[];

private:

    void frameLine( TDrawBuffer& frameBuf, short y, short n, uchar color );
    void dragWindow( TEvent& event, uchar dragMode );
    void drawIcon( int bNormal, const int ciType );
    
    friend class TDisplay;
    static const char initFrame[19];
#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TFrame( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFrame& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFrame*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFrame& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFrame* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TFrame

