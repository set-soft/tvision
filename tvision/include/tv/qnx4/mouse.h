/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNX4) && !defined(QNX4MOUSE_HEADER_INCLUDED)
#define QNX4MOUSE_HEADER_INCLUDED

class THWMouseQNX4: public THWMouse
{
   public:
      static void Init(ushort mousemode);

   protected:
      THWMouseQNX4() {};
      virtual ~THWMouseQNX4();

      static void Show();
      static void Hide();
      static void Suspend();
      static void Resume();
      static void GetEvent(MouseEventType &me);

   protected:
      static int MouseHandler(unsigned int* key, struct mouse_event* event);

   protected:
      static int    MousePositionX;
      static int    MousePositionY;
      static int    MouseButtons;
      static ushort ConsoleMode;
};

#endif // QNX4MOUSE_HEADER_INCLUDED
