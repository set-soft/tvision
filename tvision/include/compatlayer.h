/**[txh]********************************************************************

  Description:
  This header works as a distribution point. It parses the Uses_*
definitions and includes (or defines) the needed stuff.@p
  Important guidelines:@*

  Never include string.h use Use_string instead.
  strlwr and strupr are defined.@*
  stricmp and strnicmp are deprecated, use strcasecmp and strncasecmp
instead.@*
  Never use ios::bin (CLY_IOSBin).
  Never use filebuf::openprot (CLY_FBOpenProtDef).
  The O_TEXT and O_BINARY from fcntl needs Uses_fcntl.
  S_IS* from sys/stat needs Uses_sys_stat.
  Never use s/random, use s/rand which are ANSI/POSIX.
  Streambuf members: setbuf, seekoff and sync have wrappers requested with
Uses_PubStreamBuf.
  Request alloca with Uses_alloca.
  Never use movmem, use memmove instead (ANSI/POSIX).
  Even when supported the old calls to fexpand, isDir, pathValid,
validFileName, getCurDir, isWild, __file_exists, relativePath and driveValid
should be replaced by a CLY_* function.
  When possible POSIX compliant replacements are supplied for: glob, fnmatch
and regex.
  Added itoa for faster and safe integer to string conversion.

  Copyright (c) 2000-2005 by Salvador E. Tropea
  Covered by the GPL license.

***************************************************************************/

/* A global protection isn't suitable for all projects. Collisions should
   be solved individually.
#ifndef CLY_CompatLayerIncluded
#define CLY_CompatLayerIncluded
*/

#include <tv/configtv.h>

#ifdef Uses_fstream_simple
 #ifdef HAVE_SSC
  #define Uses_SSC_Streams 1
 #else
  #define Uses_fstream
 #endif
#endif

#ifdef Uses_iostream_simple
 #ifdef HAVE_SSC
  #define Uses_SSC_Streams 1
 #else
  #define Uses_iostream
 #endif
#endif

#ifdef Uses_strstream_simple
 #ifdef HAVE_SSC
  #define Uses_SSC_Streams 1
 #else
  #define Uses_strstream
 #endif
#endif

#ifdef Uses_SSC_Streams
 #define Uses_stdio
#endif

#ifdef __cplusplus
 #define CLY_CFunc extern "C"
#else
 #define CLY_CFunc extern
#endif

#if !defined(CLY_DoNotDefineSizedTypes) && !defined(CLY_SizedTypesDefined)
#define CLY_SizedTypesDefined 1
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
#elif defined(TVComp_BCPP) || defined(TVComp_MSC) || defined (TVComp_Watcom)
typedef unsigned __int64 uint64;
typedef          __int64 int64;
#else
 #error Can not define uint64 type: unknown compiler.
#endif
#endif /* CLY_DoNotDefineSizedTypes */

#if defined(HAVE_64BITS_POINTERS) || (defined(TVComp_MSC) && defined(_WIN64))
typedef uint64 uipointer;
#else
typedef uint32 uipointer;
#endif
/* Macros to cast a pointer into a 64 bits unsigned and viceversa */
#undef  CLY_PointerToUI64
#undef  CLY_UI64ToPointer
#define CLY_UI64ToPointer(a) ((void *)(uipointer)(a))
#define CLY_PointerToUI64(a) ((uint64)(uipointer)(a))

#if defined(CLY_DefineUTypes) || defined(__cplusplus)
/* The following are just aliases and the size is platform dependant */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
#endif /* CLY_DefineUTypes */


#ifdef Uses_stdio
  #undef  Include_stdio
  #define Include_stdio 1
#endif

#ifdef Uses_stdarg
  #undef  Include_stdarg
  #define Include_stdarg 1
#endif

#ifdef Uses_CLYFileAttrs
 /* For mode_t */
 #undef  Uses_sys_stat
 #define Uses_sys_stat 1
#endif

#undef  FSTREAM_HEADER
#define FSTREAM_HEADER   <fstream.h>
#undef  STRSTREAM_HEADER
#define STRSTREAM_HEADER <strstream.h>
#undef  IOMANIP_HEADER
#define IOMANIP_HEADER   <iomanip.h>
#undef  IOSTREAM_HEADER
#define IOSTREAM_HEADER  <iostream.h>
#undef  IOCTL_HEADER
#define IOCTL_HEADER     <sys/ioctl.h>
#undef  NEW_HEADER
#define NEW_HEADER       <new>

#ifdef Uses_ioctl
 #undef  Include_ioctl
 #define Include_ioctl 1
#endif

#ifdef Uses_time
 #undef  Include_time
 #define Include_time 1
#endif

#ifdef Uses_signal
 #undef  Include_signal
 #define Include_signal 1
#endif

/* The following are unconditionally defined for all platforms so here I avoid
   redefinitions. */
#undef CLY_Packed
#undef NEVER_RETURNS
#undef RETURN_WHEN_NEVER_RETURNS
#undef DeleteArray
#undef IfStreamGetLine
#undef CLY_UseCrLf
#undef CLY_Have_UGID
#undef CLY_HaveDriveLetters
#undef CLY_IsValidDirSep
#undef PATHSEPARATOR
#undef PATHSEPARATOR_
#undef DIRSEPARATOR
#undef DIRSEPARATOR_
#undef CLY_ISOCpp98
#undef CLY_filebuf
#undef CLY_strstreambuf
#undef CLY_int_filebuf
#undef CLY_streambuf
#undef CLY_OpenModeT
#undef CLY_StreamPosT
#undef CLY_StreamOffT
#undef CLY_IOSSeekDir
#undef CLY_FBOpenProtDef
#undef CLY_NewFBFromFD
#undef CLY_PubSetBuf
#undef CLY_HaveFBAttach
#undef CLY_FBOpen
#undef CLY_IOSBin
#undef CLY_IOSOut
#undef CLY_IOSIn
#undef CLY_IOSApp
#undef CLY_IOSAtE
#undef CLY_IOSBeg
#undef CLY_IOSCur
#undef CLY_IOSEnd
#undef CLY_IOSBadBit
#undef CLY_IOSEOFBit
#undef CLY_IOSFailBit
#undef CLY_IOSGoodBit
#undef CLY_PubSeekOff
#undef CLY_PubSync
#undef CLY_std
#undef CreateStrStream
#undef GetStrStream
#undef UsingNamespaceStd
#undef CLY_HiddenDifferent
#undef CLY_DummyTStreamRW
#undef CLY_Have_snprintf
#undef CLY_destroy
#undef CLY_DONT_DEFINE_MIN_MAX
#undef CLY_Redraw
#undef CLY_SAFE_MEMCPY
#undef CLY_EXPORT
#undef CLY_BROKEN_WATCOM_SCOPE

#define CLY_BROKEN_WATCOM_SCOPE protected

#ifdef HAVE_UNSAFE_MEMCPY
 #define CLY_SAFE_MEMCPY 0
#else
 /* All known implementations supports overlap when copying forwards */
 /* But it isn't portable:

     The `memcpy' function copies SIZE bytes from the object beginning
     at FROM into the object beginning at TO.  The behavior of this
     function is undefined if the two arrays TO and FROM overlap; use
     `memmove' instead if overlapping is possible.

    Currently we assume that's safe, but any platform where it isn't can change it.
 */
 #define CLY_SAFE_MEMCPY 1
#endif

/* Most targets doesn't define destroy, but BC++ 5.6 defines
   _stl::destroy and looks like it collides with any destroy not defined
   inside another namespace. For this reason portable code should use
   CLY_destroy and not destroy. To allow compiling old applications using
   "clean" targets CLY_destroy is defined as destroy (then undefined for
   BC++ 5.6) */
#define CLY_destroy destroy

/* Assume it as default */
#define CLY_Have_snprintf 1

/* Most C++ compilers doesn't need it, only MSVC seems to need *all* the
   virtual members (even when not used) to link */
#define CLY_DummyTStreamRW(cla)

/* Watcom C++ used for QNX4 doesn't make any difference between Redraw
   and redraw members. */
#define CLY_Redraw Redraw

/* The default is CLY_EXPORT expanded to nothing. */
#define CLY_EXPORT

