/**[txh]********************************************************************

  Copyright (c) 2001 by Salvador E. Tropea

  Description: 
  That's the DOS low level mouse class.
  It have more than one driver inside that is selected externally.
  1) It can use a mouse callback or poll the mouse.
  2) It can let the mouse driver draw the cursor or do it in the class.
  
***************************************************************************/

class THWMouseDOS : public THWMouse
{
protected:
 THWMouseDOS() {};
 virtual ~THWMouseDOS();

 static void Show();
 static void Hide();
 static void DrawMouse(int x, int y);
 static void Suspend();
 static void Resume();
 static void GetEvent(MouseEventType &me);

 // Needed to initialize it.
 static void Init();
 friend class TDisplayDOS;

 // Specific for this driver:
 // Choose if the mouse will be emulated or the OS driver will draw it.
 static void setEmulation(int emulate);
 // Sets the start of video memory (only needed for emulated mouse)
 static void setScreenBase(ulong base);

 // Address of the start of video memory, use setScreenBase()
 static ulong screenBase;

public
 // Indicates if the initialization will install a Real Mode Callback to
 // hook the mouse. That's enabled by default when the OS isn't Win NT.
 // Set it to 0 before initialization to avoid hooking the mouse.
 static char  useMouseHandler;
};

