/* QNX RtP screen routines header.
   Copyright (c) 2002-2003 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNXRtP) && !defined(QNXRTPSCR_HEADER_INCLUDED)
#define QNXRTPSCR_HEADER_INCLUDED

class TDisplayQNXRtP: virtual public TDisplay
{
   protected:
      TDisplayQNXRtP() {};
      virtual ~TDisplayQNXRtP();
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
      static inline void safeput(char* &p, char* cap);

   protected:
      static int cur_x;
      static int cur_y;
      static int tty_fd;
      static int cursorStart;
      static int cursorEnd;

      static ushort mode;

      static volatile sig_atomic_t windowSizeChanged;
};

class TScreenQNXRtP: public TDisplayQNXRtP, public TScreen
{
   public:
      TScreenQNXRtP();
      virtual ~TScreenQNXRtP();

      friend class TGKeyQNXRtP;
      friend class THWMouseQNXRtP;
      friend class TDisplayQNXRtP;

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
      static void SpecialKeysDisable(int fd);
      static void SpecialKeysRestore(int fd);
      static void StartNCurses();
      static void SetGTables();
      static void RestoreGTables();
      static void sigWindowSizeChanged(int sig);
      static void SendToTerminal(const char* const value);
      static void RestoreScreen();
      static void mapColor(char* &p, int col);
      static void writeBlock(int dst, int len, ushort* old, ushort* src);
 
   protected:
      static struct termios old_term;
      static struct termios new_term;
      static cc_t oldKeys[5];
      static int old_col;
      static int old_fore;
      static int old_back;
      static int force_redraw;
};

#endif // QNXRTPSCR_HEADER_INCLUDED
