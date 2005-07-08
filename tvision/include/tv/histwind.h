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

#if defined( Uses_THistoryWindow ) && !defined( __THistoryWindow )
#define __THistoryWindow

class TListViewer;
class TRect;
class TWindow;
class TInputLine;

class CLY_EXPORT THistInit
{

public:

    THistInit( TListViewer *(*cListViewer)( TRect, TWindow *, ushort ) );

protected:

    TListViewer *(*createListViewer)( TRect, TWindow *, ushort );

};

/* ---------------------------------------------------------------------- */
/*      THistoryWindow                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = HistoryViewer normal text                                   */
/*        7 = HistoryViewer selected text                                 */
/* ---------------------------------------------------------------------- */

class CLY_EXPORT THistoryWindow : public TWindow, public virtual THistInit
{

public:

    THistoryWindow( const TRect& bounds, ushort historyId );

    virtual TPalette& getPalette() const;
    virtual void getSelection( char *dest );
    static TListViewer *initViewer( TRect, TWindow *, ushort );

protected:

    TListViewer *viewer;
};

#endif  // Uses_THistoryWindow

