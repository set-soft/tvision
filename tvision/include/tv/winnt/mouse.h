/* WinNT screen routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// This headers needs windows header
#if defined(TVOS_Win32) && !defined(WINNTMOUSE_HEADER_INCLUDED)
#define WINNTMOUSE_HEADER_INCLUDED

class THWMouseWinNT : public THWMouse
{
public:
 static void Init();

 friend class TScreenWinNT;

protected:
 THWMouseWinNT() {};
 virtual ~THWMouseWinNT();

 static void GetEvent(MouseEventType &me);
 static void Resume();

 // For this driver
 static INPUT_RECORD inpRec;
};

#endif // WINNTMOUSE_HEADER_INCLUDED

