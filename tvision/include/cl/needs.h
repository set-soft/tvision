/**[txh]********************************************************************

  Description:
  This header defines which functions must be defined for each supported
compiler.@p

  Copyright (c) 2000-2001 by Salvador E. Tropea
  Covered by the GPL license.

***************************************************************************/

#include <tv/configtv.h>

#ifdef __GNUC__
 // GNU c is supported for various OSs:
 
 #define NEEDS_GETCURDIR
 
 // Under Win32 MingW defines it in specs
 #ifdef __MINGW32__
  #define NEEDS_FIXPATH
  #define NEEDS_GLOB
  #define NEEDS_FNMATCH
  #define NEEDS_REGEX
  #define NEEDS_GETOPT
  #define NEEDS_MKSTEMP
 #endif
 
 // Under DOS djgpp defines it
 #ifdef __DJGPP__
  #define NEEDS_GETOPT
 #endif
 
 // Under Linux defines it
 #ifdef __linux__
  // Strings
  #define NEEDS_STRLWR
  #define NEEDS_STRUPR
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_ITOA
 #endif
 
 // Generic UNIX system
 #if defined(TVOS_UNIX) && !defined(TVOSf_Linux)
  // Strings
  #define NEEDS_STRLWR
  #define NEEDS_STRUPR
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_ITOA
 #endif
#endif


// BC++ 5.5 for Win32 is supported
#ifdef __TURBOC__
 #define NEEDS_FIXPATH
 #define NEEDS_GLOB
 #define NEEDS_FNMATCH
 #define NEEDS_UNC
 #define NEEDS_REGEX
 #define NEEDS_GETOPT
 #define NEEDS_OPENDIR
 #define NEEDS_MKSTEMP
#endif


// MSVC will be supported if volunteers tests it or Microsoft decides to
// give it for free ;-). After all Borland released BC++ 5.5.
#if (defined(_MSVC) || defined(__MSC_VER)) && !defined(_MSC_VER)
#define _MSC_VER
#endif

#ifdef _MSC_VER
 #define NEEDS_GETCURDIR
 #define NEEDS_FIXPATH
 #define NEEDS_GLOB
 #define NEEDS_FNMATCH
 #define NEEDS_REGEX
 #define NEEDS_GETOPT
 #define NEEDS_MKSTEMP
#endif
