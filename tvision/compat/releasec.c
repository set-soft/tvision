/* Copyright (C) 1996-1998 Robert H”hne */
/* Modified by Salvador E. Tropea, Vadim Bolodorov and Anatoli Soltan */
#include <tv/configtv.h>

#ifdef TVCompf_djgpp
#include <dpmi.h>

void CLY_ReleaseCPU()
{
 __dpmi_yield(); // Release the time slice
}
#endif // DJGPP


#ifdef TVOS_Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// I don't know if the following difference could be unified:
#ifdef TVOSf_NT
// SET: Anatoli's port for NT releases the CPU until the console received
// some message.
extern void __tvWin32Yield(int micros);

void CLY_ReleaseCPU()
{
 __tvWin32Yield(-1);
}
#else
// By Vadim Beloborodov to be used on WIN32 console
// SET: Vadim's port just waits 0.1 seconds. I think that's because he uses
// another thread for input.
void CLY_ReleaseCPU()
{
 Sleep(100);
}
#endif

#endif // Win32



#ifdef TVOS_UNIX
#include <unistd.h>

void CLY_ReleaseCPU()
{
 usleep(1000);   // Linux, release 1 ms
}
#endif
