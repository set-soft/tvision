/**[txh]********************************************************************

  Description:
  This module provides some functions to know the file attributes and altere
some of them in an abstract way. Note that I wrote only the functions I need
for SETEdit and not a full set.

 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.

***************************************************************************/

#define CLY_DoNotDefineUTypes
#define Uses_CLYFileAttrs
#include <compatlayer.h>


#ifdef TVCompf_djgpp
/*
  In DOS owr best choice is _chmod, it can give information as file hidden
  or system file.
*/
#include <io.h>

void CLY_GetFileAttributes(CLY_mode_t *mode, struct stat *statVal,
                           const char *fileName)
{
 *mode=_chmod(fileName,0,0);
}

int CLY_SetFileAttributes(CLY_mode_t *newmode, const char *fileName)
{
 return _chmod(fileName,1,*newmode)==-1 ? 0 : 1;
}

void CLY_FileAttrReadOnly(CLY_mode_t *mode)
{
 *mode|=1;
}

void CLY_FileAttrReadWrite(CLY_mode_t *mode)
{
 *mode&=0xFE;
}

int  CLY_FileAttrIsRO(CLY_mode_t *mode)
{
 return (*mode) & 1;
}

void CLY_FileAttrModified(CLY_mode_t *mode)
{
 *mode|=0x20;
}

void CLY_GetDefaultFileAttr(CLY_mode_t *mode)
{
 *mode=0x20;
}
#define FILE_ATTRS_DEFINED
#endif // TVCompf_djgpp



#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && \
    !defined(FILE_ATTRS_DEFINED)
/*
  In UNIX the best way is just use chmod which is POSIX and should be
enough.
*/
#include <unistd.h>

void CLY_GetFileAttributes(CLY_mode_t *mode, struct stat *statVal,
                           const char *fileName)
{
 mode->mode =statVal->st_mode;
 mode->user =statVal->st_uid;
 mode->group=statVal->st_gid;
}

int CLY_SetFileAttributes(CLY_mode_t *newmode, const char *fileName)
{
 return !(chmod(fileName,newmode->mode) ||
          chown(fileName,newmode->user,newmode->group));
}

void CLY_FileAttrReadOnly(CLY_mode_t *mode)
{
 mode->mode&= ~S_IWUSR;
}

void CLY_FileAttrReadWrite(CLY_mode_t *mode)
{
 mode->mode|=S_IWUSR;
}

int  CLY_FileAttrIsRO(CLY_mode_t *mode)
{
 return (mode->mode & S_IWUSR)==0;
}

void CLY_FileAttrModified(CLY_mode_t *mode)
{
}

void CLY_GetDefaultFileAttr(CLY_mode_t *mode)
{ // 644, umask will kill the denied ones if that's needed
 mode->mode=S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR;
 mode->user=getuid();
 mode->group=getgid();
}
#define FILE_ATTRS_DEFINED
#endif



#if defined(TVOS_Win32) && !defined(FILE_ATTRS_DEFINED)
/*
  In Win32 systems there are an API call for it.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void CLY_GetFileAttributes(CLY_mode_t *mode, struct stat *statVal,
                           const char *fileName)
{
 *mode=statVal->st_mode; // Just to avoid unused warning
 *mode=GetFileAttributes(fileName);
}

int CLY_SetFileAttributes(CLY_mode_t *newmode, const char *fileName)
{
 return SetFileAttributes(fileName,*newmode);
}

void CLY_FileAttrReadOnly(CLY_mode_t *mode)
{
 *mode|=FILE_ATTRIBUTE_READONLY;
}

void CLY_FileAttrReadWrite(CLY_mode_t *mode)
{
 *mode&=~FILE_ATTRIBUTE_READONLY;
}

int  CLY_FileAttrIsRO(CLY_mode_t *mode)
{
 return (*mode & FILE_ATTRIBUTE_READONLY);
}

void CLY_FileAttrModified(CLY_mode_t *mode)
{
 *mode|=FILE_ATTRIBUTE_ARCHIVE;
}

void CLY_GetDefaultFileAttr(CLY_mode_t *mode)
{
 *mode=(CLY_mode_t)FILE_ATTRIBUTE_ARCHIVE;
}
#define FILE_ATTRS_DEFINED
#endif
