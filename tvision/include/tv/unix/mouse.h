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

 friend class TScreenUNIX;
};
#endif

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
