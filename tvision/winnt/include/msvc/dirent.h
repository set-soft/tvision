/********************************************************************\
*File   : dirent.h
*
*Subject:
*   Substitution of Unix dirent.h for Win32
*
*Authors:
*   SAA	 Anatoli Soltan
*
*HISTORY:
*   Feb 18 1997  SAA
*       initial implementation; struct dirent has only
*       one field: d_name
*   Mar 15 2000  SAA added __cplusplus condition
*********************************************************************/

#ifndef __DIRENT_H
#define __DIRENT_H

#include <stdlib.h>	/* _MAX_FNAME */

#ifdef __cplusplus
extern "C" {
#endif

struct dirent
{
    char    d_name[_MAX_FNAME];
};

struct _DIR;
typedef struct _DIR DIR;

DIR* opendir(const char* name);
struct dirent* readdir(DIR* dir);
int closedir(DIR* dir);

#ifdef __cplusplus
}
#endif

#endif /* __DIRENT_H */