#ifdef TVComp_GCC
/* GNU C is supported for various OSs: */

 #ifdef Uses_chdir
  #undef  Uses_unistd
  #define Uses_unistd
 #endif
 #ifdef Uses_string
  #undef  Include_string
  #define Include_string 1
 #endif
 #ifdef Uses_limits
  #undef  Include_limits
  #define Include_limits 1
 #endif
 #ifdef Uses_fcntl
  #undef  Include_fcntl
  #define Include_fcntl 1
 #endif
 #ifdef Uses_sys_stat
  #undef  Include_sys_stat
  #define Include_sys_stat 1
 #endif
 #ifdef Uses_ctype
  #undef  Include_ctype
  #define Include_ctype 1
 #endif
 #ifdef Uses_unistd
  #undef  Include_unistd
  #define Include_unistd 1
 #endif
 #ifdef Uses_access
  #undef  Include_unistd
  #define Include_unistd 1
 #endif
 #ifdef Uses_alloca
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_free
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_stdlib
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_dirent
  #undef  Include_dirent
  #define Include_dirent 1
 #endif
 #ifdef Uses_abort
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_AllocLocal
  #undef  AllocLocalStr
  #define AllocLocalStr(s,l) char s[l]
  #undef  AllocLocalUShort
  #define AllocLocalUShort(s,l) ushort s[l]
 #endif
 #define NEVER_RETURNS __attribute__((noreturn))
 #define RETURN_WHEN_NEVER_RETURNS
 #define CLY_Packed __attribute__((packed))
 /* SET: Anything allocated with new[] should be deleted with [].
    Pointed out by Laurynas Biveinis. */
 #define DeleteArray(a) delete[] a
 #ifdef Uses_getcurdir
  #undef  getcurdir
  #define getcurdir CLY_getcurdir
 #endif
 /* Not yet supported for Win9x with Ming
    SET: I guess they can be defined for Win9x, but Anatoli's code seems to
    be using NETAPI32.DLL functions that only exists under NT. */
 #undef  CLY_IsUNCShare
 #define CLY_IsUNCShare(a) (0)
 #undef  CLY_IsUNC
 #define CLY_IsUNC(a)      (0)
 #ifdef Uses_ftell
  #undef  Include_stdio
  #define Include_stdio 1
 #endif
 #ifdef Uses_strstream
  #undef  Include_strstream
  #define Include_strstream 1
 #endif
 /* ifstream::getline behaves strangely in BC++
    I take the gcc implementation. */
 #define IfStreamGetLine(istream,buffer,size) \
         istream.getline(buffer,size)

 /* ISO C++ 1998 standard changed a lot of details in the standard C++
    library. GCC implemented it in version 3.0. BC++ implemented some
    stuff in versions like BC++ 5.5. So that's a real mess. */
 #if __GNUC__>=3
  #if __GNUC__>=4 || __GNUC_MINOR__>=4
   // gcc>=3.4. It have __gnu_cxx::stdio_filebuf class.
   #define CLY_filebuf       __gnu_cxx::stdio_filebuf<char>
   #define CLY_int_filebuf   CLY_filebuf
   #define CLY_NewFBFromFD(buf,f) buf=new CLY_int_filebuf(fdopen(f,"rb+"),ios::in|ios::out|ios::binary)
   #undef  FSTREAM_HEADER
   #define FSTREAM_HEADER  <ext/stdio_filebuf.h>
  #else
   // gcc 3.1 needs a special filebuf
   #define CLY_filebuf       std::filebuf
   #if __GNUC_MINOR__<1
    #define CLY_int_filebuf   CLY_filebuf
    #define CLY_NewFBFromFD(buf,f) buf=new CLY_int_filebuf(fdopen(f,"rb+"),ios::in|ios::out|ios::binary)
   #else
    #undef  CLY_DefineSpecialFileBuf
    #define CLY_DefineSpecialFileBuf 1
    #define CLY_NewFBFromFD(buf,f) buf=new CLY_int_filebuf(f,ios::in|ios::out|ios::binary)
   #endif
   #undef  FSTREAM_HEADER
   #define FSTREAM_HEADER  <fstream>
  #endif
  #define CLY_streambuf      std::streambuf
  #define CLY_strstreambuf   std::strstreambuf
  #define CLY_ISOCpp98 1
  #define CLY_OpenModeT      std::ios::openmode
  #define CLY_StreamPosT     std::streampos
  #define CLY_StreamOffT     std::streamoff
  #define CLY_IOSSeekDir     std::ios::seekdir
  #define CLY_FBOpenProtDef  0
  #define CLY_PubSetBuf(a,b) pubsetbuf(a,b)
  #define CLY_FBOpen(a,b,c)  open(a,b)
  #define CLY_IOSBin         std::ios::binary
  #define CLY_IOSOut         std::ios::out
  #define CLY_IOSIn          std::ios::in
  #define CLY_IOSApp         std::ios::app
  #define CLY_IOSAtE         std::ios::ate
  #define CLY_IOSBeg         std::ios::beg
  #define CLY_IOSCur         std::ios::cur
  #define CLY_IOSEnd         std::ios::end
  #define CLY_IOSBadBit      std::ios::badbit
  #define CLY_IOSEOFBit      std::ios::eofbit
  #define CLY_IOSFailBit     std::ios::failbit
  #define CLY_IOSGoodBit     std::ios::goodbit
  #define CLY_PubSeekOff     pubseekoff
  #define CLY_PubSync        pubsync
  #define CLY_std(a)         std::a
  #define UsingNamespaceStd  using namespace std;
  #define CreateStrStream(os,buf,size) std::string buf; std::ostringstream os(buf)
  #define GetStrStream(os,buf) os.str().c_str()
  #ifdef Uses_StrStream
   #undef  Include_sstream
   #define Include_sstream 1
  #endif
  #undef  IOMANIP_HEADER
  #define IOMANIP_HEADER  <iomanip>
  #undef  IOSTREAM_HEADER
  #define IOSTREAM_HEADER <iostream>
  #undef  STRSTREAM_HEADER
  #define STRSTREAM_HEADER <strstream>
 #else
  #define CLY_filebuf        filebuf
  #define CLY_int_filebuf    filebuf
  #define CLY_strstreambuf   strstreambuf
  #define CLY_streambuf      streambuf
  #define CLY_OpenModeT      int
  #define CLY_StreamPosT     streampos
  #define CLY_StreamOffT     streamoff
  #define CLY_IOSSeekDir     ios::seek_dir
  #define CLY_FBOpenProtDef  filebuf::openprot
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(f)
  #define CLY_PubSetBuf(a,b) setbuf(a,b)
  #define CLY_HaveFBAttach   1
  #define CLY_FBOpen(a,b,c)  open(a,b,c)
  #define CLY_IOSBin         ios::bin
  #define CLY_IOSOut         ios::out
  #define CLY_IOSIn          ios::in
  #define CLY_IOSApp         ios::app
  #define CLY_IOSAtE         ios::ate
  #define CLY_IOSBeg         ios::beg
  #define CLY_IOSCur         ios::cur
  #define CLY_IOSEnd         ios::end
  #define CLY_IOSBadBit      ios::badbit
  #define CLY_IOSEOFBit      ios::eofbit
  #define CLY_IOSFailBit     ios::failbit
  #define CLY_IOSGoodBit     ios::goodbit
  #define CLY_PubSeekOff     seekoff
  #define CLY_PubSync        sync
  #define CLY_std(a)         a
  #define UsingNamespaceStd
  #define CreateStrStream(os,buf,size) char buf[size]; \
                                       ostrstream os(buf,sizeof(buf))
  #define GetStrStream(os,buf) buf
  #ifdef Uses_StrStream
   #undef  Include_strstream
   #define Include_strstream 1
  #endif
 #endif
 
 /* Use the internal bool type for Boolean */
 #undef Boolean
 #undef False
 #undef True
 #define Boolean bool
 #define True true
 #define False false

 #ifdef TVCompf_MinGW
  // MinGW uses MSVC runtime. Read more about the MSVC bug in the MSVC
  // section.
  #undef CLY_Have_snprintf
  #define CLY_UseCrLf 1
  #define CLY_HaveDriveLetters 1
  /* I guess they are the same as MSVC: */
  #undef  FA_ARCH
  #define FA_ARCH   0x00  /* _A_NORMAL */
  #undef  FA_DIREC
  #define FA_DIREC  0x10  /* _A_SUBDIR */
  #undef  FA_RDONLY
  #define FA_RDONLY 0x01  /* _A_RDONLY */
  #ifdef Uses_filelength
   #undef  filelength
   #define filelength _filelength
   #undef  Include_io
   #define Include_io 1
  #endif
  #define PATHSEPARATOR ';'
  #define PATHSEPARATOR_ ";"
  /* SET: Not sure why Vadim wanted it. Perhaps to make the port more usable
     for users accustomed to back slashes, but then the library isn't uniform
     across platforms */
  #define DIRSEPARATOR '\\'
  #define DIRSEPARATOR_ "\\"
  #define CLY_IsValidDirSep(a) (a=='/' || a=='\\')
  #ifdef Uses_fixpath
   CLY_CFunc void _fixpath(const char *in, char *out);
  #endif
  #ifdef Uses_HaveLFNs
   #undef  OS_HaveLFNs
   #define OS_HaveLFNs 1
  #endif
  #ifdef Uses_glob
   #undef  Include_cl_glob
   #define Include_cl_glob 1
  #endif
  #ifdef Uses_fnmatch
   #undef  Include_cl_fnmatch
   #define Include_cl_fnmatch 1
  #endif
  #ifdef Uses_regex
   #undef  Include_cl_regex
   #define Include_cl_regex 1
  #endif
  #ifdef Uses_getopt
   #undef  Include_cl_getopt
   #define Include_cl_getopt 1
  #endif
  #ifdef Uses_io
   #undef  Include_io
   #define Include_io 1
  #endif
  #ifdef Uses_utime
   /* I can't beleive they forgot the header when the function is there! */
   #undef  Include_cl_utime
   #define Include_cl_utime 1
   #undef  Include_time
   #define Include_time 1
  #endif
  #ifdef Uses_mkstemp
   CLY_CFunc int mkstemp(char *_template);
  #endif
  #ifdef Uses_getcwd
   #undef  Include_io
   #define Include_io 1
  #endif
  #ifdef Uses_itoa
   #undef  Include_stdlib
   #define Include_stdlib 1
  #endif
  #ifdef Uses_direct
   #undef  Include_direct
   #define Include_direct 1
  #endif
  #ifdef Uses_dir
   #undef  Include_dir
   #define Include_dir 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #undef  Uses_CLY_ssize_t
  #define Uses_CLY_ssize_t 1
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
  #ifndef usleep
   #define usleep(microseconds) CLY_YieldProcessor(microseconds)
  #endif
  #ifndef __MINGW32_MAJOR_VERSION
   // MinGW people is really ignorant about gcc.
   // This definition should be done by gcc itself.
   #include <_mingw.h>
  #endif
  #if defined(Uses_alloca) && (__MINGW32_MAJOR_VERSION>=2) && !defined(alloca)
   // Why needed? that's a really idiot thing.
   #define alloca __builtin_alloca
  #endif
  // MinGW lacks ioctl.h
  #undef Include_ioctl
 #endif

 /* Win32/Cygwin -- OH! */
 #ifdef TVCompf_Cygwin
  #define CLY_HiddenDifferent 1
  #define CLY_Have_UGID 1
  #undef  FA_ARCH
  #undef  FA_DIREC
  #undef  FA_RDONLY
  #define FA_ARCH   0x01
  #define FA_DIREC  0x02
  #define FA_RDONLY 0x04
  #ifdef Uses_filelength
   #undef  filelength
   #define filelength CLY_filelength
  #endif
  #define PATHSEPARATOR ':'
  #define PATHSEPARATOR_ ":"
  #define DIRSEPARATOR '/'
  #define DIRSEPARATOR_ "/"
  #define CLY_IsValidDirSep(a) (a=='/')
  #ifdef Uses_fixpath
   CLY_CFunc void _fixpath(const char *in, char *out);
  #endif
  #ifdef Uses_HaveLFNs
   #undef  OS_HaveLFNs
   #define OS_HaveLFNs 1
  #endif
  #ifdef Uses_glob
   #undef  Include_glob
   #define Include_glob 1
  #endif
  #ifdef Uses_fnmatch
   #undef  Include_cl_fnmatch
   #define Include_cl_fnmatch 1
  #endif
  #ifdef Uses_regex
   #undef  Include_sys_types
   #define Include_sys_types 1
   #undef  Include_regex
   #define Include_regex 1
  #endif
  #ifdef Uses_getopt
   #undef  Include_getopt
   #define Include_getopt 1
  #endif
  #ifdef Uses_utime
   #undef  Include_utime
   #define Include_utime 1
  #endif
  #ifdef Uses_mkstemp
   #undef  Include_stdio
   #define Include_stdio 1
  #endif
  #ifdef Uses_getcwd
   #undef  Include_unistd
   #define Include_unistd 1
  #endif
  #ifdef Uses_itoa
   CLY_CFunc char *itoa(int value, char *string, int radix);
  #endif
  #ifdef Uses_dir
   #undef  Include_dir
   #define Include_dir 1
   #undef  Include_direct
   #define Include_direct 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
 #endif
 
 /* Under DOS djgpp defines it */
 #ifdef TVCompf_djgpp
  #define CLY_UseCrLf 1
  #define CLY_HaveDriveLetters 1
  #ifdef Uses_filelength
   #undef  Include_io
   #define Include_io 1
  #endif
  #define PATHSEPARATOR ';'
  #define PATHSEPARATOR_ ";"
  #define DIRSEPARATOR '/'
  #define DIRSEPARATOR_ "/"
  #define CLY_IsValidDirSep(a) (a=='/' || a=='\\')
  #ifdef Uses_fixpath
   #undef  Include_sys_stat
   #define Include_sys_stat 1
  #endif
  #ifdef Uses_HaveLFNs
   #undef  Include_fcntl
   #define Include_fcntl 1
   #undef  DJGPP_HaveLFNs
   #define DJGPP_HaveLFNs 1
  #endif
  #ifdef Uses_glob
   #undef  Include_glob
   #define Include_glob 1
  #endif
  #ifdef Uses_fnmatch
   #undef  Include_fnmatch
   #define Include_fnmatch 1
  #endif
  #ifdef Uses_regex
   #undef  Include_sys_types
   #define Include_sys_types 1
   #undef  Include_regex
   #define Include_regex 1
  #endif
  #ifdef Uses_getopt
   #undef  Include_cl_getopt
   #define Include_cl_getopt 1
  #endif
  #ifdef Uses_io
   #undef  Include_io
   #define Include_io 1
  #endif
  #ifdef Uses_utime
   #undef  Include_utime
   #define Include_utime 1
  #endif
  #ifdef Uses_mkstemp
   #undef  Include_stdio
   #define Include_stdio 1
  #endif
  #ifdef Uses_getcwd
   #undef  Include_unistd
   #define Include_unistd 1
  #endif
  #ifdef Uses_itoa
   #undef  Include_stdlib
   #define Include_stdlib 1
  #endif
  #ifdef Uses_direct
   #undef  Include_direct
   #define Include_direct 1
  #endif
  #ifdef Uses_dir
   #undef  Include_dir
   #define Include_dir 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
  #undef CLY_Have_snprintf
 #endif

 /* Common to all UNIX systems */
 #ifdef TVOS_UNIX
  /* Filesystem */
  #define CLY_HiddenDifferent 1
  #define CLY_Have_UGID 1
  #undef  FA_ARCH
  #undef  FA_DIREC
  #undef  FA_RDONLY
  #define FA_ARCH   0x01
  #define FA_DIREC  0x02
  #define FA_RDONLY 0x04
  #define PATHSEPARATOR ':'
  #define PATHSEPARATOR_ ":"
  #define DIRSEPARATOR '/'
  #define DIRSEPARATOR_ "/"
  #define CLY_IsValidDirSep(a) (a=='/')
  #ifdef Uses_HaveLFNs
   #undef  OS_HaveLFNs
   #define OS_HaveLFNs 1
  #endif
  /* Non standard */
  #ifdef Uses_string
   CLY_CFunc char *strlwr(char *s);
   CLY_CFunc char *strupr(char *s);
  #endif
  #ifdef Uses_itoa
   CLY_CFunc char *itoa(int value, char *string, int radix);
  #endif
  #ifdef Uses_filelength
   #undef  filelength
   #define filelength CLY_filelength
  #endif
  #ifdef Uses_fixpath
   CLY_CFunc void _fixpath(const char *in, char *out);
  #endif
  /* POSIX */
  #ifdef Uses_glob
   #undef  Include_glob
   #define Include_glob 1
  #endif
  #ifdef Uses_fnmatch
   #undef  Include_fnmatch
   #define Include_fnmatch 1
  #endif
  #ifdef Uses_regex
   #undef  Include_sys_types
   #define Include_sys_types 1
   #undef  Include_regex
   #define Include_regex 1
  #endif
  #ifdef Uses_utime
   #undef  Include_utime
   #define Include_utime 1
  #endif
  #ifdef Uses_mkstemp
   #undef  Include_stdio
   #define Include_stdio 1
  #endif
  #ifdef Uses_getcwd
   #undef  Include_unistd
   #define Include_unistd 1
  #endif
  /* Headers */
  #ifdef Uses_direct
   #undef  Include_direct
   #define Include_direct 1
  #endif
  /* dir.h isn't POSIX.
  #ifdef Uses_dir
   #undef  Include_dir
   #define Include_dir 1
  #endif*/
 #endif

 /* Linux (glibc) */
 #ifdef TVOSf_Linux
  #ifdef Uses_nl_langinfo
   #undef  Include_langinfo
   #define Include_langinfo 1
  #endif
  #ifdef Uses_getopt
   #undef  Include_getopt
   #define Include_getopt 1
  #endif
  #ifdef Uses_CLYFileAttrs
   /* Is that a bug? shouldn't be sys/stat.h enough? */
   #undef  Include_sys_types
   #define Include_sys_types 1
  #endif
  #ifdef Uses_getline
   #undef  Include_stdio
   #define Include_stdio 1
   /* That's a GNU extension and needs it */
   #undef  _GNU_SOURCE
   #define _GNU_SOURCE
   #define CLY_getline getline
  #endif
  // Avoid including libintl.h, we have the prototypes
  // This is a temporal workaround.
  //#undef  _LIBINTL_H
  //#define _LIBINTL_H 1
  // Needed to successfully compile with g++ 4.0.3
  // Donald R. Ziesig 2006-11-02
  #if __GNUC__ >= 4
   #include <libintl.h>
  #endif
 #endif

 /* Solaris using gcc but not glibc */
 #ifdef TVOSf_Solaris
  #ifdef Uses_getopt
   #undef  Include_cl_getopt
   #define Include_cl_getopt 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Include_langinfo
   #define Include_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
  #ifdef TV_MAJOR_VERSION
   // size_t needs a explicit include for Solaris.
   // That's used by TV headers by default.
   #undef  Include_stdio
   #define Include_stdio 1
  #endif
  // Solaris 2.7 w/gcc 2.95 doesn't need alloca.h
  // Solaris 9 w/gcc 3.3.2 does ?!
  #if defined(Uses_alloca) && (__GNUC__>=3)
   #undef  Include_alloca
   #define Include_alloca 1
  #endif
  // At least Solaris 9 have a non POSIX snprintf.
  // It fails to work for NULL pointers as POSIX states.
  #undef CLY_Have_snprintf
 #endif

 #if defined(TVOSf_QNX4) 
  #if defined(Uses_alloca) && !defined(alloca)
   #define alloca __builtin_alloca
  #endif // alloca

  #ifdef Uses_string
   #undef  Include_string
   #define Include_string 1
   #undef  strncasecmp
   #define strncasecmp strcasencmp

   // declare strcase* functions here to avoid unix.h inclusion.
   #include <sys/types.h>

   CLY_CFunc int strcasecmp(const char* s1, const char* s2);
   CLY_CFunc int strcasencmp(const char* s1, const char* s2, size_t n);

  #endif // string
  
  #if defined(Uses_stdlib) || defined(Uses_unistd)
   #undef  Include_process
   #define Include_process 1
  #endif // process

  #undef CLY_Redraw
  #define CLY_Redraw ForcedRedraw

  #undef  IOCTL_HEADER
  #define IOCTL_HEADER    <ioctl.h>

  #undef CLY_Have_snprintf
 #endif // TVOSf_QNX4

 /* OpenBSD, tested with 3.4 */
 #ifdef TVOSf_OpenBSD
  #ifdef Uses_getopt
   #undef  Include_getopt
   #define Include_getopt 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
 #endif

 /* NetBSD, tested with 1.6.1 */
 #ifdef TVOSf_NetBSD
  #ifdef Uses_getopt
   #undef  Include_getopt
   #define Include_getopt 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
 #endif

 /* OSF1 (Tru64) */
 #ifdef TVOSf_Tru64
  #ifdef Uses_alloca
   #undef  Include_alloca
   #define Include_alloca 1
  #endif
 #endif

 /* Darwin (MacOS X) */
 #ifdef TVOSf_Darwin
  // At least MacOS X 10.2 have a non POSIX snprintf.
  // It fails to work for NULL pointers as POSIX states.
  #undef CLY_Have_snprintf
 #endif

 /* Generic UNIX system */
 #if defined(TVOS_UNIX) && !defined(TVOSf_Linux) && !defined(TVOSf_Solaris) \
     && !defined(TVOSf_OpenBSD) && !defined(TVOSf_NetBSD)
  #ifdef Uses_getopt
   #undef  Include_cl_getopt
   #define Include_cl_getopt 1
  #endif
  #ifdef Uses_nl_langinfo
   #undef  Uses_CLY_nl_langinfo
   #define Uses_CLY_nl_langinfo 1
  #endif
  #ifdef Uses_getline
   #undef  Uses_CLY_getline
   #define Uses_CLY_getline 1
  #endif
 #endif
