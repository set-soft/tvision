/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Anatoli Soltan
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#include <cl/needs.h>

#ifdef TVCompf_djgpp
#define Uses_ctype
#include <compatlayer.h>
#include <dpmi.h>

int CLY_DriveValid( char drive )
{
 __dpmi_regs r;
 r.x.ax = 0x1C00;
 r.x.dx = uctoupper(drive) - 'A' + 1;
 __dpmi_int(0x21,&r);
 if (r.h.al != 0xFF) return 1;
 return 0;
}
#endif

#ifdef TVOS_UNIX
int CLY_DriveValid( char drive )
{
 return 0;
}
#endif

#ifdef TVOS_Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define Uses_ctype
#include <compatlayer.h>

int CLY_DriveValid( char drive )
{
 DWORD drives = GetLogicalDrives();
 return 1 & (drives >> (uctoupper(drive) - 'A'));
}
#endif
