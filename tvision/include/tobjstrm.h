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
};
#endif

#define __link( s )             \
  extern TStreamableClass s;    \
  static fLink force ## s =     \
    { (fLink *)&force ## s, (TStreamableClass *)&s };

#if defined( Uses_TStreamable )

#include <streambl.h>

#endif

#if defined( Uses_TStreamableClass )

#include <strmblcl.h>

#endif

#if defined( Uses_TStreamableTypes )

#include <strmblty.h>

#endif

#if defined( Uses_TPWrittenObjects )

#include <pwritobj.h>

#endif

#if defined( Uses_TPReadObjects )

#include <preadobj.h>

#endif

#if defined( Uses_pstream )

#include <pstream.h>

#endif

#if defined( Uses_ipstream )

#include <ipstream.h>

#endif

#if defined( Uses_opstream )

#include <opstream.h>

#endif

#if defined( Uses_iopstream )

#include <iopstrm.h>

#endif

#if defined( Uses_fpbase )

#include <fpbase.h>

#endif

#if defined( Uses_ifpstream )

#include <ifpstrm.h>

#endif

#if defined( Uses_ofpstream )

#include <ofpstrm.h>

#endif

#if defined( Uses_fpstream )

#include <fpstream.h>

#endif


