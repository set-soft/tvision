/**[txh]********************************************************************

  Description:
  This header defines which functions must be defined for each supported
compiler.@p

  Copyright (c) 2000-2001 by Salvador E. Tropea
  Covered by the GPL license.

***************************************************************************/

#include <tv/configtv.h>

#ifdef TVComp_GCC
 // GNU c is supported for various OSs:
 
 #define NEEDS_GETCURDIR
 
 // Win32 MinGW
 #ifdef TVCompf_MinGW
  #define NEEDS_FIXPATH
  #define NEEDS_GLOB
  #define NEEDS_FNMATCH
  #define NEEDS_REGEX
  #define NEEDS_GETOPT
  #define NEEDS_MKSTEMP
  #define NEEDS_NL_LANGINFO
  #define NEEDS_GETLINE
 #endif
 
 // Win32 Cygwin
 #ifdef TVCompf_Cygwin
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_NL_LANGINFO
  #define NEEDS_FNMATCH
  #define NEEDS_ITOA
  #define NEEDS_GETLINE
  #define NEEDS_SNPRINTF
 #endif
 
 // DOS djgpp
 #ifdef TVCompf_djgpp
  #define NEEDS_GETOPT
  #define NEEDS_NL_LANGINFO
  #define NEEDS_GETLINE
  #define NEEDS_SNPRINTF
 #endif
 
 // Linux
 #ifdef TVOSf_Linux
  // Strings
  #define NEEDS_STRLWR
  #define NEEDS_STRUPR
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_ITOA
 #endif
 
 // Solaris
 #ifdef TVOSf_Solaris
  // Strings
  #define NEEDS_STRLWR
  #define NEEDS_STRUPR
  #define NEEDS_GETOPT
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_ITOA
  #define NEEDS_NL_LANGINFO
  #define NEEDS_GETLINE
 #endif

 // Generic UNIX system
 #if defined(TVOS_UNIX) && !defined(TVOSf_Linux) && !defined(TVOSf_Solaris)
  // Strings
  #define NEEDS_STRLWR
  #define NEEDS_STRUPR
  #define NEEDS_FIXPATH
  #define NEEDS_FILELENGTH
  #define NEEDS_ITOA
  #define NEEDS_NL_LANGINFO
  #define NEEDS_GETLINE
  #if defined(TVOSf_FreeBSD) || defined(TVOSf_QNXRtP)
   #define NEEDS_GETOPT
  #endif
 #endif
#endif


// BC++ 5.5 for Win32 is supported
#ifdef TVComp_BCPP
 #define NEEDS_FIXPATH
 #define NEEDS_GLOB
 #define NEEDS_FNMATCH
 #define NEEDS_UNC
 #define NEEDS_REGEX
 #define NEEDS_GETOPT
 #define NEEDS_OPENDIR
 #define NEEDS_MKSTEMP
 #define NEEDS_NL_LANGINFO
 #define NEEDS_IFSTREAMGETLINE
 #define NEEDS_GETLINE
#endif


// MSVC will be supported if volunteers tests it or Microsoft decides to
// give it for free ;-). After all Borland released BC++ 5.5.
#if (defined(_MSVC) || defined(__MSC_VER)) && !defined(_MSC_VER)
#define _MSC_VER
#endif

#ifdef TVComp_MSC
 #define NEEDS_GETCURDIR
 #define NEEDS_FIXPATH
 #define NEEDS_GLOB
 #define NEEDS_FNMATCH
 #define NEEDS_REGEX
 #define NEEDS_GETOPT
 #define NEEDS_MKSTEMP
 #define NEEDS_NL_LANGINFO
 #define NEEDS_GETLINE
 #define NEEDS_OPENDIR
#endif

