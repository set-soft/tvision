#ifndef __DIR_H
#define __DIR_H

#include <direct.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef MAXPATH
#define MAXPATH MAX_PATH
#endif 

#ifndef FA_NORMAL
#define FA_NORMAL   0x00        /* Normal file, no attributes */
#define FA_RDONLY   0x01        /* Read only attribute */
#define FA_HIDDEN   0x02        /* Hidden file */
#define FA_SYSTEM   0x04        /* System file */
#define FA_LABEL    0x08        /* Volume label */
#define FA_DIREC    0x10        /* Directory */
#define FA_ARCH     0x00        /* Archive */
#endif /* FA_NORMAL */

#ifndef _FFBLK_DEF
#define _FFBLK_DEF
struct  ffblk   {
    long            ff_reserved;
    long            ff_fsize;
    unsigned long   ff_attrib;
    unsigned short  ff_ftime;
    unsigned short  ff_fdate;
    char            ff_name[MAXPATH];
};
#endif

#define getdisk() (_getdrive() + 1)


#ifdef __cplusplus
extern "C" {
#endif

int findfirst( const char *__path,
               struct ffblk *__ffblk,
               int __attrib );
int findnext( struct ffblk *__ffblk );
int findclose( struct ffblk *__ffblk );

#ifdef __cplusplus
}
#endif


#endif /* __DIR_H */