#endif /* TVComp_GCC */



/* BC++ 5.5 and 5.6 for Win32 are supported */
#ifdef TVComp_BCPP
 #define CLY_UseCrLf 1
 #define CLY_HaveDriveLetters 1
 #define CLY_Packed
 #ifndef CLY_BooleanDefined
  #define CLY_BooleanDefined 1
  /* Simple Boolean type */
  enum Boolean { False, True };
 #endif
 #ifdef Uses_string
  #undef  Include_string
  #define Include_string 1
  #undef  strncasecmp
  #define strncasecmp strnicmp
  #undef  strcasecmp
  #define strcasecmp  stricmp
 #endif
 #ifdef Uses_abort
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_limits
  #undef  Include_limits
  #define Include_limits 1
 #endif
 #ifdef Uses_fcntl
  #undef  Include_fcntl
  #define Include_fcntl 1
 #endif
 #ifdef Uses_sys_stat
  #undef  Include_sys_stat
  #define Include_sys_stat 1
 #endif
 #ifdef Uses_unistd
  #undef  Include_cl_unistd
  #define Include_cl_unistd 1
  /* Most unistd equivalents are here: */
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_access
  #undef  Include_io
  #define Include_io 1
  #undef  R_OK
  #define R_OK 4
  #undef  W_OK
  #define W_OK 2
  #undef  F_OK
  #define F_OK 0
  // No test for execute => just exists
  #undef  X_OK
  #define X_OK 0
 #endif
 #ifdef Uses_ctype
  #undef  Include_ctype
  #define Include_ctype 1
 #endif
 #ifdef Uses_filelength
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_getcurdir
  #undef  Include_dir
  #define Include_dir 1
 #endif
 #ifdef Uses_AllocLocal
  #undef  AllocLocalStr
  #define AllocLocalStr(s,l) char* s = (char*)alloca(l)
  #undef  AllocLocalUShort
  #define AllocLocalUShort(s,l) ushort *s = (ushort*)alloca(sizeof(ushort) * (l))
  #undef  Uses_alloca
  #define Uses_alloca 1
 #endif
 #ifdef Uses_alloca
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #ifdef Uses_free
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #define NEVER_RETURNS
 #define RETURN_WHEN_NEVER_RETURNS return 0
 #undef  __attribute__
 #define __attribute__( value )
 #undef  __inline__
 #define __inline__ inline
 #define DeleteArray(a) delete[] a
 #define PATHSEPARATOR ';'
 #define PATHSEPARATOR_ ";"
 #define DIRSEPARATOR '/'
 #define DIRSEPARATOR_ "/"
 #define CLY_IsValidDirSep(a) (a=='/' || a=='\\')
 #ifdef Uses_fixpath
  CLY_CFunc void _fixpath(const char *in, char *out);
 #endif
 /* Checks for UNCs under Win9x and NT provided by Anantoli Soltan */
 CLY_CFunc int CLY_IsUNC(const char* path);
 CLY_CFunc int CLY_IsUNCShare(const char* path);
 #ifdef Uses_HaveLFNs
  #undef  OS_HaveLFNs
  #define OS_HaveLFNs 1
 #endif
 #ifdef Uses_glob
  #undef  Include_cl_glob
  #define Include_cl_glob 1
 #endif
 #ifdef Uses_fnmatch
  #undef  Include_cl_fnmatch
  #define Include_cl_fnmatch 1
 #endif
 #ifdef Uses_regex
  #undef  Include_cl_regex
  #define Include_cl_regex 1
 #endif
 #ifdef Uses_getopt
  #undef  Include_cl_getopt
  #define Include_cl_getopt 1
 #endif
 #ifdef Uses_io
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_dirent
  #undef  Include_cl_dirent
  #define Include_cl_dirent 1
 #endif
 #ifdef Uses_ftell
  #undef  Include_io
  #define Include_io 1
  #undef  Include_stdio
  #define Include_stdio 1
 #endif
 #ifdef Uses_stdlib
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_utime
  #undef  Include_utime
  #define Include_utime 1
 #endif
 #ifdef Uses_mkstemp
  CLY_CFunc int mkstemp(char *_template);
 #endif
 #ifdef Uses_getcwd
  #undef  Include_dir
  #define Include_dir 1
 #endif
 #ifdef Uses_itoa
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_direct
  #undef  Include_direct
  #define Include_direct 1
 #endif
 #ifdef Uses_dir
  #undef  Include_dir
  #define Include_dir 1
 #endif
 #ifdef Uses_strstream
  #undef  Include_strstream
  #define Include_strstream 1
 #endif
 #ifdef Uses_nl_langinfo
  #undef  Uses_CLY_nl_langinfo
  #define Uses_CLY_nl_langinfo 1
 #endif
 #undef  Uses_CLY_ssize_t
 #define Uses_CLY_ssize_t 1
 #ifdef Uses_getline
  #undef  Uses_CLY_getline
  #define Uses_CLY_getline 1
 #endif
 #ifndef usleep
  // Doesn't work, needs to be fixed.
  #define usleep(microseconds) CLY_YieldProcessor(microseconds)
 #endif
 // BC++ lacks ioctl.h
 #undef Include_ioctl

 /* ifstream::getline behaves strangely in BC++
    I take the gcc implementation, here is a replacement. */
 #define IfStreamGetLine(istream,buffer,size) \
         CLY_IfStreamGetLine(istream,buffer,size)
 #ifdef Uses_IfStreamGetLine
  #undef  Uses_fstream
  #define Uses_fstream 1
  #undef  Uses_CLY_IfStreamGetLine
  #define Uses_CLY_IfStreamGetLine 1
 #endif

 #if __BORLANDC__>=0x560
  // BC++ 5.6 (Builder 6)
  #define CLY_ISOCpp98 1
  #define CLY_DONT_DEFINE_MIN_MAX 1
  // BC++ 5.6 supports the "open(int fd)" UNIX extension.
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(); buf->open(f)
  #undef  CLY_destroy
  // Note: that 0777 fails to create the file and 0666 creates a hidden file.
  #define CLY_FBOpenProtDef  0
 #else
  // BC++ 5.5 and 5.5.1 (Free command line tools)
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(f)
  #define CLY_FBOpenProtDef  0666
 #endif

 #define CLY_int_filebuf    filebuf
 #define CLY_filebuf        filebuf
 #define CLY_strstreambuf   strstreambuf
 #define CLY_streambuf      streambuf
 #define CLY_OpenModeT      int
 #define CLY_StreamPosT     streampos
 #define CLY_StreamOffT     streamoff
 #define CLY_IOSSeekDir     ios::seek_dir
 #define CLY_PubSetBuf(a,b) pubsetbuf(a,b)
 #define CLY_FBOpen(a,b,c)  open(a,b,c)
 #define CLY_IOSBin         ios::binary
 #define CLY_IOSOut         ios::out
 #define CLY_IOSIn          ios::in
 #define CLY_IOSApp         ios::app
 #define CLY_IOSAtE         ios::ate
 #define CLY_IOSBeg         ios::beg
 #define CLY_IOSCur         ios::cur
 #define CLY_IOSEnd         ios::end
 #define CLY_IOSBadBit      ios::badbit
 #define CLY_IOSEOFBit      ios::eofbit
 #define CLY_IOSFailBit     ios::failbit
 #define CLY_IOSGoodBit     ios::goodbit
 #define CLY_PubSeekOff     pubseekoff
 #define CLY_PubSync        pubsync
 #define CLY_std(a)         a
 #define CreateStrStream(os,buf,size) char buf[size]; \
                                      ostrstream os(buf,sizeof(buf))
 #define GetStrStream(os,buf) buf
 #ifdef Uses_StrStream
  #undef  Include_strstream
  #define Include_strstream 1
 #endif
 #define UsingNamespaceStd
