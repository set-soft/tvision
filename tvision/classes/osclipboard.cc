/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Module: TVOSClipboard
  Header: osclipboard.h
  Description: 
  OS independent clipboard system class. Uses to access the OS clipboard.@*
  It was created to abstract the X11 and Windows clipboards.
  
***************************************************************************/

#define Uses_TVOSClipboard
#include <tv.h>

const char   *TVOSClipboard::name="None";
int           TVOSClipboard::available=0;
int         (*TVOSClipboard::copy)(int id, const char *buffer, unsigned len)=NULL;
char       *(*TVOSClipboard::paste)(int id)=NULL;
void        (*TVOSClipboard::destroy)()=NULL;
