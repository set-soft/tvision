/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_string
#include <compatlayer.h>

/**[txh]********************************************************************

  Description:
  Extracts from path the directory part and filename part. If 'dir' and/or
'file' == NULL, it is not filled. The directory will have a trailing slash.

***************************************************************************/

void CLY_ExpandPath(const char *path, char *dir, char *file)
{
 const char *tag = strrchr(path, DIRSEPARATOR);

 if (tag != NULL)
 {
   if (file)
     strcpy(file, tag + 1);
   if (dir)
   {
     strncpy(dir, path, tag - path + 1);
     dir[tag - path + 1] = '\0';
   }
 }
 else
 {
   /* there is only the file name */
   if (file)
     strcpy(file, path);
   if (dir)
     dir[0] = '\0';
 }
}