#endif /* TVComp_BCPP */


/* Open Watcom for Win32 support */
/* Derived from BC++ section */
#ifdef TVComp_Watcom
 /* Support for the library as a DLL. */
 #undef CLY_EXPORT
 #ifdef CLY_DLL
  #define CLY_EXPORT __declspec(dllexport)
 #else
  #define CLY_EXPORT __declspec(dllimport)
 #endif
 /* Watcom generates the following error for validate.h:
    include\tv\validate.h(127): Error! E346: col(33) protected base class accessed to convert cast expression
    So we just use public there.
 */
 #undef  CLY_BROKEN_WATCOM_SCOPE
 #define CLY_BROKEN_WATCOM_SCOPE public
 /* Disables useless warnings. */
 #pragma warning 555 9
 #define CLY_UseCrLf 1
 #define CLY_HaveDriveLetters 1
 #define CLY_Packed
 #ifndef CLY_BooleanDefined
  #define CLY_BooleanDefined 1
  /* Simple Boolean type */
  enum Boolean { False, True };
 #endif
 #ifdef Uses_string
  #undef  Include_string
  #define Include_string 1
  #undef  strncasecmp
  #define strncasecmp strnicmp
  #undef  strcasecmp
  #define strcasecmp  stricmp
 #endif
 #ifdef Uses_abort
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_limits
  #undef  Include_limits
  #define Include_limits 1
 #endif
 #ifdef Uses_fcntl
  #undef  Include_fcntl
  #define Include_fcntl 1
 #endif
 #ifdef Uses_sys_stat
  #undef  Include_sys_stat
  #define Include_sys_stat 1
 #endif
 #ifdef Uses_unistd
  #undef  Include_cl_unistd
  #define Include_cl_unistd 1
  /* Most unistd equivalents are here: */
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_access
  #undef  Include_io
  #define Include_io 1
  #undef  R_OK
  #define R_OK 4
  #undef  W_OK
  #define W_OK 2
  #undef  F_OK
  #define F_OK 0
  // No test for execute => just exists
  #undef  X_OK
  #define X_OK 1 // Watcom defines 1 for it and F_OK as 0
 #endif
 #ifdef Uses_ctype
  #undef  Include_ctype
  #define Include_ctype 1
 #endif
 #ifdef Uses_filelength
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_getcurdir
  #undef  getcurdir
  #define getcurdir CLY_getcurdir
 #endif
 #ifdef Uses_AllocLocal
  #undef  AllocLocalStr
  #define AllocLocalStr(s,l) char* s = (char*)alloca(l)
  #undef  AllocLocalUShort
  #define AllocLocalUShort(s,l) ushort *s = (ushort*)alloca(sizeof(ushort) * (l))
  #undef  Uses_alloca
  #define Uses_alloca 1
 #endif
 #ifdef Uses_alloca
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #ifdef Uses_free
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #define NEVER_RETURNS
 #define RETURN_WHEN_NEVER_RETURNS return 0
 #undef  __attribute__
 #define __attribute__( value )
 #undef  __inline__
 #define __inline__ inline
 #define DeleteArray(a) delete[] a
 #define PATHSEPARATOR ';'
 #define PATHSEPARATOR_ ";"
 #define DIRSEPARATOR '/'
 #define DIRSEPARATOR_ "/"
 #define CLY_IsValidDirSep(a) (a=='/' || a=='\\')
 #ifdef Uses_fixpath
  CLY_CFunc void _fixpath(const char *in, char *out);
 #endif
 /* Checks for UNCs under Win9x and NT provided by Anantoli Soltan */
 CLY_CFunc int CLY_IsUNC(const char* path);
 CLY_CFunc int CLY_IsUNCShare(const char* path);
 #ifdef Uses_HaveLFNs
  #undef  OS_HaveLFNs
  #define OS_HaveLFNs 1
 #endif
 #ifdef Uses_glob
  #undef  Include_cl_glob
  #define Include_cl_glob 1
 #endif
 #ifdef Uses_fnmatch
  #undef  Include_cl_fnmatch
  #define Include_cl_fnmatch 1
 #endif
 #ifdef Uses_regex
  #undef  Include_cl_regex
  #define Include_cl_regex 1
 #endif
 #ifdef Uses_getopt
  #undef  Include_cl_getopt
  #define Include_cl_getopt 1
 #endif
 #ifdef Uses_io
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_dirent
  //#undef  Include_dirent
  //#define Include_dirent 1
  // Watcom defines DIR and dirent in direct.h
  #undef Include_direct
  #define Include_direct 1
 #endif
 #ifdef Uses_ftell
  #undef  Include_io
  #define Include_io 1
  #undef  Include_stdio
  #define Include_stdio 1
 #endif
 #ifdef Uses_stdlib
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_utime
  #undef  Include_utime
  #define Include_utime 1
 #endif
 #ifdef Uses_mkstemp
  CLY_CFunc int mkstemp(char *_template);
 #endif
 #ifdef Uses_getcwd
  #undef  Include_dir
  #define Include_dir 1
 #endif
 #ifdef Uses_itoa
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_direct
  #undef  Include_direct
  #define Include_direct 1
 #endif
 #ifdef Uses_dir
  #undef  Include_dir
  #define Include_dir 1
 #endif
 #ifdef Uses_strstream
  #undef  Include_strstream
  #define Include_strstream 1
 #endif
 #ifdef Uses_nl_langinfo
  #undef  Uses_CLY_nl_langinfo
  #define Uses_CLY_nl_langinfo 1
 #endif
 #undef  Uses_CLY_ssize_t
 #define Uses_CLY_ssize_t 1
 #ifdef Uses_getline
  #undef  Uses_CLY_getline
  #define Uses_CLY_getline 1
 #endif
 #ifndef usleep
  // Doesn't work, needs to be fixed.
  #define usleep(microseconds) CLY_YieldProcessor(microseconds)
 #endif
 // BC++ lacks ioctl.h
 #undef Include_ioctl

 /* ifstream::getline behaves strangely in BC++
    I take the gcc implementation, here is a replacement. */
 #define IfStreamGetLine(istream,buffer,size) \
         CLY_IfStreamGetLine(istream,buffer,size)
 #ifdef Uses_IfStreamGetLine
  #undef  Uses_fstream
  #define Uses_fstream 1
  #undef  Uses_CLY_IfStreamGetLine
  #define Uses_CLY_IfStreamGetLine 1
 #endif

 #if __BORLANDC__>=0x560
  // BC++ 5.6 (Builder 6)
  #define CLY_ISOCpp98 1
  #define CLY_DONT_DEFINE_MIN_MAX 1
  // BC++ 5.6 supports the "open(int fd)" UNIX extension.
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(); buf->open(f)
  #undef  CLY_destroy
  // Note: that 0777 fails to create the file and 0666 creates a hidden file.
  #define CLY_FBOpenProtDef  0
 #else
  // BC++ 5.5 and 5.5.1 (Free command line tools)
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(f)
  #define CLY_FBOpenProtDef  0666
 #endif

 #define CLY_int_filebuf    filebuf
 #define CLY_filebuf        filebuf
 #define CLY_strstreambuf   strstreambuf
 #define CLY_streambuf      streambuf
 #define CLY_OpenModeT      int
 #define CLY_StreamPosT     streampos
 #define CLY_StreamOffT     streamoff
 #define CLY_IOSSeekDir     ios::seek_dir
 // Documentation to the diference in setbuf and pubsetbuf:
 // http://groups-beta.google.com/group/powersoft.public.watcom_c_c++.general/browse_frm/thread/8927d7c2b04acd93/1d37ccff2009cf7e?q=watcom+pubsetbuf&rnum=1#1d37ccff2009cf7e
 #define CLY_PubSetBuf(a,b) setbuf(a,b)
 #define CLY_FBOpen(a,b,c)  open(a,b,c)
 #define CLY_IOSBin         ios::binary
 #define CLY_IOSOut         ios::out
 #define CLY_IOSIn          ios::in
 #define CLY_IOSApp         ios::app
 #define CLY_IOSAtE         ios::ate
 #define CLY_IOSBeg         ios::beg
 #define CLY_IOSCur         ios::cur
 #define CLY_IOSEnd         ios::end
 #define CLY_IOSBadBit      ios::badbit
 #define CLY_IOSEOFBit      ios::eofbit
 #define CLY_IOSFailBit     ios::failbit
 #define CLY_IOSGoodBit     ios::goodbit
 #define CLY_PubSeekOff     seekoff
 #define CLY_PubSync        sync
 #define CLY_std(a)         a
 #define CreateStrStream(os,buf,size) char buf[size]; \
                                      ostrstream os(buf,sizeof(buf))
 #define GetStrStream(os,buf) buf
 #ifdef Uses_StrStream
  #undef  Include_strstream
  #define Include_strstream 1
 #endif
 #define UsingNamespaceStd
