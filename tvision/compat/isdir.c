/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_sys_stat
#include <compatlayer.h>

int CLY_IsDir(const char *str)
{
 struct stat s;
 
 if (CLY_IsUNCShare(str))
    return 1;
 if (stat(str,&s)==0 && S_ISDIR(s.st_mode))
    return 1;
 return 0;
}
