/* UNIX mouse handler routines header.
   Copyright by Salvador E. Tropea (SET) (2001-2002)
   Covered by the GPL license. */
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && !defined(TVOSf_QNXRtP) &&\
    !defined(UNIXMOUSE_INCLUDED)
#define UNIXMOUSE_INCLUDED

class THWMouseUNIX : public THWMouse
{
protected:
 THWMouseUNIX() {};
 virtual ~THWMouseUNIX();

 static void Show();
 static void Hide();
 static int  DrawMouse(int x, int y);

 static void Init();
};

#ifdef HAVE_NCURSES
class THWMouseXTerm : public THWMouseUNIX
{
protected:
 THWMouseXTerm() {};
 virtual ~THWMouseXTerm();

 static void Suspend();
 static void Resume();

 static void Init();

 friend class TScreenUNIX;
};
#endif

#endif // UNIXMOUSE_INCLUDED
