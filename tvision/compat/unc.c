/*
  Copyright (c) 2000 by Anatoli Soltan.
  Covered by the GPL license.
*/
#include <cl/needs.h>

#ifdef NEEDS_UNC
#define WIN32_LEAN_AND_MEAN
/* UNICODE must be defined because all Net APIs are UNICODE */
#define UNICODE
#include <windows.h>
#include <lmcons.h>
#include <lmshare.h>
#include <lmapibuf.h>
#include <malloc.h>

#define is_slash(c) (c == '/' || c == '\\')
#define is_term(c) (c == '/' || c == '\\' || c == '\0')

int CLY_IsUNC(const char* path)
{
  if (!is_slash(path[0]) || !is_slash(path[1]) || is_term(path[2]))
    return 0;
  return 1;
}

int CLY_IsUNCShare(const char* path)
{
  char* path2 = alloca(strlen(path) + 1);
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
  if (!p || p[1] == '\0')
  {
    SHARE_INFO_1* pshi;
    NET_API_STATUS status;
    int serverlen, sharelen;
    WCHAR *wserver, *wshare;
    int ret;

    if (p) *p = '\0';
    serverlen = strlen(server);
    sharelen = strlen(share);
    wserver = (WCHAR*)alloca((serverlen + 1) * sizeof(WCHAR));
    wshare = (WCHAR*)alloca((sharelen + 1) * sizeof(WCHAR));

    MultiByteToWideChar(CP_ACP, 0, server, serverlen, wserver, serverlen);
    MultiByteToWideChar(CP_ACP, 0, share, sharelen, wshare, sharelen);
    wserver[serverlen] = 0;
    wshare[sharelen] = 0;

    status = NetShareGetInfo(wserver, wshare, 1, (LPBYTE*)&pshi);
    ret = (status == 0) && (pshi->shi1_type == STYPE_DISKTREE);
    NetApiBufferFree(pshi);
    return ret;
  }

  return 0;
}
#endif // NEEDS_UNC

/* vi: set ts=8 sw=2 : */

