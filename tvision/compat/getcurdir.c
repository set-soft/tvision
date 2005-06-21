/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Vadim Beloborodov
 Copyright (C) 2000      Anatoli Soltan
 Copyright (C) 2000-2005 Salvador E. Tropea
 Covered by the GPL license.
*/

#include <cl/needs.h>

#define Uses_limits
#define Uses_string
#define Uses_unistd
#include <compatlayer.h>

#ifdef NEEDS_GETCURDIR

#ifdef TVCompf_djgpp
// DOS version by Robert
#include <dir.h>

int CLY_getcurdir(int drive, char *buffer)
{
  if (!drive) getwd(buffer);
  else
  {
    int d = getdisk();
    setdisk(drive-1);
    getwd(buffer);
    setdisk(d);
  }
  strcpy(buffer,buffer+3);
  return 0;
}
#endif // DJGPP

#if defined(TVOS_Win32) && !defined(TVComp_BCPP) && !defined(TVCompf_Cygwin)
// MSVC/MingW/OW Windows version by Vadim and Anatoli
#include <direct.h>

int CLY_getcurdir(int drive, char *buffer)
{
  _getdcwd(drive, buffer,PATH_MAX);
  strcpy(buffer,buffer+3);
  return 0;
}
#endif // MSVC/MingW/OW

#if defined(TVOS_UNIX) || defined(TVCompf_Cygwin)
// Linux and CygWin version

int CLY_getcurdir(int drive, char *buffer)
{
  getcwd(buffer, PATH_MAX);
  return 0;
}
#endif

#endif // NEEDS_GETCURDIR

void CLY_GetCurDirSlash(char *dir)
{
 getcwd(dir,PATH_MAX);
 if (dir[strlen(dir)-1]!=DIRSEPARATOR)
    strcat(dir,DIRSEPARATOR_);
}


