/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
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
#define _(msg) (msg)
#endif

#if !defined( __ )
#define __(msg) GETTEXT_NOOP(msg)
#endif

#endif
