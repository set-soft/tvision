/* Skeleton screen routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
/*
  This header is an example that could help somebody trying to write a new
driver. The word "Skeleton" should be replaced by the target i.e. the OS.
*/
// Here specify if this header requires some system specific header
#ifndef SKELETONSCR_HEADER_INCLUDED
#define SKELETONSCR_HEADER_INCLUDED

// virtual to avoid problems with multiple inheritance
class TDisplaySkeleton : virtual public TDisplay
{
protected:
 // Usually TDisplay contructor does nothing. All the magic is in TScreen
 // constructor, a class derived from it.
 TDisplaySkeleton() {};
 // We will use casts to base classes, destructors must be pointers.
 // Usually the magic goes in ~TScreenSkeleton
 virtual ~TDisplaySkeleton();
 // This sets the pointers of TDisplay to point to this class.
 static void   Init();

 // Behaviors to replace TDisplay.
 // Note all of this is protected and is supposed to be used through TScreen
 // You can name these members as you like. Not all have to be implemented
 // and all have defaults.
 // Set text cursor position
 static void   SetCursorPos(unsigned x, unsigned y);
 // Find text cursor position
 static void   GetCursorPos(unsigned &x, unsigned &y);
 // What's the shape of the text cursor?
 static void   GetCursorShape(unsigned &start, unsigned &end);
 // Set the shape of the text cursor
 static void   SetCursorShape(unsigned start, unsigned end);
 // How many lines have the display?
 static ushort GetRows();
 // How many columns?
 static ushort GetCols();
 // Set a video mode number, usually not implemented, but sometimes you need to
 // set some defaults here.
 static void   SetCrtMode(ushort mode);
 // Set a video mode using an external program
 static void   SetCrtModeExt(char *mode);
 // Did the display changed its size since last call?
 static int    CheckForWindowSize(void);
 // What's the title of our window?
 static const char *GetWindowTitle(void);
 // Set the title for our window
 static int    SetWindowTitle(const char *name);

 // Variables for this driver
 // Here you define variables needed by your driver.
};

// With this order the destructor will be called first for TScreenSkeleton,
// TScreen, TDisplaySkeleton and finally TDisplay.
class TScreenSkeleton : public TDisplaySkeleton, public TScreen
{
public:
 // Put the driver detection/initialization in this constructor
 TScreenSkeleton();
 // We will use casts to base classes, destructors must be pointers
 // Here you can put the code needed to restore the state of the machine
 // before initialization. Be very careful we the order and avoid doble
 // deinitializations.
 virtual ~TScreenSkeleton();

 // Usually our guts are needed by the key, mouse and/or display.
 // Here you can allow access to those classes.
 friend class TGKeySkeleton;
 friend class THWMouseSkeleton;
 friend class TDisplaySkeleton;

protected:
 // Memorize the state of the console and set our state.
 static void   Resume();
 // Restore the state the console had.
 static void   Suspend();
 // This does a translation between video mode numbers passed to setVideoMode
 // and the values passed to setCrtMode.
 static ushort fixCrtMode(ushort mode);
 // This sets some internal variables with the current state of the console.
 // Take a look at the default in tscreen.cc
 static void   setCrtData();
 // Clear the contents of the whole screen. Here just to be compatible with
 // original TV.
 static void   clearScreen();
 // See TDisplaySkeleton::SetCrtMode, this is the function expossed to the
 // application.
 static void   setVideoMode(ushort mode);
 // See TDisplaySkeleton::SetCrtModeExt, this is the function expossed to the
 // application.
 static void   setVideoModeExt(char *mode);
 // The following four functions works with the screen buffer.
 // Internally TV uses ushort arrays to store the characters and their
 // attributes (8 bits each one). The organization is the same used by
 // PC VGA video cards. The format is independent of the endianess of
 // the architecture. If you cast the "short *" array to "unsigned char *"
 // the even positions will hold the characters and the odd positions the
 // attributes. The upper nibble of the attribute is the background and
 // the lower is the foreground color.
 // The driver should assign to screenBuffer the address of a buffer
 // containing the actual data displayed in the screen. In TV 1.x for DOS
 // it was a pointer to the video memory, in TV 2.0 it can be a pointer to
 // a buffer in memory or just any value, but can't remain NULL or
 // nothing will be sent to the screen.
 // When screenBuffer points to a buffer in memory setCharacter* should
 // output to the screen *and* hold a copy in this buffer. In this case
 // the default getCharacter* will do the job.
 // Get the char/attribute pairs at offset position (x=offset%cols,
 // y=offset/cols).
 static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
 static ushort getCharacter(unsigned dst);
 // Sets the char/attribute pairs.
 static void   setCharacter(unsigned offset, ushort value);
 static void   setCharacters(unsigned dst, ushort *src, unsigned len);
 // Calls an external program, if pidChild pointer is not null the routine
 // should try to do it in background and return the process ID.
 static int    System(const char *command, pid_t *pidChild);
 
protected:
 // Put support routines and variables here
};

#endif // SKELETONSCR_HEADER_INCLUDED

