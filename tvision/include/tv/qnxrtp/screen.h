/* QNX RtP screen routines header.
   Copyright (c) 2002 by Mike Gorchak
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
      static void   SetCrtModeExt(char *mode);
      static int    CheckForWindowSize(void);
      static const char *GetWindowTitle(void);
      static int    SetWindowTitle(const char *name);
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
      static void   setVideoModeExt(char *mode);
      static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
      static ushort getCharacter(unsigned dst);
      static void   setCharacter(unsigned offset, ushort value);
      static void   setCharacters(unsigned dst, ushort *src, unsigned len);
      static int    System(const char *command, pid_t *pidChild);
 
   protected:
};

#endif // QNXRTPSCR_HEADER_INCLUDED
