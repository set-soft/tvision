/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to enhance functionality.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TMenuView                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuView ) && !defined( __TMenuView )
#define __TMenuView

class TRect;
class TMenu;
struct TEvent;

class TMenuView : public TView
{

public:

    TMenuView( const TRect& bounds, TMenu *aMenu, TMenuView *aParent );
    TMenuView( const TRect& bounds );

    void setBounds( const TRect& bounds );
    virtual ushort execute();
    TMenuItem *findItem( char ch );
    virtual TRect getItemRect( TMenuItem *item );
    virtual ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    TMenuItem *hotKey( ushort keyCode );
    TMenuView *newSubView( const TRect& bounds,
                           TMenu *aMenu,
                           TMenuView *aParentMenu
                         );
    // SET: Looks like some users really likes the original behavior of
    // having 1 space around menu items. As it reduces the number of menues
    // we can have. I added a conditional way to control it. The code is in
    // TMenuBar, when you create a menu bar (or chanBounds it) the Bar
    // calculates the length of the items and if they are greater than size.x
    // enters in the compatMenu mode. "Norberto Alfredo Bensa (Beto)"
    // <norberto.bensa@abaconet.com.ar> sent me an uncoditional patch that I used
    // as base. This variable is 0 by default (TMenuView constructor)
    char compactMenu;

protected:

    TMenuView *parentMenu;
    TMenu *menu;
    TMenuItem *current;

    // SET: Added to make the code easier to read and to implement some
    // alternatives.
    Boolean keyToItem(TEvent &event);
    Boolean keyToHotKey(TEvent &event);

private:

    void nextItem();
    void prevItem();
    void trackKey( Boolean findNext );
    Boolean mouseInOwner( TEvent& e );
    Boolean mouseInMenus( TEvent& e );
    void trackMouse( TEvent& e );
    TMenuView *topMenu();
    Boolean updateMenu( TMenu *menu );
    void do_a_select( TEvent& );
    TMenuItem *findHotKey( TMenuItem *p, ushort keyCode );
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }
    static void writeMenu( opstream&, TMenu * );
    static TMenu *readMenu( ipstream& );

protected:

    TMenuView( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TMenuView& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuView*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuView& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuView* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

inline TMenuView::TMenuView( const TRect& bounds,
                             TMenu *aMenu,
                             TMenuView *aParent
                           ) :
    TView(bounds), compactMenu ( 0 ), parentMenu( aParent ), menu( aMenu ),
    current( 0 )
{
     eventMask |= evBroadcast;
}

inline TMenuView::TMenuView( const TRect& bounds ) :
    TView(bounds), compactMenu(0), parentMenu(0), menu(0), current(0)
{
     eventMask |= evBroadcast;
}

#endif  // Uses_TMenuView

