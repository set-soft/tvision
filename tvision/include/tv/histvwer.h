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
/*      class THistoryViewer                                              */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Active                                                      */
/*        2 = Inactive                                                    */
/*        3 = Focused                                                     */
/*        4 = Selected                                                    */
/*        5 = Divider                                                     */
/* ---------------------------------------------------------------------- */

#if defined( Uses_THistoryViewer ) && !defined( __THistoryViewer )
#define __THistoryViewer

class TRect;
class TScrollBar;

class CLY_EXPORT THistoryViewer : public TListViewer
{

public:

    THistoryViewer( const TRect& bounds,
                    TScrollBar *aHScrollBar,
                    TScrollBar *aVScrollBar,
                    ushort aHistoryId
                  );

    virtual TPalette& getPalette() const;
    virtual void getText( char *dest, ccIndex item, short maxLen );
    virtual void handleEvent( TEvent& event );
    int historyWidth();

protected:

    ushort historyId;

};

#endif  // Uses_THistoryViewer

