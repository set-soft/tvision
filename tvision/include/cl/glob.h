#ifndef __GLOB_H
#define __GLOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

typedef struct {
  size_t gl_pathc;
  char **gl_pathv;
  size_t gl_offs;
} glob_t;

#define GLOB_APPEND	0x01
#define	GLOB_DOOFFS	0x02
#define GLOB_ERR	0x04
#define GLOB_MARK	0x08
#define GLOB_NOCHECK	0x10
#define GLOB_NOESCAPE	0x20
#define GLOB_NOSORT	0x40

#define GLOB_ABORTED	1
#define GLOB_NOMATCH	2
#define GLOB_NOSPACE	3

int  glob(const char *_pattern, int _flags, int (*_errfunc)(const char *_epath, int _eerrno), glob_t *_pglob);
void globfree(glob_t *_pglob);

#ifdef __cplusplus
}
#endif

#endif /* __GLOB_H */
