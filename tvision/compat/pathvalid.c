/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Anatoli Soltan
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_string
#define Uses_limits
#include <compatlayer.h>

// CheckOnlyDrive(dir) and CheckDriveValid(a)
#if CLY_HaveDriveLetters
#define CheckOnlyDrive(dir) if (!unc && strlen(dir)<=3)\
                               return CLY_DriveValid(dir[0])
#define CheckDriveValid(a)  if (!unc && !CLY_DriveValid(dir[0]))\
                               return 0;
#define DeclareUNCVar       int unc=0
#endif
// By default is a nop
#ifndef CheckOnlyDrive
#define CheckOnlyDrive(dir)
#endif
#ifndef CheckDriveValid
#define CheckDriveValid(a)
#endif
#ifndef DeclareUNCVar
#define DeclareUNCVar
#endif

// CheckUNC(path)
#ifdef TVComp_BCPP
#define CheckUNC(path) \
  unc = CLY_IsUNC(path); \
  if (unc && CLY_IsUNCShare(path)) \
     return 1
#endif
// By default is a nop
#ifndef CheckUNC
#define CheckUNC(path)
#endif

int CLY_PathValid(const char *path)
{
 char dir[PATH_MAX];
 char name[PATH_MAX];
 DeclareUNCVar;

 // Set UNC variable and return if that's an UNC path
 CheckUNC(path);
 CLY_ExpandPath(path, dir, name);
 // return True if that's just a valid drive letter
 CheckOnlyDrive(dir);
 // Force it to look like a directory
 if (*dir && dir[strlen(dir)-1]==DIRSEPARATOR)
    strcat(dir,".");
 else
    strcat(dir,DIRSEPARATOR_".");
 // return False if the drive letter is invalid
 CheckDriveValid(dir[0]);
 return CLY_IsDir(dir);
}
