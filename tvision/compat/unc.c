/*
  Copyright (c) 2000 by Anatoli Soltan.
  Covered by the GPL license.
*/
#include <cl/needs.h>

#ifdef NEEDS_UNC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>

#if (__MINGW32_MAJOR_VERSION>=2) && !defined(alloca)
 #define alloca __builtin_alloca
#endif

int CLY_isUNC_helper_NT(const char* server, const char* share);
int CLY_isUNC_helper_95(const char* server, const char* share);

#define is_slash(c) (c == '/' || c == '\\')
#define is_term(c) (c == '/' || c == '\\' || c == '\0')

/**[txh]********************************************************************

  Description:
  Returns 1 if passed path looks like an UNC name. That is it starts with
//server.
  
  Return: 1 if UNC
  
***************************************************************************/

int CLY_IsUNC(const char* path)
{
  if (!is_slash(path[0]) || !is_slash(path[1]) || is_term(path[2]))
    return 0;
  return 1;
}

/**[txh]********************************************************************

  Description:
  Returns 1 if passed path refers to a network share. That is the path
matches //server/share[/[.]] mask and the share really exists.
  
  Return: 1 if that's an existing share.
  
***************************************************************************/

int CLY_IsUNCShare(const char* path)
{
  static int nt = -1;
  char* path2 = (char*)alloca(strlen(path) + 1);
  char* p;
  const char *server, *share;

  strcpy(path2, path);

  if (!CLY_IsUNC(path2))
    return 0;
  server = path2 + 2;
  p = strchr(server, '/');
  if (!p)
    p = strchr(server, '\\');
  if (!p)
    return 0;
  if (p[1] == '\0')
    return 0;
  *p = '\0';

  share = p + 1;
  p = strchr(share, '/');
  if (!p)
    p = strchr(share, '\\');
  if (!p || p[1] == '\0' || (p[1] == '.' && p[2] == '\0'))
  {
    int ret;
    if (p) *p = '\0';
    if (nt == -1 || nt == 1)
    {
      ret = CLY_isUNC_helper_NT(server, share);
      nt = !(ret == -1);
    }
    if (nt == 0)
    {
      ret = CLY_isUNC_helper_95(server, share);
      if (ret == -1) nt = -1, ret = 0;
    }
    return ret;
  }

  return 0;
}
#endif // NEEDS_UNC

/* vi: set ts=8 sw=2 : */

