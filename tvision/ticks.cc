/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__

#include <sys/farptr.h>
#include <go32.h>

unsigned short TICKS(void)
{
  return _farpeekw(_dos_ds,0x46c);
}
#endif // DJGPP



#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h> //for GetTickCount

unsigned short TICKS(void)
{
  //  X ms * 1s/1000ms * 18.2ticks/s = X/55 ticks, roughly.
  return GetTickCount() / 55;
}

#endif // Win32



#if !defined(__DJGPP__) && !defined(_WIN32)

#include <sys/time.h>

unsigned short TICKS(void)
{
  struct timeval val;
  gettimeofday(&val,(struct timezone *)NULL);
  return (val.tv_sec*18 + (val.tv_usec*18)/1000000);
//  return clock();
}
#endif

