/*
 Copyright (C) 2001,2002      Salvador E. Tropea
 Covered by the GPL license.
*/
#define Uses_stdlib
#include <compatlayer.h>

#if (defined(TVOS_DOS) || defined(TVOS_Win32)) && !defined(TVCompf_Cygwin)
CLY_EXPORT char *CLY_GetShellName(void)
{
 return getenv("COMSPEC");
}
#endif

#if defined(TVOS_UNIX) || defined(TVCompf_Cygwin)
char *CLY_GetShellName(void)
{
 return getenv("SHELL");
}
#endif
