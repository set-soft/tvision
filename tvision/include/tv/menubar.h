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
/*      class TMenuBar                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuBar ) && !defined( __TMenuBar )
#define __TMenuBar

class TRect;
class TMenu;

class CLY_EXPORT TMenuBar : public TMenuView
{

public:

    TMenuBar( const TRect& bounds, TMenu *aMenu );
    TMenuBar( const TRect& bounds, TSubMenu &aMenu );
    ~TMenuBar();

    void computeLength();
    virtual void draw();
    virtual TRect getItemRect( TMenuItem *item );
    virtual void changeBounds(const TRect& bounds);
    
#if !defined( NO_STREAM )
private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TMenuBar( StreamableInit );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TMenuBar& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuBar*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuBar& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuBar* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TMenuBar

