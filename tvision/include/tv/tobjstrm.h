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

typedef unsigned P_id_type;

// SET: Here we implement a trick to pull the needed TStreamableClass-es.
// We have two approaches:
// 1) We create a structure with a pointer to itself and another to the
//    object to register the stream for the class.
// 2) Same but we use a constructor in the structure.
// The first methode was tested with gcc 2.6.3 to 3.4.1. It seems to fail
// for gcc 3.4.1, but we know it works for most versions of gcc.
// The second was specifically created to BC++ compiler.
// The BC++ approach should work for any C++ compiler, but fails for gcc 3.3.5.
// For this reason I'm keeping both and switching to the second approach
// only for gcc>=3.4.
// GCC 3.4.x and 4.0.0 introduces another problem: statics are optimized out.
// Even when they have side-effects.
// Another option for gcc (suggested by Andris):
// #define __link( s ) __asm__(".global  _"#s);

#undef TV_Use_Constructor_For_fLink_def
#undef TV_Use_Namespace_instead_of_Static
#if defined(TVComp_BCPP)
 #define TV_Use_Constructor_For_fLink_def 1
 #define TV_Use_Namespace_instead_of_Static 0
#elif __GNUC__>=3 && (__GNUC__>=4 || __GNUC_MINOR__>=4)
 #define TV_Use_Constructor_For_fLink_def 1
 #define TV_Use_Namespace_instead_of_Static 1
#else
 #define TV_Use_Constructor_For_fLink_def 0
#endif

#if !defined(TV__fLink_def)
#define TV__fLink_def
struct fLink
{
    fLink *f;
    class TStreamableClass *t;
    #if TV_Use_Constructor_For_fLink_def
    fLink(fLink* af, class TStreamableClass *at) : f(af), t(at) {}
    #endif
};
#endif

#if TV_Use_Constructor_For_fLink_def
 // This approach fails for gcc 3.3.5 so we can't use it for all gcc versions.
 #if TV_Use_Namespace_instead_of_Static
  // Ok for gcc 3.4.4 and 4.0.0
  // Suggested by Andris Pavenis he said: Anonymous namespace is very similar to static
  // (it's a separate for each compiled source). So we are not going to have name
  // collisions.
  #define __link( s )             \
    extern TStreamableClass s;    \
    namespace {                   \
    fLink force ## s              \
      ( (fLink *)&force ## s, (TStreamableClass *)&s ); \
    }
 #else
  // Ok for BC++ 5.5/6
  // Fails with gcc 3.4+ because the static code gets "optimized" That's a bug from my
  // point of view because the static code have external side-effects (calls a constructor)
  #define __link( s )             \
    extern TStreamableClass s;    \
    static fLink force ## s       \
      ( (fLink *)&force ## s, (TStreamableClass *)&s );
 #endif
#else
 // Ok for gcc 2.7.2, 2.8.1, 2.95.3/4, 3.3.5 and all MSVC versions we tried.
 // Fails for gcc 3.4+ read above.
 #define __link( s )             \
   extern TStreamableClass s;    \
   static fLink force ## s =     \
     { (fLink *)&force ## s, (TStreamableClass *)&s };
#endif

#if defined( Uses_TStreamable )

#include <tv/streambl.h>

#endif

#if defined( Uses_TStreamableClass )

#include <tv/strmblcl.h>

#endif

#if defined( Uses_TStreamableTypes )

#include <tv/strmblty.h>

#endif

#if defined( Uses_TPWrittenObjects )

#include <tv/pwritobj.h>

#endif

#if defined( Uses_TPReadObjects )

#include <tv/preadobj.h>

#endif

#if defined( Uses_pstream )

#include <tv/pstream.h>

#endif

#if defined( Uses_ipstream )

#include <tv/ipstream.h>

#endif

#if defined( Uses_opstream )

#include <tv/opstream.h>

#endif

#if defined( Uses_iopstream )

#include <tv/iopstrm.h>

#endif

#if defined( Uses_fpbase )

#include <tv/fpbase.h>

#endif

#if defined( Uses_ifpstream )

#include <tv/ifpstrm.h>

#endif

#if defined( Uses_ofpstream )

#include <tv/ofpstrm.h>

#endif

#if defined( Uses_fpstream )

#include <tv/fpstream.h>

#endif


