/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Vadim Beloborodov
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#include <cl/needs.h>

#if defined(TVCompf_djgpp) || defined(TVComp_BCPP) || defined(TVOS_UNIX) || \
    defined(TVCompf_Cygwin)
#define Uses_fixpath
#define Uses_string
#define Uses_limits
#include <compatlayer.h>

void CLY_fexpand( char *rpath )
{
 char path[PATH_MAX];
 _fixpath(rpath,path);
 strcpy(rpath,path);
}
#define FEXPAND_DEFINED
#endif


#if defined(TVOS_Win32) && !defined(FEXPAND_DEFINED)
// Code provided by Vadim Beloborodov
// This should be unified with the NT version and use _fixpath.

#define Uses_string
#define Uses_limits
#define Uses_stdlib
#define Uses_ctype
#define Uses_direct
#include <compatlayer.h>

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
	*dest = 0;                // zero terminator
	dest = path;
	src = path;
	while( *src != 0 ) {
		if ((*src == '\\')&&(src[1] == '\\'))
			src++;
		else
			*dest++ = *src++;
	}
	*dest = 0;                // zero terminator
}

void CLY_fexpand( char *rpath )
{
	char path[PATH_MAX]="";
	char drive[PATH_MAX]="";
	char dir[PATH_MAX]="";
	char file[PATH_MAX]="";
	char ext[PATH_MAX]="";
   char *p;

	_splitpath( rpath, drive, dir, file, ext );
	if( *drive == 0 ) {
		drive[0] = _getdrive() - 1 + 'A';
		drive[1] = ':';
		drive[2] = '\0';
	}
	drive[0] = toupper(drive[0]);
	if( (*dir == 0) || (dir[0] != '\\' && dir[0] != '/') ) {
		char curdir[PATH_MAX];
      int len;
		_getdcwd( drive[0] - 'A' + 1, curdir, PATH_MAX );
		len = strlen(curdir);
		if (curdir[len - 1] != '\\') {
			curdir[len] = '\\';
			curdir[len + 1] = 0;
		}
		strcat( curdir, dir );
		if( *curdir != '\\' && *curdir != '/' ) {
			*dir = '\\';
			strcpy( dir+1, curdir );
		} else
			strcpy( dir, curdir );
	}

	p = dir;
	while( (p = strchr( p, '/' )) != 0 )
		*p = '\\';
	squeeze( dir );
	_makepath( path, drive, dir, file, ext );
	strcpy( rpath, path );
}
#endif

