/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000-2002 Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_ctype
#define Uses_stdlib
#include <compatlayer.h>

#if defined(TVOS_DOS) || defined(TVOS_Win32)
 #define StartsWithDrive(path) (ucisalpha(path[0]) && path[1]==':')
#else
 #define StartsWithDrive(path) (0)
#endif

#if defined(TVOS_Win32) && !defined(TVCompf_Cygwin)
 #define IsHomeSymbol(path) (0)
#else
inline static int
is_slash(int c)
{
  return CLY_IsValidDirSep(c);
}
 #define IsHomeSymbol(path) (path[0]=='~' && (!path[1] || CLY_IsValidDirSep(path[1]))\
                      && getenv("HOME"))
#endif

int CLY_IsRelativePath(const char *path)
{
 return
  !(path[0] && (CLY_IsValidDirSep(path[0]) || StartsWithDrive(path) || IsHomeSymbol(path)));
}
