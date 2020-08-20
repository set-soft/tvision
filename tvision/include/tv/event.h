/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Robert H”hne to be used for RHIDE.
 * SET: Hardware abstaction with ideas from JASC.
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

// This class is the base hardware interface with the mouse and shouldn't
// be used directly. You should use TMouse instead which is derived from
// it. That's why most members are protected.
// See thwmouse.cc
class CLY_EXPORT THWMouse
{
protected:
 THWMouse();
 THWMouse(const THWMouse&) {};
 virtual ~THWMouse();

 static void show();
 static void hide();

 // Needed by some drivers to communicate the size of the screen.
 static void (*setRange)(ushort, ushort);
 static void getEvent(MouseEventType &me);
 // This function could fail according to the hardware.
 // In TV 2.0 it just doesn't exist when you compile for 32 bits.
 static void (*registerHandler)(unsigned, void (*)());

 static void suspend();
 static void resume();

 // Inline members:
 // Disable mouse
 static void inhibit();
 // Is mouse installed?
 static Boolean present();

 // SET: To override just part of the behavior:
 static void (*Show)();
 static void (*Hide)();
 static void (*Suspend)();
 static void (*Resume)();
 static void (*GetEvent)(MouseEventType &me);

 // SET: This is optional, is only needed if the harware uses forceEvent.
 static int  (*drawMouse)(int x, int y);

 // SET: Default behaviors
 static void defaultShow();
 static void defaultHide();
 static void defaultSetRange(ushort, ushort);
 static void defaultGetEvent(MouseEventType&);
 static void defaultRegisterHandler(unsigned, void (*)());
 static void defaultSuspend();
 static void defaultResume();
 static int  defaultDrawMouse(int x, int y);

public:
 // SET: Used to externally force a mouse event.
 // This is only used internally.
 static void forceEvent(int x, int y, int buttons);

protected:
 // This indicates how many buttons have the mouse. Is also used to determine
 // if the mouse is present, a value of 0 is mouse not available. See the
 // present() member.
 static uchar buttonCount;
 // SET: Suspend sets buttonCount to 0 to disable the mouse. The default
 // resume behavior is to restore this value.
 static uchar oldButtonCount;
 // SET: Just to avoid redundant calls
 static char  visible;
 // SET: Data used to force an event externally
 static MouseEventType forcedME;
 static char forced;
 static uchar btBeforeForce;

 // SET: Moved to the protected section
 static Boolean handlerInstalled;
 static Boolean noMouse;
 // The following counter is incremented when the mouse pointer is updated
 // by the driver. Only useful when done asynchronically.
 static volatile unsigned drawCounter;
};

inline Boolean THWMouse::present()
{
 return Boolean(buttonCount!=0);
}

inline void THWMouse::inhibit()
{
 noMouse=True;
}

// This class exposses the mouse interface.
class CLY_EXPORT TMouse : public THWMouse
{
public:
 TMouse();
 ~TMouse();

 static void show();
 static void hide();

 static void suspend();
 static void resume();

 static void setRange( ushort, ushort );

 static void getEvent( MouseEventType& );
 static void registerHandler( unsigned, void (*)() );
 static Boolean present();

 static void resetDrawCounter();
 static unsigned getDrawCounter();
};

inline void TMouse::show()
{
 THWMouse::show();
}

inline void TMouse::hide()
{
 THWMouse::hide();
}

inline void TMouse::suspend()
{
 THWMouse::suspend();
}

inline void TMouse::resume()
{
 THWMouse::resume();
}

inline void TMouse::setRange(ushort rx, ushort ry)
{
 THWMouse::setRange(rx,ry);
}

inline void TMouse::getEvent(MouseEventType& me)
{
 THWMouse::getEvent(me);
}

inline void TMouse::registerHandler(unsigned mask, void (*func)())
{
 THWMouse::registerHandler(mask,func);
}

inline Boolean TMouse::present()
{
 return THWMouse::present();
}

inline void TMouse::resetDrawCounter()
{
 drawCounter=0;
}

inline unsigned TMouse::getDrawCounter()
{
 return drawCounter;
}
/****************************************************************************************/

struct CharScanType
{
 uchar charCode;        // The character encoded in the application code page
 uchar scanCode;
};

struct KeyDownEvent
{
 CharScanType charScan;
 ushort keyCode;        // Internal code, used for special keys (i.e. arrows)
 ushort shiftState;
 uchar  raw_scanCode;
 uint32 charCode;       // The Unicode16 of the key when the driver is in
                        // Unicode16 mode. 0xFFFF if no character is associated.
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