#endif /* TVComp_Watcom */



/* MSVC will be supported if volunteers tests it or Microsoft decides to
   give it for free ;-). After all Borland released BC++ 5.5. */
#if (defined(_MSVC) || defined(__MSC_VER)) && !defined(_MSC_VER)
 #if !defined(__MSC_VER) // let M$ compiler be version 1.00, if nothing else has been defined.
    #define __MSC_VER 100
 #endif
 #define _MSC_VER __MSC_VER
#endif

/*
 Note about MSVC and compiler versions:
 It looks like you must add 6 to get the compiler version. I guess they created MSVC 1.0
when compiler version 7.0 was released.
 Compiler version   MSVC version
      10.00             4.0
      11.00             5.0 (just a guess)
      12.00             6.0
      13.00             7.0
      13.10             7.1 (.NET 2003 Architect)
      14.00             8.0 (Visual C++ 2005)
*/
#ifdef TVComp_MSC
 // Note: snprintf and vsnprintf seems to be available in some versions but not in others.
 // But looks like _snprintf and _vsnprintf is available in 1000 and 1300 versions.
 // So currently I assume the _* versions are available for all versions.
 //
 // Note 2: at least MSVC 1200 (6.0) have a wrong implemetation of vsnprintf, it have an
 // assert checking that the buffer is != NULL and/or size != 0. This is wrong. According
 // to the IEEE 1003.1 (POSIX 1003.1) standard (Issue 6 2004):
 // ... "If /n/ is zero, nothing shall be written and /s/ may be a null pointer." ...
 // And when the result value is explained says:
 // "Upon successful completion, the /snprintf/() function shall return the number of bytes
 // that would be written to /s/ had /n/ been sufficiently large excluding the terminating
 // null byte."
 //
 // As I don't know which version fixed this bug I use the replacement that really works.
 //
 #undef CLY_Have_snprintf
 #define CLY_UseCrLf 1
 #define CLY_HaveDriveLetters 1
 #define CLY_Packed
 /*
   I'm disabling warnings 4311/2 that generates:
   'type cast' : pointer truncation from 'void *' to 'uipointer'
   'type cast' : conversion from 'uipointer' to 'void *' of greater size
   This is just the result of using /Wp64 and is just a potential issue.
   We already define uipointer as a 64 bits value for Win64.
 */
 #if _MSC_VER >= 1310
   #pragma warning( disable : 4311 )
   #pragma warning( disable : 4312 )
 #endif
 /*
   I'm disabling warning 4996 that generates:
      warning C4996: '_access' was declared deprecated
      E:\MVS8\VC\INCLUDE\io.h(193) : see declaration of '_access'
      Message: 'The POSIX name for this item is deprecated. Instead, use the ISO C++
      conformant name: _access. See online help for details.'
   This is a nonsense, POSIX is an standard and changing the names of the functions makes
   the code incompatible with POSIX compliant compilers.
   I don't think that's a solution to make the compiler ISO C++ compliant. It could be a
   command line option (i.e. deprecate POSIX functions).
 */
 #if _MSC_VER >= 1400
   #pragma warning( disable : 4996 )
 #endif
 /* Avoid warnings for most C standard library */
 #if _MSC_VER >= 1400 && !defined(_CRT_SECURE_NO_DEPRECATE)
   #define _CRT_SECURE_NO_DEPRECATE
 #endif
 #if _MSC_VER <= 1000
   // MSVC 4.0 reports version 10.0
   // Only new.h exists
   #undef  NEW_HEADER
   #define NEW_HEADER      <new.h>
   #undef Include_ioctl
   // bool isn't defined
   #if defined(__cplusplus) && !defined(CLY_BOOL_TRUE_FALSE)
    #define CLY_BOOL_TRUE_FALSE
    typedef int bool;
    const int true=1, false=0;
   #endif
 #endif
 #ifndef CLY_BooleanDefined
  #define CLY_BooleanDefined 1
   #if _MSC_VER >= 1500
     #undef Boolean
     #undef False
     #undef True
     #define Boolean bool
     #define True true
     #define False false
   #else
    /* Simple Boolean type */
    enum Boolean { False, True };
   #endif
 #endif
 #ifdef Uses_string
  #undef  Include_string
  #define Include_string 1
  #if _MSC_VER >= 1400
   // ISO C++ version of the POSIX name
   #undef  strncasecmp
   #define strncasecmp _strnicmp
   #undef  strcasecmp
   #define strcasecmp  _stricmp
   #undef  strdup
   #define strdup(a)   _strdup(a)
  #else
   #undef  strncasecmp
   #define strncasecmp strnicmp
   #undef  strcasecmp
   #define strcasecmp  stricmp
  #endif
 #endif
 #ifdef Uses_chdir
  #undef  Include_direct
  #define Include_direct 1
  #undef  chdir
  #define chdir _chdir
 #endif
 #ifdef Uses_getcwd
  #undef  Include_direct
  #define Include_direct 1
  #if _MSC_VER >= 1400
   #undef  getcwd
   #define getcwd _getcwd
  #endif
 #endif
 #ifdef Uses_stdlib
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_abort
  #undef  Include_process
  #define Include_process 1
 #endif
 #ifdef Uses_limits
  #undef  Include_limits
  #define Include_limits 1
 #endif
 #ifdef Uses_stdio
  #undef  popen
  #define popen _popen
  #undef  pclose
  #define pclose _pclose
  #undef  pipe
  #define pipe _pipe
 #endif
 #ifdef Uses_fcntl
  #undef  Include_fcntl
  #define Include_fcntl 1
  /* open and creat belongs here */
  #undef  Include_io
  #define Include_io 1
  #undef  open
  #define open  _open
  #undef  creat
  #define creat _creat
  #undef  CLY_DefineFileConstants
  #define CLY_DefineFileConstants 1
 #endif
 #ifdef Uses_sys_stat
  #undef  Include_sys_stat
  #define Include_sys_stat 1
  #undef  Fake_S_IS
  #define Fake_S_IS 1
  #undef  CLY_DefineIRW
  #define CLY_DefineIRW 1
 #endif
 #ifdef Uses_unistd
  #undef  Include_unistd
  /* Doesn't exist
     #define Include_unistd 1 */
  /* Most unistd equivalents are here: */
  #undef  Include_io
  #define Include_io 1
  /* close, dup, dup2, read, write and lseek is defined here */
  #undef  CLY_Define_Inline_IO_Translations
  #define CLY_Define_Inline_IO_Translations 1
  /* Standard file descriptors.  */
  #ifndef STDIN_FILENO
   #define STDIN_FILENO  0 /* Standard input.  */
  #endif
  #ifndef STDOUT_FILENO
   #define STDOUT_FILENO 1 /* Standard output.  */
  #endif
  #ifndef STDERR_FILENO
   #define STDERR_FILENO 2 /* Standard error output.  */
  #endif
 #endif
 #ifdef Uses_access
  #undef  Include_io
  #define Include_io 1
  #undef  access
  #define access(a,b) _access(a,b)
  #undef  R_OK
  #define R_OK 4
  #undef  W_OK
  #define W_OK 2
  #undef  F_OK
  #define F_OK 0
  // No test for execute => just exists
  #undef  X_OK
  #define X_OK 0
 #endif
 #ifdef Uses_ctype
  #undef  Include_ctype
  #define Include_ctype 1
 #endif
 #ifdef Uses_filelength
  #undef  filelength
  #define filelength _filelength
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_AllocLocal
  #undef  AllocLocalStr
  #define AllocLocalStr(s,l) char* s = (char*)alloca(l)
  #undef  AllocLocalUShort
  #define AllocLocalUShort(s,l) ushort *s = (ushort*)alloca(sizeof(ushort) * (l))
  #undef  Uses_alloca
  #define Uses_alloca 1
 #endif
 #ifdef Uses_alloca
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #ifdef Uses_free
  #undef  Include_malloc
  #define Include_malloc 1
 #endif
 #ifdef Uses_getcurdir
  #undef  getcurdir
  #define getcurdir CLY_getcurdir
 #endif
 #define NEVER_RETURNS
 #define RETURN_WHEN_NEVER_RETURNS return 0
 #undef  __attribute__
 #define __attribute__( value )
 #undef  __inline__
 #define __inline__ inline
 #pragma warning( disable : 4250 )
 /* SET: MSVC have a non-standard delete[] support. It doesn't follow last
    standard. And which is worst doesn't understand it.
    Vadim Beloborodov pointed out this missfeature. */
 #define DeleteArray(a) delete (void *)a
 /* These values are returned by findfirst. */
 #undef  FA_ARCH
 #define FA_ARCH   0x00  /* _A_NORMAL */
 #undef  FA_DIREC
 #define FA_DIREC  0x10  /* _A_SUBDIR */
 #undef  FA_RDONLY
 #define FA_RDONLY 0x01  /* _A_RDONLY */
 #define PATHSEPARATOR ';'
 #define PATHSEPARATOR_ ";"
 /* SET: Not sure why Vadim wanted it. Perhaps to make the port more usable
    for users accustomed to back slashes, but then the library isn't uniform
    across platforms */
 #define DIRSEPARATOR '\\'
 #define DIRSEPARATOR_ "\\"
 #define CLY_IsValidDirSep(a) (a=='/' || a=='\\')
 #ifdef Uses_fixpath
  CLY_CFunc void _fixpath(const char *in, char *out);
 #endif
 /* Checks for UNCs under Win9x and NT provided by Anantoli Soltan */
 CLY_CFunc int CLY_IsUNC(const char* path);
 CLY_CFunc int CLY_IsUNCShare(const char* path);
 #ifdef Uses_HaveLFNs
  #undef  OS_HaveLFNs
  #define OS_HaveLFNs 1
 #endif
 #ifdef Uses_glob
  #undef  Include_cl_glob
  #define Include_cl_glob 1
 #endif
 #ifdef Uses_fnmatch
  #undef  Include_cl_fnmatch
  #define Include_cl_fnmatch 1
 #endif
 #ifdef Uses_regex
  #undef  Include_cl_regex
  #define Include_cl_regex 1
 #endif
 #ifdef Uses_getopt
  #undef  Include_cl_getopt
  #define Include_cl_getopt 1
 #endif
 #ifdef Uses_io
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_dirent
  #undef  Include_cl_dirent
  #define Include_cl_dirent 1
 #endif
 #ifdef Uses_ftell
  #undef  Include_stdio
  #define Include_stdio 1
 #endif
 #ifdef Uses_utime
  #undef  Include_sys_utime
  #define Include_sys_utime 1
  /*#undef  utime
  #define utime _utime
  #undef  utimbuf
  #define utimbuf _utimbuf*/
 #endif
 #ifdef Uses_mkstemp
  CLY_CFunc int mkstemp(char *_template);
 #endif
 #ifdef Uses_getcwd
  #undef  Include_io
  #define Include_io 1
 #endif
 #ifdef Uses_itoa
  #undef  Include_stdlib
  #define Include_stdlib 1
 #endif
 #ifdef Uses_direct
  #undef  Include_direct
  #define Include_direct 1
 #endif
 #ifdef Uses_dir
  #undef  Include_cl_dir
  #define Include_cl_dir 1
 #endif
 #ifdef Uses_strstream
  #undef  Include_strstrea
  #define Include_strstrea 1
 #endif
 #ifdef Uses_nl_langinfo
  #undef  Uses_CLY_nl_langinfo
  #define Uses_CLY_nl_langinfo 1
 #endif
 #undef  Uses_CLY_ssize_t
 #define Uses_CLY_ssize_t 1
 #ifdef Uses_getline
  #undef  Uses_CLY_getline
  #define Uses_CLY_getline 1
 #endif

 /* ifstream::getline behaves strangely in BC++
    I take the gcc implementation. I hope MSVC is like it. */
 #define IfStreamGetLine(istream,buffer,size) \
         istream.getline(buffer,size)

 #undef  CLY_DummyTStreamRW
 #define CLY_DummyTStreamRW(cla) void write(opstream &os) { cla::write(os); }; \
                                 void *read(ipstream &is) { return cla::read(is); };
 // From Mike:
 // v1300 is MSVC 7.00, not fully ISO C++ 1998
 // v1310 is MSVC 7.10 (aka .NET 2003 Architect), ISO C++ 1998 compliant
 #if _MSC_VER > 1300
  // Taked from gcc 3.1 definitions
  #define CLY_filebuf        std::filebuf
  #define CLY_strstreambuf   std::strstreambuf
  #define CLY_int_filebuf    CLY_filebuf
  #define CLY_NewFBFromFD(buf,f) buf=new CLY_int_filebuf(fdopen(f,"rb+"))
  #define CLY_streambuf      std::streambuf
  #define CLY_ISOCpp98 1
  #define CLY_OpenModeT      std::ios::openmode
  #define CLY_StreamPosT     std::streampos
  #define CLY_StreamOffT     std::streamoff
  #define CLY_IOSSeekDir     std::ios::seekdir
  #define CLY_FBOpenProtDef  0
  #define CLY_PubSetBuf(a,b) pubsetbuf(a,b)
  #define CLY_FBOpen(a,b,c)  open(a,b)
  #define CLY_IOSBin         std::ios::binary
  #define CLY_IOSOut         std::ios::out
  #define CLY_IOSIn          std::ios::in
  #define CLY_IOSApp         std::ios::app
  #define CLY_IOSAtE         std::ios::ate
  #define CLY_IOSBeg         std::ios::beg
  #define CLY_IOSCur         std::ios::cur
  #define CLY_IOSEnd         std::ios::end
  #define CLY_IOSBadBit      std::ios::badbit
  #define CLY_IOSEOFBit      std::ios::eofbit
  #define CLY_IOSFailBit     std::ios::failbit
  #define CLY_IOSGoodBit     std::ios::goodbit
  #define CLY_PubSeekOff     pubseekoff
  #define CLY_PubSync        pubsync
  #define CLY_std(a)         std::a
  #define UsingNamespaceStd  using namespace std;
  #define CreateStrStream(os,buf,size) std::string buf; std::ostringstream os(buf)
  #define GetStrStream(os,buf) os.str().c_str()
  #ifdef Uses_StrStream
   #undef  Include_sstream
   #define Include_sstream 1
  #endif
  // I'm not sure about the version, but .NET 2003 seems to only have the headers without .h
  #undef  FSTREAM_HEADER
  #define FSTREAM_HEADER  <fstream>
  #undef  IOMANIP_HEADER
  #define IOMANIP_HEADER  <iomanip>
  #undef  IOSTREAM_HEADER
  #define IOSTREAM_HEADER <iostream>
  #undef  STRSTREAM_HEADER
  #define STRSTREAM_HEADER <strstream>
  #if defined(Include_ioctl)
     #undef Include_ioctl
  #endif // Include_ioctl
 #else
  #define CLY_filebuf        filebuf
  #define CLY_int_filebuf    filebuf
  #define CLY_strstreambuf   strstreambuf
  #define CLY_streambuf      streambuf
  #define CLY_OpenModeT      int
  #define CLY_StreamPosT     streampos
  #define CLY_StreamOffT     streamoff
  #define CLY_IOSSeekDir     ios::seek_dir
  #define CLY_FBOpenProtDef  filebuf::openprot
  #define CLY_NewFBFromFD(buf,f) buf=new filebuf(f)
  #define CLY_PubSetBuf(a,b) setbuf(a,b)
  #define CLY_HaveFBAttach   1
  #define CLY_FBOpen(a,b,c)  open(a,b,c)
  #define CLY_IOSBin         ios::binary
  #define CLY_IOSOut         ios::out
  #define CLY_IOSIn          ios::in
  #define CLY_IOSApp         ios::app
  #define CLY_IOSAtE         ios::ate
  #define CLY_IOSBeg         ios::beg
  #define CLY_IOSCur         ios::cur
  #define CLY_IOSEnd         ios::end
  #define CLY_IOSBadBit      ios::badbit
  #define CLY_IOSEOFBit      ios::eofbit
  #define CLY_IOSFailBit     ios::failbit
  #define CLY_IOSGoodBit     ios::goodbit
  #define CLY_PubSeekOff     seekoff
  #define CLY_PubSync        sync
  #define CLY_std(a)         a
  #define UsingNamespaceStd
  #define CreateStrStream(os,buf,size) char buf[size]; \
                                       ostrstream os(buf,sizeof(buf))
  #define GetStrStream(os,buf) buf
  #ifdef Uses_StrStream
   #undef  Include_strstrea
   #define Include_strstrea 1
  #endif
 #endif
