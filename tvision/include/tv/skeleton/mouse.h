/* Skeleton mouse routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// For more information look in event.h
#ifndef SKELETONMOUSE_HEADER_INCLUDED
#define SKELETONMOUSE_HEADER_INCLUDED

class THWMouseSkeleton : public THWMouse
{
public:
 // Setup the pointers to point our members
 static void Init();

protected:
 // Note that even contructor/destructor are protected
 // Usually nothing goes here and the initialization code goes in Init()
 THWMouseSkeleton() {};
 // Here you can put the code needed to restore the state of the machine
 // before initialization. Be very careful we the order and avoid doble
 // deinitializations.
 virtual ~THWMouseSkeleton();

 // Function replacements
 // Make the mouse cursor visible
 static void Show();
 // Make the mouse cursor invisible
 static void Hide();
 // Put the mouse in the original state
 static void Suspend();
 // Memorize current state and setup our state
 static void Resume();
 // Get a mouse event and fill the structure. If no mouse event is available
 // fill the structure with the same data you used last time.
 static void GetEvent(MouseEventType &me);

 // The setup of the pointers to point our members is done in Init()
 // Don't forget to setup buttonCount or the class will say the mouse isn't
 // present.

 // Put support routines and variables here
};

#endif // SKELETONMOUSE_HEADER_INCLUDED

