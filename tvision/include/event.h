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

#if defined( Uses_TEvent ) && !defined( __TEvent )
#define __TEvent

struct MouseEventType
{
    uchar buttons;
    Boolean doubleClick;
    TPoint where;
};

class THWMouse
{

protected:

    THWMouse();
    THWMouse( const THWMouse& ) {};
    virtual ~THWMouse();

    static void show();
    static void hide();

    static void setRange( ushort, ushort );
    static void getEvent( MouseEventType& );
    static void registerHandler( unsigned, void (*)() );
    static Boolean present();

    static void suspend();
    static void resume();
    static void inhibit();

protected:

    static uchar buttonCount;

private:

    static Boolean handlerInstalled;
    static Boolean noMouse;

};

inline Boolean THWMouse::present()
{
    return Boolean( buttonCount != 0 );
}

inline void THWMouse::inhibit()
{
    noMouse = True;
}

class TMouse : public THWMouse
{

public:

    TMouse();
    ~TMouse();

    static void show();
    static void hide();

    static void setRange( ushort, ushort );

    static void getEvent( MouseEventType& );
    static void registerHandler( unsigned, void (*)() );
    static Boolean present();

    static void suspend() { THWMouse::suspend(); }
    static void resume();

};

inline void TMouse::show()
{
    THWMouse::show();
}

inline void TMouse::resume()
{
  THWMouse::resume();
//  show();
}

inline void TMouse::hide()
{
    THWMouse::hide();
}

inline void TMouse::setRange( ushort rx, ushort ry )
{
    THWMouse::setRange( rx, ry );
}

inline void TMouse::getEvent( MouseEventType& me )
{
    THWMouse::getEvent( me );
}

inline void TMouse::registerHandler( unsigned mask, void (*func)() )
{
    THWMouse::registerHandler( mask, func );
}

inline Boolean TMouse::present()
{
    return THWMouse::present();
}

struct CharScanType
{
 uchar charCode;
 uchar scanCode;
};

struct KeyDownEvent
{
 CharScanType charScan;
 ushort keyCode;
 ushort shiftState;
 uchar raw_scanCode;
};

struct MessageEvent
{
    ushort command;
    union
        {
        void *infoPtr;
        long infoLong;
        ushort infoWord;
	short infoInt;
        uchar infoByte;
        char infoChar;
        };
};

struct TEvent
{
    ushort what;
    union
    {
        MouseEventType mouse;
        KeyDownEvent keyDown;
	MessageEvent message;
    };
    void getMouseEvent();
    void getKeyEvent();
};

#endif  // Uses_TEvent

