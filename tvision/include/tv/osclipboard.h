/**************************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Description:
  OS independent clipboard system class. Uses to access the OS clipboard.
  
***************************************************************************/

class TScreen;
class TScreenX11;

class TVOSClipboard
{
public:
 TVOSClipboard() {};
 ~TVOSClipboard() { if (destroy) destroy(); };

 static int   (*copy)(int id, const char *buffer, unsigned len);
 static char *(*paste)(int id);
 static const
        char   *getName() { return name; };
 static int     isAvailable() { return available; };

protected:
 static void  (*destroy)();
 static const char *name;
 static int available;

 friend class TScreen;
 friend class TScreenX11;
};

