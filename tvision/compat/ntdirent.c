/* vi: set ts=8 sw=4 : */
/********************************************************************\
*File   : ntdirent.c
*
*Subject:
*   Win32 implementation of Unix directory functions
*
*Authors:
*   SAA	 Anatoli Soltan
*
*HISTORY:
*   Feb 18 1997  SAA   implemented opendir, readdir, closedir
*   Sep 14 1999  SAA   changed _alloca to alloca to make mingw32 work
*   Mar 15 2000  SAA   check for alloca definition to make it work
*                      on BCB
*********************************************************************/
#include <cl/needs.h>

#ifdef NEEDS_OPENDIR

#include <malloc.h>
#include <cl/dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <io.h>

#ifndef TVComp_GCC
 #ifndef alloca
  #define alloca _alloca
 #endif
#else
 #if (__MINGW32_MAJOR_VERSION>=2) && !defined(alloca)
  #define alloca __builtin_alloca
 #endif
#endif

#ifndef S_IFDIR
#ifdef _S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
#endif

struct _DIR
{
    long		_sig;	   /* structure's signature */
    long		_findh;    /* find handle */
    struct dirent	_dirent;   /* storage for readdir */
    struct _finddata_t	_data;     /* buffer for _findnext */
    int			_status;   /* status of previos _findnext */
};

#define _DIR_SIGNATURE	0xDEDEEDED


DIR* opendir(const char* name)
{
    size_t  len;
    char*   path;
    DIR*    dir = NULL;
    struct stat dirstat;

    /* name is NULL */
    if (!name) {
	errno = ENOENT;
	return NULL;
    }

    path = (char*)name;
    while (isspace(*path)) path++;

    /* name is empty string */
    if (!*path) {
	errno = ENOENT;
	return NULL;
    }

    /* add "*" mask to the dir name */
    len = strlen(name);
    path = (char *)alloca(len + 5);
    strcpy(path, name);
    if (path[len-1] == '/' || path[len-1] == '\\')
	strcpy(path + len, "*");
    else
	strcpy(path + len, "\\*");

    /* name is not exist */
    if (stat(name, &dirstat))
	return NULL;

    /* name is not a directory */
    if (!(dirstat.st_mode & S_IFDIR)) {
	errno = ENOTDIR;
	return NULL;
    }

    /* alloc DIR structure */
    dir = (DIR*)malloc(sizeof(DIR));
    if (!dir) {
	errno = ENOMEM;
	return NULL;
    }

    /* initialize DIR structure */
    dir->_sig = _DIR_SIGNATURE;
    /* do first find */
    dir->_findh = _findfirst(path, &dir->_data);
    if (dir->_findh == -1)
	dir->_status = 0;
    else
	dir->_status = 1;

    return dir;
}


struct dirent* readdir(DIR* dir)
{
    int	e;
    /* bad DIR pointer */
    if (!dir || dir->_sig != (int)_DIR_SIGNATURE) {
	errno = EBADF;
	return NULL;
    }

    /* previos _findfirst or _findnext failed */
    if (!dir->_status) {
	return NULL;
    }

    /* copy previosly found name */
    strcpy(dir->_dirent.d_name, dir->_data.name);

    /* do next find */
    e = errno;
    if (_findnext(dir->_findh, &dir->_data)) {
	errno = e;
	dir->_status = 0;
    }

    return &dir->_dirent;
}


int closedir(DIR* dir)
{
    /* bad DIR pointer */
    if (!dir || dir->_sig != (int)_DIR_SIGNATURE) {
	errno = EBADF;
	return -1;
    }

    /* close find handle */
    if (dir->_findh != -1)
	_findclose(dir->_findh);
    free(dir);

    return 0;
}
#endif /* NEEDS_OPENDIR */
