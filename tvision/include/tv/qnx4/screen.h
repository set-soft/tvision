/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNX4) && !defined(QNX4SCR_HEADER_INCLUDED)
#define QNX4SCR_HEADER_INCLUDED

#define NUM_MODES 18

struct _VideoModes
{
   unsigned int videomode;
   unsigned int x;
   unsigned int y;
};

#define QNX_CONSOLE_RAW   0
#define QNX_CONSOLE_PTERM 1
#define QNX_CONSOLE_XQSH  2

class TDisplayQNX4: virtual public TDisplay
{
   protected:
      TDisplayQNX4() {};
      virtual ~TDisplayQNX4();
      static void   Init();

      static void   SetCursorPos(unsigned x, unsigned y);
      static void   GetCursorPos(unsigned &x, unsigned &y);
      static void   GetCursorShape(unsigned &start, unsigned &end);
      static void   SetCursorShape(unsigned start, unsigned end);
      static ushort GetRows();
      static ushort GetCols();
      static ushort GetCrtMode();
      static void   SetCrtMode(ushort mode);
      static int    SetCrtModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);
      static void   SetCrtModeExt(char* mode);
      static int    CheckForWindowSize(void);
      static const char* GetWindowTitle(void);
      static int    SetWindowTitle(const char* name);

   protected:
      static ushort CursorLastX;
      static ushort CursorLastY;
      static ushort CursorShapeStart;
      static ushort CursorShapeEnd;
      static char   ConsoleMode;
      static ushort IgnoreConsoleResizing;
      static struct _VideoModes modes[NUM_MODES];

   public:
      static ushort ConsoleResizing;
};

class TScreenQNX4: public TDisplayQNX4, public TScreen
{
   public:
      TScreenQNX4();
      virtual ~TScreenQNX4();

      friend class TGKeyQNX4;
      friend class THWMouseQNX4;
      friend class TDisplayQNX4;

   protected:
      static void   Resume();
      static void   Suspend();
      static ushort FixCrtMode(ushort mode);
      static void   SetCrtData();
      static void   clearScreen();
      static void   SetVideoMode(ushort mode);
      static int    SetVideoModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);
      static void   SetVideoModeExt(char* mode);
      static void   getCharacters(unsigned offset,ushort* buf, unsigned count);
      static ushort getCharacter(unsigned dst);
      static void   setCharacter(unsigned offset, ushort value);
      static void   setCharacters(unsigned dst, ushort* src, unsigned len);
      static int    System(const char* command, pid_t* pidChild, int in, int out, int err);

   protected:
      static int    InitTermLib();
      static void   SpecialKeysDisable(int fd);
      static void   SpecialKeysRestore(int fd);

   protected:
      static unsigned char DefaultRadioButton;
      static cc_t oldKeys[5];
      static ushort oldScreenSizeX;
      static ushort oldScreenSizeY;
      static ushort ForceModeChange;
      static ushort* UserScreenData;
};

#endif // QNX4SCR_HEADER_INCLUDED
