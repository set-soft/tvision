#ifndef LINUXMOUSE_INCLUDED
#define LINUXMOUSE_INCLUDED

#include <tv/unix/mouse.h>

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
