/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Modified by Vadim Beloborodov to be used on WIN32 console */
#include <io.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <sys/stat.h>
#include <ttypes.h>
#include <tvutil.h>
#include <tvconfig.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

unsigned short win_ver()
{
  return 0;
}

int getcurdir(int drive, char *buffer)
{
	_getdcwd(drive, buffer, PATH_MAX );
	strcpy(buffer,buffer+3);
	return 0;
}

extern "C" long __filelength(int fhandle)
{
	return _filelength(fhandle);
}

/*
 * Expands a path into its directory and file components.
 */

void expandPath(const char *path, char *dir, char *file)
{
  char *tag = strrchr(path, DIRSEPARATOR);

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

Boolean driveValid( char drive )
{
	drive = toupper(drive);
	DWORD mask = 0x01 << (drive - 'A');
	return (Boolean)(GetLogicalDrives() & mask);
}

Boolean isDir( const char *str )
{
  struct stat s;
  if (stat(str, &s) == 0 && (s.st_mode&_S_IFDIR)) return True;
  return False;
}

Boolean pathValid( const char *path )
{
  char dir[PATH_MAX];
  char name[PATH_MAX];
  
  expandPath(path, dir, name);
  if (strlen(dir) <= 3)
    return driveValid(dir[0]);
  if (*dir && dir[strlen(dir)-1] == DIRSEPARATOR) strcat(dir, ".");
  else strcat(dir, DIRSEPARATOR_".");

  if (driveValid(dir[0]) == False)
    return False;
  return isDir(dir);
}

Boolean validFileName( const char *fileName )
{
  static const char * illegalChars= "<>|/\"";

  const char *slash = strrchr(fileName,DIRSEPARATOR);
  if (!slash)
  {
    if (strpbrk(fileName,illegalChars) != 0)
      return False;
    return True;
  }
  if (strpbrk(slash+1,illegalChars) != 0)
    return False;
  return pathValid(fileName);
}

void getCurDir( char *dir )
{
  getcwd(dir, PATH_MAX);
  if (dir[strlen(dir)-1] != DIRSEPARATOR)
    strcat(dir,DIRSEPARATOR_);
}

Boolean isWild( const char *f )
{
    return Boolean( strpbrk( f, "?*" ) != 0 );
}

static void squeeze( char *path )
{
	char *
	dest = path;
	char *
	src = path;
	while( *src != 0 ) {
		if (*src == '.')
			if (src[1] == '.') {
				src += 2;
				if (dest > path) {
					dest--;
					while ((*--dest != '\\')&&(dest > path)) // back up to the previous '\'
						;
					dest++;         // move to the next position
				}
			} else if (src[1] == '\\')
				src++;
			else
				*dest++ = *src++;
		else
			*dest++ = *src++;
	}
	*dest = EOS;                // zero terminator
	dest = path;
	src = path;
	while( *src != 0 ) {
		if ((*src == '\\')&&(src[1] == '\\'))
			src++;
		else
			*dest++ = *src++;
	}
	*dest = EOS;                // zero terminator
}

void fexpand( char *rpath )
{
	char path[PATH_MAX]="";
	char drive[PATH_MAX]="";
	char dir[PATH_MAX]="";
	char file[PATH_MAX]="";
	char ext[PATH_MAX]="";

	_splitpath( rpath, drive, dir, file, ext );
	if( *drive == 0 ) {
		drive[0] = _getdrive() - 1 + 'A';
		drive[1] = ':';
		drive[2] = '\0';
	}
	drive[0] = toupper(drive[0]);
	if( (*dir == 0) || (dir[0] != '\\' && dir[0] != '/') ) {
		char curdir[PATH_MAX];
		_getdcwd( drive[0] - 'A' + 1, curdir, PATH_MAX );
		int
		len = strlen(curdir);
		if (curdir[len - 1] != '\\') {
			curdir[len] = '\\';
			curdir[len + 1] = EOS;
		}
		strcat( curdir, dir );
		if( *curdir != '\\' && *curdir != '/' ) {
			*dir = '\\';
			strcpy( dir+1, curdir );
		} else
			strcpy( dir, curdir );
	}

	char *p = dir;
	while( (p = strchr( p, '/' )) != 0 )
		*p = '\\';
	squeeze( dir );
	_makepath( path, drive, dir, file, ext );
	strcpy( rpath, path );
}
