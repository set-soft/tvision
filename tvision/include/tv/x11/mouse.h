/* X11 mouse routines header.
   Copyright (c) 2001-2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && defined(HAVE_X11) && !defined(X11MOUSE_HEADER_INCLUDED)
#define X11MOUSE_HEADER_INCLUDED

class THWMouseX11 : public THWMouse
{
public:
 static void Init();

protected:
 THWMouseX11() {};
 virtual ~THWMouseX11();

 static void GetEvent(MouseEventType &me);

 // For this driver
 static int getMouseEvent();

 static unsigned mouseButtons;
 static unsigned mouseX,mouseY;
 static unsigned buttonTranslate[8];
};

#endif // X11MOUSE_HEADER_INCLUDED

