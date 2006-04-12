/*
  Copyright (c) 2000 by Anatoli Soltan.
  Covered by the GPL license.
*/
#include <cl/needs.h>

#ifdef NEEDS_UNC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <svrapi.h>
#include <malloc.h>
#include <string.h>

#if (__MINGW32_MAJOR_VERSION>=2) && !defined(alloca)
 #define alloca __builtin_alloca
#endif

#if _MSC_VER >= 1400
 #undef  strupr
 #define strupr(a)   _strupr(a)
#endif

static HMODULE hLib;
static API_RET_TYPE 
(APIENTRY *pNetShareGetInfo) (
    const char FAR *     pszServer,
    const char FAR *     pszNetName,
    short                sLevel,
    char FAR *           pbBuffer,
    unsigned short       cbBuffer,
    unsigned short FAR * pcbTotalAvail
    );

/**[txh]********************************************************************

  Description:
  Internal helper to validate an UNC for Win9x/Me.
  
  Return: 1 if that's an existing share.
  
***************************************************************************/

int CLY_isUNC_helper_95(const char* server, const char* share)
{
    struct share_info_1*  pshi1;
    struct share_info_50* pshi50;
    NET_API_STATUS status;
    short level;
    unsigned short cbTotalAvail;
    int ret;
    char *userver, *ushare;

    /*
     * Win95 wants server name to be prefixed by two slashes and share
     * name to be uppercased.
     */
    userver = alloca(strlen(server) + 3);
    ushare = alloca(strlen(share) + 1);
    userver[0] = '/';
    userver[1] = '/';
    strcpy(userver + 2, server);
    strcpy(ushare, share);
    strupr(ushare);
    
    if (!hLib)
    {
        hLib = LoadLibrary(TEXT("SVRAPI.DLL"));
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

    level = 50;
    status = (*pNetShareGetInfo)(userver, ushare, level, NULL, 0, &cbTotalAvail);
    if (status == ERROR_INVALID_LEVEL)
    {
        level = 1;
        status = (*pNetShareGetInfo)(userver, ushare, level, NULL, 0, &cbTotalAvail);
    }
    if (status != NERR_BufTooSmall)
        return 0;

    pshi1 = (struct share_info_1 *)malloc(cbTotalAvail);
    pshi50 = (struct share_info_50 *)pshi1;
    status = (*pNetShareGetInfo)(userver, ushare, level, (char*)pshi1, cbTotalAvail, &cbTotalAvail);
    ret = (status == 0) &&
        ((level == 1 ? pshi1->shi1_type : pshi50->shi50_type) == STYPE_DISKTREE);
    free(pshi1);
    return ret;
    
clean:
    if (hLib)
    {
        FreeLibrary(hLib);
        hLib = NULL;
        pNetShareGetInfo = NULL;
    }
    return ret;
}
#endif // NEEDS_UNC

