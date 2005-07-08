/*
 * help.h
 *
 * Turbo Vision - Version 2.0
 *
 * Copyright (c) 1994 by Borland International
 * All Rights Reserved.
 *
 * Modified by Sergio Sigala <ssigala@globalnet.it>
 * Modified by Salvador E. Tropea <set@ieee.org>, <set@users.sourceforge.net>
 */

#if defined(Uses_THelpWindow) && !defined(THelpWindow_Included)
#define THelpWindow_Included

// THelpViewer

class CLY_EXPORT THelpViewer : public TScroller
{
public:

    THelpViewer( const TRect&, TScrollBar*, TScrollBar*, THelpFile*, ushort );
    ~THelpViewer();

    virtual void changeBounds( const TRect& );
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& );
    void makeSelectVisible( int, TPoint&, uchar&, int& );
    void switchToTopic( int );

    THelpFile *hFile;
    THelpTopic *topic;
    int selected;
};

// THelpWindow

class CLY_EXPORT THelpWindow : public TWindow
{

    static const char * helpWinTitle;

public:

    THelpWindow( THelpFile*, ushort );

    virtual TPalette& getPalette() const;
};


extern void notAssigned( opstream& s, int value );

extern TCrossRefHandler crossRefHandler;

#endif // Uses_THelpWindow && !THelpWindow_Included

