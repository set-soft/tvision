/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <cl/needs.h>

#ifdef NEEDS_GLOB
#define Uses_stdlib
#define Uses_glob
#define Uses_free
#include <compatlayer.h>

void globfree(glob_t *_pglob)
{
  size_t i;
  if (!_pglob->gl_pathv)
    return;
  for (i=0; i<_pglob->gl_pathc; i++)
    if (_pglob->gl_pathv[i])
      free(_pglob->gl_pathv[i]);
  free(_pglob->gl_pathv);
}
#endif // NEEDS_GLOB
