/* Copyright (C) 1998 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1997 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
/* Modified for Win32 by Anatoli Soltan */

#include <cl/needs.h>

#ifdef NEEDS_GLOB
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <ctype.h>
#ifndef TVComp_MSC
 #include <dir.h>
#endif

#define Uses_glob
#define Uses_unistd
#define Uses_fnmatch
#define Uses_alloca
#include <compatlayer.h>

typedef struct Save {
  struct Save *prev;
  char *entry;
} Save;

static Save *save_list;
static int save_count;
static int flags;
static int (*errfunc)(const char *epath, int eerno);
static char *pathbuf;
static int wildcard_nesting;
static char use_lfn;
static char preserve_case;
static char slash;

static int glob2(const char *pattern, char *epathbuf, int lower, int caseless);
static int glob_dirs(const char *rest, char *epathbuf, int first, int lower, int caseless);
static int add(const char *path);
static int str_compare(const void *va, const void *vb);

static int _preserve_fncase(void)
{
  return 1;
}

static int
add(const char *path)
{
  Save *sp;
  for (sp=save_list; sp; sp=sp->prev)
    if (stricmp(sp->entry, path) == 0)
      return 0;
  sp = (Save *)malloc(sizeof(Save));
  if (sp == 0)
    return 1;
  sp->entry = (char *)malloc(strlen(path)+1);
  if (sp->entry == 0)
  {
    free(sp);
    return 1;
  }
/*  printf("add: `%s'\n", path); */
  strcpy(sp->entry, path);
  sp->prev = save_list;
  save_list = sp;
  save_count++;
  return 0;
}

