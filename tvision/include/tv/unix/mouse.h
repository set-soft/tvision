#ifndef UNIXMOUSE_INCLUDED
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

#endif // UNIXMOUSE_INCLUDED
