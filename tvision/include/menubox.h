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
/*      class TMenuBox                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuBox ) && !defined( __TMenuBox )
#define __TMenuBox

class TRect;
class TMenu;
class TMenuView;
class TDrawBuffer;

class TMenuBox : public TMenuView
{

public:

    TMenuBox( const TRect& bounds, TMenu *aMenu, TMenuView *aParentMenu);

    virtual void draw();
    virtual TRect getItemRect( TMenuItem *item );

    static char frameChars[];
    static char oframeChars[];
    static char rightArrow;
    static char orightArrow;

private:

    void frameLine( TDrawBuffer&, short n );
    void drawLine( TDrawBuffer& );

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TMenuBox( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TMenuBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuBox* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TMenuBox

