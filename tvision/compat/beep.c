/*
 Copyright (C) 2000      Salvador E. Tropea
 Copyright (C) 2000      Anatoli Soltan
 Covered by the GPL license.
*/

#include <compatlayer.h>

#ifdef TVOSf_djgpp
#include <pc.h>
#include <dos.h>

void CLY_Beep(void)
{
 sound(1000);
 delay(100);
 nosound();
}
#define BEEP_DEFINED
#endif

#ifdef TVOS_Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void CLY_Beep(void)
{
 MessageBeep(0xFFFFFFFF);
}
#define BEEP_DEFINED
#endif

#ifndef BEEP_DEFINED
void CLY_Beep(void)
{
}
#endif
