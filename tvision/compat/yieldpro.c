#include <tv/configtv.h>

#ifdef TVOSf_djgpp
#include <dpmi.h>
#include <unistd.h>

void CLY_YieldProcessor(int micros)
{
 __dpmi_yield();
 if (micros>0)
    usleep(micros);
}
#endif

#ifdef TVOS_UNIX
#include <unistd.h>

void CLY_YieldProcessor(int micros)
{
 if (micros<0)
    micros=10;
 usleep(micros);
}
#endif

#ifdef TVOSf_NT
void CLY_YieldProcessor(int micros)
{
 extern void __tvWin32Yield(int micros);
 if (micros<0)
    micros=27472; // 1000000 / (18.2 * 2)
 __tvWin32Yield(micros);
}
#endif

#if defined(TVOS_Win32) && !defined(TVOSf_NT)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void CLY_YieldProcessor(int micros)
{
 Sleep(micros/1000);
}
#endif
