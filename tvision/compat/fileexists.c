/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000-2001 Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_access
#include <compatlayer.h>

int CLY_FileExists(const char *fname)
{
 return (access(fname,R_OK)==0);
}
