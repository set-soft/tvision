/**************************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Description:
  OS independent clipboard system class. Uses to access the OS clipboard.
  
***************************************************************************/

class TScreen;
class TVDOSClipboard;
class TVWin32Clipboard;
class TVX11Clipboard;

class CLY_EXPORT TVOSClipboard
{
public:
 TVOSClipboard() {};
 ~TVOSClipboard() { if (destroy) destroy(); };

 static int   (*copy)(int id, const char *buffer, unsigned len);
 static char *(*paste)(int id, unsigned &length);
 static const
        char   *getName() { return name; };
 static int     isAvailable() { return available; };
 static const
        char   *getError();
 static void    clearError() { error=0; };
 static void  (*destroy)();

protected:
 static const char *name;
 static int available;
 static int error;
 static int errors;
 static const char **nameErrors;

 friend class TScreen;
 friend class TVDOSClipboard;
 friend class TVX11Clipboard;
 friend class TVWin32Clipboard;
};