#endif

#ifdef Uses_IOS_BIN
 #undef  IOS_BIN
 #define IOS_BIN CLY_IOSBin
#endif

#ifdef Uses_snprintf
 #ifdef CLY_Have_snprintf
  #undef  Include_stdio
  #define Include_stdio 1
  #ifdef TVComp_MSC
   // Note: Currently unused because the implementation is wrong. See notes in
   // the MSVC section.
   #define CLY_snprintf  _snprintf
   #define CLY_vsnprintf _vsnprintf
  #else
   #define CLY_snprintf  snprintf
   #define CLY_vsnprintf vsnprintf
  #endif
 #else
  #undef  Include_stdarg
  #define Include_stdarg 1
 #endif
#endif

// Simple Streams Compatibility hack *EXPERIMENTAL*
#ifdef HAVE_SSC
 #undef CLY_filebuf
 #undef CLY_strstreambuf
 #undef CLY_int_filebuf
 #undef CLY_streambuf
 #undef CLY_OpenModeT
 #undef CLY_StreamPosT
 #undef CLY_StreamOffT
 #undef CLY_IOSSeekDir
 #undef CLY_FBOpenProtDef
 #undef CLY_NewFBFromFD
 #undef CLY_PubSetBuf
 #undef CLY_HaveFBAttach
 #undef CLY_FBOpen
 #undef CLY_IOSBin
 #undef CLY_IOSOut
 #undef CLY_IOSIn
 #undef CLY_IOSApp
 #undef CLY_IOSAtE
 #undef CLY_IOSBeg
 #undef CLY_IOSCur
 #undef CLY_IOSEnd
 #undef CLY_IOSBadBit
 #undef CLY_IOSEOFBit
 #undef CLY_IOSFailBit
 #undef CLY_IOSGoodBit
 #undef CLY_PubSeekOff
 #undef CLY_PubSync
 #undef CLY_DefineSpecialFileBuf
 #undef CLY_ISOCpp98

 #define CLY_filebuf        SSC_filebuf
 #define CLY_int_filebuf    SSC_filebuf
 #define CLY_strstreambuf   SSC_strstreambuf
 #define CLY_streambuf      SSC_streambuf
 #define CLY_OpenModeT      SSC_ios::openmode
 #define CLY_StreamPosT     SSC_ios::seekoff
 #define CLY_StreamOffT     SSC_ios::seekoff
 #define CLY_IOSSeekDir     SSC_ios::seekdir
 #define CLY_FBOpenProtDef  0666
 #define CLY_NewFBFromFD(buf,f) buf=new SSC_filebuf(f,SSC_ios::in|SSC_ios::out|SSC_ios::binary)
 #define CLY_PubSetBuf(a,b) setbuf(a,b)
 #define CLY_HaveFBAttach   1
 #define CLY_FBOpen(a,b,c)  open(a,b,c)
 #define CLY_IOSBin         SSC_ios::binary
 #define CLY_IOSOut         SSC_ios::out
 #define CLY_IOSIn          SSC_ios::in
 #define CLY_IOSApp         SSC_ios::app
 #define CLY_IOSAtE         SSC_ios::ate
 #define CLY_IOSBeg         SSC_ios::beg
 #define CLY_IOSCur         SSC_ios::cur
 #define CLY_IOSEnd         SSC_ios::end
 #define CLY_IOSBadBit      SSC_ios::badbit
 #define CLY_IOSEOFBit      SSC_ios::eofbit
 #define CLY_IOSFailBit     SSC_ios::failbit
 #define CLY_IOSGoodBit     SSC_ios::goodbit
 #define CLY_PubSeekOff     seekoff
 #define CLY_PubSync        sync
#endif

CLY_CFunc void CLY_YieldProcessor(int micros);
CLY_CFunc void CLY_ReleaseCPU();
/* Return the number of ticks (on MSDOS 1 tick is 1/18 sec),
   this is used to compute the double click */
CLY_CFunc unsigned short CLY_Ticks(void);
/* An utility to split directory and file components of a path:
   Extracts from path the directory part and filename part.
   if 'dir' and/or 'file' == NULL, it is not filled.
   The directory will have a trailing slash. */
