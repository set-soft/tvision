/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#if !defined( __TTYPES_H )
#define __TTYPES_H

#include <tv/configtv.h>

#ifdef TVComp_GCC
#undef Boolean
#undef False
#undef True
#define Boolean bool
#define True true
#define False false
#else
enum Boolean { False, True };
#endif

/* The following types should be platform independent */
typedef signed char int8;
typedef short       int16;
typedef int         int32;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
#if defined(TVComp_GCC)
typedef unsigned long long uint64;
typedef          long long int64;
#elif defined(TVComp_BCPP) || defined(TVComp_MSC)
typedef unsigned __int64 uint64;
typedef          __int64 int64;
#else
#error Can not define uint64 type: unknown compiler.
#endif

/* The following are just aliases and the size is platform dependant */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

const char EOS = '\0';

enum StreamableInit { streamableInit };

class ipstream;
class opstream;
class TStreamable;
class TStreamableTypes;

typedef int32 ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const int ccNotFound = -1;

extern uchar specialChars[];
extern uchar o_specialChars[];

#endif	// __TTYPES_H
