/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1999-2001 by Salvador Eduardo Tropea */

#ifdef FORCE_INTL_SUPPORT
#define HAVE_INTL_SUPPORT 1
#else
#include <tv/configtv.h>
#endif

#ifdef HAVE_INTL_SUPPORT

#ifdef TVCompf_djgpp
 // Avoid collisions with conio
 #undef gettext
#endif
#ifdef __cplusplus
extern "C" {
#endif
// We know the prototypes and it helps with the dummy
char *textdomain(const char *domainname);
char *bindtextdomain(const char *domainname, const char *dirname);
char *gettext(const char *msgid);
char *gettext__(const char *msgid);
#ifdef __cplusplus
}
#endif
//#include <libintl.h>

// Must use __DJGPP__ here
#ifndef __DJGPP__
# if !defined(__GLIBC__) || __GLIBC__<2
   // By default use gettext
#  define gettext__ gettext
# else
#  if (__GLIBC__==2 && __GLIBC_MINOR__>0) || __GLIBC__>2
#   define gettext__ gettext
#  else
    // exception: glibc 2.0 needs __gettext
#   define gettext__ __gettext
#  endif
# endif
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

#else  /* HAVE_INTL_SUPPORT */

/***************************** NO intl support ***************************/
#ifndef __RH_INTL_H__
#define __RH_INTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#if !defined( GETTEXT )
#define GETTEXT gettext__
#endif

#if !defined( gettext_noop )
#define gettext_noop(msg) (msg)
#endif

#if !defined( GETTEXT_NOOP )
#define GETTEXT_NOOP gettext_noop
#endif

#if !defined( TEXTDOMAIN )
#define TEXTDOMAIN 
#endif

#if !defined( BINDTEXTDOMAIN )
#define BINDTEXTDOMAIN
#endif

#if !defined( _ )
#define _(msg) ((char *)msg)
#endif

#if !defined( __ )
#define __(msg) GETTEXT_NOOP(msg)
#endif

#endif

#endif /* else HAVE_INTL_SUPPORT */
