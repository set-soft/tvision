/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Header: TVOSClipboard
  Module: OS Clipboard
  Comments:
  OS independent clipboard system class. Uses to access the OS clipboard.@*
  It was created to abstract the X11 and Windows clipboards.
  
***************************************************************************/

#define Uses_TVOSClipboard
#include <tv.h>

const char   *TVOSClipboard::name="None";
int           TVOSClipboard::available=0;
int         (*TVOSClipboard::copy)(int id, const char *buffer, unsigned len)=NULL;
char       *(*TVOSClipboard::paste)(int id, unsigned &length)=NULL;
void        (*TVOSClipboard::destroy)()=NULL;
int           TVOSClipboard::error=0;
int           TVOSClipboard::errors=0;
const char  **TVOSClipboard::nameErrors=NULL;


const char *TVOSClipboard::getError()
{
 if (error>errors || !error || !nameErrors)
    return NULL;
 return _(nameErrors[error]);
}


