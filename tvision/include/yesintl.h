/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __cplusplus
extern "C" {
#endif

char *gettext__(const char *);
char *textdomain__(const char *);
char *bindtextdomain__(const char *,const char *);

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
#define TEXTDOMAIN textdomain__
#endif

#if !defined( BINDTEXTDOMAIN )
#define BINDTEXTDOMAIN bindtextdomain__
#endif

#if !defined( _ )
#define _(msg) GETTEXT(msg)
#endif

#if !defined( __ )
#define __(msg) GETTEXT_NOOP(msg)
#endif