CLY_CFunc void CLY_ExpandPath(const char *path, char *dir, char *file);
/* An utility function. It makes the path an absolute one and replaces the
   original value passed. Should use fixpath. */
CLY_CFunc void CLY_fexpand(char *rpath);
#undef  fexpand
#define fexpand(a) CLY_fexpand(a)
/* Utility function to know if a drive letter is valid. */
CLY_CFunc int  CLY_DriveValid(char drive);
#ifdef __cplusplus
 #define driveValid(a) (CLY_DriveValid(a) ? True : False)
#endif
/* Utility function to know is a file is in fact a directory */
CLY_CFunc int CLY_IsDir(const char *str);
#ifdef __cplusplus
 #define isDir(a) (CLY_DriveValid(a) ? True : False)
#endif
/* Utility function to know if a path is valid (existing directory or file) */
CLY_CFunc int CLY_PathValid(const char *path);
#ifdef __cplusplus
 #define pathValid(a) (CLY_PathValid(a) ? True : False)
#endif
/* Utility function to know if a file name is valid to create a new file */
CLY_CFunc int CLY_ValidFileName(const char *fileName);
#ifdef __cplusplus
 #define validFileName(a) (CLY_ValidFileName(a) ? True : False)
#endif
/* Utility function to know the current directory including a trailing
   slash */
CLY_CFunc void CLY_GetCurDirSlash(char *dir);
#undef  getCurDir
#define getCurDir(a) CLY_GetCurDirSlash(a)
/* Utility function to know if a path contains a supported wildcard */
CLY_CFunc int CLY_IsWild(const char *f);
#ifdef __cplusplus
 #define isWild(a) (CLY_IsWild(a) ? True : False)
#endif
/* Utility function to know if a file exist and we can read from it */
CLY_CFunc int CLY_FileExists(const char *fname);
#ifndef TVCompf_djgpp
 #undef  __file_exists
 #define __file_exists(a) CLY_FileExists(a)
#endif
/* Utility function to know if a path is relative */
CLY_CFunc int CLY_IsRelativePath(const char *path);
#ifdef __cplusplus
 #define relativePath(a) (CLY_IsRelativePath(a) ? True : False)
#endif
/* Utility function to generated a beep */
#define CLY_Beep() TScreen::beep()
/* Used internally, just call filelength prior request */
CLY_CFunc long CLY_filelength(int);
/* Used internally, just call getcurdir prior request */
CLY_CFunc int  CLY_getcurdir(int drive, char *buffer);

/* cl/unistd.h includes dir.h */
#ifdef Include_cl_unistd
 #undef  Include_dir
 #define Include_dir 1
#endif

#ifdef Uses_ifsFileLength
 #undef  Uses_fstream
 #define Uses_fstream 1
#endif

#ifdef Uses_CLY_getline
 /* Prototypes uses ssize_t and size_t */
 #undef  Include_sys_types
 #define Include_sys_types 1
#endif

#ifdef Uses_sys_types
 #undef  Include_sys_types
 #define Include_sys_types 1
#endif

#if defined(Include_sys_types) && !defined(Included_sys_types)
 #define Included_sys_types 1
 #include <sys/types.h>
 /* Platforms where sys/types.h doesn't define ssize_t: */
 #if defined(Uses_CLY_ssize_t) && !defined(CLY_ssize_t)
  #define CLY_ssize_t 1
  typedef long ssize_t;
 #endif
#endif

#if defined(Include_string) && !defined(Included_string)
 #define Included_string 1
 #include <string.h>
 #if defined(TVOSf_QNXRtP)
  #include <strings.h> // QNX RtP requires this include for functions strcasecmp, etc.
 #endif // TVOSf_QNXRtP
 #ifndef CLY_memcpy
  #if CLY_SAFE_MEMCPY
   #define CLY_memcpy(a,b,c) memcpy(a,b,c)
  #else
   #define CLY_memcpy(a,b,c) memmove(a,b,c)
  #endif
 #endif
#endif

#if defined(Include_stdio) && !defined(Included_stdio)
 // Looks like Watcom needs it before other headers.
 // Is this a temporal hack? should be clarified
 #define Included_stdio 1
 #include <stdio.h>
#endif

#if defined(Include_limits) && !defined(Included_limits)
 #define Included_limits 1
 #include <limits.h>
 #ifndef PATH_MAX /* BC++ and MSVC */
  #define PATH_MAX 512
 #endif
#endif

#if defined(Include_fcntl) && !defined(Included_fcntl)
 #define Included_fcntl 1
 #include <fcntl.h>
 #ifndef O_TEXT /* UNIX */
  #define O_TEXT (0)
 #endif
 #ifndef O_BINARY /* UNIX */
  #define O_BINARY (0)
 #endif
 #ifdef CLY_DefineFileConstants
  #if !defined(O_RDONLY) && defined(_O_RDONLY)
   #define  O_RDONLY _O_RDONLY
  #endif
  #if !defined(O_WRONLY) && defined(_O_WRONLY)
   #define  O_WRONLY _O_WRONLY
  #endif
  #if !defined(O_RDWR) && defined(_O_RDWR)
   #define  O_RDWR _O_RDWR
  #endif
  #if !defined(O_APPEND) && defined(_O_APPEND)
   #define  O_APPEND _O_APPEND
  #endif
  #if !defined(O_CREAT) && defined(_O_CREAT)
   #define  O_CREAT _O_CREAT
  #endif
  #if !defined(O_EXCL) && defined(_O_EXCL)
   #define  O_EXCL _O_EXCL
  #endif
  #if !defined(O_TRUNC) && defined(_O_TRUNC)
   #define  O_TRUNC _O_TRUNC
  #endif
 #endif
#endif

#if defined(Include_sys_stat) && !defined(Included_sys_stat)
 #define Included_sys_stat 1
 #include <sys/stat.h>
 #ifdef Fake_S_IS
  #undef  S_ISDIR
  #define S_ISDIR(m)  ((m) & S_IFDIR)
  #undef  S_ISCHR
  #define S_ISCHR(m)  ((m) & S_IFCHR)
  #undef  S_ISBLK
  #define S_ISBLK(m)  ((m) & S_IFBLK)
  #undef  S_ISREG
  #define S_ISREG(m)  ((m) & S_IFREG)
  #undef  S_ISFIFO
  #define S_ISFIFO(m) ((m) & S_IFIFO)
 #endif
 #ifdef CLY_DefineIRW
  #ifndef S_IREAD
   #define S_IREAD _S_IREAD
  #endif
  #ifndef S_IWRITE
   #define S_IWRITE _S_IWRITE
  #endif
 #endif
#endif

#if defined(Include_ioctl) && !defined(Included_ioctl)
 #define Included_ioctl 1
 #include IOCTL_HEADER
#endif

#if defined(Include_stdlib) && !defined(Included_stdlib)
 #define Included_stdlib 1
 #include <stdlib.h>
#endif

#if defined(Include_alloca) && !defined(Included_alloca)
 /* OSF1 (Tru64), some combinations of Solaris and gcc too. */
 #define Included_alloca 1
 #include <alloca.h>
#endif

#if defined(Include_malloc) && !defined(Included_malloc)
 /* BC++ and MSVC defines alloca here */
 #define Included_malloc 1
 #include <malloc.h>
#endif

#if defined(Include_io) && !defined(Included_io)
 #define Included_io 1
 #undef _CRT_INSECURE_OPEN // Visual Studio .NET 2005 Express Beta Edition
 #include <io.h>
 #ifdef CLY_Define_Inline_IO_Translations
  /* Needed for MSVC */
  /* Note we must avoid macros here or we risk to collide with TV read/write
     C++ members. */
  #ifndef CLY_close_inline_defined
   #define CLY_close_inline_defined 1
   __inline int close(int FILEDES)
    { return _close(FILEDES); }
  #endif
  #ifndef CLY_dup_inline_defined
   #define CLY_dup_inline_defined 1
   __inline int dup(int OLD)
    { return _dup(OLD); }
  #endif
  #ifndef CLY_dup2_inline_defined
   #define CLY_dup2_inline_defined 1
   __inline int dup2(int OLD, int NEW)
    { return _dup2(OLD,NEW); }
  #endif
  #if (_MSC_VER<1310) && !defined(CLY_read_inline_defined)
   #define CLY_read_inline_defined 1
   __inline long read(int FILEDES, void *BUFFER, unsigned long SIZE)
    { return _read(FILEDES,BUFFER,SIZE); }
  #endif
  #if (_MSC_VER<1310) && !defined(CLY_write_inline_defined)
   #define CLY_write_inline_defined 1
   __inline long write(int FILEDES, void *BUFFER, unsigned long SIZE)
    { return _write(FILEDES,BUFFER,SIZE); }
  #endif
  #ifndef CLY_lseek_inline_defined
   #define CLY_lseek_inline_defined 1
   __inline long lseek(int FILEDES, long OFFSET, int WHENCE)
    { return _lseek(FILEDES,OFFSET,WHENCE); }
  #endif
 #endif
#endif

#if defined(Include_direct) && !defined(Included_direct)
 #define Included_direct 1
 #include <direct.h>
#endif

#if defined(Include_dir) && !defined(Included_dir)
 #define Included_dir 1
 #include <dir.h>
#endif

#if defined(Include_cl_dir) && !defined(Included_cl_dir)
 #define Included_cl_dir 1
 #include <cl/dir.h>
#endif

#if defined(Include_langinfo) && !defined(Included_langinfo)
 #define Included_langinfo 1
 #include <langinfo.h>
 /* Workaround a bug in glibc */
 #if !defined(RADIXCHAR) && defined(DECIMAL_POINT)
  #define RADIXCHAR DECIMAL_POINT
 #endif
#endif


#if defined(Include_ctype) && !defined(Included_ctype)
 #define Included_ctype 1
 #include <ctype.h>
 /* The following macros are defined to avoid passing negative values to
    the ctype functions in the common case: isxxxx(char).
    They are suitable for x86 DOS and Linux, perhaps not for others */
 #define ucisalnum(a)  isalnum((unsigned char)a)
 #define ucisalpha(a)  isalpha((unsigned char)a)
 #define ucisascii(a)  isascii((unsigned char)a)
 #define uciscntrl(a)  iscntrl((unsigned char)a)
 #define ucisdigit(a)  isdigit((unsigned char)a)
 #define ucisgraph(a)  isgraph((unsigned char)a)
 #define ucislower(a)  islower((unsigned char)a)
 #define ucisprint(a)  isprint((unsigned char)a)
 #define ucispunct(a)  ispunct((unsigned char)a)
 #define ucisspace(a)  isspace((unsigned char)a)
 #define ucisupper(a)  isupper((unsigned char)a)
 #define ucisxdigit(a) isxdigit((unsigned char)a)
 #define uctoascii(a)  toascii((unsigned char)a)
 #define uctolower(a)  tolower((unsigned char)a)
 #define uctoupper(a)  toupper((unsigned char)a)
#endif

#if defined(Include_unistd) && !defined(Included_unistd)
 #define Included_unistd 1
 #include <unistd.h>
 // It just fills in the gaps
 #include <cl/unistd.h>
#endif

#if defined(Include_process) && !defined(Included_process)
 #define Included_process 1
 #include <process.h>
