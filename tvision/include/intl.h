/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libintl.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __DJGPP__
char *gettext__(const char *);
#else
# if !defined(__GLIBC__) || __GLIBC__ < 2
# define gettext__ gettext
# else
# if (__GLIBC__==2 && __GLIBC_MINOR__>0) || __GLIBC__>2
# define gettext__ gettext
# else
# define gettext__ __gettext
# endif
# endif
#endif

#ifdef __cplusplus
}
#endif

#if !defined( GETTEXT )
#define GETTEXT gettext__
#endif

#if !defined( gettext_noop )
#define gettext_noop(msg) msg
#endif

#if !defined( GETTEXT_NOOP )
#define GETTEXT_NOOP gettext_noop
#endif

#if !defined( TEXTDOMAIN )
#define TEXTDOMAIN textdomain
#endif

#if !defined( BINDTEXTDOMAIN )
#define BINDTEXTDOMAIN bindtextdomain
#endif

#if !defined( _ )
#define _(msg) GETTEXT(msg)
#endif

#if !defined( __ )
#define __(msg) GETTEXT_NOOP(msg)
#endif
