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

class TSubMenu;
class TMenuItem;
class TStatusDef;
class TStatusItem;

TSubMenu& operator + ( TSubMenu& s, TMenuItem& i );
TSubMenu& operator + ( TSubMenu& s1, TSubMenu& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusItem& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusDef& s2 );

#if defined( Uses_TMenuItem )

#include <menuitem.h>

#endif

#if defined( Uses_TSubMenu )

#include <submenu.h>

#endif

#if defined( Uses_TMenu )

#include <menu.h>

#endif

#if defined( Uses_TMenuView )

#include <menuview.h>

#endif

#if defined( Uses_TMenuBar )

#include <menubar.h>

#endif

#if defined( Uses_TMenuBox )

#include <menubox.h>

#endif

#if defined( Uses_TStatusItem )

#include <statsitm.h>

#endif

#if defined( Uses_TStatusDef )

#include <statsdef.h>

#endif

#if defined( Uses_TStatusLine )

#include <statslin.h>

#endif


