/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Vadim Beloborodov
 Copyright (C) 2000      Anatoli Soltan
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#include <cl/needs.h>
#ifdef NEEDS_GETCURDIR
#define Uses_limits
#define Uses_string
#include <compatlayer.h>

#ifdef TVOSf_djgpp
// DOS version by Robert
#include <unistd.h>
#include <dir.h>
#include <string.h>

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
#endif

#if defined(TVOS_Win32) && !defined(__TURBOC__)
// MSVC/MingW Windows version by Vadim and Anatoli
#include <direct.h>
#define Uses_limits
#define Uses_string
#include <compatlayer.h>

int CLY_getcurdir(int drive, char *buffer)
{
  _getdcwd(drive, buffer,PATH_MAX);
  strcpy(buffer,buffer+3);
  return 0;
}
#endif

#ifdef TVOS_UNIX
// Linux version
#include <unistd.h>

int CLY_getcurdir(int drive, char *buffer)
{
  getcwd(buffer, PATH_MAX);
  return 0;
}
#endif

#else  // NEEDS_GETCURDIR

#define Uses_limits
#define Uses_string
#define Uses_unistd
#include <compatlayer.h>

#endif // else NEEDS_GETCURDIR

void CLY_GetCurDirSlash(char *dir)
{
 getcwd(dir,PATH_MAX);
 if (dir[strlen(dir)-1]!=DIRSEPARATOR)
    strcat(dir,DIRSEPARATOR_);
}