#endif

#if defined(Include_cl_unistd) && !defined(Included_cl_unistd)
 #define Included_cl_unistd 1
 #include <cl/unistd.h>
#endif

#if defined(Include_glob) && !defined(Included_glob)
 #define Included_glob 1
 /* POSIX */
 #include <glob.h>
#endif

#if defined(Include_cl_glob) && !defined(Included_cl_glob)
 #define Included_cl_glob 1
 /* Replacement */
 #include <cl/glob.h>
#endif

#if defined(Include_fnmatch) && !defined(Included_fnmatch)
 #define Included_fnmatch 1
 /* POSIX */
 #include <fnmatch.h>
#endif

#if defined(Include_cl_fnmatch) && !defined(Included_cl_fnmatch)
 #define Included_cl_fnmatch 1
 /* Replacement */
 #include <cl/fnmatch.h>
#endif

#if defined(Include_regex) && !defined(Included_regex)
 #define Included_regex 1
 /* POSIX */
 #include <regex.h>
#endif

#if defined(Include_cl_regex) && !defined(Included_cl_regex)
 #define Included_cl_regex 1
 /* Replacement */
 #include <cl/regex.h>
#endif

#if defined(Include_getopt) && !defined(Included_getopt)
 #define Included_getopt 1
 #include <getopt.h>
 // Define our aliases
 #define CLY_optarg             optarg
 #define CLY_optind             optind
 #define CLY_opterr             opterr
 #define CLY_optopt             optopt
 #define CLY_option             option
 #define CLY_no_argument        no_argument
 #define CLY_required_argument  required_argument
 #define CLY_optional_argument  optional_argument
 #define CLY_getopt             getopt
 #define CLY_getopt_long        getopt_long
 #define CLY_getopt_long_only   getopt_long_only
#endif

#if defined(Include_cl_getopt) && !defined(Included_cl_getopt)
 #define Included_cl_getopt 1
 // This header defines all as CLY_* to avoid conflicts
 #include <cl/getopt.h>
#endif

#if defined(Include_dirent) && !defined(Included_dirent)
 #define Included_dirent 1
 #include <dirent.h>
#endif

#if defined(Include_cl_dirent) && !defined(Included_cl_dirent)
 #define Included_cl_dirent 1
 #include <cl/dirent.h>
#endif

#if defined(Include_time) && !defined(Included_time)
 #define Included_time 1
 #include <time.h>
#endif

#if defined(Include_utime) && !defined(Included_utime)
 #define Included_utime 1
 #include <utime.h>
#endif

#if defined(Include_stdarg) && !defined(Included_stdarg)
 #define Included_stdarg 1
 #include <stdarg.h>
#endif

#if defined(Include_signal) && !defined(Included_signal)
 #define Include_signal 1
 #include <signal.h>
#endif

#if defined(Include_sys_utime) && !defined(Included_sys_utime)
 #define Included_sys_utime 1
 #include <sys/utime.h>
#endif

#if defined(Include_cl_utime) && !defined(Included_cl_utime)
 #define Included_cl_utime 1
 #include <cl/utime.h>
#endif

#if defined(Uses_allegro) && defined(HAVE_ALLEGRO)
 // Currently a trivial condition
 #undef  Include_allegro
 #define Include_allegro 1
#endif

#if defined(Include_allegro)
 #if !defined(Included_allegro)
  #define Included_allegro 1
  #include <allegro.h>
 #endif
#else
 #undef  END_OF_MAIN
 #define END_OF_MAIN()
#endif

#if defined(Include_strstream) && !defined(Included_strstream)
 #define Included_strstream 1
 #include STRSTREAM_HEADER
#endif

#if defined(Include_strstrea) && !defined(Included_strstrea)
 #define Included_strstrea 1
 #include <strstrea.h>
#endif

#if defined(Include_sstream) && !defined(Included_sstream)
 #define Included_sstream 1
 #include <sstream>
#endif

#if defined(Uses_fstream) && !defined(Included_fstream)
 #define Included_fstream 1
 #include FSTREAM_HEADER
#endif

#if defined(Uses_iomanip) && !defined(Included_iomanip)
 #define Included_iomanip 1
 #include IOMANIP_HEADER
#endif

#if defined(Uses_iostream) && !defined(Included_iostream)
 #define Included_iostream 1
 #include IOSTREAM_HEADER
#endif

#if defined(Uses_SSC_Streams) && !defined(Included_SSC_Streams)
 #include <tv/ssc_streams.h>
#endif

#if defined(Uses_CLYFileAttrs) && !defined(Uses_CLYFileAttrsDef)
#define Uses_CLYFileAttrsDef
/* Equivalent to mode_t */
#ifdef CLY_Have_UGID
/* In systems with User and Group ID the mode is an structure */
typedef struct
{
 mode_t mode,writemask;
 uid_t  user;
 gid_t  group;
} CLY_mode_t;
#else
typedef unsigned int CLY_mode_t;
#endif
/* Utility function to find the attributes of a file. You must call stat
   first and pass the st_mode member of stat's struct in statVal. */
CLY_EXPORT CLY_CFunc void CLY_GetFileAttributes(CLY_mode_t *mode, struct stat *statVal, const char *fileName);
/* The reverse. The file must be closed! */
CLY_EXPORT CLY_CFunc int CLY_SetFileAttributes(CLY_mode_t *newmode, const char *fileName);
/* This function alters mode content so the attribute indicates that the
   owner of the file can't read from it */
CLY_EXPORT CLY_CFunc void CLY_FileAttrReadOnly(CLY_mode_t *mode);
/* This function alters mode content so the attribute indicates that the
   owner of the file can read from it */
CLY_EXPORT CLY_CFunc void CLY_FileAttrReadWrite(CLY_mode_t *mode);
/* Returns !=0 if the file is read-only */
CLY_EXPORT CLY_CFunc int  CLY_FileAttrIsRO(CLY_mode_t *mode);
/* Sets the attribute that indicates the file was modified */
CLY_EXPORT CLY_CFunc void CLY_FileAttrModified(CLY_mode_t *mode);
/* It returns a mode that can be used for a newly created file */
CLY_EXPORT CLY_CFunc void CLY_GetDefaultFileAttr(CLY_mode_t *mode);
#endif

/* Returns the name of the shell command */
CLY_CFunc CLY_EXPORT char *CLY_GetShellName(void);

/* v/snprintf functions */
#if defined(Uses_snprintf) && !defined(CLY_Have_snprintf)
CLY_EXPORT CLY_CFunc int CLY_vsnprintf(char *string, size_t length, const char * format, va_list args);
CLY_EXPORT CLY_CFunc int CLY_snprintf(char *string, size_t length, const char * format, ...);
#endif

#ifdef Uses_ifsFileLength
CLY_EXPORT long CLY_ifsFileLength(CLY_std(ifstream) &f);
#endif

#ifdef Uses_CLY_IfStreamGetLine
CLY_EXPORT int CLY_IfStreamGetLine(CLY_std(ifstream) &is, char *buffer, unsigned len);
#endif

/* Internal definition of nl_langinfo */
#if defined(Uses_CLY_nl_langinfo) && !defined(CLY_nl_langinfo_Defined)
#define CLY_nl_langinfo_Defined 1
typedef int nl_item;
#define CURRENCY_SYMBOL   0 /*currency_symbol*/
#define CRNCYSTR          0 /*currency_symbol*/    /*deprecated*/
#define DECIMAL_POINT     1 /*decimal_point*/
#define RADIXCHAR         1 /*decimal_point*/      /*deprecated*/
#define GROUPING          2 /*grouping*/
#define INT_CURR_SYMBOL   3 /*int_curr_symbol*/
#define MON_DECIMAL_POINT 4 /*mon_decimal_point*/
#define MON_GROUPING      5 /*mon_grouping*/
#define NEGATIVE_SIGN     6 /*negative_sign*/
#define POSITIVE_SIGN     7 /*positive_sign*/
#define THOUSANDS_SEP     8 /*thousands_sep*/
#define THOUSEP           8 /*thousands_sep*/      /*deprecated*/
/*char*/
#define FRAC_DIGITS       9 /*frac_digits*/
#define INT_FRAC_DIGITS  10 /*int_frac_digits*/
#define N_CS_PRECEDES    11 /*n_cs_precedes*/
#define N_SEP_BY_SPACE   12 /*n_sep_by_space*/
#define P_SIGN_POSN      13 /*p_sign_posn*/
#define N_SIGN_POSN      14 /*n_sign_posn*/
#define P_CS_PRECEDES    15 /*p_cs_precedes*/
#define P_SEP_BY_SPACE   16 /*p_sep_by_space*/

#define nl_langinfo CLY_nl_langinfo
CLY_CFunc CLY_EXPORT char *CLY_nl_langinfo(nl_item item);
#endif /* defined(Uses_CLY_nl_langinfo) && !defined(CLY_nl_langinfo_Defined) */

#ifdef Uses_CLY_getline
CLY_CFunc CLY_EXPORT ssize_t CLY_getstr(char **lineptr, size_t *n, FILE *stream,
                                        char terminator, int offset, int limit);
CLY_CFunc CLY_EXPORT ssize_t CLY_getline(char **lineptr, size_t *n, FILE *stream);
CLY_CFunc CLY_EXPORT ssize_t CLY_getline_safe(char **lineptr, size_t *n,
                                              FILE *stream, int limit);
#endif

#undef CLY_High16
#undef CLY_Low16
#ifdef TV_BIG_ENDIAN
 /* Most RISC machines */
 #define CLY_High16(a)  ((a) & 0xFF)
 #define CLY_Low16(a)   ((a) >> 8)
#else
 /* Intel machines */
 #define CLY_High16(a)  ((a) >> 8)
 #define CLY_Low16(a)   ((a) & 0xFF)
#endif

#if defined(DJGPP_HaveLFNs) && !defined(CLY_HaveLFNs_Defined)
#define CLY_HaveLFNs_Defined 1
inline
int CLY_HaveLFNs()
{
 return _use_lfn(0);
}
#endif

#if defined(OS_HaveLFNs) && !defined(OS_HaveLFNs_Defined)
#define OS_HaveLFNs_Defined 1
inline
int CLY_HaveLFNs()
{
 return 1;
}
#endif

/* Is that an EOL char? */
/* Ask for no */
#undef  CLY_IsntEOL
#define CLY_IsntEOL(a) (a!='\r' && a!='\n')
/* Ask for yes */
#undef  CLY_IsEOL
#define CLY_IsEOL(a)   (a=='\r' || a=='\n')

#undef CLY_LenEOL
#undef CLY_crlf

/* MSS memory leak debugger */
#ifdef MSS
 #include <mss.h>
#endif

#undef CLY_IsTrueEOL
#ifdef CLY_UseCrLf
 #define CLY_LenEOL     2
 /* This asks if the EOL is really usable for the OS */
 #define CLY_IsTrueEOL(a) (a=='\r' || a=='\n')
 /* String containing EOL */
 #define CLY_crlf "\r\n"
#else
 #define CLY_LenEOL     1
 #define CLY_crlf "\n"
 #define CLY_IsTrueEOL(a) (a=='\n')
#endif

#undef CLY_CFunc

/*#endif // CLY_CompatLayerIncluded */

