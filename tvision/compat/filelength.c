/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/
#include <cl/needs.h>

#ifdef NEEDS_FILELENGTH
#include <unistd.h>

long CLY_filelength(int fhandle)
{
 long oldval;
 long retval;

 oldval=lseek(fhandle,0,SEEK_CUR);
 if (oldval==-1L) return -1L;
 retval=lseek(fhandle,0,SEEK_END);
 if (retval==-1L) return -1L;
 if (lseek(fhandle,oldval,SEEK_SET)==-1L) return -1L;
 return retval;
}
#endif
