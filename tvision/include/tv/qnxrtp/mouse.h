/* QNX RtP mouse routines header.
   Copyright (c) 2002-2003 by Mike Gorchak
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

   protected:
      static int mousehandle;
      static int MousePositionX;
      static int MousePositionY;
      static int MouseButtons;

      static int MouseMickeyPositionX;
      static int MouseMickeyPositionY;

   protected:
      static int  OpenDeviMouse();
      static void CloseDeviMouse();
      static void ProcessDeviMouse();
};

#endif // QNXRTPMOUSE_HEADER_INCLUDED
