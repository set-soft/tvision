/* QNX RtP mouse routines header.
   Copyright (c) 2002 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNXRtP) && !defined(QNXRTPMOUSE_HEADER_INCLUDED)
#define QNXRTPMOUSE_HEADER_INCLUDED

class THWMouseQNXRtP: public THWMouse
{
public:
 static void Init();

protected:
 THWMouseQNXRtP() {};
 virtual ~THWMouseQNXRtP();

 static void Show();
 static void Hide();
 static void Suspend();
 static void Resume();
 static void GetEvent(MouseEventType &me);
};

#endif // QNXRTPMOUSE_HEADER_INCLUDED
