/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_ctype
#include <compatlayer.h>

#if defined(TVOS_DOS) || defined(TVOS_Win32)
#define StartsWithDrive(path) (ucisalpha(path[0]) && path[1]==':')
#else
#define StartsWithDrive(path) (0)
#endif

int CLY_IsRelativePath(const char *path)
{
 return
  !(path[0] && (CLY_IsValidDirSep(path[0]) || StartsWithDrive(path)));
}