static int
glob_dirs(const char *rest, char *epathbuf, int first, /* rest is ptr to null or ptr after slash, bp after slash */
	  int lower, int caseless)
{
//  struct ffblk ff;
  HANDLE hf;
  WIN32_FIND_DATA wfd;
  BOOL done;

/*  printf("glob_dirs[%d]: rest=`%s' %c epathbuf=`%s' %c pathbuf=`%s'\n",
	 wildcard_nesting, rest, *rest, epathbuf, *epathbuf, pathbuf); */

  if (first)
  {
    if (*rest)
    {
      if (glob2(rest, epathbuf, lower, caseless) == GLOB_NOSPACE)
	return GLOB_NOSPACE;
    }
    else
    {
      char sl = epathbuf[-1];
      *epathbuf = 0;
/*      printf("end, checking `%s'\n", pathbuf); */
      if (epathbuf == pathbuf)
      {
	epathbuf[0] = '.';
	epathbuf[1] = 0;
      }
      else
	epathbuf[-1] = 0;
      if (CLY_FileExists(pathbuf))
	if (add(pathbuf))
	  return GLOB_NOSPACE;
      epathbuf[-1] = sl;
    }
  }

  strcpy(epathbuf, "*.*");
  hf = FindFirstFile(pathbuf, &wfd);
  done = (hf == INVALID_HANDLE_VALUE);
  while (!done)
  {
    if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	&& (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")))
    {
      char *tp;

      strcpy(epathbuf, wfd.cFileName);
      tp = epathbuf + strlen(epathbuf);
      *tp++ = slash;
      *tp = 0;

      wildcard_nesting++;
      if (*rest)
      {
	if (glob2(rest, tp, lower, caseless) == GLOB_NOSPACE)
	  return GLOB_NOSPACE;
      }
      else
      {
	if (!(flags & GLOB_MARK))
	  tp[-1] = 0;
	if (add(pathbuf))
	  return GLOB_NOSPACE;
	tp[-1] = slash;
      }
      *tp = 0;
      if (glob_dirs(rest, tp, 0, lower, caseless) == GLOB_NOSPACE)
	return GLOB_NOSPACE;
      wildcard_nesting--;
    }
    done = !FindNextFile(hf, &wfd);
  }
  FindClose(hf);
  return 0;
}

static int
glob2(const char *pattern, char *epathbuf,  /* both point *after* the slash */
      int lower, int caseless)
{
  const char *pp, *pslash;
  char *bp;
  HANDLE hf;
  WIN32_FIND_DATA wfd;
  char *my_pattern;
  int done;

  if (strcmp(pattern, "...") == 0)
  {
    return glob_dirs(pattern+3, epathbuf, 1, lower, caseless);
  }
  if (strncmp(pattern, "...", 3) == 0 && (pattern[3] == '\\' || pattern[3] == '/'))
  {
    slash = pattern[3];
    return glob_dirs(pattern+4, epathbuf, 1, lower, caseless);
  }

  *epathbuf = 0;
  /* copy as many non-wildcard segments as possible */
  pp = pattern;
  bp = epathbuf;
  pslash = bp-1;
  while (1)
  {
    if (*pp == ':' || *pp == '\\' || *pp == '/')
    {
      pslash = bp;
      if (strcmp(pp+1, "...") == 0
	  || (strncmp(pp+1, "...", 3) == 0 && (pp[4] == '/' || pp[4] == '\\')))
      {
	if (*pp != ':')
	  slash = *pp;
/*	printf("glob2: dots at `%s'\n", pp); */
	*bp++ = *pp++;
	break;
      }
    }

    else if (*pp == '*' || *pp == '?' || *pp == '[')
    {
      if (pslash > pathbuf)
	strncpy(epathbuf, pattern, pslash - pathbuf);
      pp = pattern + (pslash - epathbuf) + 1;
      bp = epathbuf + (pslash - epathbuf) + 1;
      break;
    }

    else if (*pp == 0)
    {
      break;
    }

    /* Upper-case or mixed-case patterns force case-sensitive
       matches in `fnmatch' for LFN filesystems.  They also
       suppress downcasing 8+3 filenames (on all filesystems).  */
    else if (!preserve_case)
    {
      if (*pp >= 'A' && *pp <= 'Z')
      {
	if (use_lfn)
	  caseless = 0;
	lower = 0;
      }
      else if (*pp >= 'a' && *pp <= 'z')
	lower = 1;
    }

    *bp++ = *pp++;
  }
  *bp = 0;

  if (*pp == 0) /* end of pattern? */
  {
    if (CLY_FileExists(pathbuf))
    {
      if (flags & GLOB_MARK)
      {
	HANDLE _hf;
	WIN32_FIND_DATA _wfd;
	_hf = FindFirstFile(pathbuf, &_wfd);
        //findfirst(pathbuf, &_ff, FA_RDONLY|FA_SYSTEM|FA_DIREC|FA_ARCH);
        if (_hf != INVALID_HANDLE_VALUE && _wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          char *_pathbuf = pathbuf + strlen(pathbuf);
          *_pathbuf++ = '/';
          *_pathbuf = 0;
        }
	FindClose(_hf);
      }
      if (add(pathbuf))
	return GLOB_NOSPACE;
    }
    return 0;
  }
/*  printf("glob2: initial segment is `%s'\n", pathbuf); */
  if (wildcard_nesting)
  {
    char s = bp[-1];
    bp[-1] = 0;
    if (!CLY_FileExists(pathbuf))
      return 0;
    bp[-1] = s;
  }

  for (pslash = pp; *pslash && *pslash != '\\' && *pslash != '/';  pslash++)
    if (!preserve_case)
    {
      if (*pslash >= 'A' && *pslash <= 'Z')
      {
	if (use_lfn)
	  caseless = 0;
	lower = 0;
      }
      else if (*pslash >= 'a' && *pslash <= 'z')
	lower = 1;
    }

  if (*pslash)
    slash = *pslash;
  my_pattern = (char *)alloca(pslash - pp + 1);
  if (my_pattern == 0)
    return GLOB_NOSPACE;
  strncpy(my_pattern, pp, pslash - pp);
  my_pattern[pslash-pp] = 0;

/*  printf("glob2: `%s' `%s'\n", pathbuf, my_pattern); */

  if (strcmp(my_pattern, "...") == 0)
  {
    if (glob_dirs(*pslash ? pslash+1 : pslash, bp, 1, lower, caseless) == GLOB_NOSPACE)
      return GLOB_NOSPACE;
    return 0;
  }

  strcpy(bp, "*.*");

  hf = FindFirstFile(pathbuf, &wfd);
  done = (hf == INVALID_HANDLE_VALUE);
  //done = findfirst(pathbuf, &ff, FA_RDONLY|FA_SYSTEM|FA_DIREC|FA_ARCH);
  while (!done)
  {
    if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0
	|| (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")))
    {
      if (fnmatch(my_pattern, wfd.cFileName,
		  FNM_NOESCAPE|FNM_PATHNAME|(caseless ? FNM_NOCASE : 0)) == 0)
      {
	strcpy(bp, wfd.cFileName);
	if (*pslash)
	{
	  char *tp = bp + strlen(bp);
	  *tp++ = *pslash;
	  *tp = 0;
/*	  printf("nest: `%s' `%s'\n", pslash+1, pathbuf); */
	  wildcard_nesting++;
	  if (glob2(pslash+1, tp, lower, caseless) == GLOB_NOSPACE)
	    return GLOB_NOSPACE;
	  wildcard_nesting--;
	}
	else
	{
/*	  printf("ffmatch: `%s' matching `%s', add `%s'\n",
		 wfd.cFileName, my_pattern, pathbuf); */
	  if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && (flags & GLOB_MARK))
	  {
	    bp[strlen(bp)+1] = 0;
	    bp[strlen(bp)] = slash;
	  }
	  if (add(pathbuf))
	    return GLOB_NOSPACE;
	}
      }
    }
    done = !FindNextFile(hf, &wfd);
  } 

  return 0;
}

static int
str_compare(const void *va, const void *vb)
{
  return strcmp(*(const char * const *)va, *(const char * const *)vb);
}

int
glob(const char *_pattern, int _flags, int (*_errfunc)(const char *_epath, int _eerrno), glob_t *_pglob)
{
  char path_buffer[2000];
  int l_ofs, l_ptr;

  pathbuf = path_buffer+1;
  flags = _flags;
  errfunc = _errfunc;
  wildcard_nesting = 0;
  save_count = 0;
  save_list = 0;
  preserve_case = (char)_preserve_fncase();
  slash = '/';

  if (!(_flags & GLOB_APPEND)) 
    {
      _pglob->gl_pathc = 0;
      _pglob->gl_pathv = NULL;
      if (!(flags & GLOB_DOOFFS))
        _pglob->gl_offs = 0;
    }
  if (glob2(_pattern, pathbuf, preserve_case ? 0 : 1, /*preserve_case ? 0 :*/ 1) == GLOB_NOSPACE)
    {
      return GLOB_NOSPACE;
    }

  if (save_count == 0)
  {
    if (flags & GLOB_NOCHECK)
    {
      if (add(_pattern))
	return GLOB_NOSPACE;
    }
    else
      return GLOB_NOMATCH;
  }

  if (flags & GLOB_DOOFFS)
    l_ofs = _pglob->gl_offs;
  else
    l_ofs = 0;

  if (flags & GLOB_APPEND)
  {
    _pglob->gl_pathv = (char **)realloc(_pglob->gl_pathv, (l_ofs + _pglob->gl_pathc + save_count + 1) * sizeof(char *));
    if (_pglob->gl_pathv == 0)
      return GLOB_NOSPACE;
    l_ptr = l_ofs + _pglob->gl_pathc;
  }
  else
  {
    _pglob->gl_pathv = (char* *)malloc((l_ofs + save_count + 1) * sizeof(char *));
    if (_pglob->gl_pathv == 0)
      return GLOB_NOSPACE;
    l_ptr = l_ofs;
    if (l_ofs)
      memset(_pglob->gl_pathv, 0, l_ofs * sizeof(char *));
  }

  l_ptr += save_count;
  _pglob->gl_pathv[l_ptr] = 0;
  while (save_list)
  {
    Save *s = save_list;
    l_ptr --;
    _pglob->gl_pathv[l_ptr] = save_list->entry;
    save_list = save_list->prev;
    free(s);
  }
  if (!(flags & GLOB_NOSORT))
    qsort(_pglob->gl_pathv + l_ptr, save_count, sizeof(char *), str_compare);

  _pglob->gl_pathc = l_ptr + save_count;

  return 0;
}
#endif // NEEDS_GLOB

