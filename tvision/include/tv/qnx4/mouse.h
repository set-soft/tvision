/* QNX 4 mouse routines header.
   */

#if defined(TVOSf_QNX4) && !defined(QNX4MOUSE_HEADER_INCLUDED)
#define QNX4MOUSE_HEADER_INCLUDED

class THWMouseQNX4: public THWMouse
{
public:
 static void Init();

protected:
 THWMouseQNX4() {};
 virtual ~THWMouseQNX4();

 static void Show();
 static void Hide();
 static void Suspend();
 static void Resume();
 static void GetEvent(MouseEventType &me);
};

#endif // QNX4MOUSE_HEADER_INCLUDED
