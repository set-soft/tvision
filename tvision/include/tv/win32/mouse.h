/* Win32 screen routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// This headers needs windows header
#if defined(TVOS_Win32) && !defined(WIN32MOUSE_HEADER_INCLUDED)
#define WIN32MOUSE_HEADER_INCLUDED

const unsigned eventMouseQSize=300;

class THWMouseWin32 : public THWMouse
{
public:
 static void Init();
 static void DeInit();

 friend class TScreenWin32;

protected:
 THWMouseWin32() {};
 virtual ~THWMouseWin32();

 static void GetEvent(MouseEventType &me);
 static void Resume();

 // For this driver
 // Put a mouse event in the queue
 static void putConsoleMouseEvent(MouseEventType &mouse);
 // Process a mouse event from Windows
 static void HandleMouseEvent();

 static MouseEventType *evMouseIn;
 static MouseEventType *evLastMouseIn;
 static MouseEventType *evMouseOut;
 static MouseEventType evMouseQueue[eventMouseQSize];
 static unsigned evMouseLength;
 static CRITICAL_SECTION lockMouse;
};

#endif // WIN32MOUSE_HEADER_INCLUDED

