/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)

  Description:
  Win32 Clipboard routines.
  
***************************************************************************/

#if defined(TVOS_Win32) && !defined(WIN32CLIP_HEADER_INCLUDED)
#define WIN32CLIP_HEADER_INCLUDED

class TScreenWin32;
class TScreenWinNT;

class TVWin32Clipboard
{
public:
 TVWin32Clipboard() {};

protected:
 // Replacements for the TVOSClipboard class
 static int   copy(int id, const char *buffer, unsigned len);
 static char *paste(int id, unsigned &length);
 static void  destroy();

 // Members fo this class
 static void  Init();
 static void  getErrorString();

 static const
        char *win32NameError[];

 friend class TScreenWin32;
 friend class TScreenWinNT;
 friend class TScreenWinGr;  /* JASC, feb 2006, also compatible */
};

#endif // WIN32CLIP_HEADER_INCLUDED

