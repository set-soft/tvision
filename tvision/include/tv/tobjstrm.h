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

#if !defined( __fLink_def )
#define __fLink_def
struct fLink
{
    fLink *f;
    class TStreamableClass *t;
    #ifdef TVComp_BCPP
    fLink(fLink* af, class TStreamableClass *at) : f(af), t(at) {}
    #endif
};
#endif

#ifdef TVComp_BCPP
#define __link( s )             \
  extern TStreamableClass s;    \
  static fLink force ## s       \
    ( (fLink *)&force ## s, (TStreamableClass *)&s );
#else
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


