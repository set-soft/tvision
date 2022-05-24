/*

  This file isn't based in original TV file anymore.
  It was originally based on it and modified by Robert H”hne to be used for
RHIDE. But currently it was rethinked from the start by Salvador E. Tropea.
  Copyright (c) 2000-2003 by Salvador E. Tropea. Covered by the GPL license.
  Most of the OS dependent code was moved to the "Compat LaYer" library.

*/

#if !defined( CONFIG_H_INCLUDED )
#define CONFIG_H_INCLUDED

// The result of sizeof is a size_t which is defined here.
// So I think is better to include it.
#include <stddef.h>
#include <sys/types.h>
#include <limits.h>

const int eventQSize = 16;
const int maxCollectionSize = UINT_MAX/sizeof( void * );

// May be I'll remove that sometimes
#ifdef TVOS_DOS
const int maxViewWidth = 132;
#else // at least on linux in a xterm it can be more than 132
const int maxViewWidth = 1024;
#endif

const int maxFindStrLen    = 80;
const int maxReplaceStrLen = 80;

#include <tv/intl.h>

#ifndef pid_t
# define pid_t int
#endif

#endif // CONFIG_H_INCLUDED
