/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__

#include <sys/farptr.h>
#include <go32.h>

unsigned short TICKS(void)
{
  return _farpeekw(_dos_ds,0x46c);
}

#else

#include <sys/time.h>

unsigned short TICKS(void)
{
  struct timeval val;
  gettimeofday(&val,(struct timezone *)NULL);
  return (val.tv_sec*18 + (val.tv_usec*18)/1000000);
//  return clock();
}

#endif

