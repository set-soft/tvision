/* QNX 4 screen routines header.
    */

#if defined(TVOSf_QNX4) && !defined(QNX4SCR_HEADER_INCLUDED)
#define QNX4SCR_HEADER_INCLUDED

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
      static void   SetCrtMode(ushort mode);
      static void   SetCrtModeExt(char* mode);
      static int    CheckForWindowSize(void);
      static const char* GetWindowTitle(void);
      static int    SetWindowTitle(const char* name);

   protected:
      static ushort ScreenSizeX;
      static ushort ScreenSizeY;
      static ushort CursorLastX;
      static ushort CursorLastY;
      static ushort CursorShapeStart;
      static ushort CursorShapeEnd;
      static char   ConsoleMode;
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
      static ushort fixCrtMode(ushort mode);
      static void   setCrtData();
      static void   clearScreen();
      static void   setVideoMode(ushort mode);
      static void   setVideoModeExt(char* mode);
      static void   getCharacters(unsigned offset,ushort* buf, unsigned count);
      static ushort getCharacter(unsigned dst);
      static void   setCharacter(unsigned offset, ushort value);
      static void   setCharacters(unsigned dst, ushort* src, unsigned len);
      static int    System(const char* command, pid_t* pidChild, int in, int out, int err);

   protected:
      static int    InitTermLib();

   protected:
      static unsigned char DefaultRadioButton;
};

#endif // QNX4SCR_HEADER_INCLUDED
