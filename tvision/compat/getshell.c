/*
 Copyright (C) 2001      Salvador E. Tropea
 Covered by the GPL license.
*/

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
