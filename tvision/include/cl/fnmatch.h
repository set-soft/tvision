#ifndef __FNMATCH_H
#define __FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define FNM_NOESCAPE	0x01
#define FNM_PATHNAME	0x02
#define FNM_PERIOD	0x04
#define FNM_NOCASE	0x08

#define FNM_NOMATCH	1
#define FNM_ERROR	2

int fnmatch(const char *_pattern, const char *_string, int _flags);

#ifdef __cplusplus
}
#endif

#endif /* __FNMATCH_H */
