/*
 Copyright (C) 2001      Salvador E. Tropea
 Covered by the GPL license.
*/
#define Uses_stdlib
#define CLY_DoNotDefineUTypes
#include <compatlayer.h>

#if defined(TVOS_DOS) || defined(TVOS_Win32)
char *CLY_GetShellName(void)
{
 return getenv("COMSPEC");
}
#endif

#ifdef TVOS_UNIX
char *CLY_GetShellName(void)
{
 return getenv("SHELL");
}
#endif
