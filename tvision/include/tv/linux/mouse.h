/* Linux keyboard handler routines header.
   Copyright by Salvador E. Tropea (SET) (2001-2002)
   Covered by the GPL license. */

#include <tv/unix/mouse.h>

#if defined(TVOSf_Linux) && !defined(LINUXMOUSE_INCLUDED)
#define LINUXMOUSE_INCLUDED

#ifdef HAVE_GPM
class THWMouseGPM : public THWMouseUNIX
{
protected:
 THWMouseGPM() {};
 virtual ~THWMouseGPM();

 static void Suspend();
 static void Resume();
 static void GetEvent(MouseEventType &me);

 static void Init();

 friend class TScreenLinux;
 friend class TScreenUNIX;
};
#endif

#endif // LINUXMOUSE_INCLUDED
