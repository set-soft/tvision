/*
  Copyright (c) 2000 by Anatoli Soltan.
  Covered by the GPL license.
*/
#include <cl/needs.h>

#ifdef NEEDS_UNC
#define WIN32_LEAN_AND_MEAN
/* UNICODE must be defined because all Net APIs on WinNT are UNICODE */
#define UNICODE
#include <windows.h>
#include <lmcons.h>
#include <lmshare.h>
#include <lmapibuf.h>
#include <malloc.h>

#if (__MINGW32_MAJOR_VERSION>=2) && !defined(alloca)
 #define alloca __builtin_alloca
#endif

static HMODULE hLib;
static NET_API_STATUS
(NET_API_FUNCTION *pNetShareGetInfo) (
    IN  LPTSTR  servername,
    IN  LPTSTR  netname,
    IN  DWORD   level,
    OUT LPBYTE  *bufptr
    );
static NET_API_STATUS
(NET_API_FUNCTION *pNetApiBufferFree) (
    IN LPVOID Buffer
    );

/**[txh]********************************************************************

  Description:
  Internal helper to validate an UNC for Win NT (2k, XP, etc.).
  
  Return: 1 if that's an existing share.
  
***************************************************************************/

int CLY_isUNC_helper_NT(const char* server, const char* share)
{
    SHARE_INFO_1* pshi;
    NET_API_STATUS status;
    int serverlen, sharelen;
    WCHAR *wserver, *wshare;
    int ret;
    
    if (!hLib)
    {
        hLib = LoadLibraryA("NETAPI32.DLL");
        if (!hLib)
        {
            ret = -1;
            goto clean;
        }
    }
    if (!pNetShareGetInfo)
    {
        pNetShareGetInfo = (void*)GetProcAddress(hLib, "NetShareGetInfo");
        if (!pNetShareGetInfo)
        {
            ret = -1;
            goto clean;
        }
    }
    if (!pNetApiBufferFree)
    {
        pNetApiBufferFree = (void*)GetProcAddress(hLib, "NetApiBufferFree");
        if (!pNetApiBufferFree)
        {
            ret = -1;
            goto clean;
        }
    }

    serverlen = strlen(server);
    sharelen = strlen(share);
    wserver = (WCHAR*)alloca((serverlen + 1) * sizeof(WCHAR));
    wshare = (WCHAR*)alloca((sharelen + 1) * sizeof(WCHAR));

    MultiByteToWideChar(CP_ACP, 0, server, serverlen, wserver, serverlen);
    MultiByteToWideChar(CP_ACP, 0, share, sharelen, wshare, sharelen);
    wserver[serverlen] = 0;
    wshare[sharelen] = 0;

    status = (*pNetShareGetInfo)(wserver, wshare, 1, (LPBYTE*)&pshi);
    if (status == ERROR_CALL_NOT_IMPLEMENTED)
    {
        ret = -1;
        goto clean;
    }
    ret = (status == 0) && (pshi->shi1_type == STYPE_DISKTREE);
    (*pNetApiBufferFree)(pshi);
    return ret;
    
clean:
    if (hLib)
    {
        FreeLibrary(hLib);
        hLib = NULL;
        pNetShareGetInfo  = NULL;
        pNetApiBufferFree = NULL;
    }
    return ret;
}
#endif
